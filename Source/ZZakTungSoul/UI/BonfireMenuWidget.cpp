// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BonfireMenuWidget.h"
#include "Components/Button.h"
#include "Warrior/WarriorController.h"

void UBonfireMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (Button_LevelUp)
	{
		Button_LevelUp->OnClicked.AddDynamic(this, &UBonfireMenuWidget::OnLevelUpClicked);
	}

	if (Button_Teleport)
	{
		Button_Teleport->OnClicked.AddDynamic(this, &UBonfireMenuWidget::OnTeleportClicked);
	}

	if (Button_Leave)
	{
		Button_Leave->OnClicked.AddDynamic(this, &UBonfireMenuWidget::OnLeaveClicked);
	}
}

void UBonfireMenuWidget::OnLevelUpClicked()
{
	if (AWarriorController* PC = Cast<AWarriorController>(GetOwningPlayer()))
	{
		PC->ToggleStatusWindow();
	}

}

void UBonfireMenuWidget::OnTeleportClicked()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, TEXT("[화톳불] 텔레포트"));
}

void UBonfireMenuWidget::OnLeaveClicked()
{
	if (AWarriorController* PC = Cast<AWarriorController>(GetOwningPlayer()))
	{
		PC->CloseBonfireMenu();
	}
}
