// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include <Enemy/EnemyStates.h>
#include "BTTask_SetNormalEnemyState.generated.h"

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API UBTTask_SetNormalEnemyState : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_SetNormalEnemyState();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	// 이동속도 상태 설정
	UPROPERTY(EditAnywhere, Category = "State")
	EEnemyMoveState MoveState = EEnemyMoveState::Idle;

	// 행동 상태 설정
	UPROPERTY(EditAnywhere, Category = "State")
	EEnemyActionState ActionState = EEnemyActionState::None;

	// 공격 선택 설정
	UPROPERTY(EditAnywhere, Category = "State")
	EEnemySelectAttack SelectAttack = EEnemySelectAttack::None;
};
