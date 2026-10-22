// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EnemyAIData.generated.h"

// 전방 선언
class UBehaviorTree;

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API UEnemyAIData : public UDataAsset
{
	GENERATED_BODY()
	
////// Behavior Tree Section
public:
	// Main Behavior Tree
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BehaviorTree|Main")
	TObjectPtr<UBehaviorTree> MainBT;

	// Sub Behavior Tree - Combat
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BehaviorTree|Combat")
	TSoftObjectPtr<UBehaviorTree> CombatBT;

	// Sub Behavior Tree - Patrol
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BehaviorTree|Patrol")
	TSoftObjectPtr<UBehaviorTree> PatrolBT;

////// Movement Speed Section
public:
	// Patrol Speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Patrol")
	float PatrolMovementSpeed;

	// Chase Speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Chase")
	float ChaseMovementSpeed;

	// Strafe Speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Strafe")
	float StrafeMovementSpeed;

	// Search Speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Search")
	float SearchMovementSpeed;

////// Distance / Range Section
public:
	// 전투 상황 인지 범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Range|Combat")
	float CombatRange;

	// 공격 가능 범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Range|Attack")
	float AttackRange;

	// 최소 유지 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Range|MaintainDistance")
	float MinMaintainDistance;
};
