// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/TeleportWidget.h"
#include <UI/BonfireMenuButtonWidget.h>
#include <Components/Button.h>
#include <Components/ScrollBox.h>
#include <Components/TextBlock.h>
#include <Warrior/WarriorController.h>
#include <Warrior/WarriorPlayerState.h>
#include <Interactable/BonFire.h>
#include <Kismet/GameplayStatics.h>
#include <EngineUtils.h>

void UTeleportWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// 취소 버튼 델리게이트 바인딩.
	if (Button_Cancel)
	{
		Button_Cancel->OnClicked.AddDynamic(this, &UTeleportWidget::OnCancelClicked);
	}
}

void UTeleportWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 화면 열릴 때마다 최신 목록 갱신.
	RefreshTeleportList();
}


UTeleportWidget::UTeleportWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<USoundBase> ButtonSoundRef(
		TEXT("/Game/Assets/Sound/cardboard.cardboard")
	);
	if (ButtonSoundRef.Succeeded())
	{
		ButtonSound = ButtonSoundRef.Object;
	}
}

void UTeleportWidget::RefreshTeleportList()
{
	if (!ScrollBox_BonfireList || !BonfireButtonClass)
	{
		return;
	}
	// 기존 목록 초기화.
	ScrollBox_BonfireList->ClearChildren();

	// 활성화 화톳불 데이터 가져오기.
	const TArray<FBonfireTeleportEntry> BonfireEntries = GetAvailableBonfires();

	// 각 화톳불마다 UI 버튼 동적 생성 및 바인딩.
	for (const FBonfireTeleportEntry& Entry : BonfireEntries)
	{
		UBonfireMenuButtonWidget* ButtonWidget = CreateWidget<UBonfireMenuButtonWidget>(this, BonfireButtonClass);
		if (ButtonWidget)
		{
			ButtonWidget->InitializeBonfireButton(Entry.BonfireID, Entry.DisplayName);
			
			// 버튼 클릭시 RequestTeleport 호출 연결.
			ButtonWidget->OnBonfireButtonClicked.AddDynamic(this, &UTeleportWidget::RequestTeleport);
			ButtonWidget->OnUIButtonClicked.BindUObject(this, &UTeleportWidget::PlayButtonSound);

			ScrollBox_BonfireList->AddChild(ButtonWidget);
		}
		
	}

}

TArray<FBonfireTeleportEntry> UTeleportWidget::GetAvailableBonfires() const
{
	TArray<FBonfireTeleportEntry> AvailableBonfires;

	AWarriorPlayerState* PS = GetOwningPlayerState<AWarriorPlayerState>();
	if (!PS)
	{
		return AvailableBonfires;
	}

	const TArray<FName>& LitIDs = PS->GetLitBonfireIds();
	if (LitIDs.Num() == 0)
	{
		return AvailableBonfires;
	}
	// 월드 내 화톳불 순회하면서 활성화된 화톳불 DisplayName 매칭.
	for (TActorIterator<ABonfire> It(GetWorld()); It; ++It)
	{
		const FName CurrentID = It->GetBonfireID();
		if (LitIDs.Contains(CurrentID))
		{
			FBonfireTeleportEntry Entry;
			Entry.BonfireID = CurrentID;
			Entry.DisplayName = It->GetBonfireDisplayName();

			// 에디터 상 이름 공란인 경우 ID를 기본 이름으로 대체.
			if (Entry.DisplayName.IsEmpty())
			{
				Entry.DisplayName = FText::FromName(CurrentID);
			}

			AvailableBonfires.Add(Entry);
		}
	}
	return AvailableBonfires;
}

void UTeleportWidget::RequestTeleport(const FName& TargetBonfireID)
{
	if (TargetBonfireID.IsNone())
	{
		return;
	}

	// 컨트롤러에게 해당 화톳불로 순간이동 요청.
	if (AWarriorController* PC = Cast<AWarriorController>(GetOwningPlayer()))
	{
		PC->TeleportToBonfire(TargetBonfireID);
	}
}

void UTeleportWidget::OnCancelClicked()
{
	// 메인 메뉴에게 취소 방송.
	OnTeleportCanceled.Broadcast();
}

void UTeleportWidget::PlayButtonSound()
{
	UGameplayStatics::PlaySound2D(this, ButtonSound);
}
