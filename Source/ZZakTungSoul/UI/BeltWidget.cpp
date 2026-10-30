// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BeltWidget.h"
#include <ItemData/ItemDataAsset.h>
#include <Component/EquipmentComponent.h>
#include <Component/InventoryComponent.h>
#include <Components/Image.h>


void UBeltWidget::OnEquipmentChanged()
{
	FGuid InstanceID = Equipment->GetEquippedItem(EEquipmentSlot::UpperBelt);
    UpperBeltImage->SetBrushFromTexture(GetTexture(InstanceID));

    InstanceID = Equipment->GetEquippedItem(EEquipmentSlot::LowerBelt);
    LowerBeltImage->SetBrushFromTexture(GetTexture(InstanceID));
}

UTexture2D* UBeltWidget::GetTexture(FGuid InstanceID)
{
    if (InstanceID == FGuid())
    {
        return nullptr;
    }

    const FInventoryItem* Item = Inventory->FindItemByInstanceID(InstanceID);

    // FGuid로 아이템 데이터 가져오기
    UItemDataAsset* ItemData = Inventory->GetItemData(*Item);
    if (!ItemData)
    {
        return nullptr;
    }

    // 버튼에 아이템 아이콘 이미지 넣어주기
    auto ItemIcon = ItemData->Icon;
    if (ItemIcon.IsPending())
    {
        ItemIcon.LoadSynchronous();
    }

    return ItemIcon.Get();
}
