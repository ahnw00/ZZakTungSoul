// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <ItemData/ItemDataAsset.h>
#include "BeltWidget.generated.h"

// 전방 선언
class UItemDataAsset;

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API UBeltWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//void OnEquipmentChanged(EEquipmentSlot InSlot, UItemDataAsset* InItemData);
	
	void OnEquipmentChanged(EEquipmentSlot InSlot, UItemDataAsset* InItemData);

private:
	UPROPERTY(meta = (Bindwidget))
	TObjectPtr<class UImage> UpperBeltImage;

	UPROPERTY(meta = (Bindwidget))
	TObjectPtr<class UImage> LowerBeltImage;
};
