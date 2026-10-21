// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_NormalAttack.generated.h"

// 전방 선언
class UAnimMontage;

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API UBTTask_NormalAttack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_NormalAttack();

protected:
	// 공격 Task 실행 함수
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// BehavioTree에 의해 중단 되었을 때 처리할 함수
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// Task 완료시 초기화 처리 함수
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

private:
	// 실행 상태 정리 함수
	void CleanUp();

	// Montage종료 Delegate
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

protected:
	// 현재 Montage를 재생하는 AnimInstance
	TWeakObjectPtr<UAnimInstance> OwnerAnimInstance;

	// 현재 Task를 실행하는 BehaviorTree
	TWeakObjectPtr<UBehaviorTreeComponent> OwnerBTComp;

	// 현재 실행 중인지 확인하는 플래그
	uint32 bIsAttacking : 1 = false;

protected:
	// 재생할 Montage
	UPROPERTY(EditAnywhere, Category = "Attack")
	TObjectPtr<UAnimMontage> AttackMontage;

	// Montage 재생 속도
	UPROPERTY(EditAnywhere, Category = "Attack")
	float PlayRate = 1.0f;
};
