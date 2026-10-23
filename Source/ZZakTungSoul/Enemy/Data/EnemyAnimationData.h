// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
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
	TObjectPtr<UAnimMontage> NormalAttack;

	// 강 공격
	UPROPERTY(EditAnywhere, Category = "Montage|Attack")
	TObjectPtr<UAnimMontage> StrongAttack;

	// 하나로 이어져 있는 콤보 공격
	UPROPERTY(EditAnywhere, Category = "Montage|Attack")
	TObjectPtr<UAnimMontage> ComboAttack;

	// 콤보 공격에 사용할 Montage 배열
	UPROPERTY(EditAnywhere, Category = "Montage|Attack")
	TArray<TObjectPtr<UAnimMontage>> ComboAttackArray;
};
