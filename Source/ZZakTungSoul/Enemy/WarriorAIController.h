// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "WarriorAIController.generated.h"

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API AWarriorAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AWarriorAIController();

	virtual void OnPossess(APawn* InPawn) override;

	// 탐지한 플레이어(Object) 담을 Blackboard변수의 Key 값
	static const FName TargetKey;
	// 순찰 포인트(Vector) 담을 Blackboard변수의 Key 값
	static const FName PatrolPosKey;

private:
	UPROPERTY()
	class UBehaviorTree* BTAsset;

	UPROPERTY()
	class UBlackboardData* BBAsset;
};
