// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/FilterButtonWidget.h"
#include <Components/Button.h>
#include <Components/TextBlock.h>


void UFilterButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	FilterButton->OnClicked.AddDynamic(this, &UFilterButtonWidget::OnButtonClicked);
}

void UFilterButtonWidget::OnButtonClicked()
{
	OnFilterSelected.ExecuteIfBound(FilterType);
	OnFilterClicked.ExecuteIfBound();
}

void UFilterButtonWidget::SetFilterText(FText InText)
{
	FilterText->SetText(InText);
}
