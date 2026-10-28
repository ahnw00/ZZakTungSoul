// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <UI/ItemListWidget.h>
#include "FilterButtonWidget.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnFilterSelected, EItemListFilter /*FilterType*/);
DECLARE_DELEGATE(FOnFilterClicked);

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API UFilterButtonWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnButtonClicked();

public:
	void SetFilterText(FText InText);

public:
	FORCEINLINE void SetParent(UUserWidget* InWidget)
	{
		ParentWidget = InWidget;
	}

	FORCEINLINE void SetFilterType(EItemListFilter InType)
	{
		FilterType = InType;
	}

public:
	FOnFilterSelected OnFilterSelected;
	FOnFilterClicked OnFilterClicked;
	
private:
	UPROPERTY(EditAnywhere, Category = "ItemListWidget")
	TObjectPtr<UUserWidget> ParentWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> FilterButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> FilterText;

	UPROPERTY()
	EItemListFilter FilterType;

	UPROPERTY()
	EItemListMode ListMode;
};
