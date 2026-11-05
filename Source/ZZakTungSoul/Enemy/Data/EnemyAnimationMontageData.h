// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include <Enemy/Data/EnemyAttackData.h>
#include "EnemyAnimationMontageData.generated.h"

// 전방 선언
class UAnimMontage;

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API UEnemyAnimationMontageData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	// 회피
	UPROPERTY(EditAnywhere, Category = "Montage|Avoid")
	TObjectPtr<UAnimMontage> AvoidMontage;

	// 콤보 공격 01
	UPROPERTY(EditAnywhere, Category = "Montage|Attack")
	FEnemyAttackData ComboAttack01;

	// 콤보 공격 02
	UPROPERTY(EditAnywhere, Category = "Montage|Attack")
	FEnemyAttackData ComboAttack02;

	// 피격(Poise)
	UPROPERTY(EditAnywhere, Category = "Montage|Hit")
	TObjectPtr<UAnimMontage> HitMontage;

	// 그로기(Stance)
	UPROPERTY(EditAnywhere, Category = "Montage|Hit")
	TObjectPtr<UAnimMontage> GroggyMontage;

	// 사망
	UPROPERTY(EditAnywhere, Category = "Montage|Dead")
	TObjectPtr<UAnimMontage> DeadMontage;
};
