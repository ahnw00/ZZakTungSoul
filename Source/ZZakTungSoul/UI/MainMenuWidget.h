// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"



// 전방 선언.
class UButton;

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	// 버튼 클릭 핸들러.
	UFUNCTION()
	void OnNewGameClicked();
	UFUNCTION()
	void OnLoadGameClicked();
	UFUNCTION()
	void OnExitGameClicked();

protected:
	// UMG 바인딩 버튼.
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> Button_NewGame;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> Button_LoadGame;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> Button_ExitGame;

	// 이동할 메인 인게임 맵(기본값: BaeTestMap).
	UPROPERTY(EditDefaultsOnly, Category = "MainMenu|Map")
	FName InGameMapName = TEXT("BaeTestMap");

	// 세이브 슬롯.
	const FString SlotName = TEXT("SaveSlot");
	

};
