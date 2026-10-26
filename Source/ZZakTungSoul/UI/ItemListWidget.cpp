// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ItemListWidget.h"
#include "ItemButtonWidget.h"
#include <Component/InventoryComponent.h>
#include <Components/UniformGridPanel.h>

#include <Engine/AssetManager.h>


void UItemListWidget::NativeConstruct()
{
    Super::NativeConstruct();

    ListAllItems();
}

UItemListWidget::UItemListWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    static ConstructorHelpers::FClassFinder<UItemButtonWidget> ItemButtonClassRef(
        TEXT("/Game/Blueprints/UI/WBP_ItemButton.WBP_ItemButton_C")
    );
    if (ItemButtonClassRef.Succeeded())
    {
        ItemButtonClass = ItemButtonClassRef.Class;
    }
}

void UItemListWidget::InitializeItemList(EItemListMode InMode, EItemListFilter InInitialFilter)
{
}

void UItemListWidget::ListItemsByType(EItemType InType)
{
    TArray<FInventoryItem> Items = Inventory->GetItems();

    int32 Count = Items.Num();
    int32 Index = 0;
    for (const auto& Item : Items)
    {
        UItemDataAsset* ItemData = LoadItemData(Item);
        if (!ItemData)
        {
            continue;
        }

        // 나열하려는 타입의 아이템이 아니면 넘어가기
        if (ItemData->ItemType != InType)
        {
            continue;
        }

        AddItemButton(ItemData, Index);
        ++Index;
    }
}

void UItemListWidget::ListItemsBySlot(EEquipmentSlot InSlot)
{
}

//void UItemListWidget::ToggleItemListWidget()
//{
//    if (!IsInViewport())
//    {
//        AddToViewport();
//    }
//
//    if (GetVisibility() == ESlateVisibility::Visible)
//    {
//        SetVisibility(ESlateVisibility::Collapsed);
//        //bShowMouseCursor = false;
//        //SetInputMode(FInputModeGameOnly());
//    }
//    else
//    {
//        SetVisibility(ESlateVisibility::Visible);
//        //bShowMouseCursor = true;
//        //SetInputMode(FInputModeGameAndUI());
//    }
//}

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

void UItemListWidget::AddItemButton(UItemDataAsset* InItemData, int32 Index)
{
    if (!InItemData || !ItemButtonClass || !ItemContainer)
    {
        return;
    }

    UItemButtonWidget* ItemButton = CreateWidget<UItemButtonWidget>(this, ItemButtonClass);
    if (!ItemButton)
    {
        return;
    }

    ItemButton->SetItemData(InItemData);

    const int32 Row = Index / ContainerWidth;
    const int32 Column = Index % ContainerWidth;

    ItemContainer->AddChildToUniformGrid(ItemButton, Row, Column);
}

void UItemListWidget::ListAllItems()
{
    if (!Inventory)
    {
        return;
    }

    const TArray<FInventoryItem>& Items = Inventory->GetItems();
    int32 Count = Items.Num();

    for (int i = 0; i < Count; ++i)
    {
        UItemDataAsset* ItemData = LoadItemData(Items[i]);
        AddItemButton(ItemData, i);
    }
}

UItemDataAsset* UItemListWidget::LoadItemData(const FInventoryItem& Item)
{
    // 아이템 구조체를 통해 PrimaryAssetId 가져오기
    FPrimaryAssetId AssetId = Item.AssetID;

    UAssetManager& Manager = UAssetManager::Get();

    // AssetId 통해서 SoftObjectPtr(경로) 가져오기
    FSoftObjectPtr AssetPtr(Manager.GetPrimaryAssetPath(AssetId));

    // 아직 로드되지 않았다면
    if (AssetPtr.IsPending())
    {
        // 동기화로 로드
        AssetPtr.LoadSynchronous();
    }

    UItemDataAsset* ItemData = Cast<UItemDataAsset>(AssetPtr.Get());
    return ItemData;
}
