// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/EquipmentComponent.h"

// Sets default values for this component's properties
UEquipmentComponent::UEquipmentComponent()
{

	// ...
}

void UEquipmentComponent::EquipItem(FGuid InstanceID, EEquipmentSlot Slot)
{
	if (EquippedItems[Slot] != InstanceID)
	{
		EquippedItems[Slot] = InstanceID;
	}
}

void UEquipmentComponent::UnequipItem(EEquipmentSlot Slot)
{
	EquippedItems.Remove(Slot);
}
