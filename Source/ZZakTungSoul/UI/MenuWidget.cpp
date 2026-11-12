// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MenuWidget.h"
#include <UI/ItemListWidget.h>
#include <Component/EquipmentComponent.h>
#include <Component/InventoryComponent.h>
#include <Kismet/GameplayStatics.h>


void UMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

    UE_LOG(LogTemp, Warning, TEXT("MenuWidget: %p"), this);
    UE_LOG(LogTemp, Warning, TEXT("Before Add: %d"), Buttons.Num());

    // 제일 윗줄 버튼들
    Buttons.Add(InventoryButton);
    Buttons.Add(StatButton);
    Buttons.Add(SettingButton);
    // 두번째 줄 버튼들
    Buttons.Add(RightWeaponButton);
    Buttons.Add(LeftWeaponButton);
    // 세번째 줄 버튼들
    Buttons.Add(HeadButton);
    Buttons.Add(ChestButton);
    Buttons.Add(ArmsButton);
    Buttons.Add(LegsButton);
    // 네번째 줄 버튼들
    Buttons.Add(UpperBeltButton);
    Buttons.Add(LowerBeltButton);

    UE_LOG(LogTemp, Warning, TEXT("After Add: %d"), Buttons.Num());

    for (const auto Button : Buttons)
    {
        Button->SetParentWidget(this);
    }

    if (Equipment)
    {
        Equipment->OnEquipItem.AddUObject(this, &UMenuWidget::ChangeButtonsImage);
    }

    //ChangeButtonsImage();
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
        WBP_ItemListWidget->SetSlotText(EItemListFilter::All);
        WBP_ItemListWidget->ResetInfo();
        break;

    case EMenuType::Stat:
        break;

    case EMenuType::Settings:
        UGameplayStatics::OpenLevel(this, TEXT("BaeTestMainMenuMap"));
        break;

    case EMenuType::RightHand:
        WBP_ItemListWidget->SetListMode(EItemListMode::EquipmentSlot);
        WBP_ItemListWidget->InitializeItemList(EItemListFilter::RightHand);
        WBP_ItemListWidget->SetEquipSlot(EEquipmentSlot::RightHand);
        WBP_ItemListWidget->SetSlotText(EItemListFilter::RightHand);
        WBP_ItemListWidget->ResetInfo();
        break;

    case EMenuType::LeftHand:
        WBP_ItemListWidget->SetListMode(EItemListMode::EquipmentSlot);
        WBP_ItemListWidget->InitializeItemList(EItemListFilter::LeftHand);
        WBP_ItemListWidget->SetEquipSlot(EEquipmentSlot::LeftHand);
        WBP_ItemListWidget->SetSlotText(EItemListFilter::LeftHand);
        WBP_ItemListWidget->ResetInfo();
        break;

    case EMenuType::Head:
        WBP_ItemListWidget->SetListMode(EItemListMode::EquipmentSlot);
        WBP_ItemListWidget->InitializeItemList(EItemListFilter::Head);
        WBP_ItemListWidget->SetEquipSlot(EEquipmentSlot::Head);
        WBP_ItemListWidget->SetSlotText(EItemListFilter::Head);
        WBP_ItemListWidget->ResetInfo();
        break;

    case EMenuType::Chest:
        WBP_ItemListWidget->SetListMode(EItemListMode::EquipmentSlot);
        WBP_ItemListWidget->InitializeItemList(EItemListFilter::Chest);
        WBP_ItemListWidget->SetEquipSlot(EEquipmentSlot::Chest);
        WBP_ItemListWidget->SetSlotText(EItemListFilter::Chest);
        WBP_ItemListWidget->ResetInfo();
        break;

    case EMenuType::Arms:
        WBP_ItemListWidget->SetListMode(EItemListMode::EquipmentSlot);
        WBP_ItemListWidget->InitializeItemList(EItemListFilter::Arms);
        WBP_ItemListWidget->SetEquipSlot(EEquipmentSlot::Arms);
        WBP_ItemListWidget->SetSlotText(EItemListFilter::Arms);
        WBP_ItemListWidget->ResetInfo();
        break;

    case EMenuType::Legs:
        WBP_ItemListWidget->SetListMode(EItemListMode::EquipmentSlot);
        WBP_ItemListWidget->InitializeItemList(EItemListFilter::Legs);
        WBP_ItemListWidget->SetEquipSlot(EEquipmentSlot::Legs);
        WBP_ItemListWidget->SetSlotText(EItemListFilter::Legs);
        WBP_ItemListWidget->ResetInfo();
        break;

    case EMenuType::UpperBelt:
        WBP_ItemListWidget->SetListMode(EItemListMode::BeltSlot);
        WBP_ItemListWidget->InitializeItemList(EItemListFilter::UpperBelt);
        WBP_ItemListWidget->SetEquipSlot(EEquipmentSlot::UpperBelt);
        WBP_ItemListWidget->SetSlotText(EItemListFilter::UpperBelt);
        WBP_ItemListWidget->ResetInfo();
        break;

    case EMenuType::LowerBelt:
        WBP_ItemListWidget->SetListMode(EItemListMode::BeltSlot);
        WBP_ItemListWidget->InitializeItemList(EItemListFilter::LowerBelt);
        WBP_ItemListWidget->SetEquipSlot(EEquipmentSlot::LowerBelt);
        WBP_ItemListWidget->SetSlotText(EItemListFilter::LowerBelt);
        WBP_ItemListWidget->ResetInfo();
        break;

    default:
        break;
    }
}

void UMenuWidget::SetInventory(UInventoryComponent* InInventory)
{
    Inventory = InInventory;
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

void UMenuWidget::ChangeButtonsImage()
{
    if (Buttons.Num() == 0)
    {
        return;
    }

    if (!Equipment)
    {
        return;
    }

    for (const auto Button : Buttons)
    {
        EMenuType ButtonType = Button->MenuType;
        FGuid InstanceID;

        switch (ButtonType)
        {
        case EMenuType::Inventory:
            continue;
        case EMenuType::Stat:
            continue;
        case EMenuType::Settings:
            continue;
        case EMenuType::RightHand:
            InstanceID = Equipment->GetEquippedItem(EEquipmentSlot::RightHand);
            break;
        case EMenuType::LeftHand:
            InstanceID = Equipment->GetEquippedItem(EEquipmentSlot::LeftHand);
            break;
        case EMenuType::Head:
            InstanceID = Equipment->GetEquippedItem(EEquipmentSlot::Head);
            break;
        case EMenuType::Chest:
            InstanceID = Equipment->GetEquippedItem(EEquipmentSlot::Chest);
            break;
        case EMenuType::Arms:
            InstanceID = Equipment->GetEquippedItem(EEquipmentSlot::Arms);
            break;
        case EMenuType::Legs:
            InstanceID = Equipment->GetEquippedItem(EEquipmentSlot::Legs);
            break;
        case EMenuType::UpperBelt:
            InstanceID = Equipment->GetEquippedItem(EEquipmentSlot::UpperBelt);
            break;
        case EMenuType::LowerBelt:
            InstanceID = Equipment->GetEquippedItem(EEquipmentSlot::LowerBelt);
            break;
        default:
            break;
        }

        UTexture2D* InTexture = GetTexture(InstanceID);
        SetButtonImage(Buttons[static_cast<int32>(ButtonType) - 1], InTexture);
    }
}

UTexture2D* UMenuWidget::GetTexture(FGuid InstanceID)
{
    if (InstanceID == FGuid())
    {
        return nullptr;
    }

    const FInventoryItem* Item = Inventory->FindItemByInstanceID(InstanceID);
    if (!Item)
    {
        return nullptr;
    }

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

void UMenuWidget::SetButtonImage(UMenuButtonWidget* Button, UTexture2D* InTexture)
{
    if (!Button)
    {
        return;
    }

    Button->SetItemImage(InTexture);
}
