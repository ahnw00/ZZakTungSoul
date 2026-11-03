// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemDataAsset.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Weapon,      // 무기와 방패 그리고 활 등
	Armor,       // 방어구
	Consumable,  // 소모 아이템(투척 아이템(다 쓰면 인벤토리에서 제거), 물약(다 써도 0으로 남아있어야 함))
	KeyItem		 // 열쇠 같은 사용 아이템
};

UENUM(BlueprintType)
enum class EEquipmentSlot : uint8
{
	None,
	RightHand,
	LeftHand,
	Head,
	Chest,
	Arms,
	Legs,
	UpperBelt,
	LowerBelt
};

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API UItemDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("ItemData", GetFName());
	}
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGuid InstanceID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName ItemID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText ItemName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EItemType ItemType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<USkeletalMesh> SkeletalMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector RelaiveLocation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FRotator EquipRotation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector RelativeScale = FVector(1.f, 1.f, 1.f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MaxStack = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<EEquipmentSlot> AllowedSlots;
};
