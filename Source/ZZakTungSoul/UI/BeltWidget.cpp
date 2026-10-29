// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BeltWidget.h"
#include <ItemData/ItemDataAsset.h>
#include <Components/Image.h>


void UBeltWidget::OnEquipmentChanged(UItemDataAsset* InItemData, EEquipmentSlot InSlot)
{
	if (InSlot != EEquipmentSlot::UpperBelt && 
		InSlot != EEquipmentSlot::LowerBelt)
	{
		return;
	}

	if (!InItemData)
	{
		return;
	}

	// Icon을 TSoftObjectPtr로 가지고 있어서 로드 안되어있다면 로드
	if (InItemData->Icon.IsPending())
	{
		InItemData->Icon.LoadSynchronous();
	}

	if (InSlot == EEquipmentSlot::UpperBelt)
	{
		UpperBeltImage->SetBrushFromTexture(InItemData->Icon.Get());
	}
	else if (InSlot == EEquipmentSlot::LowerBelt)
	{
		LowerBeltImage->SetBrushFromTexture(InItemData->Icon.Get());
	}
}
