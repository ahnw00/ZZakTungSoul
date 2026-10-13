// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PlayerHUDWidget.h"
#include <Components/ProgressBar.h>
#include <Character/CharacterStatComponent.h>
#include <Warrior/Warrior.h>

void UPlayerHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 플레이어 가져오기.
	AWarrior* Warrior = Cast<AWarrior>(GetOwningPlayerPawn());
	if (nullptr == Warrior)
	{
		return;
	}
	// 전사 스탯 가져오기.
	UCharacterStatComponent* StatComp = Warrior->GetCharacterStatComponent();
	if (nullptr == StatComp)
	{
		return;
	}
	// 델리게이트 연결.
	StatComp->OnHPChanged.AddDynamic(this, &UPlayerHUDWidget::UpdateHP);
	StatComp->OnFPChanged.AddDynamic(this, &UPlayerHUDWidget::UpdateFP);
	StatComp->OnStaminaChanged.AddDynamic(this, &UPlayerHUDWidget::UpdateStamina);
	// 게임 시작 시 현재 스탯으로 화면 즉시 초기화.
	StatComp->BroadcastAllStats();
}

void UPlayerHUDWidget::UpdateHP(float CurrentHP, float MaxHP)
{
	if (ProgressBar_HP && MaxHP > 0.f)
	{
		const float Ratio = FMath::Clamp(CurrentHP / MaxHP, 0.f, 1.f);
		ProgressBar_HP->SetPercent(Ratio);
	}
}

void UPlayerHUDWidget::UpdateFP(float CurrentFP, float MaxFP)
{
	if (ProgressBar_FP && MaxFP > 0.f)
	{
		const float Ratio = FMath::Clamp(CurrentFP / MaxFP, 0.f, 1.f);
		ProgressBar_FP->SetPercent(Ratio);
	}
}

void UPlayerHUDWidget::UpdateStamina(float CurrentStamina, float MaxStamina)
{
	if (ProgressBar_Stamina && MaxStamina > 0.f)
	{
		const float Ratio = FMath::Clamp(CurrentStamina / MaxStamina, 0.f, 1.f);
		ProgressBar_Stamina->SetPercent(Ratio);
	}
}
