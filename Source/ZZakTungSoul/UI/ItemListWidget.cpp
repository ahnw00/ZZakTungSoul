// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ItemListWidget.h"
#include "ItemButtonWidget.h"
#include "FilterButtonWidget.h"
#include <Component/InventoryComponent.h>
#include <Components/UniformGridPanel.h>
#include <Components/TextBlock.h>
#include <Components/CanvasPanel.h>
#include <Components/Image.h>

#include <Warrior/WarriorController.h>

#include <Engine/AssetManager.h>


void UItemListWidget::NativeConstruct()
{
    Super::NativeConstruct();

    //ListAllItems();
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

    static ConstructorHelpers::FClassFinder<UFilterButtonWidget> FilterButtonClassRef(
        TEXT("/Game/Blueprints/UI/WBP_FilterButton.WBP_FilterButton_C")
    );
    if (FilterButtonClassRef.Succeeded())
    {
        FilterButtonClass = FilterButtonClassRef.Class;
    }
}

void UItemListWidget::InitializeItemList(EItemListFilter InInitialFilter)
{
    CreateFilterButtons();
    ListItemsByFilter(InInitialFilter);
}

void UItemListWidget::ListItemsByFilter(EItemListFilter InFilter)
{
    ItemContainer->ClearChildren();

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

        if (IsAllowed(InFilter, ItemData))
        {
            AddItemButton(Item.InstanceID, Index);
            ++Index;
        }
    }
}

void UItemListWidget::CreateFilterButtons()
{
    // Todo: 필터 버튼 담을 컨테이너 초기화
    FilterContainer->ClearChildren();

    TArray<EItemListFilter> Filters;

    if(ListMode == EItemListMode::Inventory)
    {
        Filters = {
            EItemListFilter::All,
            EItemListFilter::Consumable,
            EItemListFilter::Weapon,
            EItemListFilter::Armor
        };
    }
    else if (ListMode == EItemListMode::EquipmentSlot)
    {
        Filters = {
            EItemListFilter::LeftHand,
            EItemListFilter::RightHand,
            EItemListFilter::Head,
            EItemListFilter::Chest,
            EItemListFilter::Arms,
            EItemListFilter::Legs
        };
    }
    else //if (ListMode == EItemListMode::BeltSlot)
    {
        Filters = {
            EItemListFilter::UpperBelt,
            EItemListFilter::LowerBelt
        };
    }

    int32 Column = 0;
    const UEnum* Enum = StaticEnum<EItemListFilter>();
    for (EItemListFilter Filter : Filters)
    {
        // 필터 버튼 위젯 생성
        // Filter 값을 버튼에 전달
        // FilterContainer에 추가
        UFilterButtonWidget* FilterButton = CreateWidget<UFilterButtonWidget>(this, FilterButtonClass);
        if (!FilterButton)
        {
            continue;
        }
        FilterButton->OnFilterSelected.AddUObject(this, &UItemListWidget::ListItemsByFilter);
        FilterButton->OnFilterClicked.BindUObject(this, &UItemListWidget::ResetInfo);
        FilterButton->SetFilterType(Filter);
        FilterButton->SetFilterText(Enum->GetDisplayNameTextByValue(static_cast<int64>(Filter)));

        FilterButton->OnFilterSelected.AddUObject(this, &UItemListWidget::SetEquipmentSlotByFilter);
        
        FilterContainer->AddChildToUniformGrid(FilterButton, 0, Column);
        ++Column;
    }
}

void UItemListWidget::AddItemButton(const FGuid& InstanceID, int32 Index)
{
    if (!ItemButtonClass || !ItemContainer)
    {
        return;
    }

    UItemButtonWidget* ItemButton = CreateWidget<UItemButtonWidget>(this, ItemButtonClass);
    if (!ItemButton)
    {
        return;
    }

    // 델리게이트에 ShowInfo 바인드해주기
    ItemButton->OnItemSelected.BindUObject(this, &UItemListWidget::ShowInfo);

    if (ListMode != EItemListMode::Inventory)
    {
        ItemButton->SetEquipSlot(CurrentEquipSlot);

        // OnItemClicked에 장비 착용 바인드해주기
        ItemButton->OnItemClicked.BindUObject(this, &UItemListWidget::EquipItem);
    }

    //ItemButton->SetItemData(InItemData);
    ItemButton->SetInstanceID(InstanceID);

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
        AddItemButton(Items[i].InstanceID, i);
    }
}

void UItemListWidget::ResetInfo()
{
    for (int32 i = 0; i < InfoWindow->GetChildrenCount(); ++i)
    {
        UWidget* Child = InfoWindow->GetChildAt(i);

        if (UTextBlock* TextBlock = Cast<UTextBlock>(Child))
        {
            if (TextBlock->GetName().StartsWith(TEXT("Txt_")))
            {
                TextBlock->SetText(FText::FromString(TEXT("--")));
            }
        }
        else if (UImage* Icon = Cast<UImage>(Child))
        {
            Icon->SetBrush(FSlateBrush());
        }
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

bool UItemListWidget::IsAllowed(EItemListFilter InFilterType, UItemDataAsset* ItemData)
{
    switch (InFilterType)
    {
    case EItemListFilter::All:
        return true;
        
    case EItemListFilter::Consumable:
        if (ItemData->ItemType == EItemType::Consumable)
        {
            return true;
        }
        break;

    case EItemListFilter::Weapon:
        if (ItemData->ItemType == EItemType::Weapon)
        {
            return true;
        }
        break;

    case EItemListFilter::Armor:
        if (ItemData->ItemType == EItemType::Armor)
        {
            return true;
        }
        break;

    case EItemListFilter::RightHand:
        if (ItemData->ItemType != EItemType::Armor)
        {
            return true;
        }
        break;

    case EItemListFilter::LeftHand:
        if (ItemData->ItemType != EItemType::Armor)
        {
            return true;
        }
        break;

    case EItemListFilter::Head:
        if (ItemData->AllowedSlots.Find(EEquipmentSlot::Head) != INDEX_NONE)
        {
            return true;
        }
        break;

    case EItemListFilter::Chest:
        if (ItemData->AllowedSlots.Find(EEquipmentSlot::Chest) != INDEX_NONE)
        {
            return true;
        }
        break;

    case EItemListFilter::Arms:
        if (ItemData->AllowedSlots.Find(EEquipmentSlot::Arms) != INDEX_NONE)
        {
            return true;
        }
        break;

    case EItemListFilter::Legs:
        if (ItemData->AllowedSlots.Find(EEquipmentSlot::Legs) != INDEX_NONE)
        {
            return true;
        }
        break;

    case EItemListFilter::UpperBelt:
        if (ItemData->AllowedSlots.Find(EEquipmentSlot::UpperBelt) != INDEX_NONE)
        {
            return true;
        }
        break;

    case EItemListFilter::LowerBelt:
        if (ItemData->AllowedSlots.Find(EEquipmentSlot::UpperBelt) != INDEX_NONE)
        {
            return true;
        }
        break;

    default:
        break;
    }

    return false;
}

void UItemListWidget::ShowInfo(const FGuid& InstanceID)
{
    // 정보 초기화
    ResetInfo();

    // InstanceID로 ItemData 가져오기
    const UItemDataAsset* ItemData = Inventory->FindItemDataByInstanceID(InstanceID);
    if (!ItemData)
    {
        return;
    }

    // 정보로 표시할 변수들 이름값으로 가져와
    for (TFieldIterator<FProperty> PropIt(ItemData->GetClass()); PropIt; ++PropIt)
    {
        // 프로퍼티가 텍스처인지 확인(이미지 넣어주는거)
        if (FSoftObjectProperty* SoftObjectProperty = CastField<FSoftObjectProperty>(*PropIt))
        {
            if (SoftObjectProperty->PropertyClass->IsChildOf(UTexture2D::StaticClass()))
            {
                const FSoftObjectPtr SoftObject = SoftObjectProperty->GetPropertyValue_InContainer(ItemData);
                if (SoftObject.IsPending())
                {
                    SoftObject.LoadSynchronous();
                }
                UTexture2D* Texture = Cast<UTexture2D>(SoftObject.Get());

                UImage* Icon = Cast<UImage>(GetWidgetFromName(TEXT("Icon")));
                if (Icon)
                {
                    Icon->SetBrushFromTexture(Texture);
                    continue;
                }
            }
        }

        // 속성의 이름 값
        const FName PropName(PropIt->GetName());

        const FName TextControlName = *FString::Printf(TEXT("Txt_%s"), *PropIt->GetName());

        UTextBlock* InfoTextBlock = Cast<UTextBlock>(GetWidgetFromName(TextControlName));
        if (!InfoTextBlock)
        {
            continue;
        }

        FText InfoText;
        
        if (FTextProperty* TextProperty = CastField<FTextProperty>(*PropIt))
        {
            // FText 프로퍼티 값 가져오기
            InfoText = TextProperty->GetPropertyValue_InContainer(ItemData);
        }
        else if (FFloatProperty* NumProperty = CastField<FFloatProperty>(*PropIt))
        {
            // float 프로퍼티 값 가져오기
            float InfoFloat = 0.f;
            InfoFloat = NumProperty->GetPropertyValue_InContainer(ItemData);

            InfoText = FText::FromString(FString::SanitizeFloat(InfoFloat));
        }

        InfoTextBlock->SetText(InfoText);
    }
}

void UItemListWidget::SetEquipmentSlotByFilter(EItemListFilter InFilter)
{
    switch (InFilter)
    {
    case EItemListFilter::RightHand:
        CurrentEquipSlot = EEquipmentSlot::RightHand;
        break;
    case EItemListFilter::LeftHand:
        CurrentEquipSlot = EEquipmentSlot::LeftHand;
        break;
    case EItemListFilter::Head:
        CurrentEquipSlot = EEquipmentSlot::Head;
        break;
    case EItemListFilter::Chest:
        CurrentEquipSlot = EEquipmentSlot::Chest;
        break;
    case EItemListFilter::Arms:
        CurrentEquipSlot = EEquipmentSlot::Arms;
        break;
    case EItemListFilter::Legs:
        CurrentEquipSlot = EEquipmentSlot::Legs;
        break;
    case EItemListFilter::UpperBelt:
        CurrentEquipSlot = EEquipmentSlot::UpperBelt;
        break;
    case EItemListFilter::LowerBelt:
        CurrentEquipSlot = EEquipmentSlot::LowerBelt;
        break;
    default:
        CurrentEquipSlot = EEquipmentSlot::None;
        break;
    }
}

void UItemListWidget::EquipItem(const FGuid& InstanceID)
{
    //equipmentcomponent->EquipItem(ItemData->InstanceID, CurrentEquipSlot);
    AWarriorController* WC = Cast<AWarriorController>(GetOwningPlayer());
    if (!WC)
    {
        return;
    }

    WC->EquipItem(InstanceID, CurrentEquipSlot);
    UE_LOG(LogTemp, Log, TEXT("Equip Item"));
}
