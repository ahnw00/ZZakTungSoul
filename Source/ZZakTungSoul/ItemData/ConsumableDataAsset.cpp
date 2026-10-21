// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemData/ConsumableDataAsset.h"

UConsumableDataAsset::UConsumableDataAsset()
{
	ItemType = EItemType::Consumable;

	AllowedSlots.Add(EEquipmentSlot::UpperBelt);
	AllowedSlots.Add(EEquipmentSlot::LowerBelt);
}
