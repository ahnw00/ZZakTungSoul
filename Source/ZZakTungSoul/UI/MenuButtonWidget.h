// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuButtonWidget.generated.h"

// 메뉴 스위처에 이 순서로 위젯들이 자식으로 배치되어있어야해
UENUM(BlueprintType)
enum class EMenuType : uint8
{
	Menu,
	Inventory,
	Stat,
	Settings,
	RightHand,
	LeftHand,
	Head,
	Chest,
	Arms,
	Legs,
	UpperBelt,
	LowerBelt
};

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API UMenuButtonWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnButtonClicked();

public:
	FORCEINLINE void SetParentWidget(UUserWidget* InParentWidget) 
	{ 
		ParentWidget = InParentWidget; 
	}

public:
	void ResetButtonImage();

	void SetItemImage(UTexture2D* InTexture);

	void SetButtonImage();

public:
	// 버튼마다 열 메뉴의 타입을 꼭 에디터에서 지정해줘야함!!
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	EMenuType MenuType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	UTexture2D* ButtonImage;

private:
	UPROPERTY(VisibleAnywhere, Category = "Widget")
	UUserWidget* ParentWidget;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> MenuButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> MenuButtonImage;
};
