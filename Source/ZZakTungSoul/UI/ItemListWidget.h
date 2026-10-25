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

// 전방 선언
class UInventoryComponent;

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API UItemListWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UItemListWidget(const FObjectInitializer& ObjectInitializer);

	FORCEINLINE void SetInventory(UInventoryComponent* InInventory) { Inventory = InInventory; }

	void InitializeItemList(
		EItemListMode InMode,
		EItemListFilter InInitialFilter);

	UFUNCTION(BlueprintCallable)
	void ListItemsByType(EItemType InType);

	UFUNCTION(BlueprintCallable)
	void ListItemsBySlot(EEquipmentSlot InSlot);

	//void ToggleItemListWidget();

private:
	// 필터 버튼들 생성 함수
	// 장비 변경 버튼을 눌렀느냐, 인벤토리 버튼을 눌렀느냐에 따라
	// 생성되는 필터 버튼이 달라
	void CreateFilterButtons();
	
	// 리스트 창에 띄워질 아이템 버튼 생성 함수
	void AddItemButton(UItemDataAsset* InItemData, int32 Index);

private:
	UPROPERTY()
	TObjectPtr<class UInventoryComponent> Inventory;

	UPROPERTY()
	EItemListMode ListMode;

	UPROPERTY()
	EItemListFilter CurrentFilter;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UUniformGridPanel> ItemContainer;

	const int32 ContainerWidth = 5;

	UPROPERTY()
	TSubclassOf<class UItemButtonWidget> ItemButtonClass;

	UPROPERTY()
	TArray<TObjectPtr<class UItemButtonWidget>> ItemButtons;
};
