// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void UpdateHP(float CurrentHP, float MaxHP);
	UFUNCTION()
	void UpdateFP(float CurrentFP, float MaxFP);
	UFUNCTION()
	void UpdateStamina(float CurrentStamina, float MaxStamina);

protected:
	// [BindWidget Progress Bar]
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UProgressBar> ProgressBar_HP;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UProgressBar> ProgressBar_FP;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UProgressBar> ProgressBar_Stamina;
};
