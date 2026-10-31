#include <Enemy/Boss/AI/Task/BTTask_DashAttack.h>
#include <Enemy/Boss/Clown/BossClown.h>
#include <AIController.h>
#include <BehaviorTree/BehaviorTreeComponent.h>
#include <BehaviorTree/BlackboardComponent.h>

UBTTask_DashAttack::UBTTask_DashAttack()
{
	NodeName = TEXT("Clown Attack");
	bCreateNodeInstance = true;
	bNotifyTick = true;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_DashAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AttackingClown.Reset();
	ExecutionID = INDEX_NONE;

	AAIController* AI = OwnerComp.GetAIOwner();
	ABossClown* Clown = AI ? Cast<ABossClown>(AI->GetPawn()) : nullptr;
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	AActor* Target = BB ? Cast<AActor>(BB->GetValueAsObject(TargetActorKey)) : nullptr;

	if (!IsValid(Clown))
	{
		return EBTNodeResult::Failed;
	}

	int32 StartedID = INDEX_NONE;

	if (!Clown->StartClownAttack(AttackType, Target, StartedID))
	{
		return EBTNodeResult::Failed;
	}

	AttackingClown = Clown;
	ExecutionID = StartedID;
	return EBTNodeResult::InProgress;
}

void UBTTask_DashAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (!AttackingClown.IsValid())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	const EBossClownAttackResult Result = AttackingClown->GetClownAttackResult(ExecutionID);
	if (Result != EBossClownAttackResult::Running)
	{
		FinishLatentTask(OwnerComp, Result == EBossClownAttackResult::Succeeded ? EBTNodeResult::Succeeded : EBTNodeResult::Failed);
	}
}

EBTNodeResult::Type UBTTask_DashAttack::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AttackingClown.IsValid())
	{
		AttackingClown->CancelClownAttack(ExecutionID);
	}

	return EBTNodeResult::Aborted;
}

void UBTTask_DashAttack::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	if (AttackingClown.IsValid())
	{
		AttackingClown->CancelClownAttack(ExecutionID);
	}

	AttackingClown.Reset();
	ExecutionID = INDEX_NONE;

	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}
