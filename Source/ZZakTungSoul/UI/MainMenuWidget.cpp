// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenuWidget.h"

#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetSystemLibrary.h>

#include <Components/Button.h>

#include <UI/LoadingScreenWidget.h>

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
	
	// 로딩 화면을 거쳐 새 게임 시작(빈 옵션).
	StartGameWithLoadingScreen(TEXT(""));
}

void UMainMenuWidget::OnLoadGameClicked()
{
	// 세이브 파일 없으면 무시.
	if (!UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		UE_LOG(LogTemp, Log, TEXT("세이브 데이터 없음."));
		return;
	}

	// 로딩화면을 거쳐 세이브 데이터 로드(LoadGame=1 옵션).
	StartGameWithLoadingScreen(TEXT("LoadGame=1"));
}



void UMainMenuWidget::StartGameWithLoadingScreen(const FString& InMapLoadOption)
{
	// 마우스 커서 숨김 및 입력 모드 설정.
	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->SetShowMouseCursor(false);
		PC->SetInputMode(FInputModeUIOnly());
	}

	// 1. 로딩 화면 위젯 클래스가 지정되어 있으면 위젯 띄우고 비동기 로딩 시작.
	if (LoadingScreenWidgetClass)
	{
		ULoadingScreenWidget* LoadingWidget = CreateWidget<ULoadingScreenWidget>(this, LoadingScreenWidgetClass);
		if (LoadingWidget)
		{
			// 화면 최상단에 노출(ZOrder 9999).
			LoadingWidget->AddToViewport(9999);
			LoadingWidget->StartAsyncLoading(InGameMapName, InMapLoadOption);

			// 메인 메뉴 위젯은 화면에서 제거.
			RemoveFromParent();
			return;
		}
	}

	// 2. 로딩 위젯 클래스가 미지정이면 즉시 OpenLevel(안전장치 Fallback).
	RemoveFromParent();
	UGameplayStatics::OpenLevel(this, InGameMapName, true, InMapLoadOption);
}

void UMainMenuWidget::OnExitGameClicked()
{
	// 게임 완전 종료.
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}