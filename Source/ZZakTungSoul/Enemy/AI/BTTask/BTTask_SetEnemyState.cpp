// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/AI/BTTask/BTTask_SetEnemyState.h"
#include <Enemy/EnemyBase.h>

#include <AIController.h>

UBTTask_SetEnemyState::UBTTask_SetEnemyState()
{
    NodeName = TEXT("Set Enemy State");
}

EBTNodeResult::Type UBTTask_SetEnemyState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);

    // 현재 Behavior Tree를 실행하고 있는 AI Controller 획득
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        return EBTNodeResult::Failed;
    }

    // 현재 AI Controller가 제어하고 있는 Enemy 획득
    AEnemyBase* Enemy = Cast<AEnemyBase>(AIController->GetPawn());
    if (!Enemy || Enemy->IsDead())
    {
        return EBTNodeResult::Failed;
    }

    Enemy->SetEnemyState(EnemyState);

    return EBTNodeResult::Succeeded;
}
