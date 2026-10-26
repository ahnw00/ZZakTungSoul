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
    int32 MenuIndex = static_cast<int32>(MenuType);

    // 위젯에 Switcher 아래에 있는 위젯들의 순서가 enum이랑 순서가 같아야함
    PanelSwitcher->SetActiveWidgetIndex(MenuIndex);

    if (MenuType == EMenuType::Inventory)
    {
        WBP_ItemListWidget->ListAllItems();
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
