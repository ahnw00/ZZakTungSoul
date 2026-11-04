// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/AI/BTTask/BTTask_SetNormalEnemyState.h"
#include <Enemy/NormalEnemy/NormalEnemyBase.h>

#include <AIController.h>
#include <BehaviorTree/BehaviorTreeComponent.h>

UBTTask_SetNormalEnemyState::UBTTask_SetNormalEnemyState()
{
	NodeName = TEXT("Set NormalEnemy State");
}

EBTNodeResult::Type UBTTask_SetNormalEnemyState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (false == IsValid(AIController))
	{
		return EBTNodeResult::Failed;
	}

	ANormalEnemyBase* Enemy = Cast<ANormalEnemyBase>(AIController->GetPawn());
	if (false == IsValid(Enemy) || Enemy->IsDeadState())
	{
		return EBTNodeResult::Failed;
	}

	Enemy->SetEnemyMoveState(MoveState);
	Enemy->SetEnemyActionState(ActionState);
	Enemy->SetEnemySelectAttack(SelectAttack);

	return EBTNodeResult::Succeeded;
}
