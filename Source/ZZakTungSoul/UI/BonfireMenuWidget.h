// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BonfireMenuWidget.generated.h"

// 전방 선언.
class UButton;

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API UBonfireMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	

protected:

	virtual void NativeOnInitialized() override;

	// 버튼 입력시 콜백 함수.
	UFUNCTION()
	void OnLevelUpClicked();
	UFUNCTION()
	void OnTeleportClicked();
	UFUNCTION()
	void OnLeaveClicked();

protected:
	// 에디터 연결 버튼.
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> Button_LevelUp;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> Button_Teleport;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> Button_Leave;
};
