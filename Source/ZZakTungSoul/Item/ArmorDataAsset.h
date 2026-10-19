// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/ItemDataAsset.h"
#include "ArmorDataAsset.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class ZZAKTUNGSOUL_API UArmorDataAsset : public UItemDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DefensePower = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Weight = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<USkeletalMesh> ArmorMesh;
};
