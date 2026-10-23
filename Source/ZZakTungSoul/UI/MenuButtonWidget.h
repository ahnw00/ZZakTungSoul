// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuButtonWidget.generated.h"

UENUM(BlueprintType)
enum class EMenuType : uint8
{
	Equipment,
	Inventory,
	Stat,
	Settings,
	UpperBelt,
	LowerBelt
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnMenuClickedDelegate, EMenuType, MenuWidgetType);

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API UMenuButtonWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void HandleClicked();

public:
	UPROPERTY(BlueprintAssignable)
	FOnMenuClickedDelegate OnMenuClicked;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMenuType MenuType;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> MenuButton;
};
