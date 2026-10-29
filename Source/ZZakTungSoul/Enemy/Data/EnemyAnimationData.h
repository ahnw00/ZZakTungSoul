// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include <Enemy/Data/EnemyAttackData.h>
#include "EnemyAnimationData.generated.h"

// 전방 선언
class UAnimMontage;

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API UEnemyAnimationData : public UDataAsset
{
	GENERATED_BODY()
	
////// Montage Section
public:
	// 일반 공격
	UPROPERTY(EditAnywhere, Category = "Montage|Attack")
	FEnemyAttackData NormalAttack;

	UPROPERTY(EditAnywhere, Category = "Montage|Attack")
	FEnemyAttackData StrongAttack;

	UPROPERTY(EditAnywhere, Category = "Montage|Attack")
	TArray<FEnemyAttackData> ComboAttack;
};
