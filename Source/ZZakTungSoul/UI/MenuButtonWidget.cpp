// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MenuButtonWidget.h"
#include <UI/MenuWidget.h>
#include <Components/Button.h>


void UMenuButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	MenuButton->OnClicked.AddUniqueDynamic(
		this, &UMenuButtonWidget::OnButtonClicked
	);
}

void UMenuButtonWidget::OnButtonClicked()
{
	if (UMenuWidget* MenuWidget = Cast<UMenuWidget>(ParentWidget))
	{
		MenuWidget->OpenMenu(MenuType);
	}
}

void UMenuButtonWidget::ResetButtonImage()
{
	FButtonStyle Style = MenuButton->GetStyle();

	Style.Normal.SetResourceObject(nullptr);

	MenuButton->SetStyle(Style);
}

void UMenuButtonWidget::SetButtonImage(UTexture2D* InTexture)
{
	FButtonStyle Style = MenuButton->GetStyle();

	Style.Normal.SetResourceObject(InTexture);

	MenuButton->SetStyle(Style);
}
