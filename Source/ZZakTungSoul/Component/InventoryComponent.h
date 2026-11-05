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
	FPrimaryAssetId AssetID;

	UPROPERTY()
	int32 Quantity = 1;

	UPROPERTY()
	int32 UpgradeLevel = 0;

	UPROPERTY()
	int32 UseCount = 0;
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
	void AddItem(UItemDataAsset* ItemData, FGuid InID);

	UItemDataAsset* GetItemData(const FInventoryItem& Item) const;

	const FInventoryItem* FindItemByInstanceID(const FGuid& InInstanceID) const;

	const FInventoryItem* FindItemByAssetID(const FPrimaryAssetId& AssetID) const;

	const UItemDataAsset* FindItemDataByInstanceID(const FGuid& InInstanceID) const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	FOnItemObtainedDelegate OnItemObtained;

private:
	UPROPERTY()
	TArray<FInventoryItem> Items;
};
