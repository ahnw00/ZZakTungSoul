// Fill out your copyright notice in the Description page of Project Settings.


#include "BonfireMenuButtonWidget.h"

#include <Components/Button.h>
#include <Components/TextBlock.h>



void UBonfireMenuButtonWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// 버튼 클릭 이벤트 바인딩.
	if (Button_Bonfire)
	{
		Button_Bonfire->OnClicked.AddDynamic(this, &UBonfireMenuButtonWidget::OnButtonClicked);
	}
}

void UBonfireMenuButtonWidget::InitializeBonfireButton(const FName& InBonfireID, const FText& InDisplayName)
{
	AssignedBonfireID = InBonfireID;

	// 버튼 텍스트 화톳불 표시 이름 설정.
	if (TextBlock_BonfireName)
	{
		TextBlock_BonfireName->SetText(InDisplayName);
	}
}


void UBonfireMenuButtonWidget::OnButtonClicked()
{
	if (!AssignedBonfireID.IsNone())
	{
		// 부모에게 클릭됨을 방송.
		OnBonfireButtonClicked.Broadcast(AssignedBonfireID);
		OnUIButtonClicked.ExecuteIfBound();
	}
}
