// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/ItemDataAsset.h"
#include "ConsumableDataAsset.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class ZZAKTUNGSOUL_API UConsumableDataAsset : public UItemDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Quantity = 0;
};
