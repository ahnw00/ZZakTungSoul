// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/StatusWindowWidget.h"
#include "Components/TextBlock.h"
#include "Interface/CharacterStatInterface.h"
#include "Character/CharacterStatComponent.h"
#include "SaveData/WarriorSaveData.h"


void UStatusWindowWidget::NativeConstruct()
{
	Super::NativeConstruct(); // OnPossess는 Controller에만 있는 함수.

	// 창을 열 때 최신 스탯으로 새로 고침.
	RefreshStatus();
}

void UStatusWindowWidget::RefreshStatus()
{
	APawn* OwningPawn = GetOwningPlayerPawn();
	if (!OwningPawn)
	{
		return;
	}

	// [텍스트 출력용 헬퍼].
	auto SetNumberText = [](TObjectPtr<UTextBlock>& TextBlock, auto Value)
		{
			if (TextBlock)
			{
				TextBlock->SetText(FText::AsNumber(Value));
			}
		};

	// [인터페이스 단일 창구 조회].
	if (ICharacterStatInterface* StatOwner = Cast<ICharacterStatInterface>(OwningPawn))
	{
		const FWarriorSaveData* SaveData = StatOwner->GetSaveData();
		UCharacterStatComponent* StatComp = StatOwner->GetCharacterStatComponent();

		if (!SaveData || !StatComp)
		{
			return;
		}

		// [스탯].
		// GetSaveData 활용.
		SetNumberText(TextBlock_Level, SaveData->Level);
		SetNumberText(TextBlock_Souls, SaveData->Souls);
		SetNumberText(TextBlock_RequiredSouls, StatComp->GetRequiredSouls(SaveData->Level));

		// [전투 스탯].
		// CharacterStatComponent 활용.
		// [스탯]
		SetNumberText(TextBlock_Vigor, static_cast<int32>(StatComp->GetVigor()));
		SetNumberText(TextBlock_Attunement, static_cast<int32>(StatComp->GetAttunement()));
		SetNumberText(TextBlock_Endurance, static_cast<int32>(StatComp->GetEndurance()));
		SetNumberText(TextBlock_Vitality, static_cast<int32>(StatComp->GetVitality()));
		SetNumberText(TextBlock_Strength, static_cast<int32>(StatComp->GetStrength()));
		SetNumberText(TextBlock_Dexterity, static_cast<int32>(StatComp->GetDexterity()));

		// [Pool]
		SetNumberText(TextBlock_HP, static_cast<int32>(StatComp->GetMaxHP()));
		SetNumberText(TextBlock_FP, static_cast<int32>(StatComp->GetMaxFP()));
		SetNumberText(TextBlock_Stamina, static_cast<int32>(StatComp->GetMaxStamina()));

		// [전투]
		SetNumberText(TextBlock_AttackPower, static_cast<int32>(StatComp->GetAttackPower()));
		SetNumberText(TextBlock_Defense, static_cast<int32>(StatComp->GetDefense()));
		SetNumberText(TextBlock_Poise, static_cast<int32>(StatComp->GetPoise()));
		SetNumberText(TextBlock_EquipLoad, static_cast<int32>(StatComp->GetEquipLoad()));
		SetNumberText(TextBlock_Resistance, static_cast<int32>(StatComp->GetResistance()));

	}
}


