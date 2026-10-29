// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuButtonWidget.h"
#include <Components/WidgetSwitcher.h>
#include "MenuWidget.generated.h"

DECLARE_DELEGATE_OneParam(FOnMenuBtnClickedDelegate, EMenuType /*MenuWidgetType*/);

class UEquipmentComponent;

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API UMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

public:
	UFUNCTION()
	void OpenMenu(EMenuType MenuType);

	void SetInventory(class UInventoryComponent* InInventory);

	// 반환값이 true면 메뉴창 꺼줘야해
	// 반환값이 false면 메뉴창에서 한단계 더 들어간 위젯이 열려있는거라서
	// 다시 메뉴위젯으로 돌아와줘야해
	bool ResetMenuWidget();

	void ChangeButtonsImage();
	
	FORCEINLINE void SetEquipment(UEquipmentComponent* InEquipment)
	{
		Equipment = InEquipment;
	}

private:
	int32 GetMenuIndex(EMenuType InType);

	UTexture2D* GetTexture(FGuid InstanceID);

	void SetButtonImage(UMenuButtonWidget* Button, UTexture2D* InTexture);

	// BindWidget 하면 블루프린트에 있는 위젯이랑 변수 이름이 똑같아야 Bind 돼요!
protected:
	// 버튼들
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

	UPROPERTY()
	TArray<TObjectPtr<UMenuButtonWidget>> Buttons;

	// 패널 및 패널 스위처
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> PanelSwitcher;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UItemListWidget> WBP_ItemListWidget;

	UPROPERTY()
	TObjectPtr<UEquipmentComponent> Equipment;

	UPROPERTY()
	TObjectPtr<UInventoryComponent> Inventory;
};
