// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemData/WeaponDataAsset.h"

UWeaponDataAsset::UWeaponDataAsset()
{
	ItemType = EItemType::Weapon;

	AllowedSlots.Add(EEquipmentSlot::RightHand);
	AllowedSlots.Add(EEquipmentSlot::LeftHand);
}
