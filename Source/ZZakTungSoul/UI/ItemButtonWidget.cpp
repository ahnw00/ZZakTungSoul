// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ItemButtonWidget.h"
#include <Components/Button.h>


void UItemButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ItemButton->OnHovered.AddDynamic(this, &UItemButtonWidget::OnButtonHovered);
	ItemButton->OnClicked.AddDynamic(this, &UItemButtonWidget::OnButtonClicked);
}

void UItemButtonWidget::OnButtonHovered()
{
	OnItemSelected.ExecuteIfBound(InstanceID);
}

void UItemButtonWidget::OnButtonClicked()
{
	OnItemClicked.ExecuteIfBound(InstanceID);
}

void UItemButtonWidget::ResetButtonImage()
{
	FButtonStyle Style = ItemButton->GetStyle();

	Style.Normal.SetResourceObject(nullptr);
	Style.Hovered.SetResourceObject(nullptr);
	Style.Pressed.SetResourceObject(nullptr);

	ItemButton->SetStyle(Style);
}