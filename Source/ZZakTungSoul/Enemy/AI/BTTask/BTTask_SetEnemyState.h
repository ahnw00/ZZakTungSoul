// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include <Enemy/EnemyStates.h>
#include "BTTask_SetEnemyState.generated.h"

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API UBTTask_SetEnemyState : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_SetEnemyState();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category = "State")
	EEnemyState EnemyState = EEnemyState::Idle;
};
