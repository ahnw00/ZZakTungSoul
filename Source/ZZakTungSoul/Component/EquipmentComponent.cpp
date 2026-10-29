// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/EquipmentComponent.h"

// Sets default values for this component's properties
UEquipmentComponent::UEquipmentComponent()
{
    
}

void UEquipmentComponent::InitializeComponent()
{
    Super::InitializeComponent();

}

void UEquipmentComponent::EquipItem(FGuid InstanceID, EEquipmentSlot Slot)
{
    // 이미 장착된건 또 장착하려고 하면 넘어가
    if (FGuid* EquippedID = EquippedItems.Find(Slot))
    {
        if (*EquippedID == InstanceID)
        {
            return;
        }
    }

    EquippedItems.Add(Slot, InstanceID);

    OnEquipItem.Broadcast(InstanceID, Slot);
    OnEquipItemImageChanged.Broadcast();
}

void UEquipmentComponent::UnequipItem(EEquipmentSlot Slot)
{
	EquippedItems.Remove(Slot);
}
