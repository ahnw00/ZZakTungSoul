// Fill out your copyright notice in the Description page of Project Settings.


#include "Warrior/WarriorPlayerState.h"
#include <Interface/CharacterStatInterface.h>
#include <Interface/SaveLoadInterface.h>
#include <Character/CharacterStatComponent.h>

AWarriorPlayerState::AWarriorPlayerState()
{

}

void AWarriorPlayerState::LoadSaveDataFrom(const FWarriorSaveData& NewSaveData)
{
	SaveData = NewSaveData;

	// [스탯 로드].
	if (ICharacterStatInterface* StatOwner = Cast<ICharacterStatInterface>(GetPawn()))
	{
		if (UCharacterStatComponent* StatComp = StatOwner->GetCharacterStatComponent())
		{
			StatComp->UpdateCharacterStat(SaveData.Attributes);
		}
	}

	// [인벤토리, 장비 슬롯 로드]
	if(ISaveLoadInterface* LoadOwner = Cast<ISaveLoadInterface>(GetPawn()))
	{
		LoadOwner->LoadWarriorData(SaveData);
	}
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

bool AWarriorPlayerState::ApplyLevelUp(const FWarriorAttributeData& PendingPoints)
{
	// 음수 포인트는 비정상.
	if (PendingPoints.Vigor < 0 || PendingPoints.Attunement < 0 || PendingPoints.Endurance < 0 || PendingPoints.Vitality < 0 || PendingPoints.Strength < 0 || PendingPoints.Dexterity < 0)
	{
		return false;
	}

	// 올리려는 총 레벨 수 계산.
	const int32 AddedLevels
		= PendingPoints.Vigor
		+ PendingPoints.Attunement
		+ PendingPoints.Endurance
		+ PendingPoints.Vitality
		+ PendingPoints.Strength
		+ PendingPoints.Dexterity;


	if (AddedLevels <= 0)
	{
		return false;
	}

	// 스탯 컴포넌트 유효성 검사.
	ICharacterStatInterface* StatOwner = Cast<ICharacterStatInterface>(GetPawn());
	if (!StatOwner)
	{
		return false;
	}

	UCharacterStatComponent* StatComp = StatOwner->GetCharacterStatComponent();
	if (!StatComp)
	{
		return false;
	}

	// 필요한 총 소울 계산 및 소지 소울 검증.
	const int32 FromLevel = SaveData.Level;
	const int32 ToLevel = FromLevel + AddedLevels;
	const int32 TotalCost = StatComp->GetTotalRequiredSouls(FromLevel, ToLevel);

	if (SaveData.Souls < TotalCost)
	{
		return false;
	}

	// 일괄 승인
	SaveData.Souls -= TotalCost;
	SaveData.Level += AddedLevels;

	SaveData.Attributes.Vigor += PendingPoints.Vigor;
	SaveData.Attributes.Attunement += PendingPoints.Attunement;
	SaveData.Attributes.Endurance += PendingPoints.Endurance;
	SaveData.Attributes.Vitality += PendingPoints.Vitality;
	SaveData.Attributes.Strength += PendingPoints.Strength;
	SaveData.Attributes.Dexterity += PendingPoints.Dexterity;

	
	// 캐릭터 컴포넌트에 반영.
	StatComp->UpdateCharacterStat(SaveData.Attributes);
	
	return true;
}

void AWarriorPlayerState::AddSouls(int32 Amount)
{
	if (Amount <= 0)
	{
		return;
	}

	SaveData.Souls += Amount;
	UE_LOG(LogTemp, Log, TEXT("[PlayerState] 소울 획득: +%d (현재 소울: %d)"), Amount, SaveData.Souls);
}

// 화톳불 활성화 확인.
bool AWarriorPlayerState::IsBonfireLit(const FName& BonfireID) const
{
	if (BonfireID.IsNone())
	{
		return false;
	}
	return SaveData.LitBonfireIDs.Contains(BonfireID);
}

// 화톳불 등록.
bool AWarriorPlayerState::RegisterLitBonfire(const FName& BonfireID)
{
	if (BonfireID.IsNone())
	{
		return false;
	}

	if(SaveData.LitBonfireIDs.Contains(BonfireID))
	{
		return false;
	}

	SaveData.LitBonfireIDs.Add(BonfireID);
	return true;
}

