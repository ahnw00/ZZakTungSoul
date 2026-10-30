// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AIDataInterface.generated.h"

// 전방 선언
class UEnemyAnimationData;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UAIDataInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ZZAKTUNGSOUL_API IAIDataInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// 전투 상태 인지 범위 데이터
	virtual float GetCombatRangeData() = 0;

	// 공격 상태 인지 범위 데이터
	virtual float GetAttackRangeData() = 0;

	// 최소 유지 거리 데이터
	virtual float GetMinMaintainDistanceData() = 0;

	// 애니메이션 및 DamageInfo 데이터
	virtual UEnemyAnimationData* GetAnimationData() = 0;

	// 데미지 수치 정보 설정
	virtual void SetCurrentDamageInfoInterface(const struct FDamageInfo& NewDamageInfo) = 0;
};
