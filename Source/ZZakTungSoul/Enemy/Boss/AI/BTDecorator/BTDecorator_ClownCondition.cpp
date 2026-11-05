#include <Enemy/Boss/AI/BTDecorator/BTDecorator_ClownCondition.h>
#include <Enemy/Boss/Clown/BossClown.h>
#include <Enemy/AI/EnemyAIBlackboardKey.h>
#include <AIController.h>
#include <BehaviorTree/BlackboardComponent.h>
#include <GameFramework/CharacterMovementComponent.h>

UBTDecorator_ClownCondition::UBTDecorator_ClownCondition()
{
	NodeName = TEXT("Clown Range / Chance");
	FlowAbortMode = EBTFlowAbortMode::None;
}

bool UBTDecorator_ClownCondition::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	ABossClown* Clown = AIController ? Cast<ABossClown>(AIController->GetPawn()) : nullptr;
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	AActor* Target = BlackboardComp ? Cast<AActor>(BlackboardComp->GetValueAsObject(BBKEY_TARGETACTOR)) : nullptr;
	if (!IsValid(Clown) || !IsValid(Target) || Clown->IsDead() || !Clown->GetCharacterMovement()->IsMovingOnGround())
	{
		return false;
	}
	if (Clown->GetBossPhase() != EBossPhase::Phase1 && Clown->GetBossPhase() != EBossPhase::Phase2)
	{
		return false;
	}
	const UBossClownAnimationData* Data = Clown->GetClownAnimationData();
	if (!Data)
	{
		return false;
	}
	if (bCheckAttackData)
	{
		if (!Clown->GetClownAttackData(AttackType))
		{
			return false;
		}
		if ((AttackType == EBossClownAttack::TripleBalls && !Data->BallClass) || (AttackType == EBossClownAttack::ClapWave && !Data->ClapWaveClass) || (AttackType == EBossClownAttack::SpawnCards && !Data->CardClass) || (AttackType == EBossClownAttack::CubeBarrage && (!Data->CubeClass || Data->CubePieces.Num() != 27)))
		{
			return false;
		}
	}
	else if (!Data->bEnableTacticalActions)
	{
		return false;
	}
	const float PatternScale = bCheckAttackData ? Clown->GetClownPatternSelectionScale(AttackType) : 1.0f;
	if (PatternScale<=0.0f)
	{
		return false;
	}
	const float Distance = Clown->GetTargetSurfaceDistance(Target);
	const float EffectiveMinDistance = bCheckAttackData && AttackType == EBossClownAttack::CubeBarrage ? 600.0f : MinDistance;
	if (Distance<EffectiveMinDistance || Distance>MaxDistance)
	{
		return false;
	}
	if (bCheckAttackData && AttackType==EBossClownAttack::SpawnCards && Distance<850.0f)
	{
		return false;
	}
	const bool bCubePriority=bCheckAttackData && Clown->ShouldPrioritizeClownCube(Target);
	if (bCubePriority && AttackType != EBossClownAttack::CubeBarrage) { return false; }
	const bool bRanged = AttackType == EBossClownAttack::TripleBalls || AttackType == EBossClownAttack::ClapWave || AttackType == EBossClownAttack::CubeBarrage;
	if (bCheckAttackData && bRanged && Distance < 450.0f)
	{
		return false;
	}
	if (bCheckAttackData && AttackType != EBossClownAttack::SpawnCards && Clown->PrefersClownRangedAttack() && Distance > 450.0f && !bRanged)
	{
		return false;
	}
	const FVector Direction = (Target->GetActorLocation() - Clown->GetActorLocation()).GetSafeNormal2D();
	const float FacingDot = bCheckAttackData && AttackType == EBossClownAttack::CubeBarrage ? 0.2f : MinimumFacingDot;
	if (FVector::DotProduct(Clown->GetActorForwardVector(),Direction)<FacingDot)
	{
		return false;
	}
	const float BaseChance=bCubePriority || (bCheckAttackData && bRanged && Clown->PrefersClownRangedAttack()) ? 1.0f : (bCheckAttackData && AttackType==EBossClownAttack::CubeBarrage ? 0.85f : FMath::Clamp(Chance,0.0f,1.0f));
	return FMath::FRand()<BaseChance*(bCubePriority ? 1.0f : PatternScale);
}
