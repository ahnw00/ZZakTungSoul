// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/InventoryComponent.h"
#include <ItemData/ItemDataAsset.h>

#include <Engine/AssetManager.h>


// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// ...
}

void UInventoryComponent::AddItem(UItemDataAsset* ItemData, FGuid InID)
{
	if (!ItemData)
		return;

	FInventoryItem AddedItem;

	// 여러개 가질 수 있는 아이템 처리를 위한 FGuid 값
	AddedItem.InstanceID = InID;

	AddedItem.AssetID = ItemData->GetPrimaryAssetId();

	Items.Add(AddedItem);

	// 아이템 획득 시 델리게이트 호출
	OnItemObtained.ExecuteIfBound(ItemData);
}

UItemDataAsset* UInventoryComponent::GetItemData(const FInventoryItem& Item) const
{
	UAssetManager& Manager = UAssetManager::Get();

	FSoftObjectPtr AssetPtr(Manager.GetPrimaryAssetPath(Item.AssetID));

	if (AssetPtr.IsPending())
	{
		AssetPtr.LoadSynchronous();
	}

	UItemDataAsset* ItemData = Cast<UItemDataAsset>(AssetPtr.Get());
	return ItemData;
}

const FInventoryItem* UInventoryComponent::FindItemByInstanceID(const FGuid& InInstanceID) const
{
	return Items.FindByPredicate(
		[&InInstanceID](const FInventoryItem& Item)
		{
			return Item.InstanceID == InInstanceID;
		}
	);
}

const UItemDataAsset* UInventoryComponent::FindItemDataByInstanceID(const FGuid& InInstanceID) const
{
	const FInventoryItem* Item = FindItemByInstanceID(InInstanceID);
	if (!Item)
	{
		return nullptr;
	}

	UItemDataAsset* ItemData = GetItemData(*Item);
	return ItemData;
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

