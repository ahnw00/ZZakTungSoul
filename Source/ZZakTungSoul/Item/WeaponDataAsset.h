// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/ItemDataAsset.h"
#include "WeaponDataAsset.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class ZZAKTUNGSOUL_API UWeaponDataAsset : public UItemDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AttackPower = 0.f;  // 실제 플레이어 공격력 스탯에 영향을 주는 수치

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DamageNegation = 0.f; // 막기 하고 있을 때의 데미지 경감률(스탯에 영향을 주는 수치 아님)

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Weight = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<USkeletalMesh> WeaponMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UAnimMontage> AttackMontage;  // 창과 검이 같은 몽타주를 가지면 안되니까
};
