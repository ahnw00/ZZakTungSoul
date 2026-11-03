// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <ItemData/ItemDataAsset.h>
#include "EquipmentComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnEquipItem, FGuid, EEquipmentSlot);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnUnequipItem, EEquipmentSlot);
DECLARE_MULTICAST_DELEGATE(FOnEquipItemImageChanged);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZZAKTUNGSOUL_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEquipmentComponent();

protected:
	virtual void InitializeComponent() override;

public:
	void EquipItem(FGuid InstanceID, EEquipmentSlot Slot);

	void UnequipItem(EEquipmentSlot Slot);

	FORCEINLINE FGuid GetEquippedItem(EEquipmentSlot Slot) const 
	{
		const FGuid* FoundID = EquippedItems.Find(Slot);

		if (!FoundID || !FoundID->IsValid())
		{
			return FGuid();
		}

		return *FoundID;
	}

	EEquipmentSlot FindOverlappedItem(FGuid TargetID, EEquipmentSlot InSlot);

public:
	FOnEquipItem OnEquipItem;
	FOnUnequipItem OnUnequipItem;
	FOnEquipItemImageChanged OnEquipItemImageChanged;

private:
	UPROPERTY()
	TMap<EEquipmentSlot, FGuid> EquippedItems;
};
