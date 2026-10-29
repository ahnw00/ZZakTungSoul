// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/InventoryComponent.h"
#include <ItemData/ItemDataAsset.h>

#include <Engine/AssetManager.h>


// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// ...
}

void UInventoryComponent::AddItem(UItemDataAsset* ItemData)
{
	if (!ItemData)
		return;

	FInventoryItem AddedItem;
	AddedItem.InstanceID = FGuid::NewGuid();
	ItemData->InstanceID = AddedItem.InstanceID;

	AddedItem.AssetID = ItemData->GetPrimaryAssetId();
	// Todo: 여러개 가질 수 있는 아이템 처리

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

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

