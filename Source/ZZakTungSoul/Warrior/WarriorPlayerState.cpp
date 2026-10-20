// Fill out your copyright notice in the Description page of Project Settings.


#include "Warrior/WarriorPlayerState.h"
#include <Interface/CharacterStatInterface.h>
#include <Character/CharacterStatComponent.h>

AWarriorPlayerState::AWarriorPlayerState()
{

}

// 스탯 업그레이드.
bool AWarriorPlayerState::UpgradeStat(const FName& StatName, int32 RequiredSouls)
{
	// 소울 검사.
	if (RequiredSouls <= 0 || SaveData.Souls < RequiredSouls)
	{
		return false;
	}

	// 스탯 상승.
	if (StatName == TEXT("Vigor")) { SaveData.Attributes.Vigor++; }
	else if (StatName == TEXT("Attunement")) { SaveData.Attributes.Attunement++; }
	else if (StatName == TEXT("Endurance")) { SaveData.Attributes.Endurance++; }
	else if (StatName == TEXT("Vitality")) { SaveData.Attributes.Vitality++; }
	else if (StatName == TEXT("Strength")) { SaveData.Attributes.Strength++; }
	else if (StatName == TEXT("Dexterity")) { SaveData.Attributes.Dexterity++; }
	else { return false; }
	
	// 세이브데이터의 소울 감소 및 레벨 증가.
	SaveData.Souls -= RequiredSouls;
	SaveData.Level++;

	// 캐릭터 컴포넌트에 재계산 반영.
	if (ICharacterStatInterface* StatOwner = Cast<ICharacterStatInterface>(GetPawn()))
	{
		if (UCharacterStatComponent* StatComp = StatOwner->GetCharacterStatComponent())
		{
			StatComp->UpdateCharacterStat(SaveData.Attributes);
		}
	}

	return true;
}
