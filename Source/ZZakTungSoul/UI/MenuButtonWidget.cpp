// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MenuButtonWidget.h"
#include <Components/Button.h>


void UMenuButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	MenuButton->OnClicked.AddUniqueDynamic(
		this, &UMenuButtonWidget::HandleClicked
	);
}

void UMenuButtonWidget::HandleClicked()
{
	UE_LOG(LogTemp, Log, TEXT("MenuBtn Clicked"));
	OnMenuClicked.Broadcast(MenuType);
}
