// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/FilterButtonWidget.h"
#include <Components/Button.h>


void UFilterButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	FilterButton->OnClicked.AddDynamic(this, &UFilterButtonWidget::OnButtonClicked);
}

void UFilterButtonWidget::OnButtonClicked()
{
	if (!ParentWidget)
	{
		return;
	}

	if (UItemListWidget* ItemList = Cast<UItemListWidget>(ParentWidget))
	{
		//ItemList->InitializeItemList()
		//if (ListMode == EItemListMode::Inventory)
		//{
		//	
		//}
		if (FilterType == EItemListFilter::All)
		{
			ItemList->ListAllItems();
		}
		else if (FilterType == EItemListFilter::Consumable)
		{
			ItemList->ListItemsByType(EItemType::Consumable);
		}
		else if (FilterType == EItemListFilter::Weapon)
		{
			ItemList->ListItemsByType(EItemType::Weapon);
		}
		else if (FilterType == EItemListFilter::Armor)
		{
			ItemList->ListItemsByType(EItemType::Armor);
		}
		else if (FilterType == EItemListFilter::RightHand)
		{
			ItemList->ListItemsBySlot(EEquipmentSlot::RightHand);
		}
	}
}
