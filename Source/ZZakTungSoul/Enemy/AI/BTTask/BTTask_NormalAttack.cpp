// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/AI/BTTask/BTTask_NormalAttack.h"

UBTTask_NormalAttack::UBTTask_NormalAttack()
{
	NodeName = TEXT("Normal Attack");

	// AI마다 독립적인 인스턴스 사용
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTask_NormalAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Type();
}

EBTNodeResult::Type UBTTask_NormalAttack::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Type();
}

void UBTTask_NormalAttack::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{

}

void UBTTask_NormalAttack::CleanUp()
{

}

void UBTTask_NormalAttack::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{

}
