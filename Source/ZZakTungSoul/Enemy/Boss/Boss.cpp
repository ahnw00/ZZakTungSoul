#include <Enemy/Boss/Boss.h>
#include <Enemy/Boss/AI/BossAIController.h>
#include <Enemy/Boss/Data/BossAIData.h>
#include <Enemy/Boss/Data/BossAnimationData.h>
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

const UBossAnimationData* ABoss::GetBossAnimationData() const
{
	return Cast<UBossAnimationData>(EnemyAnimationData.Get());
}

float ABoss::GetCombatRangeData()
{
	return BossAIData ? BossAIData->CombatRange : 0.0f;
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

	const UBossAnimationData* AnimationData = GetBossAnimationData();

	if (!AnimationData || !AnimationData->OpeningAttack.Montage ||
		!AnimationData->OpeningAttack.DamageInfo.IsValidIndex(0))
	{
		UE_LOG(LogTemp, Error, TEXT("Boss: BossAnimationData, opening montage and DamageInfo[0] are required."));
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

	const EBossPresentation CompletedPresentation = ActivePresentation;

	//---------------------------------------------------------------
	// 정리 이벤트보다 먼저 무효화하여 중복 완료 방지.
	//
	//---------------------------------------------------------------
	ActivePresentation = EBossPresentation::None;
	ActivePresentationID = INDEX_NONE;

	bLifecycleChanging = true;
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

	const UBossAnimationData* AnimationData = GetBossAnimationData();
	UAnimMontage* OpeningMontage = AnimationData->OpeningAttack.Montage.Get();

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
	HandleHpChanged(EnemyStatComponet->GetCurrentHp(), EnemyStatComponet->GetMaxHp());
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

	UE_LOG(LogTemp, Log, TEXT("Boss: %s"), *Tree->GetName());
}

//---------------------------------------------------------------
// BT, 이동, 공격 판정 정리.
//
//---------------------------------------------------------------
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
	const UBossAnimationData* AnimationData = GetBossAnimationData();

	if (!Anim || !AnimationData || !AnimationData->OpeningAttack.Montage)
	{
		return;
	}

	UAnimMontage* OpeningMontage = AnimationData->OpeningAttack.Montage.Get();

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
	// 전투가 아닌 상태에서는 공격 피해 차단.
	//
	//---------------------------------------------------------------
	if (!bEncounterStarted || bLifecycleChanging || IsDead() ||
		BossPhase == EBossPhase::Waiting || BossPhase == EBossPhase::Intro ||
		BossPhase == EBossPhase::PhaseTransition)
	{
		return;
	}

	if (BossPhase != EBossPhase::OpeningAttack)
	{
		Super::HandleAttackTraceHit(Hit);
		return;
	}

	const UBossAnimationData* AnimationData = GetBossAnimationData();

	if (!AnimationData || !AnimationData->OpeningAttack.DamageInfo.IsValidIndex(0))
	{
		return;
	}

	//---------------------------------------------------------------
	// 첫 공격은 보스 애니메이션 데이터의 데미지 정보 사용.
	//
	//---------------------------------------------------------------
	if (ICombatDamageInterface* Target = Cast<ICombatDamageInterface>(Hit.GetActor()))
	{
		Target->ReceiveDamage(AnimationData->OpeningAttack.DamageInfo[0], Hit);
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