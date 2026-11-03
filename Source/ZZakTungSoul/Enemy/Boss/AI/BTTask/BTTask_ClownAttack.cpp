#include <Enemy/Boss/AI/BTTask/BTTask_ClownAttack.h>
#include <Enemy/Boss/Clown/BossClown.h>
#include <AIController.h>
#include <BehaviorTree/BehaviorTreeComponent.h>
#include <BehaviorTree/BlackboardComponent.h>

UBTTask_ClownAttack::UBTTask_ClownAttack()
{
	NodeName = TEXT("Clown Attack");
	bCreateNodeInstance = true;
	bNotifyTick = true;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_ClownAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	ABossClown* Clown = AIController ? Cast<ABossClown>(AIController->GetPawn()) : nullptr;
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	AActor* Target = BlackboardComp ? Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey)) : nullptr;

	if (!IsValid(Clown) || !IsValid(Target))
	{
		return EBTNodeResult::Failed;
	}

	int32 StartedID = INDEX_NONE;
	if (!Clown->StartClownAttack(AttackType, Target, StartedID, Style, RecoveryScale))
	{
		return EBTNodeResult::Failed;
	}
	AttackingClown = Clown;
	ExecutionID = StartedID;
	return EBTNodeResult::InProgress;
}

void UBTTask_ClownAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
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

EBTNodeResult::Type UBTTask_ClownAttack::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	CleanUp();
	return EBTNodeResult::Aborted;
}

void UBTTask_ClownAttack::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
	CleanUp();
}

void UBTTask_ClownAttack::CleanUp()
{
	if (AttackingClown.IsValid())
	{
		AttackingClown->CancelClownAttack(ExecutionID);
	}
	AttackingClown.Reset();
	ExecutionID = INDEX_NONE;
}
