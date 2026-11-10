// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MenuButtonWidget.h"
#include <UI/MenuWidget.h>
#include <Components/Button.h>
#include <Components/Image.h>


void UMenuButtonWidget::NativePreConstruct()
{
	SetButtonImage();
	ResetButtonImage();
}

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

// 버튼 위에 있는 아이템 이미지 초기화
void UMenuButtonWidget::ResetButtonImage()
{
	MenuButtonImage->SetColorAndOpacity(FLinearColor::Transparent);
}

// 버튼 위에 있는 아이템 이미지 세팅
void UMenuButtonWidget::SetItemImage(UTexture2D* InTexture)
{
	if (!InTexture)
	{
		ResetButtonImage();
		return;
	}
	MenuButtonImage->SetBrushFromTexture(InTexture);
	MenuButtonImage->SetColorAndOpacity(FLinearColor::White);
}

// 초기 버튼 기본 이미지 세팅
void UMenuButtonWidget::SetButtonImage()
{
	FButtonStyle Style = MenuButton->GetStyle();

	Style.Normal.SetResourceObject(ButtonImage);
	Style.Hovered.SetResourceObject(ButtonImage);
	Style.Pressed.SetResourceObject(ButtonImage);

	MenuButton->SetStyle(Style);
	//MenuButtonImage->SetBrushFromSoftTexture(InTexture);
}
