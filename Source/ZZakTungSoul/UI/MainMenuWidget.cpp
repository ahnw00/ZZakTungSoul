// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenuWidget.h"

#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetSystemLibrary.h>

#include <Components/Button.h>

void UMainMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// 버튼 클릭 이벤트 바인딩.
	if (Button_NewGame)
	{
		Button_NewGame->OnClicked.AddDynamic(this, &UMainMenuWidget::OnNewGameClicked);
	}
	if (Button_LoadGame)
	{
		Button_LoadGame->OnClicked.AddDynamic(this, &UMainMenuWidget::OnLoadGameClicked);
	}
	if (Button_ExitGame)
	{
		Button_ExitGame->OnClicked.AddDynamic(this, &UMainMenuWidget::OnExitGameClicked);
	}
}

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 세이브 파일 없으면 LoadGame 버튼 비활성.
	if (Button_LoadGame)
	{
		const bool bHasSaveData = UGameplayStatics::DoesSaveGameExist(SlotName, 0);
		Button_LoadGame->SetIsEnabled(bHasSaveData);
	}
}

void UMainMenuWidget::OnNewGameClicked()
{
	// 새게임 시작, 세이브 슬롯 있으면 초기화.
	if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		UGameplayStatics::DeleteGameInSlot(SlotName, 0);
	}

	// [입력 모드 복구]
	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->SetShowMouseCursor(false);
		PC->SetInputMode(FInputModeGameOnly());
	}

	// [위젯 제거]
	RemoveFromParent();

	// 인게임 맵으로 레벨 전환.
	UGameplayStatics::OpenLevel(this, InGameMapName);
}

void UMainMenuWidget::OnLoadGameClicked()
{
	// 세이브 파일 없으면 무시.
	if (!UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("세이브 데이터 없음."));
		return;
	}

	// [입력 모드 복구]
	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->SetShowMouseCursor(false);
		PC->SetInputMode(FInputModeGameOnly());
	}

	// [위젯 제거]
	RemoveFromParent();


	// 세이브 데이터 있으면 인게임 맵으로 전환.
	UGameplayStatics::OpenLevel(this, InGameMapName);
}

void UMainMenuWidget::OnExitGameClicked()
{
	// 게임 완전 종료.
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}
