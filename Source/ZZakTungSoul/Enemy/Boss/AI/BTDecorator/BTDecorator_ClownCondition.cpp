#include "Enemy/Boss/AI/BTDecorator/BTDecorator_ClownCondition.h"
#include "Enemy/Boss/Clown/BossClown.h"
#include "Enemy/AI/EnemyAIBlackboardKey.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_ClownCondition::UBTDecorator_ClownCondition()
{
	NodeName = TEXT("Clown Range / Chance");
	FlowAbortMode = EBTFlowAbortMode::None;
}

bool UBTDecorator_ClownCondition::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const AAIController* AI = OwnerComp.GetAIOwner();
	const ABossClown* Boss = AI ? Cast<ABossClown>(AI->GetPawn()) : nullptr;
	const UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	AActor* Target = BB ? Cast<AActor>(BB->GetValueAsObject(BBKEY_TARGETACTOR)) : nullptr;
	return Boss && Boss->EvaluateClownCondition(Target, bCheckAttackData, AttackType, MinDistance, MaxDistance, Chance, MinimumFacingDot);
}
