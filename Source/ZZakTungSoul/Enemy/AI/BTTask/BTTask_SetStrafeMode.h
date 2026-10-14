// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SetStrafeMode.generated.h"

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API UBTTask_SetStrafeMode : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_SetStrafeMode();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Strafe")
	uint32 bEnableStrafe : 1 = true;
};
