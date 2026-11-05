#include <Enemy/Boss/Boss.h>
#include <Enemy/Boss/AI/BossAIController.h>
#include <Enemy/Boss/Data/BossAIData.h>
#include <Enemy/Data/EnemyAttackData.h>
#include <Enemy/Components/EnemyStatComponent.h>
#include <Enemy/Components/AttackTraceComponent.h>
#include <Enemy/AI/EnemyAIBlackboardKey.h>
#include <Enemy/EnemyAIController.h>
#include <Interface/CombatDamageInterface.h>
#include <Data/DamageInfo.h>
#include <BehaviorTree/BehaviorTree.h>
#include <BehaviorTree/BehaviorTreeComponent.h>
#include <BehaviorTree/BlackboardComponent.h>
#include <Animation/AnimInstance.h>
#include <Animation/AnimMontage.h>
#include <Components/SkeletalMeshComponent.h>
#include <Components/CapsuleComponent.h>
#include <GameFramework/CharacterMovementComponent.h>

ABoss::ABoss()
{
	//---------------------------------------------------------------
	// 보스 공통 AIController 설정.
	//
	//---------------------------------------------------------------
	AIControllerClass = ABossAIController::StaticClass();

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ABoss::BeginPlay()
{
	Super::BeginPlay();

	//---------------------------------------------------------------
	// 재도전 시 복구할 시작 위치 저장.
	//
	//---------------------------------------------------------------
	InitialTransform = GetActorTransform();

	if (EnemyStatComponet)
	{
		EnemyStatComponet->OnEnemyHpChanged.AddUObject(this, &ABoss::HandleHpChanged);
	}

	BossPhase = EBossPhase::Waiting;
	SetEnemyState(EEnemyState::Idle);
}

void ABoss::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	//---------------------------------------------------------------
	// 콜백을 중단하기 전에 현재 전투 무효화.
	//
	//---------------------------------------------------------------
	bLifecycleChanging = true;
	bEncounterStarted = false;
	++EncounterID;

	if (EnemyStatComponet)
	{
		EnemyStatComponet->OnEnemyHpChanged.RemoveAll(this);
	}

	StopPresentation();
	StopEncounterActions();
	ResetBossGimmicks();

	Super::EndPlay(EndPlayReason);
}

void ABoss::UpdateMovementSpeed()
{
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	if (!Movement || !EnemyStatComponet)
	{
		return;
	}

	switch (CurrentState)
	{
	case EEnemyState::Idle:
		Movement->MaxWalkSpeed = 0.0f;
		break;

	case EEnemyState::Patrol:
		Movement->MaxWalkSpeed = EnemyStatComponet->GetWalkSpeed();
		break;

	case EEnemyState::Chase:
		Movement->MaxWalkSpeed = EnemyStatComponet->GetChaseSpeed();
		break;

	case EEnemyState::Strafe:
		Movement->MaxWalkSpeed = EnemyStatComponet->GetStrafeSpeed();
		break;

	case EEnemyState::Attack:
		Movement->MaxWalkSpeed = 0.0f;
		break;

	case EEnemyState::Search:
		Movement->MaxWalkSpeed = EnemyStatComponet->GetWalkSpeed();
		break;

	case EEnemyState::Hit:
		Movement->MaxWalkSpeed = 0.0f;
		break;

	case EEnemyState::Dead:
		Movement->MaxWalkSpeed = 0.0f;
		break;

	case EEnemyState::FastChase:
		break;

	default:
		break;
	}
}

float ABoss::GetTargetSurfaceDistance(const AActor* Target) const
{
	if (!IsValid(Target))
	{
		return TNumericLimits<float>::Max();
	}

	const UCapsuleComponent* TargetCapsule=Target->FindComponentByClass<UCapsuleComponent>();
	const float TargetRadius=TargetCapsule ? TargetCapsule->GetScaledCapsuleRadius() : 0.0f;
	return FMath::Max(0.0f, FVector::Dist2D(GetCapsuleComponent()->GetComponentLocation(), TargetCapsule ? TargetCapsule->GetComponentLocation() : Target->GetActorLocation())-GetCapsuleComponent()->GetScaledCapsuleRadius()-TargetRadius);
}

const FEnemyAttackData* ABoss::GetOpeningAttackData() const
{
	return nullptr;
}

float ABoss::GetAttackRangeData()
{
	return BossAIData ? BossAIData->AttackRange : 0.0f;
}

float ABoss::GetMinMaintainDistanceData()
{
	return BossAIData ? BossAIData->MinMaintainDistance : 0.0f;
}

//---------------------------------------------------------------
// 전투 시작에 필요한 AI, 페이즈, 애니메이션 데이터 확인.
//
//---------------------------------------------------------------
bool ABoss::ValidateEncounter() const
{
	const AEnemyAIController* AI = Cast<AEnemyAIController>(GetController());

	if (!AI || !EnemyStatComponet || !AttackTraceComponent ||
		!BossAIData || !BossAIData->Phase1BT || !BossAIData->Phase2BT)
	{
		UE_LOG(LogTemp, Error, TEXT("Boss: controller, components, BossAIData and phase trees are required."));
		return false;
	}

	//---------------------------------------------------------------
	// 두 페이즈가 동일한 Blackboard를 사용하는지 확인.
	//
	//---------------------------------------------------------------
	if (!BossAIData->Phase1BT->BlackboardAsset ||
		BossAIData->Phase1BT->BlackboardAsset != BossAIData->Phase2BT->BlackboardAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("Boss: both phase trees must use the same Blackboard asset."));
		return false;
	}

	const FEnemyAttackData* OpeningAttack = GetOpeningAttackData();

	if (!OpeningAttack || !OpeningAttack->Montage ||
		!OpeningAttack->DamageInfo.IsValidIndex(0))
	{
		UE_LOG(LogTemp, Error, TEXT("Boss: opening attack montage and DamageInfo[0] are required."));
		return false;
	}

	if (!GetMesh()->GetAnimInstance())
	{
		UE_LOG(LogTemp, Error, TEXT("Boss: animation instance is required."));
		return false;
	}

	return true;
}

//---------------------------------------------------------------
// 대기 상태에서 타깃을 전달받아 보스전 시작.
//
//---------------------------------------------------------------
bool ABoss::StartEncounter(AActor* Target)
{
	if (!HasActorBegunPlay() || bLifecycleChanging || bEncounterStarted ||
		BossPhase != EBossPhase::Waiting || IsDead() || !IsValid(Target) ||
		Target == this || !ValidateEncounter())
	{
		return false;
	}

	EncounterTarget = Target;
	bEncounterStarted = true;
	++EncounterID;

	StopCombat();
	SetEnemyState(EEnemyState::Idle);

	BossPhase = EBossPhase::Intro;
	StartPresentation(EBossPresentation::Intro);

	return true;
}

//---------------------------------------------------------------
// 연출 실행 번호를 발급하고 시작 요청.
//
//---------------------------------------------------------------
void ABoss::StartPresentation(EBossPresentation Presentation)
{
	if (!bEncounterStarted || bLifecycleChanging || IsDead() ||
		Presentation == EBossPresentation::None || ActivePresentationID != INDEX_NONE)
	{
		return;
	}

	ActivePresentation = Presentation;
	ActivePresentationID = ++PresentationSerial;

	if (Presentation==EBossPresentation::Phase2Transition)
	{
		BeginPhase2Transition(ActivePresentationID);
		return;
	}
	OnPresentationStarted(Presentation, ActivePresentationID);
}

//---------------------------------------------------------------
// 정상 종료와 스킵은 같은 완료 경로 사용.
//
//---------------------------------------------------------------
void ABoss::CompletePresentation(int32 PresentationID)
{
	if (!bEncounterStarted || bLifecycleChanging || IsDead() ||
		ActivePresentationID == INDEX_NONE || ActivePresentationID != PresentationID)
	{
		return;
	}

	// 기존 공용 완료 함수를 직접 호출해도 수색/등장 단계를 건너뛸 수 없음.
	if (ActivePresentation==EBossPresentation::Phase2Transition && !bPhase2EmergenceCompleted) { return; }

	const EBossPresentation CompletedPresentation = ActivePresentation;

	//---------------------------------------------------------------
	// 정리 이벤트보다 먼저 무효화하여 중복 완료 방지.
	//
	//---------------------------------------------------------------
	ActivePresentation = EBossPresentation::None;
	ActivePresentationID = INDEX_NONE;

	bLifecycleChanging = true;
	if (CompletedPresentation==EBossPresentation::Phase2Transition) { StopPhase2Transition(false); }
	OnPresentationStopped(CompletedPresentation, PresentationID, false);
	bLifecycleChanging = false;

	//---------------------------------------------------------------
	// 정리 이벤트에서 보스가 제거되었다면 진행 중단.
	//
	//---------------------------------------------------------------
	if (IsActorBeingDestroyed() || !bEncounterStarted || IsDead())
	{
		return;
	}

	switch (CompletedPresentation)
	{
	case EBossPresentation::Intro:
		BeginOpeningAttack();
		break;

	case EBossPresentation::Phase2Transition:
		EnterCombatPhase(EBossPhase::Phase2);
		break;

	default:
		break;
	}
}

//---------------------------------------------------------------
// 연출 취소는 전투 초기화로 연결.
//
//---------------------------------------------------------------
void ABoss::CancelPresentation(int32 PresentationID)
{
	if (bLifecycleChanging || ActivePresentationID == INDEX_NONE ||
		ActivePresentationID != PresentationID)
	{
		return;
	}

	ResetEncounter();
}

//---------------------------------------------------------------
// 활성 연출을 무효화한 뒤 외부 연출 자원 정리.
//
//---------------------------------------------------------------
void ABoss::StopPresentation()
{
	if (ActivePresentationID == INDEX_NONE)
	{
		return;
	}

	const EBossPresentation StoppedPresentation = ActivePresentation;
	const int32 StoppedID = ActivePresentationID;

	//---------------------------------------------------------------
	// 중단 처리 중 도착하는 완료 알림을 무시하도록 먼저 무효화.
	//
	//---------------------------------------------------------------
	ActivePresentation = EBossPresentation::None;
	ActivePresentationID = INDEX_NONE;

	if (StoppedPresentation==EBossPresentation::Phase2Transition) { StopPhase2Transition(true); }
	OnPresentationStopped(StoppedPresentation, StoppedID, true);
}

void ABoss::OnPresentationStarted_Implementation(EBossPresentation Presentation, int32 PresentationID)
{
	//---------------------------------------------------------------
	// 연출을 연결하지 않았다면 즉시 다음 단계 진행.
	//
	//---------------------------------------------------------------
	CompletePresentation(PresentationID);
}

void ABoss::OnPresentationStopped_Implementation(EBossPresentation Presentation, int32 PresentationID, bool bCancelled)
{

}

//---------------------------------------------------------------
// 인트로 완료 후 타깃 방향으로 정렬하고 첫 공격 실행.
//
//---------------------------------------------------------------
void ABoss::BeginOpeningAttack()
{
	if (!bEncounterStarted || bLifecycleChanging || BossPhase != EBossPhase::Intro)
	{
		return;
	}

	if (!ValidateEncounter() || !EncounterTarget.IsValid())
	{
		ResetEncounter();
		return;
	}

	const FEnemyAttackData* OpeningAttack = GetOpeningAttackData();
	UAnimMontage* OpeningMontage = OpeningAttack->Montage.Get();

	BossPhase = EBossPhase::OpeningAttack;
	SetEnemyState(EEnemyState::Attack);

	FVector Direction = EncounterTarget->GetActorLocation() - GetActorLocation();
	Direction.Z = 0.0f;

	if (!Direction.IsNearlyZero())
	{
		SetActorRotation(Direction.Rotation());
	}

	UAnimInstance* Anim = GetMesh()->GetAnimInstance();

	if (Anim->Montage_Play(OpeningMontage) <= 0.0f)
	{
		UE_LOG(LogTemp, Error, TEXT("Boss: opening montage failed; continuing to phase 1."));
		EnterCombatPhase(EBossPhase::Phase1);
		return;
	}

	//---------------------------------------------------------------
	// 현재 전투 번호를 종료 콜백에 전달.
	//
	//---------------------------------------------------------------
	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &ABoss::OnOpeningEnded, EncounterID);
	Anim->Montage_SetEndDelegate(EndDelegate, OpeningMontage);
}

//---------------------------------------------------------------
// 현재 전투의 첫 공격 종료일 때만 1페이즈 시작.
//
//---------------------------------------------------------------
void ABoss::OnOpeningEnded(UAnimMontage* Montage, bool bInterrupted, int32 InEncounterID)
{
	if (!bEncounterStarted || bLifecycleChanging || InEncounterID != EncounterID ||
		BossPhase != EBossPhase::OpeningAttack)
	{
		return;
	}

	AttackTraceComponent->EndTrace();

	EnterCombatPhase(EBossPhase::Phase1);
}

//---------------------------------------------------------------
// 선택한 페이즈의 Blackboard와 BT 실행.
//
//---------------------------------------------------------------
void ABoss::EnterCombatPhase(EBossPhase NewPhase)
{
	if (!bEncounterStarted || bLifecycleChanging || IsDead() || !BossAIData)
	{
		return;
	}

	if (!EncounterTarget.IsValid())
	{
		ResetEncounter();
		return;
	}

	if (NewPhase != EBossPhase::Phase1 && NewPhase != EBossPhase::Phase2)
	{
		return;
	}

	AEnemyAIController* AI = Cast<AEnemyAIController>(GetController());
	if (!AI)
	{
		return;
	}

	StopCombat();

	UBehaviorTree* Tree = NewPhase == EBossPhase::Phase2 ? BossAIData->Phase2BT.Get() : BossAIData->Phase1BT.Get();
	UBlackboardComponent* BB = AI->GetBlackboardComponent();

	if (!Tree || !Tree->BlackboardAsset || !AI->UseBlackboard(Tree->BlackboardAsset, BB))
	{
		SetEnemyState(EEnemyState::Idle);
		UE_LOG(LogTemp, Error, TEXT("Boss: phase Blackboard initialization failed."));
		ResetEncounter();
		return;
	}

	BossPhase = NewPhase;
	// 첫 공격 실패 등 어느 경로로 1페이즈에 들어와도 현재 체력을 확인.
	if (NewPhase==EBossPhase::Phase1 && EnemyStatComponet)
	{
		HandleHpChanged(EnemyStatComponet->GetCurrentHp(),EnemyStatComponet->GetMaxHp());
		if (BossPhase!=EBossPhase::Phase1) { return; }
	}

	BB->SetValueAsObject(BBKEY_TARGETACTOR, EncounterTarget.Get());
	BB->SetValueAsVector(BBKEY_HOMELOCATION, InitialTransform.GetLocation());
	BB->SetValueAsBool(BBKEY_HASLINEOFSIGHT, false);

	SetEnemyState(EEnemyState::Chase);

	if (StartBossCombatBehavior(EncounterTarget.Get()))
	{
		return;
	}

	if (!AI->RunBehaviorTree(Tree))
	{
		UE_LOG(LogTemp, Error, TEXT("Boss: phase tree failed to run."));
		ResetEncounter();
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Boss: %s"), *Tree->GetName());
}

//---------------------------------------------------------------
// BT, 이동, 공격 판정 정리.
//
//---------------------------------------------------------------
bool ABoss::StartBossCombatBehavior(AActor* Target)
{
	return false;
}

void ABoss::StopCombat()
{
	if (AEnemyAIController* AI = Cast<AEnemyAIController>(GetController()))
	{
		if (UBehaviorTreeComponent* BT = Cast<UBehaviorTreeComponent>(AI->GetBrainComponent()))
		{
			BT->StopTree(EBTStopMode::Forced);
		}

		AI->StopMovement();
		AI->ClearFocus(EAIFocusPriority::Gameplay);
	}

	if (AttackTraceComponent)
	{
		AttackTraceComponent->EndTrace();
	}

	GetCharacterMovement()->StopMovementImmediately();
}

//---------------------------------------------------------------
// 첫 공격의 종료 콜백을 해제한 뒤 몽타주 중단.
//
//---------------------------------------------------------------
void ABoss::StopOpening()
{
	if (AttackTraceComponent)
	{
		AttackTraceComponent->EndTrace();
	}

	UAnimInstance* Anim = GetMesh()->GetAnimInstance();
	const FEnemyAttackData* OpeningAttack = GetOpeningAttackData();

	if (!Anim || !OpeningAttack || !OpeningAttack->Montage)
	{
		return;
	}

	UAnimMontage* OpeningMontage = OpeningAttack->Montage.Get();

	FOnMontageEnded EmptyDelegate;
	Anim->Montage_SetEndDelegate(EmptyDelegate, OpeningMontage);
	Anim->Montage_Stop(0.0f, OpeningMontage);
}

//---------------------------------------------------------------
// 실행 중인 전투 행동 전체 중단.
//
//---------------------------------------------------------------
void ABoss::StopEncounterActions()
{
	StopCombat();
	StopOpening();

	if (UAnimInstance* Anim = GetMesh()->GetAnimInstance())
	{
		Anim->StopAllMontages(0.0f);
	}

	//---------------------------------------------------------------
	// 몽타주 중단 시 실행되는 Notify 이후에도 판정 정리.
	//
	//---------------------------------------------------------------
	if (AttackTraceComponent)
	{
		AttackTraceComponent->EndTrace();
	}
}

//---------------------------------------------------------------
// 실패한 전투를 시작 전 상태로 복구.
//
//---------------------------------------------------------------
void ABoss::ResetEncounter()
{
	if (!HasActorBegunPlay() || IsActorBeingDestroyed() || bLifecycleChanging ||
		BossPhase == EBossPhase::Dead || !EnemyStatComponet)
	{
		return;
	}

	bLifecycleChanging = true;

	//---------------------------------------------------------------
	// 중단 콜백보다 먼저 현재 전투 무효화.
	//
	//---------------------------------------------------------------
	bEncounterStarted = false;
	++EncounterID;

	BossPhase = EBossPhase::Waiting;

	StopPresentation();
	StopEncounterActions();
	ResetBossGimmicks();

	EncounterTarget.Reset();

	//---------------------------------------------------------------
	// Blackboard에 남은 전투 대상 제거.
	//
	//---------------------------------------------------------------
	if (AEnemyAIController* AI = Cast<AEnemyAIController>(GetController()))
	{
		if (UBlackboardComponent* BB = AI->GetBlackboardComponent())
		{
			BB->ClearValue(BBKEY_TARGETACTOR);
			BB->SetValueAsBool(BBKEY_HASLINEOFSIGHT, false);
		}
	}

	//---------------------------------------------------------------
	// 체력, 누적 자세 수치, 전투 플래그 복구.
	//
	//---------------------------------------------------------------
	EnemyStatComponet->SetCurrentHp(EnemyStatComponet->GetMaxHp());
	EnemyStatComponet->SetCurrentStance(0.0f);
	EnemyStatComponet->SetPoiseFlag(false);
	EnemyStatComponet->SetStanceFlag(false);

	CurrentDamageInfo = FDamageInfo();
	bIsGuard = false;
	bIsInvincibility = false;

	SetStrafeMode(false);
	SetEnemyState(EEnemyState::Idle);

	//---------------------------------------------------------------
	// 초기 위치와 회전 복구.
	//
	//---------------------------------------------------------------
	GetCharacterMovement()->StopMovementImmediately();
	SetActorTransform(InitialTransform, false, nullptr, ETeleportType::TeleportPhysics);

	bLifecycleChanging = false;
}

//---------------------------------------------------------------
// 보스별 생성한 기믹과 타이머는 자식 클래스에서 정리.
//
//---------------------------------------------------------------
void ABoss::ResetBossGimmicks()
{

}

//---------------------------------------------------------------
// 사망 우선 처리 후 페이즈 전환 조건 확인.
//
//---------------------------------------------------------------
void ABoss::HandleHpChanged(float CurrentHp, float MaxHp)
{
	if (bLifecycleChanging || !bEncounterStarted || BossPhase == EBossPhase::Dead)
	{
		return;
	}

	if (CurrentHp <= 0.0f)
	{
		BossPhase = EBossPhase::Dead;
		bEncounterStarted = false;
		++EncounterID;

		SetEnemyState(EEnemyState::Dead);

		bLifecycleChanging = true;

		StopPresentation();
		StopEncounterActions();
		ResetBossGimmicks();

		bLifecycleChanging = false;
		return;
	}

	//---------------------------------------------------------------
	// 1페이즈 공격을 중단하고 전환 연출 시작.
	//
	//---------------------------------------------------------------
	if (BossPhase == EBossPhase::Phase1 && BossAIData && MaxHp > 0.0f &&
		CurrentHp / MaxHp <= FMath::Clamp(BossAIData->Phase2HpRatio, 0.01f, 0.99f))
	{
		BossPhase = EBossPhase::PhaseTransition;

		bLifecycleChanging = true;

		StopEncounterActions();
		ResetBossGimmicks();
		SetEnemyState(EEnemyState::Idle);

		bLifecycleChanging = false;

		StartPresentation(EBossPresentation::Phase2Transition);
	}
}

void ABoss::ReceiveDamage(const FDamageInfo& DamageInfo, const FHitResult& Hit)
{
	//---------------------------------------------------------------
	// 대기, 연출, 초기화, 사망 상태에서는 피해 차단.
	//
	//---------------------------------------------------------------
	if (!bEncounterStarted || bLifecycleChanging || IsDead() ||
		BossPhase == EBossPhase::Waiting || BossPhase == EBossPhase::Intro ||
		BossPhase == EBossPhase::PhaseTransition)
	{
		return;
	}

	Super::ReceiveDamage(DamageInfo, Hit);
}

void ABoss::HandleAttackTraceHit(const FHitResult& Hit)
{
	//---------------------------------------------------------------
	// 공통 보스에서는 첫 공격 피해만 처리.
	// 일반 전투 공격은 보스 자식 클래스에서 처리.
	//
	//---------------------------------------------------------------
	if (!bEncounterStarted || bLifecycleChanging || IsDead() || BossPhase != EBossPhase::OpeningAttack)
	{
		return;
	}

	const FEnemyAttackData* OpeningAttack = GetOpeningAttackData();

	if (!OpeningAttack || !OpeningAttack->DamageInfo.IsValidIndex(0))
	{
		return;
	}

	if (ICombatDamageInterface* Target = Cast<ICombatDamageInterface>(Hit.GetActor()))
	{
		Target->ReceiveDamage(OpeningAttack->DamageInfo[0], Hit);
	}
}

void ABoss::InitializeWeapon()
{
	Super::InitializeWeapon();

	//---------------------------------------------------------------
	// 맨손 공격은 본체 메시의 TraceStart와 TraceEnd 소켓 사용.
	//
	//---------------------------------------------------------------
	if (!WeaponMesh && AttackTraceComponent)
	{
		AttackTraceComponent->SetTraceMesh(GetMesh());
	}
}

void ABoss::BeginPhase2Transition(int32 TransitionID)
{
	Phase2TransitionID=TransitionID;
	bPhase2EmergenceCompleted=false;
	bPhase2SavedHidden=IsHidden();
	bPhase2SavedCollision=GetActorEnableCollision();
	UCharacterMovementComponent* Movement=GetCharacterMovement();
	Phase2SavedMovementMode=Movement->MovementMode;
	Phase2SavedCustomMovementMode=Movement->CustomMovementMode;
	bPhase2StateSaved=true;
	Movement->DisableMovement();
	SetPhase2TransitionStage(EBossPhase2TransitionStage::Disappearing);
}

bool ABoss::IsPhase2TransitionStep(int32 TransitionID, EBossPhase2TransitionStage Stage) const
{
	return TransitionID!=INDEX_NONE && !bLifecycleChanging && bEncounterStarted && !IsDead() && BossPhase==EBossPhase::PhaseTransition
		&& ActivePresentation==EBossPresentation::Phase2Transition && ActivePresentationID==TransitionID && Phase2TransitionID==TransitionID && Phase2TransitionStage==Stage;
}

void ABoss::SetPhase2TransitionStage(EBossPhase2TransitionStage Stage)
{
	GetWorldTimerManager().ClearTimer(Phase2StageTimer);
	Phase2TransitionStage=Stage;
	const int32 TransitionID=Phase2TransitionID;
	// 수색은 플레이어의 발견 트리거를 기다리고 시간으로 완료하지 않음.
	if (bUsePhase2PlaceholderTimings && Stage!=EBossPhase2TransitionStage::Searching)
	{
		const float Delay=Stage==EBossPhase2TransitionStage::Disappearing ? Phase2DisappearPlaceholderTime : Phase2EmergencePlaceholderTime;
		FTimerDelegate NextStage=FTimerDelegate::CreateWeakLambda(this,[this,TransitionID,Stage]()
		{
			switch (Stage)
			{
			case EBossPhase2TransitionStage::Disappearing: CompletePhase2Disappear(TransitionID); break;
			case EBossPhase2TransitionStage::Emerging: CompletePhase2Emergence(TransitionID); break;
			default: break;
			}
		});
		// 이벤트가 동기적으로 다음 단계를 완료해도 이전 단계 타이머가 남지 않도록 먼저 예약.
		GetWorldTimerManager().SetTimer(Phase2StageTimer,NextStage,FMath::Max(Delay,0.01f),false);
	}
	UE_LOG(LogTemp,Log,TEXT("Boss phase2 transition %d: stage %d"),TransitionID,static_cast<int32>(Stage));
	OnPhase2StageRequested.Broadcast(this,Stage,TransitionID);
}

bool ABoss::CompletePhase2Disappear(int32 TransitionID)
{
	if (!IsPhase2TransitionStep(TransitionID,EBossPhase2TransitionStage::Disappearing)) { return false; }
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetPhase2TransitionStage(EBossPhase2TransitionStage::Searching);
	return true;
}

bool ABoss::NotifyPhase2Found(AActor* Finder, int32 TransitionID, AActor* EmergencePoint)
{
	if (!IsPhase2TransitionStep(TransitionID,EBossPhase2TransitionStage::Searching) || !IsValid(Finder) || Finder!=EncounterTarget.Get()) { return false; }
	AActor* Point=IsValid(EmergencePoint) ? EmergencePoint : Phase2EmergencePoint.Get();
	if (IsValid(Point))
	{
		// 마커의 스케일은 사용하지 않아 보스 크기는 그대로 유지.
		SetActorLocationAndRotation(Point->GetActorLocation(),Point->GetActorRotation(),false,nullptr,ETeleportType::TeleportPhysics);
	}
	SetActorHiddenInGame(bPhase2SavedHidden);
	SetPhase2TransitionStage(EBossPhase2TransitionStage::Emerging);
	return true;
}

bool ABoss::CompletePhase2Emergence(int32 TransitionID)
{
	if (!IsPhase2TransitionStep(TransitionID,EBossPhase2TransitionStage::Emerging)) { return false; }
	bPhase2EmergenceCompleted=true;
	CompletePresentation(TransitionID);
	return BossPhase==EBossPhase::Phase2;
}

void ABoss::StopPhase2Transition(bool bCancelled)
{
	GetWorldTimerManager().ClearTimer(Phase2StageTimer);
	const int32 StoppedID=Phase2TransitionID;
	Phase2TransitionID=INDEX_NONE;
	Phase2TransitionStage=EBossPhase2TransitionStage::None;
	bPhase2EmergenceCompleted=false;
	if (bPhase2StateSaved)
	{
		bPhase2StateSaved=false;
		if (!IsActorBeingDestroyed())
		{
			SetActorHiddenInGame(bPhase2SavedHidden);
			if (!IsDead())
			{
				SetActorEnableCollision(bPhase2SavedCollision);
				GetCharacterMovement()->SetMovementMode(static_cast<EMovementMode>(Phase2SavedMovementMode),Phase2SavedCustomMovementMode);
			}
		}
	}
	if (StoppedID!=INDEX_NONE) { OnPhase2TransitionStopped.Broadcast(this,StoppedID,bCancelled); }
}
