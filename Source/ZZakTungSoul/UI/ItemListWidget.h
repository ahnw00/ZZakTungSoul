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
	EquipmentSlot,
	BeltSlot
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
	Head,
	Chest,
	Arms,
	Legs,

	// 벨트 슬롯 필터
	UpperBelt,
	LowerBelt
};

// 전방 선언
class UInventoryComponent;
class UUniformGridPanel;



/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API UItemListWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
public:
	UItemListWidget(const FObjectInitializer& ObjectInitializer);

	FORCEINLINE void SetInventory(UInventoryComponent* InInventory) { Inventory = InInventory; }

	void InitializeItemList(EItemListFilter InInitialFilter);

	UFUNCTION(BlueprintCallable)
	void ListItemsByFilter(EItemListFilter InFilter);

	FORCEINLINE void SetListMode(EItemListMode InMode)
	{
		ListMode = InMode;
	}

	// 테스트용
	void ListAllItems();

	void ResetInfo();


private:
	// 필터 버튼들 생성 함수
	// 장비 변경 버튼을 눌렀느냐, 인벤토리 버튼을 눌렀느냐에 따라
	// 생성되는 필터 버튼이 달라
	void CreateFilterButtons();
	
	// 리스트 창에 띄워질 아이템 버튼 생성 함수
	void AddItemButton(const FGuid& InstanceID, int32 Index);

	UItemDataAsset* LoadItemData(const struct FInventoryItem& Item);

	bool IsAllowed(EItemListFilter InFilterType, UItemDataAsset* ItemData);

	// 정보 표시
	void ShowInfo(const FGuid& InstanceID);

	void SetEquipmentSlotByFilter(EItemListFilter InFilter);

	void EquipItem(const FGuid& InstanceID);

public:
	FORCEINLINE void SetEquipSlot(EEquipmentSlot InSlot) { CurrentEquipSlot = InSlot; }

	void SetSlotText(EItemListFilter InFilter);


private:
	UPROPERTY()
	TObjectPtr<class UInventoryComponent> Inventory;

	UPROPERTY()
	TObjectPtr<UTexture2D> ItemEmptyImage;

	// 필터 버튼 관련

	// 리스트 위젯 열 때 초기화해줘야할 변수
	UPROPERTY()
	EItemListMode ListMode;

	// 리스트 위젯 열 때 초기화해줘야할 변수
	UPROPERTY()
	EEquipmentSlot CurrentEquipSlot;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUniformGridPanel> FilterContainer;

	UPROPERTY()
	TSubclassOf<class UFilterButtonWidget> FilterButtonClass;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> SlotText;

	// 아이템 버튼 관련
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUniformGridPanel> ItemContainer;

	const int32 ContainerWidth = 5;

	UPROPERTY()
	TSubclassOf<class UItemButtonWidget> ItemButtonClass;

	UPROPERTY()
	TArray<TObjectPtr<class UItemButtonWidget>> ItemButtons;

	// 아이템 정보 창
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> InfoWindow;
};
