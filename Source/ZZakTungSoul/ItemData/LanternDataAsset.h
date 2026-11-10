// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemData/ConsumableDataAsset.h"
#include <Interface/ItemFunctionInterface.h>
#include "LanternDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API ULanternDataAsset 
	: public UConsumableDataAsset, public IItemFunctionInterface
{
	GENERATED_BODY()
	
public:
	ULanternDataAsset();

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("LanternData", GetFName());
	}

	// Inherited via IItemFunctionInterface
	void ItemFunction(AWarrior* User) const override;
};
