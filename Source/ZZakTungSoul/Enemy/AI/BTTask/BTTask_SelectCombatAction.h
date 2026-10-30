// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SelectCombatAction.generated.h"

UENUM(BlueprintType)
enum class ECombatAction : uint8
{
	Attack,
	Other
};

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API UBTTask_SelectCombatAction : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_SelectCombatAction();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	// 전투 상태 선택 함수
	ECombatAction SelectCombatAction();

protected:
	UPROPERTY(EditAnywhere, Category = "Combat|Weight")
	int32 AttackWeight = 70;

	UPROPERTY(EditAnywhere, Category = "Combat|Weight")
	int32 OtherWeight = 30;
};
