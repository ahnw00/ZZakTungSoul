// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BonfireMenuWidget.generated.h"

// 전방 선언.
class UButton;
class ULevelUpWidget;
class UTeleportWidget;
class UWidget;

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API UBonfireMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// 열려 있는 서브 메뉴 있다면 닫고 true, 없으면 false.
	bool CloseCurrentSubMenu();


protected:

	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	// 버튼 입력시 콜백 함수.
	UFUNCTION()
	void OnLevelUpClicked();
	UFUNCTION()
	void OnTeleportClicked();
	UFUNCTION()
	void OnLeaveClicked();

	// 서브메뉴 취소시 창 닫기 콜백.
	UFUNCTION()
	void OnSubMenuClosed();

	// 서브메뉴 화면 표시 통합 함수(Target이 nullptr이면 모두 숨김).
	void ShowSubMenu(UWidget* Target);

protected:
	// 에디터 연결 버튼.
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> Button_LevelUp;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> Button_Teleport;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> Button_Leave;

	// 서브메뉴 위젯(WBP_BonfireMenuWidget 자식으로 임베드 배치).
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<ULevelUpWidget> LevelUpWidget;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTeleportWidget> TeleportWidget;
};
