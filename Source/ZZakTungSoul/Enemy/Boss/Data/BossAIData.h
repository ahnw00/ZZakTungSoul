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
	// 컷씬 종료 후 한 번 실행할 폭주 공격 몽타주.
	//
	//---------------------------------------------------------------
	UPROPERTY(EditAnywhere, Category = "Boss|Opening")
	TObjectPtr<class UAnimMontage> OpeningAttackMontage;

	//---------------------------------------------------------------
	// 폭주 공격의 타격 구간당 데미지.
	//
	//---------------------------------------------------------------
	UPROPERTY(EditAnywhere, Category = "Boss|Opening", meta = (ClampMin = "0"))
	float OpeningAttackDamage = 70.0f;

	//---------------------------------------------------------------
	// 페이즈 2로 전환할 HP 비율.
	//
	//---------------------------------------------------------------
	UPROPERTY(EditAnywhere, Category = "Boss|Phase", meta = (ClampMin = "0.01", ClampMax = "0.99"))
	float Phase2HpRatio = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Range|Combat")
	float CombatRange = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Range|Attack")
	float AttackRange = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Range|MaintainDistance")
	float MinMaintainDistance = 150.0f;
};
