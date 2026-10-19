// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MenuWidget.h"

void UMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

    EquipmentButton->OnMenuClicked.AddUniqueDynamic(
        this, &UMenuWidget::OpenMenu);

    InventoryButton->OnMenuClicked.AddUniqueDynamic(
        this, &UMenuWidget::OpenMenu);

    StatButton->OnMenuClicked.AddUniqueDynamic(
        this, &UMenuWidget::OpenMenu);

    SettingButton->OnMenuClicked.AddUniqueDynamic(
        this, &UMenuWidget::OpenMenu);

    UpperBeltButton->OnMenuClicked.AddUniqueDynamic(
        this, &UMenuWidget::OpenMenu);

    LowerBeltButton->OnMenuClicked.AddUniqueDynamic(
        this, &UMenuWidget::OpenMenu);
}

void UMenuWidget::OpenMenu(EMenuType MenuType)
{
    int32 MenuIndex = static_cast<int32>(MenuType);

    // 위젯에 Switcher 아래에 있는 위젯들의 순서가 enum이랑 순서가 같아야함
    MenuSwitcher->SetActiveWidgetIndex(MenuIndex);
}
