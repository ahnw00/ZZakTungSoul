// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemData/ItemDataAsset.h"
#include "ConsumableDataAsset.generated.h"

UENUM(BlueprintType)
enum class EConsumableItemType : uint8
{
	None,
	Potion,		// 회복 물약
	Lantern,	// 등불
	Projectile	// 투사체(짱돌)
};

/**
 * 
 */
UCLASS(BlueprintType)
class ZZAKTUNGSOUL_API UConsumableDataAsset : public UItemDataAsset
{
	GENERATED_BODY()

public:
	UConsumableDataAsset();
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Quantity = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EConsumableItemType ConsumableItemType = EConsumableItemType::None;
};
