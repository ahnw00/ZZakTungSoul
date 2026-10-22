// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

// 전방 선언
class UItemDataAsset;

USTRUCT(BlueprintType)
struct FInventoryItem
{
	GENERATED_BODY()

	UPROPERTY()
	FGuid InstanceID;

	UPROPERTY()
	TObjectPtr<UItemDataAsset> ItemData = nullptr;

	UPROPERTY()
	int32 Quantity = 1;

	UPROPERTY()
	int32 UpgradeLevel = 0;
};

DECLARE_DELEGATE_OneParam(FOnItemObtainedDelegate, class UItemDataAsset* /*ObtainedItem*/)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZZAKTUNGSOUL_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

	FORCEINLINE const TArray<FInventoryItem>& GetItems() const { return Items; }
	void AddItem(UItemDataAsset* ItemData);

	FORCEINLINE UItemDataAsset* GetItemData(const FInventoryItem& item) const { return item.ItemData; };

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	FOnItemObtainedDelegate OnItemObtained;

private:
	UPROPERTY()
	TArray<FInventoryItem> Items;
};
