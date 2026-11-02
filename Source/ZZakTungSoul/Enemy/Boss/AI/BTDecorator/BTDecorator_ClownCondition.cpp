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
		if (bAvoidRecentRepeat && Clown->HasRecentClownAttack(AttackType, (AttackType == EBossClownAttack::SpawnCards || AttackType == EBossClownAttack::CubeBarrage) ? 18.0f : (AttackType == EBossClownAttack::TripleBalls || AttackType == EBossClownAttack::ClapWave ? 10.0f : 4.0f)))
		{
			return false;
		}
		const FClownPhaseOnePattern* Pattern = Clown->GetPhaseOnePattern(AttackType);
		if (!Pattern || !Pattern->bEnabled || !Pattern->Attack.Montage || !Pattern->Attack.DamageInfo.IsValidIndex(0))
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
	const float Distance = Clown->GetTargetSurfaceDistance(Target);
	if (Distance < MinDistance || Distance > MaxDistance)
	{
		return false;
	}
	if (bCheckAttackData && (AttackType == EBossClownAttack::SpawnCards || AttackType == EBossClownAttack::CubeBarrage) && Distance < 850.0f)
	{
		return false;
	}
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
	if (FVector::DotProduct(Clown->GetActorForwardVector(), Direction) < MinimumFacingDot)
	{
		return false;
	}
	return (bCheckAttackData && bRanged && Clown->PrefersClownRangedAttack()) || FMath::FRand() < FMath::Clamp(Chance, 0.0f, 1.0f);
}
