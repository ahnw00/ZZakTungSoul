// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <ItemData/ItemDataAsset.h>
#include "ItemButtonWidget.generated.h"

DECLARE_DELEGATE_OneParam(FOnItemSelected, const FGuid& /*Showed Item ID*/)
DECLARE_DELEGATE_OneParam(FOnItemClicked, const FGuid& /*Equip Item ID*/)

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

	FORCEINLINE const FGuid& GetInstanceID() const { return InstanceID; }

	FORCEINLINE void SetInstanceID(FGuid InID) { InstanceID = InID; }

	UFUNCTION()
	void OnButtonHovered();

	UFUNCTION()
	void OnButtonClicked();

	void ResetButtonImage();

	void SetItemImage(TSoftObjectPtr<UTexture2D> InTexture);

public:
	FOnItemSelected OnItemSelected;
	FOnItemClicked OnItemClicked;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> ItemButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> ItemImage;

private:
	UPROPERTY()
	TObjectPtr<UItemDataAsset> ItemData;

	UPROPERTY()
	FGuid InstanceID;

	UPROPERTY()
	EEquipmentSlot EquipSlot = EEquipmentSlot::None;
};
