// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_UpdateCombatInfo.generated.h"

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API UBTService_UpdateCombatInfo : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_UpdateCombatInfo();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	// 전투 상태 진입 범위
	UPROPERTY(EditAnywhere, Category = "Combat")
	float CombatRange = 500.0f;

	// 공격 상태 진입 범위
	UPROPERTY(EditAnywhere, Category = "Combat")
	float Attackrange = 200.0f;
};
