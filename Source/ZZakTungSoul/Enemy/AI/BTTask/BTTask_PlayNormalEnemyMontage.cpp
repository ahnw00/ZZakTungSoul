// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/AI/BTTask/BTTask_PlayNormalEnemyMontage.h"

UBTTask_PlayNormalEnemyMontage::UBTTask_PlayNormalEnemyMontage()
{
	NodeName = TEXT("Play Montage : Normal Enemy");
}

EBTNodeResult::Type UBTTask_PlayNormalEnemyMontage::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Type();
}

EBTNodeResult::Type UBTTask_PlayNormalEnemyMontage::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Type();
}

void UBTTask_PlayNormalEnemyMontage::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
}
