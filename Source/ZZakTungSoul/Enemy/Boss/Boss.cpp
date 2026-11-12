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
	// 보스 공통 AIController 설정.
	AIControllerClass = ABossAIController::StaticClass();

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ABoss::BeginPlay()
{
	Super::BeginPlay();

	// 재도전 시 복구할 시작 위치 저장.
	InitialTransform = GetActorTransform();

	if (EnemyStatComponet)
	{
		EnemyStatComponet->OnEnemyHpChanged.AddUObject(this, &ABoss::TryStartPhase2Transition);
	}

	BossPhase = EBossPhase::Waiting;
	SetEnemyState(EEnemyState::Idle);
}

void ABoss::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 콜백을 중단하기 전에 현재 전투 무효화.
	bLifecycleChanging = true;
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
	case EEnemyState::Groggy:
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

	const UCapsuleComponent* TargetCapsule = Target->FindComponentByClass<UCapsuleComponent>();
	const float TargetRadius = TargetCapsule ? TargetCapsule->GetScaledCapsuleRadius() : 0.0f;

	return FMath::Max(0.0f,
		FVector::Dist2D(GetCapsuleComponent()->GetComponentLocation(),
			TargetCapsule ? TargetCapsule->GetComponentLocation() : Target->GetActorLocation()) -
			GetCapsuleComponent()->GetScaledCapsuleRadius() - TargetRadius);
}

const FEnemyAttackData* ABoss::GetOpeningAttackData(EBossPhase) const
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

// 전투 시작에 필요한 AI, 페이즈, 애니메이션 데이터 확인.
bool ABoss::ValidateEncounter() const
{
	const AEnemyAIController* AI = Cast<AEnemyAIController>(GetController());

	if (!AI || !EnemyStatComponet || !AttackTraceComponent || !BossAIData || !BossAIData->Phase1BT ||
		!BossAIData->Phase2BT)
	{
		UE_LOG(LogTemp, Error, TEXT("Boss: controller, components, BossAIData and phase trees are required."));

		return false;
	}

	// 두 페이즈가 동일한 Blackboard를 사용하는지 확인.
	if (!BossAIData->Phase1BT->BlackboardAsset ||
		BossAIData->Phase1BT->BlackboardAsset != BossAIData->Phase2BT->BlackboardAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("Boss: both phase trees must use the same Blackboard asset."));

		return false;
	}

	const FEnemyAttackData* OpeningAttack = GetOpeningAttackData(EBossPhase::Phase1);

	if (!OpeningAttack || !OpeningAttack->Montage || !OpeningAttack->DamageInfo.IsValidIndex(0))
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

// 대기 상태에서 타깃을 전달받아 보스전 시작.
bool ABoss::StartEncounter(AActor* Target)
{
	if (!HasActorBegunPlay() || bLifecycleChanging || BossPhase != EBossPhase::Waiting ||
		IsDead() || !IsValid(Target) || Target == this || !ValidateEncounter())
	{
		return false;
	}

	EncounterTarget = Target;
	BossPhase = EBossPhase::Intro;
	++EncounterID;

	StopCombat();
	SetEnemyState(EEnemyState::Idle);

	StartPresentation(EBossPresentation::Intro);

	return true;
}

// 연출 실행 번호를 발급하고 시작 요청.
void ABoss::StartPresentation(EBossPresentation Presentation)
{
	if (bLifecycleChanging || IsDead() || Presentation == EBossPresentation::None ||
		ActivePresentationID != INDEX_NONE)
	{
		return;
	}

	ActivePresentation = Presentation;
	ActivePresentationID = ++PresentationSerial;

	if (Presentation == EBossPresentation::Phase2Transition)
	{
		BeginPhase2Transition();

		return;
	}

	OnPresentationStarted(Presentation, ActivePresentationID);
}

// 정상 종료와 스킵은 같은 완료 경로 사용.
void ABoss::CompletePresentation(int32 PresentationID)
{
	// 2페이즈는 등장 완료 신호를 받아야만 전투에 진입한다.
	if (ActivePresentation == EBossPresentation::Intro)
	{
		FinishPresentation(PresentationID);
	}
}

void ABoss::FinishPresentation(int32 PresentationID)
{
	if (bLifecycleChanging || IsDead() || ActivePresentationID == INDEX_NONE || ActivePresentationID != PresentationID)
	{
		return;
	}

	const EBossPresentation CompletedPresentation = ActivePresentation;
	bLifecycleChanging = true;
	StopPresentation(false);
	bLifecycleChanging = false;

	if (IsActorBeingDestroyed() || BossPhase == EBossPhase::Waiting || IsDead())
	{
		return;
	}

	switch (CompletedPresentation)
	{
	case EBossPresentation::Intro:
		BeginOpeningAttack(EBossPhase::Phase1);
		break;
	case EBossPresentation::Phase2Transition:
		BeginOpeningAttack(EBossPhase::Phase2);
		break;
	default:
		break;
	}
}

// 연출 취소는 전투 초기화로 연결.
void ABoss::CancelPresentation(int32 PresentationID)
{
	if (bLifecycleChanging || ActivePresentationID == INDEX_NONE || ActivePresentationID != PresentationID)
	{
		return;
	}

	ResetEncounter();
}

// 활성 연출을 무효화한 뒤 외부 연출 자원 정리.
void ABoss::StopPresentation(bool bCancelled)
{
	if (ActivePresentationID == INDEX_NONE)
	{
		return;
	}

	const EBossPresentation StoppedPresentation = ActivePresentation;
	const int32 StoppedID = ActivePresentationID;

	// 중단 처리 중 도착하는 완료 알림을 무시하도록 먼저 무효화.
	ActivePresentation = EBossPresentation::None;
	ActivePresentationID = INDEX_NONE;

	if (StoppedPresentation == EBossPresentation::Phase2Transition)
	{
		StopPhase2Transition(StoppedID, bCancelled);
	}

	OnPresentationStopped(StoppedPresentation, StoppedID, bCancelled);
}

void ABoss::OnPresentationStarted_Implementation(EBossPresentation Presentation, int32 PresentationID)
{
	// 연출을 연결하지 않았다면 즉시 다음 단계 진행.
	CompletePresentation(PresentationID);
}

void ABoss::OnPresentationStopped_Implementation(EBossPresentation Presentation, int32 PresentationID, bool bCancelled)
{
}

void ABoss::BeginOpeningAttack(EBossPhase CombatPhase)
{
	const EBossPhase ExpectedPhase = CombatPhase == EBossPhase::Phase2 ? EBossPhase::PhaseTransition : EBossPhase::Intro;
	if (bLifecycleChanging || BossPhase != ExpectedPhase)
	{
		return;
	}

	if (!ValidateEncounter() || !EncounterTarget.IsValid())
	{
		ResetEncounter();

		return;
	}

	const FEnemyAttackData* OpeningAttack = GetOpeningAttackData(CombatPhase);
	if (!OpeningAttack || !OpeningAttack->Montage)
	{
		EnterCombatPhase(CombatPhase);
		return;
	}
	if (!OpeningAttack->DamageInfo.IsValidIndex(0))
	{
		UE_LOG(LogTemp, Warning, TEXT("Boss: phase opening attack is missing DamageInfo[0]."));
		EnterCombatPhase(CombatPhase);
		return;
	}

	UAnimMontage* OpeningMontage = OpeningAttack->Montage.Get();
	OpeningCombatPhase = CombatPhase;
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
		UE_LOG(LogTemp, Warning, TEXT("Boss: opening montage failed; starting the phase behavior tree."));
		EnterCombatPhase(CombatPhase);

		return;
	}

	// 현재 전투 번호를 종료 콜백에 전달.
	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &ABoss::OnOpeningEnded, EncounterID);
	Anim->Montage_SetEndDelegate(EndDelegate, OpeningMontage);
}

// 오프닝 공격이 끝나면 해당 페이즈의 BT를 시작.
void ABoss::OnOpeningEnded(UAnimMontage* Montage, bool bInterrupted, int32 InEncounterID)
{
	if (bLifecycleChanging || InEncounterID != EncounterID ||
		BossPhase != EBossPhase::OpeningAttack)
	{
		return;
	}

	AttackTraceComponent->EndTrace();

	EnterCombatPhase(OpeningCombatPhase);
}

// 선택한 페이즈의 Blackboard와 BT 실행.
void ABoss::EnterCombatPhase(EBossPhase NewPhase)
{
	if (bLifecycleChanging || BossPhase == EBossPhase::Waiting || IsDead() || !BossAIData)
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
	if (NewPhase == EBossPhase::Phase1 && EnemyStatComponet)
	{
		TryStartPhase2Transition(EnemyStatComponet->GetCurrentHp(), EnemyStatComponet->GetMaxHp());

		if (BossPhase != EBossPhase::Phase1)
		{
			return;
		}
	}

	BB->SetValueAsObject(BBKEY_TARGETACTOR, EncounterTarget.Get());
	BB->SetValueAsVector(BBKEY_HOMELOCATION, InitialTransform.GetLocation());
	BB->SetValueAsBool(BBKEY_HASLINEOFSIGHT, false);

	SetEnemyState(EEnemyState::Chase);

	if (!AI->RunBehaviorTree(Tree))
	{
		UE_LOG(LogTemp, Error, TEXT("Boss: phase tree failed to run."));
		ResetEncounter();

		return;
	}
}

// BT, 이동, 공격 판정 정리.
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

// 실행 중인 전투 행동 전체 중단.
void ABoss::StopEncounterActions()
{
	StopCombat();

	if (UAnimInstance* Anim = GetMesh()->GetAnimInstance())
	{
		// 첫 공격 종료 콜백을 해제한 뒤 몽타주 중단.
		const FEnemyAttackData* OpeningAttack = GetOpeningAttackData(OpeningCombatPhase);
		if (OpeningAttack && OpeningAttack->Montage)
		{
			FOnMontageEnded EmptyDelegate;
			Anim->Montage_SetEndDelegate(EmptyDelegate, OpeningAttack->Montage.Get());
		}
		Anim->StopAllMontages(0.0f);
	}

	// 몽타주 중단 시 실행되는 Notify 이후에도 판정 정리.
	if (AttackTraceComponent)
	{
		AttackTraceComponent->EndTrace();
	}
}

// 실패한 전투를 시작 전 상태로 복구.
void ABoss::ResetEncounter()
{
	if (!HasActorBegunPlay() || IsActorBeingDestroyed() || bLifecycleChanging || BossPhase == EBossPhase::Dead ||
		!EnemyStatComponet)
	{
		return;
	}

	bLifecycleChanging = true;

	// 중단 콜백보다 먼저 현재 전투 무효화.
	++EncounterID;

	BossPhase = EBossPhase::Waiting;

	StopPresentation();
	StopEncounterActions();
	ResetBossGimmicks();

	EncounterTarget.Reset();

	// Blackboard에 남은 전투 대상 제거.
	if (AEnemyAIController* AI = Cast<AEnemyAIController>(GetController()))
	{
		if (UBlackboardComponent* BB = AI->GetBlackboardComponent())
		{
			BB->ClearValue(BBKEY_TARGETACTOR);
			BB->SetValueAsBool(BBKEY_HASLINEOFSIGHT, false);
		}
	}

	// 체력, 누적 자세 수치, 전투 플래그 복구.
	EnemyStatComponet->SetCurrentHp(EnemyStatComponet->GetMaxHp());
	EnemyStatComponet->SetCurrentStance(0.0f);
	EnemyStatComponet->SetPoiseFlag(false);
	EnemyStatComponet->SetStanceFlag(false);

	CurrentDamageInfo = FDamageInfo();
	bIsGuard = false;
	bIsInvincibility = false;

	SetStrafeMode(false);
	SetEnemyState(EEnemyState::Idle);

	// 초기 위치와 회전 복구.
	GetCharacterMovement()->StopMovementImmediately();
	SetActorTransform(InitialTransform, false, nullptr, ETeleportType::TeleportPhysics);

	bLifecycleChanging = false;
}

// 보스별 생성한 기믹과 타이머는 자식 클래스에서 정리.
void ABoss::ResetBossGimmicks()
{
}

void ABoss::SetDead()
{
	if (BossPhase == EBossPhase::Dead)
	{
		return;
	}

	BossPhase = EBossPhase::Dead;
	++EncounterID;
	bLifecycleChanging = true;

	StopPresentation();
	StopEncounterActions();
	ResetBossGimmicks();
	SetActorEnableCollision(false);
	Super::SetDead();

	bLifecycleChanging = false;
}

// HP 0은 EnemyBase에 연결된 사망 이벤트에서 처리한다.
void ABoss::TryStartPhase2Transition(float CurrentHp, float MaxHp)
{
	if (bLifecycleChanging || BossPhase != EBossPhase::Phase1 || !BossAIData || CurrentHp <= 0.0f || MaxHp <= 0.0f)
	{
		return;
	}

	const float Phase2Ratio = FMath::Clamp(BossAIData->Phase2HpRatio, 0.01f, 0.99f);

	if (CurrentHp / MaxHp > Phase2Ratio)
	{
		return;
	}

	BossPhase = EBossPhase::PhaseTransition;
	bLifecycleChanging = true;
	StopEncounterActions();
	ResetBossGimmicks();
	SetEnemyState(EEnemyState::Idle);
	bLifecycleChanging = false;
	StartPresentation(EBossPresentation::Phase2Transition);
}

void ABoss::ReceiveDamage(const FDamageInfo& DamageInfo, const FHitResult& Hit)
{
	// 대기, 연출, 초기화, 사망 상태에서는 피해 차단.
	if (bLifecycleChanging || (BossPhase != EBossPhase::OpeningAttack &&
		BossPhase != EBossPhase::Phase1 && BossPhase != EBossPhase::Phase2))
	{
		return;
	}

	Super::ReceiveDamage(DamageInfo, Hit);
}

void ABoss::HandleAttackTraceHit(const FHitResult& Hit)
{
	// 공통 보스에서는 첫 공격 피해만 처리.
	// 일반 전투 공격은 보스 자식 클래스에서 처리.
	if (bLifecycleChanging || IsDead() || BossPhase != EBossPhase::OpeningAttack)
	{
		return;
	}

	const FEnemyAttackData* OpeningAttack = GetOpeningAttackData(OpeningCombatPhase);

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

	// 맨손 공격은 본체 메시의 TraceStart와 TraceEnd 소켓 사용.
	if (!WeaponMesh && AttackTraceComponent)
	{
		AttackTraceComponent->SetTraceMesh(GetMesh());
	}
}

void ABoss::BeginPhase2Transition()
{
	const int32 TransitionID = ActivePresentationID;
	bPhase2SavedHidden = IsHidden();
	bPhase2SavedCollision = GetActorEnableCollision();
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	Phase2SavedMovementMode = Movement->MovementMode;
	Phase2SavedCustomMovementMode = Movement->CustomMovementMode;
	bPhase2StateSaved = true;
	Movement->DisableMovement();
	if (ActivePresentationID == TransitionID)
	{
		SetPhase2TransitionStage(EBossPhase2TransitionStage::Disappearing);
	}
}

bool ABoss::IsPhase2TransitionStep(int32 TransitionID, EBossPhase2TransitionStage Stage) const
{
	return TransitionID != INDEX_NONE && !bLifecycleChanging && !IsDead() &&
		BossPhase == EBossPhase::PhaseTransition && ActivePresentation == EBossPresentation::Phase2Transition &&
		ActivePresentationID == TransitionID && Phase2TransitionStage == Stage;
}

void ABoss::SetPhase2TransitionStage(EBossPhase2TransitionStage Stage)
{
	GetWorldTimerManager().ClearTimer(Phase2StageTimer);
	Phase2TransitionStage = Stage;
	const int32 TransitionID = ActivePresentationID;

	// 수색은 플레이어의 발견 트리거를 기다리고 시간으로 완료하지 않음.
	if (bUsePhase2PlaceholderTimings && Stage != EBossPhase2TransitionStage::Searching)
	{
		const float Delay = Stage == EBossPhase2TransitionStage::Disappearing ? Phase2DisappearPlaceholderTime : Phase2EmergencePlaceholderTime;
		FTimerDelegate NextStage = FTimerDelegate::CreateWeakLambda(this,
			[this, TransitionID, Stage]()
			{
				if (Stage == EBossPhase2TransitionStage::Disappearing)
				{
					CompletePhase2Disappear(TransitionID);
				}
				else if (Stage == EBossPhase2TransitionStage::Emerging)
				{
					CompletePhase2Emergence(TransitionID);
				}
			});

		// 이벤트가 동기적으로 다음 단계를 완료해도 이전 단계 타이머가 남지 않도록 먼저 예약.
		GetWorldTimerManager().SetTimer(Phase2StageTimer, NextStage, FMath::Max(Delay, 0.01f), false);
	}

	OnPhase2StageRequested.Broadcast(this, Stage, TransitionID);
}

bool ABoss::CompletePhase2Disappear(int32 TransitionID)
{
	if (!IsPhase2TransitionStep(TransitionID, EBossPhase2TransitionStage::Disappearing))
	{
		return false;
	}

	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetPhase2TransitionStage(EBossPhase2TransitionStage::Searching);

	return true;
}

bool ABoss::NotifyPhase2Found(AActor* Finder, int32 TransitionID, AActor* EmergencePoint)
{
	if (!IsPhase2TransitionStep(TransitionID, EBossPhase2TransitionStage::Searching) || !IsValid(Finder) ||
		Finder != EncounterTarget.Get())
	{
		return false;
	}

	AActor* Point = IsValid(EmergencePoint) ? EmergencePoint : Phase2EmergencePoint.Get();

	if (IsValid(Point))
	{
		// 마커의 스케일은 사용하지 않아 보스 크기는 그대로 유지.
		SetActorLocationAndRotation(
			Point->GetActorLocation(), Point->GetActorRotation(), false, nullptr, ETeleportType::TeleportPhysics);
	}

	SetActorHiddenInGame(bPhase2SavedHidden);
	SetPhase2TransitionStage(EBossPhase2TransitionStage::Emerging);

	return true;
}

bool ABoss::CompletePhase2Emergence(int32 TransitionID)
{
	if (!IsPhase2TransitionStep(TransitionID, EBossPhase2TransitionStage::Emerging))
	{
		return false;
	}

	FinishPresentation(TransitionID);

	return BossPhase == EBossPhase::Phase2 || (BossPhase == EBossPhase::OpeningAttack && OpeningCombatPhase == EBossPhase::Phase2);
}

void ABoss::StopPhase2Transition(int32 TransitionID, bool bCancelled)
{
	GetWorldTimerManager().ClearTimer(Phase2StageTimer);
	Phase2TransitionStage = EBossPhase2TransitionStage::None;

	if (bPhase2StateSaved)
	{
		bPhase2StateSaved = false;

		if (!IsActorBeingDestroyed())
		{
			SetActorHiddenInGame(bPhase2SavedHidden);

			if (!IsDead())
			{
				SetActorEnableCollision(bPhase2SavedCollision);
				GetCharacterMovement()->SetMovementMode(
					static_cast<EMovementMode>(Phase2SavedMovementMode), Phase2SavedCustomMovementMode);
			}
		}
	}

	OnPhase2TransitionStopped.Broadcast(this, TransitionID, bCancelled);
}
