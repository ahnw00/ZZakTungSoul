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
	//---------------------------------------------------------------
	// 페이즈 1에서 실행할 BT.
	//
	//---------------------------------------------------------------
	UPROPERTY(EditAnywhere, Category = "Boss|Phase")
	TObjectPtr<UBehaviorTree> Phase1BT;

	//---------------------------------------------------------------
	// 페이즈 2에서 실행할 BT.
	//
	//---------------------------------------------------------------
	UPROPERTY(EditAnywhere, Category = "Boss|Phase")
	TObjectPtr<UBehaviorTree> Phase2BT;

	//---------------------------------------------------------------
	// 페이즈 2로 전환할 HP 비율.
	//
	//---------------------------------------------------------------
	UPROPERTY(EditAnywhere, Category = "Boss|Phase", meta = (ClampMin = "0.01", ClampMax = "0.99"))
	float Phase2HpRatio = 0.5f;

	// 보스와 타깃의 수평 캡슐 표면 간격 기준.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Range|Attack")
	float AttackRange = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Range|MaintainDistance")
	float MinMaintainDistance = 150.0f;
};
