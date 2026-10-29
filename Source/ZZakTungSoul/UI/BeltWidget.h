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
	
	void OnEquipmentChanged();

	FORCEINLINE void SetEquipment(class UEquipmentComponent* InEquipment)
	{
		Equipment = InEquipment;
	}

	FORCEINLINE void SetInventory(class UInventoryComponent* InInventory)
	{
		Inventory = InInventory;
	}

private:
	UTexture2D* GetTexture(FGuid InstanceID);

private:
	UPROPERTY(meta = (Bindwidget))
	TObjectPtr<class UImage> UpperBeltImage;

	UPROPERTY(meta = (Bindwidget))
	TObjectPtr<class UImage> LowerBeltImage;

	UPROPERTY()
	TObjectPtr<UEquipmentComponent> Equipment;

	UPROPERTY()
	TObjectPtr<UInventoryComponent> Inventory;
};
