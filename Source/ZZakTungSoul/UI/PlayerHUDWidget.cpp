// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PlayerHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Character/CharacterStatComponent.h"
#include "Interface/CharacterStatInterface.h"



void UPlayerHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 위젯 생성 시 이미 Pawn 있으면 인터페이스를 통해 컴포넌트와 연결.
	if (ICharacterStatInterface* StatOwner = Cast<ICharacterStatInterface>(GetOwningPlayerPawn()))
	{
		BindStatComponent(StatOwner->GetCharacterStatComponent());
	}
}

void UPlayerHUDWidget::NativeDestruct()
{
	// 델리게이트 연결 정리.
	BindStatComponent(nullptr);

	Super::NativeDestruct();
}

void UPlayerHUDWidget::BindStatComponent(UCharacterStatComponent* InStatComp)
{
	// 기존 연결 컴포넌트의 이전 델리게이트 연결 해제(댕글링 방지).
	if (CurrentStatComponent.IsValid())
	{
		CurrentStatComponent->OnHPChanged.RemoveDynamic(this, &UPlayerHUDWidget::UpdateHP);
		CurrentStatComponent->OnFPChanged.RemoveDynamic(this, &UPlayerHUDWidget::UpdateFP);
		CurrentStatComponent->OnStaminaChanged.RemoveDynamic(this, &UPlayerHUDWidget::UpdateStamina);
	}
	
	// 새로운 컴포넌트 저장.
	CurrentStatComponent = InStatComp;

	// 새 컴포넌트 연결 및 현재 상태 갱신.
	if (CurrentStatComponent.IsValid())
	{
		// AddUniqueDynamic -> 이미 등록되어 있는 지 검사한다.
		CurrentStatComponent->OnHPChanged.AddUniqueDynamic(this, &UPlayerHUDWidget::UpdateHP);
		CurrentStatComponent->OnFPChanged.AddUniqueDynamic(this, &UPlayerHUDWidget::UpdateFP);
		CurrentStatComponent->OnStaminaChanged.AddUniqueDynamic(this, &UPlayerHUDWidget::UpdateStamina);

		// 초기값은 연결되는 순간 Getter로 딱 1번 확인하고, 그 이후 실시간 변화는 브로드캐스트로 전달받는다.
		// 델리게이트 바인딩은 되었으나 아직 방송된 적이 없으므로 현재는 빈 깡통상태이다.
		// 따라서 이 부분은 새 컴포넌트 연결이므로 Getter로 초기값을 확인하고 채운다.
		UpdateHP(CurrentStatComponent->GetCurrentHP(), CurrentStatComponent->GetMaxHP());
		UpdateFP(CurrentStatComponent->GetCurrentFP(), CurrentStatComponent->GetMaxFP());
		UpdateStamina(CurrentStatComponent->GetCurrentStamina(), CurrentStatComponent->GetMaxStamina());
	}
}

// 프로그레스바 헬퍼 함수.
void UPlayerHUDWidget::UpdateProgressBar(UProgressBar* Bar, float Current, float Max)
{
	if (!Bar)
	{
		return;
	}
	if (Max > 0.f)
	{
		const float Ratio = FMath::Clamp(Current / Max, 0.f, 1.f);
		Bar->SetPercent(Ratio);
	}
	else
	{
		// Max가 0이하면 0으로 초기화.
		Bar->SetPercent(0.f);
	}
}
// 프로그레스바 3종.
void UPlayerHUDWidget::UpdateHP(float CurrentHP, float MaxHP)
{
	UpdateProgressBar(ProgressBar_HP, CurrentHP, MaxHP);
}
void UPlayerHUDWidget::UpdateFP(float CurrentFP, float MaxFP)
{
	UpdateProgressBar(ProgressBar_FP, CurrentFP, MaxFP);
}
void UPlayerHUDWidget::UpdateStamina(float CurrentStamina, float MaxStamina)
{
	UpdateProgressBar(ProgressBar_Stamina, CurrentStamina, MaxStamina);
}
