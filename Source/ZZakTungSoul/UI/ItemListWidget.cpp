// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ItemListWidget.h"

void UItemListWidget::InitializeItemList(EItemListMode InMode, EItemListFilter InInitialFilter)
{
}

void UItemListWidget::ListItemsByType(EItemType InType)
{
}

void UItemListWidget::ListItemsBySlot(EEquipmentSlot InSlot)
{
}

void UItemListWidget::CreateFilterButtons()
{
    // Todo: 필터 버튼 담을 컨테이너 초기화

    TArray<EItemListFilter> Filters;

    if (ListMode == EItemListMode::EquipmentSlot)
    {
        Filters = {
            EItemListFilter::LeftHand,
            EItemListFilter::RightHand,
            EItemListFilter::Helmet,
            EItemListFilter::Chest,
            EItemListFilter::Arms,
            EItemListFilter::Legs
        };
    }
    else
    {
        Filters = {
            EItemListFilter::All,
            EItemListFilter::Consumable,
            EItemListFilter::Weapon,
            EItemListFilter::Armor
        };
    }

    for (EItemListFilter Filter : Filters)
    {
        // 필터 버튼 위젯 생성
        // Filter 값을 버튼에 전달
        // FilterContainer에 추가
    }
}
