// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Enemy/Data/EnemyAnimationData.h>
#include "BossAnimationData.generated.h"

/**
 *
 */
UCLASS()
class ZZAKTUNGSOUL_API UBossAnimationData : public UEnemyAnimationData
{
	GENERATED_BODY()

public:
	UBossAnimationData();

	////// Montage Section
public:
	//---------------------------------------------------------------
	// 인트로 종료 후 실행할 첫 공격.
	//
	//---------------------------------------------------------------
	UPROPERTY(EditAnywhere, Category = "Montage|Boss|Opening")
	FEnemyAttackData OpeningAttack;
};
