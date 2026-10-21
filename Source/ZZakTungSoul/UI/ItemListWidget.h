// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <ItemData/ItemDataAsset.h>
#include "ItemListWidget.generated.h"

UENUM(BlueprintType)
enum class EItemListMode : uint8
{
	Inventory,
	EquipmentSlot
};

UENUM(BlueprintType)
enum class EItemListFilter : uint8
{
	// 인벤토리를 열었을 때 뜨는 필터들
	All,
	Consumable,
	Weapon,
	Armor,

	// 장비를 열었을 때 뜨는 필터들
	RightHand,
	LeftHand,
	Helmet,
	Chest,
	Arms,
	Legs
};

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API UItemListWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void InitializeItemList(
		EItemListMode InMode,
		EItemListFilter InInitialFilter);

	UFUNCTION(BlueprintCallable)
	void ListItemsByType(EItemType InType);

	UFUNCTION(BlueprintCallable)
	void ListItemsBySlot(EEquipmentSlot InSlot);

private:
	void CreateFilterButtons();

private:
	UPROPERTY()
	EItemListMode ListMode;

	UPROPERTY()
	EItemListFilter CurrentFilter;
};
