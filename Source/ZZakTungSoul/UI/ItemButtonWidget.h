// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <ItemData/ItemDataAsset.h>
#include "ItemButtonWidget.generated.h"

DECLARE_DELEGATE_OneParam(FOnItemSelected, UItemDataAsset* /*Showed Item Data*/)
DECLARE_DELEGATE_OneParam(FOnItemClicked, UItemDataAsset* /*Equip Item Data*/)

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API UItemButtonWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
public:
	FORCEINLINE void SetItemData(UItemDataAsset* InItemData) { ItemData = InItemData; }

	FORCEINLINE void SetEquipSlot(EEquipmentSlot InSlot) { EquipSlot = InSlot; }

	UFUNCTION()
	void OnButtonHovered();

	UFUNCTION()
	void OnButtonClicked();

	void ResetButtonImage();

public:
	FOnItemSelected OnItemSelected;
	FOnItemClicked OnItemClicked;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> ItemButton;

private:
	UPROPERTY()
	TObjectPtr<UItemDataAsset> ItemData;

	UPROPERTY()
	EEquipmentSlot EquipSlot = EEquipmentSlot::None;
};
