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

void UEquipmentComponent::BeginPlay()
{
    Super::BeginPlay();

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

    // 다른 슬롯에 장착되어있는 아이템을 장착하려고 하면
    // 그 전의 슬롯에서 해제해줘
    EEquipmentSlot OverlappedSlot = FindOverlappedItem(InstanceID, Slot);
    if (OverlappedSlot != EEquipmentSlot::None)
    {
        OnUnequipItem.Broadcast(OverlappedSlot);
    }

    EquippedItems.Add(Slot, InstanceID);

    OnEquipItem.Broadcast();
    OnEquipItemImageChanged.Broadcast();
}

void UEquipmentComponent::UnequipItem(EEquipmentSlot Slot)
{
	EquippedItems.Remove(Slot);
    OnEquipItemImageChanged.Broadcast();
}

EEquipmentSlot UEquipmentComponent::FindOverlappedItem(FGuid TargetID, EEquipmentSlot InSlot)
{
    for (TPair<EEquipmentSlot, FGuid>& Pair : EquippedItems)
    {
        if (Pair.Key != InSlot && Pair.Value == TargetID)
        {
            Pair.Value.Invalidate();
            return Pair.Key;
        }
    }

    return EEquipmentSlot::None;
}
