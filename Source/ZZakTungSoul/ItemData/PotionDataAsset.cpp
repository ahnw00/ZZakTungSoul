// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemData/PotionDataAsset.h"

UPotionDataAsset::UPotionDataAsset()
{
	ConsumableItemType = EConsumableItemType::Potion;
	
	RelativeScale = FVector(0.1f, 0.1f, 0.1f);
}

void UPotionDataAsset::ItemFunction(AWarrior* User) const
{
	// 캐릭터 체력 회복
	User->HpRecovery(30.f);
}
