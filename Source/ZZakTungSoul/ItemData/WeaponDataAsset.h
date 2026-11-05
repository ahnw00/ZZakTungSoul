// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemData/ItemDataAsset.h"
#include "WeaponDataAsset.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class ZZAKTUNGSOUL_API UWeaponDataAsset : public UItemDataAsset
{
	GENERATED_BODY()

public:
	UWeaponDataAsset();
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AttackPower = 0.f;  // 실제 플레이어 공격력 스탯에 영향을 주는 수치

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DamageNegation = 0.f; // 막기 하고 있을 때의 데미지 경감률(스탯에 영향을 주는 수치 아님)

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Weight = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	//TSoftObjectPtr<UAnimMontage> AttackMontage;  // 창과 검이 같은 몽타주를 가지면 안되니까
	TArray<TSoftObjectPtr<UAnimMontage>> OneHandAttackMontages;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSoftObjectPtr<UAnimMontage>> TwoHandAttackMontages;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UAnimMontage> OneHandBackStepMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UAnimMontage> TwoHandBackStepMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UAnimMontage> OneHandRollMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UAnimMontage> TwoHandRollMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UAnimMontage> OneHandHitMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UAnimMontage> TwoHandHitMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UAnimMontage> OneHandStunMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UAnimMontage> TwoHandStunMontage;
};
