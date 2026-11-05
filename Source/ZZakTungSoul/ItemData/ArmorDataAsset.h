// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemData/ItemDataAsset.h"
#include "ArmorDataAsset.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class ZZAKTUNGSOUL_API UArmorDataAsset : public UItemDataAsset
{
	GENERATED_BODY()

public:
	UArmorDataAsset();
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DefensePower = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Weight = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Resistance = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Poise = 0.f;
};
