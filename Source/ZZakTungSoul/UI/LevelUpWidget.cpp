// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LevelUpWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Warrior/WarriorPlayerState.h"
#include "Interface/CharacterStatInterface.h"
#include "Character/CharacterStatComponent.h"
#include "UIHelpers.h"


// 캐싱 객체 접근 함수.
AWarriorPlayerState* ULevelUpWidget::GetWarriorPlayerState() const
{
	return GetOwningPlayerState<AWarriorPlayerState>();
}
UCharacterStatComponent* ULevelUpWidget::GetCharacterStatComponent() const
{
	if (ICharacterStatInterface* StatOwner = Cast<ICharacterStatInterface>(GetOwningPlayerPawn()))
	{
		return StatOwner->GetCharacterStatComponent();
	}
	return nullptr;
}


void ULevelUpWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// 결정/취소 버튼 바인딩.
	if (Button_Confirm)
	{
		Button_Confirm->OnClicked.AddDynamic(this, &ULevelUpWidget::CommitLevelUp);
	}
	if (Button_Cancel)
	{
		Button_Cancel->OnClicked.AddDynamic(this, &ULevelUpWidget::CancelLevelUp);
	}

}

void ULevelUpWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 창이 열릴 때마다 최신 세이브데이터로 동기화.
	InitializedLevelUp();
}

// 초기화: PlayerState 원본 복사 & 장바구니 리셋.
void ULevelUpWidget::InitializedLevelUp()
{
	AWarriorPlayerState* PS = GetWarriorPlayerState();
	if (!PS)
	{
		return;
	}
	// 원본 데이터 복사.
	BaseLevel = PS->GetSaveData().Level;
	BaseSouls = PS->GetSaveData().Souls;
	BaseAttributes = PS->GetSaveData().Attributes;

	// 가상 시뮬레이션 장바구니 0 리셋(현재 능력치로).
	VirtualLevel = BaseLevel;
	TotalRequiredSouls = 0;
	PendingPoints = FWarriorAttributeData();

	// 화면 갱신.
	RefreshUI();
}

// 가상 투자 사전 검사.
bool ULevelUpWidget::CanAddPoint() const
{
	UCharacterStatComponent* StatComp = GetCharacterStatComponent();
	if (!StatComp)
	{
		return false;
	}

	// 다음 레벨 상승시 필요한 소울 조회.
	const int32 NextLevelCost = StatComp->GetRequiredSouls(VirtualLevel);

	// 누적 소울 + 다음 비용이 보유 소울 이내 여부 검사.
	return (TotalRequiredSouls + NextLevelCost) <= BaseSouls;
}
// 가상 투자 포인트 증가.
void ULevelUpWidget::AddPoint(FName StatName)
{
	// 소울 부족 시 차단.
	if (!CanAddPoint())
	{
		return;
	}

	// 해당 스탯 가상 포인트 1 증가.
	if (StatName == TEXT("Vigor")) { PendingPoints.Vigor++; }
	else if (StatName == TEXT("Attunement")) { PendingPoints.Attunement++; }
	else if (StatName == TEXT("Endurance")) { PendingPoints.Endurance++; }
	else if (StatName == TEXT("Vitality")) { PendingPoints.Vitality++; }
	else if (StatName == TEXT("Strength")) { PendingPoints.Strength++; }
	else if (StatName == TEXT("Dexterity")) { PendingPoints.Dexterity++; }
	else return;

	// 가상 레벨 증가.
	VirtualLevel++;

	// 누적 필요 소울 재계산.
	if (UCharacterStatComponent* StatComp = GetCharacterStatComponent())
	{
		TotalRequiredSouls = StatComp->GetTotalRequiredSouls(BaseLevel, VirtualLevel);
	}

	// 화면 즉시 갱신.
	RefreshUI();
}

void ULevelUpWidget::SubPoint(FName StatName)
{
	// 해당 스탯에 가상 포인트 있는 지 검사.
	if (StatName == TEXT("Vigor") && PendingPoints.Vigor > 0) { PendingPoints.Vigor--; }
	else if (StatName == TEXT("Attunement") && PendingPoints.Attunement > 0) { PendingPoints.Attunement--; }
	else if (StatName == TEXT("Endurance") && PendingPoints.Endurance > 0) { PendingPoints.Endurance--; }
	else if (StatName == TEXT("Vitality") && PendingPoints.Vitality > 0) { PendingPoints.Vitality--; }
	else if (StatName == TEXT("Strength") && PendingPoints.Strength > 0) { PendingPoints.Strength--; }
	else if (StatName == TEXT("Dexterity") && PendingPoints.Dexterity > 0) { PendingPoints.Dexterity--; }
	else return;

	// 가상 레벨 감소.
	VirtualLevel--;

	// 누적 필요 소울 재계산.
	if (UCharacterStatComponent* StatComp = GetCharacterStatComponent())
	{
		TotalRequiredSouls = StatComp->GetTotalRequiredSouls(BaseLevel, VirtualLevel);
	}

	// 화면 즉시 갱신.
	RefreshUI();
}


// 레벨업 확정.
void ULevelUpWidget::CommitLevelUp()
{
	// 투자 포인트 없으면 무시.
	if (TotalRequiredSouls <= 0)
	{
		return;
	}

	// PlayerState에 Pending Points 넘겨 일괄 결제 승인 요청.
	AWarriorPlayerState* PS = GetWarriorPlayerState();
	if (PS && PS->ApplyLevelUp(PendingPoints))
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("[화톳불] 레벨업 완료!"));
		
		// 결제 성공 후 최신 세이브데이터로 장바구니 재동기화 및 리셋.
		InitializedLevelUp();
	}

}

// 레벨업 취소.
void ULevelUpWidget::CancelLevelUp()
{
	// 장바구니 임시 포인트 초기화.
	InitializedLevelUp();

	// 부모에게 취소 방송.
	OnLevelUpCanceled.Broadcast();
}

// 화면 텍스트 갱신.
using namespace UIHelpers;
void ULevelUpWidget::RefreshUI()
{
	// 레벨 & 소울 갱신.
	SetNumberText(TextBlock_CurrentLevel, VirtualLevel);
	SetNumberText(TextBlock_CurrentSouls, BaseSouls);
	SetNumberText(TextBlock_TotalRequiredSouls, TotalRequiredSouls);
	SetNumberText(TextBlock_RemainingSouls, BaseSouls - TotalRequiredSouls);

	// 가상 속성 합성.
	FWarriorAttributeData VirtualAttributes;
	VirtualAttributes.Vigor = BaseAttributes.Vigor + PendingPoints.Vigor;
	VirtualAttributes.Attunement = BaseAttributes.Attunement + PendingPoints.Attunement;
	VirtualAttributes.Endurance = BaseAttributes.Endurance + PendingPoints.Endurance;
	VirtualAttributes.Vitality = BaseAttributes.Vitality + PendingPoints.Vitality;
	VirtualAttributes.Strength = BaseAttributes.Strength + PendingPoints.Strength;
	VirtualAttributes.Dexterity = BaseAttributes.Dexterity + PendingPoints.Dexterity;

	// 주 스탯 화면 출력.
	UCharacterStatComponent* StatComp = GetCharacterStatComponent();
	if (!StatComp)
	{
		return;
	}
	SetNumberText(TextBlock_Vigor, StatComp->GetVigor() + PendingPoints.Vigor);
	SetNumberText(TextBlock_Attunement, StatComp->GetAttunement() + PendingPoints.Attunement);
	SetNumberText(TextBlock_Endurance, StatComp->GetEndurance() + PendingPoints.Endurance);
	SetNumberText(TextBlock_Vitality, StatComp->GetVitality() + PendingPoints.Vitality);
	SetNumberText(TextBlock_Strength, StatComp->GetStrength() + PendingPoints.Strength);
	SetNumberText(TextBlock_Dexterity, StatComp->GetDexterity() + PendingPoints.Dexterity);

	// 파생 전투 스탯 실시간 시뮬레이션.
	const FStatPreview Preview = StatComp->CalculateDerivedStats(VirtualAttributes);

	SetNumberText(TextBlock_MaxHP, static_cast<int32>(Preview.MaxHP));
	SetNumberText(TextBlock_MaxFP, static_cast<int32>(Preview.MaxFP));
	SetNumberText(TextBlock_MaxStamina, static_cast<int32>(Preview.MaxStamina));
	SetNumberText(TextBlock_EquipLoad, static_cast<int32>(Preview.EquipLoad));
	SetNumberText(TextBlock_Poise, static_cast<int32>(StatComp->GetPoise()));
	SetNumberText(TextBlock_AttackPower, static_cast<int32>(Preview.AttackPower));
	SetNumberText(TextBlock_Defense, static_cast<int32>(Preview.Defense));
	SetNumberText(TextBlock_Resistance, static_cast<int32>(Preview.Resistance));

	// 결정 버튼 활성화.
	if (Button_Confirm)
	{
		Button_Confirm->SetIsEnabled(TotalRequiredSouls > 0);
	}
}
