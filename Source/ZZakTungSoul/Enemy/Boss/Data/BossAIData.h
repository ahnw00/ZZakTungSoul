// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BossAIData.generated.h"

class UBehaviorTree;
/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API UBossAIData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BehaviorTree|Main")
	TObjectPtr<UBehaviorTree> WelcomeBT;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BehaviorTree|Main")
	TArray<TObjectPtr<UBehaviorTree>> PhaseOneBT;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Range|Combat")
	float CombatRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Range|Attack")
	float AttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Range|MaintainDistance")
	float MinMaintainDistance;
};
