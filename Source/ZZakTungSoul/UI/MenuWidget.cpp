// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MenuWidget.h"
#include <UI/ItemListWidget.h>
#include <Component/InventoryComponent.h>


void UMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

    // 제일 윗줄 버튼들
    InventoryButton->SetParentWidget(this);
    StatButton->SetParentWidget(this);
    SettingButton->SetParentWidget(this);

    // 두번째 줄 버튼들
    RightWeaponButton->SetParentWidget(this);
    LeftWeaponButton->SetParentWidget(this);

    // 세번째 줄 버튼들
    HeadButton->SetParentWidget(this);
    ChestButton->SetParentWidget(this);
    ArmsButton->SetParentWidget(this);
    LegsButton->SetParentWidget(this);

    // 네번째 줄 버튼들
    UpperBeltButton->SetParentWidget(this);
    LowerBeltButton->SetParentWidget(this);
}

void UMenuWidget::OpenMenu(EMenuType MenuType)
{
    int32 MenuIndex = GetMenuIndex(MenuType);

    // 위젯에 Switcher 아래에 있는 위젯들의 순서가 enum이랑 순서가 같아야함
    PanelSwitcher->SetActiveWidgetIndex(MenuIndex);

    // @Todo: 인벤토리 창 말고도 다른 창들도 여기서 열어줘야해
    switch (MenuType)
    {
    case EMenuType::Menu:
        break;

    case EMenuType::Inventory:
        WBP_ItemListWidget->SetListMode(EItemListMode::Inventory);
        WBP_ItemListWidget->InitializeItemList(EItemListFilter::All);
        WBP_ItemListWidget->SetEquipSlot(EEquipmentSlot::None);
        WBP_ItemListWidget->ResetInfo();
        break;

    case EMenuType::Stat:
        break;

    case EMenuType::Settings:
        break;

    case EMenuType::RightHand:
        WBP_ItemListWidget->SetListMode(EItemListMode::EquipmentSlot);
        WBP_ItemListWidget->InitializeItemList(EItemListFilter::RightHand);
        WBP_ItemListWidget->SetEquipSlot(EEquipmentSlot::RightHand);
        WBP_ItemListWidget->ResetInfo();
        break;

    case EMenuType::LeftHand:
        WBP_ItemListWidget->SetListMode(EItemListMode::EquipmentSlot);
        WBP_ItemListWidget->InitializeItemList(EItemListFilter::LeftHand);
        WBP_ItemListWidget->SetEquipSlot(EEquipmentSlot::LeftHand);
        WBP_ItemListWidget->ResetInfo();
        break;

    case EMenuType::Head:
        WBP_ItemListWidget->SetListMode(EItemListMode::EquipmentSlot);
        WBP_ItemListWidget->InitializeItemList(EItemListFilter::Head);
        WBP_ItemListWidget->SetEquipSlot(EEquipmentSlot::Head);
        WBP_ItemListWidget->ResetInfo();
        break;

    case EMenuType::Chest:
        WBP_ItemListWidget->SetListMode(EItemListMode::EquipmentSlot);
        WBP_ItemListWidget->InitializeItemList(EItemListFilter::Chest);
        WBP_ItemListWidget->SetEquipSlot(EEquipmentSlot::Chest);
        WBP_ItemListWidget->ResetInfo();
        break;

    case EMenuType::Arms:
        WBP_ItemListWidget->SetListMode(EItemListMode::EquipmentSlot);
        WBP_ItemListWidget->InitializeItemList(EItemListFilter::Arms);
        WBP_ItemListWidget->SetEquipSlot(EEquipmentSlot::Arms);
        WBP_ItemListWidget->ResetInfo();
        break;

    case EMenuType::Legs:
        WBP_ItemListWidget->SetListMode(EItemListMode::EquipmentSlot);
        WBP_ItemListWidget->InitializeItemList(EItemListFilter::Legs);
        WBP_ItemListWidget->SetEquipSlot(EEquipmentSlot::Legs);
        WBP_ItemListWidget->ResetInfo();
        break;

    case EMenuType::UpperBelt:
        WBP_ItemListWidget->SetListMode(EItemListMode::EquipmentSlot);
        WBP_ItemListWidget->InitializeItemList(EItemListFilter::UpperBelt);
        WBP_ItemListWidget->SetEquipSlot(EEquipmentSlot::UpperBelt);
        WBP_ItemListWidget->ResetInfo();
        break;

    case EMenuType::LowerBelt:
        WBP_ItemListWidget->SetListMode(EItemListMode::EquipmentSlot);
        WBP_ItemListWidget->InitializeItemList(EItemListFilter::LowerBelt);
        WBP_ItemListWidget->SetEquipSlot(EEquipmentSlot::LowerBelt);
        WBP_ItemListWidget->ResetInfo();
        break;

    default:
        break;
    }
}

void UMenuWidget::SetInventory(UInventoryComponent* InInventory)
{
    WBP_ItemListWidget->SetInventory(InInventory);
}

bool UMenuWidget::ResetMenuWidget()
{
    if (PanelSwitcher->GetActiveWidgetIndex() != 0)
    {
        PanelSwitcher->SetActiveWidgetIndex(0);
        return false;
    }

    return true;
}

int32 UMenuWidget::GetMenuIndex(EMenuType InType)
{
    switch (InType)
    {
    case EMenuType::Menu:
        return 0;
    case EMenuType::Inventory:
        return 1;
    case EMenuType::Stat:
        return 2;
    case EMenuType::Settings:
        break;
    default:
        return 1;;
    }

    return 0;
}
