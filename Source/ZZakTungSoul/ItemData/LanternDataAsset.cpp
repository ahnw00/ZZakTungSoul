// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemData/LanternDataAsset.h"

ULanternDataAsset::ULanternDataAsset()
{
	ConsumableItemType = EConsumableItemType::Lantern;
}

void ULanternDataAsset::ItemFunction(AWarrior* User) const
{
	// 캐릭터의 등불 컴포넌트에 접근해서 켜주는 함수 호출
	User->ToggleLantern();
}
