#include <Enemy/Boss/AI/BTDecorator/BTDecorator_ClownYabawiReady.h>
#include <Enemy/Boss/Clown/BossClown.h>
#include <AIController.h>
#include <BehaviorTree/BehaviorTreeComponent.h>

UBTDecorator_ClownYabawiReady::UBTDecorator_ClownYabawiReady()
{
	NodeName=TEXT("Phase 2 Yabawi Ready");
	FlowAbortMode=EBTFlowAbortMode::None;
}

bool UBTDecorator_ClownYabawiReady::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const AAIController* AI=OwnerComp.GetAIOwner();
	const ABossClown* Boss=AI ? Cast<ABossClown>(AI->GetPawn()) : nullptr;
	return Boss && Boss->CanStartClownYabawi();
}
