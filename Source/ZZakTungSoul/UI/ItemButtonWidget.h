// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemButtonWidget.generated.h"

class UItemDataAsset;

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API UItemButtonWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	FORCEINLINE void SetItemData(UItemDataAsset* InItemData) { ItemData = InItemData; }

	UFUNCTION(BlueprintCallable)
	void ShowInfo();

protected:
	//UPROPERTY(meta = (BindWidget))
	//TObjectPtr<Ubutton> ItemButton;

private:
	UPROPERTY()
	TObjectPtr<UItemDataAsset> ItemData;
};
