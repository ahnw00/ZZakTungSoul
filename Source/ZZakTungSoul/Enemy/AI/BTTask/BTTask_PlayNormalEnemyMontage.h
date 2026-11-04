// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_PlayNormalEnemyMontage.generated.h"

class UNormalEnemyAnimInstance;
class ANormalEnemyBase;
class UAnimMontage;
class UBehaviorTreeComponent;

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API UBTTask_PlayNormalEnemyMontage : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_PlayNormalEnemyMontage();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// BehavioTree에 의해 중단 되었을 때 처리할 함수
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// Task 완료시 초기화 처리 함수
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

private:
	// Montage 종료시 Task 완료 함수
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	// Task 참조 및 Delegate 정리
	void CleanUp();

protected:
	// 현재 Montage를 재생하는 AnimInstance
	TWeakObjectPtr<UNormalEnemyAnimInstance> OwnerAnimInstance;

	// 현재 Task를 실행하는 BehaviorTree
	TWeakObjectPtr<UBehaviorTreeComponent> OwnerBTComp;

	// Normal Enemy
	TWeakObjectPtr<ANormalEnemyBase> OwnerEnemy;

	// 이번에 재생한 Montage
	TWeakObjectPtr<UAnimMontage> PlayingMontage;
};
