// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BonfireMenuWidget.h"
#include "Components/Button.h"
#include "Warrior/WarriorController.h"
#include <UI/LevelUpWidget.h>
#include <UI/TeleportWidget.h>


void UBonfireMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// 메인 메뉴 버튼 바인딩.
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

	// 서브메뉴 초기화 및 취소 이벤트 바인딩.
	if (LevelUpWidget)
	{
		LevelUpWidget->OnLevelUpCanceled.AddDynamic(this, &UBonfireMenuWidget::OnSubMenuClosed);
	}
	if (TeleportWidget)
	{
		TeleportWidget->OnTeleportCanceled.AddDynamic(this, &UBonfireMenuWidget::OnSubMenuClosed);
	}
}

void UBonfireMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 메뉴가 열릴 때마다 이전 서브메뉴들을 모두 숨기고 메인 메뉴로 리셋.
	ShowSubMenu(nullptr);
}


void UBonfireMenuWidget::ShowSubMenu(UWidget* Target)
{
	// 등록 서브메뉴 순회하며 대상만 키고 나머지는 숨김.
	for (UWidget* SubMenu : { static_cast<UWidget*>(LevelUpWidget), static_cast<UWidget*>(TeleportWidget) })
	{
		if (SubMenu)
		{
			SubMenu->SetVisibility(SubMenu == Target ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
		}
	}
}

void UBonfireMenuWidget::OnSubMenuClosed()
{
	ShowSubMenu(nullptr);
}

void UBonfireMenuWidget::OnLevelUpClicked()
{
	if (LevelUpWidget)
	{
		LevelUpWidget->InitializedLevelUp();
	}
	ShowSubMenu(LevelUpWidget);
}

void UBonfireMenuWidget::OnTeleportClicked()
{
	if (TeleportWidget)
	{
		TeleportWidget->RefreshTeleportList();
	}
	ShowSubMenu(TeleportWidget);
}



void UBonfireMenuWidget::OnLeaveClicked()
{
	if (AWarriorController* PC = Cast<AWarriorController>(GetOwningPlayer()))
	{
		PC->CloseBonfireMenu();
	}
}

bool UBonfireMenuWidget::CloseCurrentSubMenu()
{
	// 레벨업 창이 열려 있는 경우.
	if (LevelUpWidget && LevelUpWidget->GetVisibility() == ESlateVisibility::Visible)
	{
		ShowSubMenu(nullptr);
		return true;
	}


	// 텔레포트 창이 열려 있는 경우.
	if (TeleportWidget && TeleportWidget->GetVisibility() == ESlateVisibility::Visible)
	{
		ShowSubMenu(nullptr);
		return true;
	}

	// 열려 있는 서브 메뉴 없음.
	return false;
}
