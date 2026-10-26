// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <UI/ItemListWidget.h>
#include "FilterButtonWidget.generated.h"

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
	FORCEINLINE void SetParent(UUserWidget* InWidget)
	{
		ParentWidget = InWidget;
	}

	FORCEINLINE void SetFilterType(EItemListFilter InType)
	{
		FilterType = InType;
	}
	
private:
	UPROPERTY(EditAnywhere, Category = "ItemListWidget")
	TObjectPtr<UUserWidget> ParentWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> FilterButton;

	UPROPERTY()
	EItemListFilter FilterType;

	UPROPERTY()
	EItemListMode ListMode;
};
