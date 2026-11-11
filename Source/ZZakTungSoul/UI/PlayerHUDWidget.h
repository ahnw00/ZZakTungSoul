// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUDWidget.generated.h"

// [전방 선언].
class UCharacterStatComponent;
class UProgressBar;
class UTextBlock;
class AWarriorPlayerState;

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// [컴포넌트 주입 함수]
	UFUNCTION(BlueprintCallable, Category = "UI|Stat")
	void BindStatComponent(UCharacterStatComponent* InStatComp);
	UFUNCTION(BlueprintCallable, Category = "UI|Souls")
	void BindPlayerState(AWarriorPlayerState* InPlayerState);

protected:
	virtual void NativeConstruct() override;
	
	// [위젯 정리 함수]
	virtual void NativeDestruct() override;

	// [프로그레스바 헬퍼]
	void UpdateProgressBar(UProgressBar* Bar, float Current, float Max);

	UFUNCTION()
	void UpdateHP(float CurrentHP, float MaxHP);
	UFUNCTION()
	void UpdateFP(float CurrentFP, float MaxFP);
	UFUNCTION()
	void UpdateStamina(float CurrentStamina, float MaxStamina);
	UFUNCTION()
	void UpdateSouls(int32 CurrentSouls);

public:
	//Getter
	FORCEINLINE class UBeltWidget* GetBeltWidget() const { return WBP_BeltWidget; }

	void TurnOnInteractUI();
	void TurnOffInteractUI();

protected:
	// [바인딩 컴포넌트 기억]
	UPROPERTY()
	TWeakObjectPtr<UCharacterStatComponent> CurrentStatComponent;
	// 캐릭터 사망시 안전하게 nullptr 만든다.

	// [BindWidget Progress Bar]
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> ProgressBar_HP;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> ProgressBar_FP;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> ProgressBar_Stamina;
	UPROPERTY()
	TWeakObjectPtr<AWarriorPlayerState> CurrentPlayerState;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Souls;

	// [BindWidget Belt]
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UBeltWidget> WBP_BeltWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUserWidget> WBP_InteractWidget;
};
