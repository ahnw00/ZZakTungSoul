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
	UE_LOG(LogTemp, Log, TEXT("MenuBtn Clicked"));
	
	if (UMenuWidget* MenuWidget = Cast<UMenuWidget>(ParentWidget))
	{
		MenuWidget->OpenMenu(MenuType);
	}
}
