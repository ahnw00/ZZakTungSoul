// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuButtonWidget.h"
#include <Components/WidgetSwitcher.h>
#include "MenuWidget.generated.h"

DECLARE_DELEGATE_OneParam(FOnMenuBtnClickedDelegate, EMenuType /*MenuWidgetType*/);

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API UMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UFUNCTION()
	void OpenMenu(EMenuType MenuType);

	// BindWidget 하면 블루프린트에 있는 위젯이랑 변수 이름이 똑같아야 Bind 돼요!
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMenuButtonWidget> InventoryButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMenuButtonWidget> StatButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMenuButtonWidget> SettingButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMenuButtonWidget> LeftWeaponButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMenuButtonWidget> RightWeaponButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMenuButtonWidget> HeadButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMenuButtonWidget> ChestButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMenuButtonWidget> ArmsButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMenuButtonWidget> LegsButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMenuButtonWidget> UpperBeltButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMenuButtonWidget> LowerBeltButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> PanelSwitcher;

	//// 메뉴 타입별 위젯 클래스
	//UPROPERTY(EditDefaultsOnly, Category = "Menu")
	//TMap<EMenuType, TSubclassOf<UUserWidget>> MenuClasses;

	//// 생성된 위젯 인스턴스 보관
	//UPROPERTY(Transient)
	//TMap<EMenuType, TObjectPtr<UUserWidget>> MenuInstances;
};
