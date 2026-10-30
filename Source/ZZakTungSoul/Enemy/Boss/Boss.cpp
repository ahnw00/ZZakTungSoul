#include "Enemy/Boss/Boss.h"
#include "Enemy/Boss/Data/BossAIData.h"
#include "Enemy/Components/EnemyStatComponent.h"
#include "Enemy/Components/AttackTraceComponent.h"
#include "Enemy/AI/EnemyAIBlackboardKey.h"
#include "Enemy/EnemyAIController.h"
#include "Interface/CombatDamageInterface.h"
#include "Data/DamageInfo.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UObject/ConstructorHelpers.h"

ABoss::ABoss()
{
	// Mesh setting
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletonMeshRef(
		TEXT("/Game/Assets/Clown_monster/Meshes/base_mesh/SKM_base_mesh.SKM_base_mesh")
	);

	if (SkeletonMeshRef.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SkeletonMeshRef.Object);

		GetMesh()->SetRelativeLocation(FVector(0, 0, -90));
		GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));

		GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		static ConstructorHelpers::FClassFinder<UAnimInstance> AnimClassRef(
			TEXT("/Game/Blueprints/Enemy/Boss/Anim/ABP_Boss.ABP_Boss_C")
		);

		if (AnimClassRef.Succeeded())
		{
			GetMesh()->SetAnimInstanceClass(AnimClassRef.Class);
		}
	
	}
}


//---------------------------------------------------------------
// 기존 초기화를 실행하고 HP 변경 이벤트 연결.
//
//---------------------------------------------------------------
void ABoss::BeginPlay()
{
    Super::BeginPlay();
    EnemyStatComponet->OnEnemyHpChanged.AddUObject(this, &ABoss::HandleHpChanged);
    SetEnemyState(EEnemyState::Idle);
}

//---------------------------------------------------------------
// 전투 시작에 필요한 에셋과 공통 Blackboard 설정 확인.
//
//---------------------------------------------------------------
bool ABoss::ValidateEncounter() const
{
    const AEnemyAIController* AI = Cast<AEnemyAIController>(GetController());

    if (!AI || !BossAIData || !BossAIData->Phase1BT || !BossAIData->Phase2BT ||
        !BossAIData->OpeningAttackMontage || !GetMesh()->GetAnimInstance())
    {
        UE_LOG(LogTemp, Error, TEXT("Boss: controller, phase trees, opening montage and AnimBP are required."));
        return false;
    }
    if (!BossAIData->Phase1BT->BlackboardAsset ||
        BossAIData->Phase1BT->BlackboardAsset != BossAIData->Phase2BT->BlackboardAsset)
    {
        UE_LOG(LogTemp, Error, TEXT("Boss: both phase trees must use the same Blackboard asset."));
        return false;
    }
    return true;
}

//---------------------------------------------------------------
// 전투 시작을 한 번만 허용하고 컷씬 시작 이벤트 호출.
//
//---------------------------------------------------------------
bool ABoss::StartEncounter(AActor* Target)
{
    if (!HasActorBegunPlay() || bEncounterStarted || IsDead() || !IsValid(Target) ||
        Target == this || !ValidateEncounter())
    {
        return false;
    }

    EncounterTarget = Target;
    bEncounterStarted = true;
    StopCombat();
    SetEnemyState(EEnemyState::Idle);
    OnIntroStarted();
    return true;
}

//---------------------------------------------------------------
// 컷씬 종료 후 타깃 방향으로 정렬하고 폭주 공격 실행.
//
//---------------------------------------------------------------
void ABoss::FinishIntro()
{
    if (!bEncounterStarted || BossPhase != EBossPhase::Intro)
    {
        return;
    }

    if (!ValidateEncounter() || !EncounterTarget.IsValid())
    {
        return;
    }

    BossPhase = EBossPhase::OpeningAttack;
    SetEnemyState(EEnemyState::Attack);
    FVector Direction = EncounterTarget->GetActorLocation() - GetActorLocation();
    Direction.Z = 0.0f;

    if (!Direction.IsNearlyZero())
    {
        SetActorRotation(Direction.Rotation());
    }

    UAnimInstance* Anim = GetMesh()->GetAnimInstance();

    if (Anim->Montage_Play(BossAIData->OpeningAttackMontage) <= 0.0f)
    {
        UE_LOG(LogTemp, Error, TEXT("Boss: opening montage failed; continuing to phase 1."));
        EnterCombatPhase(EBossPhase::Phase1);
        return;
    }

    FOnMontageEnded EndDelegate;
    EndDelegate.BindUObject(this, &ABoss::OnOpeningEnded);
    Anim->Montage_SetEndDelegate(EndDelegate, BossAIData->OpeningAttackMontage);
}

//---------------------------------------------------------------
// 폭주 공격 종료 후 전투를 시작하고 현재 HP 재확인.
//
//---------------------------------------------------------------
void ABoss::OnOpeningEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (BossPhase != EBossPhase::OpeningAttack)
    {
        return;
    }
    AttackTraceComponent->EndTrace();
    //---------------------------------------------------------------
    // 폭주 몽타주가 중단된 경우에도 전투 단계로 진행.
    //
    //---------------------------------------------------------------
    EnterCombatPhase(EBossPhase::Phase1);
    HandleHpChanged(EnemyStatComponet->GetCurrentHp(), EnemyStatComponet->GetMaxHp());
}

//---------------------------------------------------------------
// 기존 BT를 중단하고 선택한 페이즈의 BT 실행.
//
//---------------------------------------------------------------
void ABoss::EnterCombatPhase(EBossPhase NewPhase)
{
    if (IsDead() || !BossAIData)
    {
        return;
    }
    AEnemyAIController* AI = Cast<AEnemyAIController>(GetController());

    if (!AI)
    {
        return;
    }
    StopCombat();
    BossPhase = NewPhase;
    UBehaviorTree* Tree = NewPhase == EBossPhase::Phase2 ? BossAIData->Phase2BT.Get() : BossAIData->Phase1BT.Get();
    UBlackboardComponent* BB = AI->GetBlackboardComponent();


    UE_LOG(LogTemp, Log, TEXT("Boss: %s"), *Tree->GetName());

    if (!Tree || !Tree->BlackboardAsset || !AI->UseBlackboard(Tree->BlackboardAsset, BB))
    {
        SetEnemyState(EEnemyState::Idle);
        UE_LOG(LogTemp, Error, TEXT("Boss: phase Blackboard initialization failed."));
        return;
    }
    BB->SetValueAsObject(BBKEY_TARGETACTOR, EncounterTarget.Get());
    BB->SetValueAsVector(BBKEY_HOMELOCATION, GetActorLocation());
    BB->SetValueAsBool(BBKEY_HASLINEOFSIGHT, false);
    SetEnemyState(EEnemyState::Chase);

    if (!AI->RunBehaviorTree(Tree))
    {
        SetEnemyState(EEnemyState::Idle);
        UE_LOG(LogTemp, Error, TEXT("Boss: phase tree failed to run."));
    }
}

//---------------------------------------------------------------
// BT와 이동 및 남아 있는 공격 판정 정리.
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
    AttackTraceComponent->EndTrace();
    GetCharacterMovement()->StopMovementImmediately();
}

//---------------------------------------------------------------
// 사망을 우선 처리하고 페이즈 1의 HP 임계값 확인.
//
//---------------------------------------------------------------
void ABoss::HandleHpChanged(float CurrentHp, float MaxHp)
{
    if (BossPhase == EBossPhase::Dead)
    {
        return;
    }
    if (CurrentHp <= 0.0f)
    {
        BossPhase = EBossPhase::Dead;
        SetEnemyState(EEnemyState::Dead);
        StopCombat();
        StopOpening();
        return;
    }
    if (BossPhase == EBossPhase::Phase1 && BossAIData && MaxHp > 0.0f &&
        CurrentHp / MaxHp <= FMath::Clamp(BossAIData->Phase2HpRatio, 0.01f, 0.99f))
        EnterCombatPhase(EBossPhase::Phase2);
}

void ABoss::ReceiveDamage(const FDamageInfo& DamageInfo, const FHitResult& Hit)
{
    //---------------------------------------------------------------
    // 컷씬 단계와 사망 상태에서는 피격 처리하지 않음.
    //
    //---------------------------------------------------------------
    if (BossPhase == EBossPhase::Intro || IsDead())
    {
        return;
    }

    Super::ReceiveDamage(DamageInfo, Hit);
}

//---------------------------------------------------------------
// 일반 공격은 부모 판정을 재사용하고 폭주 공격만 별도 데미지 적용.
//
//---------------------------------------------------------------
void ABoss::HandleAttackTraceHit(const FHitResult& Hit)
{
    if (BossPhase == EBossPhase::Intro || IsDead())
    {
        return;
    }

    if (BossPhase != EBossPhase::OpeningAttack)
    {
        Super::HandleAttackTraceHit(Hit);
        return;
    }
    if (ICombatDamageInterface* Target = Cast<ICombatDamageInterface>(Hit.GetActor()))
    {
        FDamageInfo Damage;
        Damage.AttackDamage = BossAIData->OpeningAttackDamage;
        Target->ReceiveDamage(Damage, Hit);
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

//---------------------------------------------------------------
// 몽타주 종료 콜백을 해제하고 폭주 공격 정지.
//
//---------------------------------------------------------------
void ABoss::StopOpening()
{
    UAnimInstance* Anim = GetMesh()->GetAnimInstance();

    if (!Anim || !BossAIData || !BossAIData->OpeningAttackMontage)
    {
        return;
    }

    FOnMontageEnded EmptyDelegate;
    Anim->Montage_SetEndDelegate(EmptyDelegate, BossAIData->OpeningAttackMontage);
    Anim->Montage_Stop(0.0f, BossAIData->OpeningAttackMontage);
    AttackTraceComponent->EndTrace();
}

//---------------------------------------------------------------
// HP 변경 이벤트 연결을 해제하고 전투 실행 상태 정리.
//
//---------------------------------------------------------------
void ABoss::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    EnemyStatComponet->OnEnemyHpChanged.RemoveAll(this);
    StopCombat();
    StopOpening();
    Super::EndPlay(EndPlayReason);
}
