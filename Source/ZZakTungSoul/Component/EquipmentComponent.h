// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <ItemData/ItemDataAsset.h>
#include "EquipmentComponent.generated.h"

DECLARE_TS_MULTICAST_DELEGATE_TwoParams(FOnEquipItem, FGuid, EEquipmentSlot);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZZAKTUNGSOUL_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEquipmentComponent();

public:
	void EquipItem(FGuid InstanceID, EEquipmentSlot Slot);

	void UnequipItem(EEquipmentSlot Slot);

	FORCEINLINE FGuid GetEquippedItem(EEquipmentSlot Slot) const 
	{
		if (EquippedItems.IsEmpty() || !EquippedItems[Slot].IsValid())
		{
			return FGuid();
		}

		return EquippedItems[Slot]; 
	}

public:
	FOnEquipItem OnEquipItem;

private:
	UPROPERTY()
	TMap<EEquipmentSlot, FGuid> EquippedItems;
};
