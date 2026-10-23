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

	AddedItem.ItemData = ItemData;
	// Todo: 여러개 가질 수 있는 아이템 처리

	Items.Add(AddedItem);

	// 아이템 획득 시 델리게이트 호출
	OnItemObtained.ExecuteIfBound(ItemData);
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

