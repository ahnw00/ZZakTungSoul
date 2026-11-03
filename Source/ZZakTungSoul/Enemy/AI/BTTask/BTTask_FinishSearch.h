// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FinishSearch.generated.h"

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API UBTTask_FinishSearch : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_FinishSearch();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
