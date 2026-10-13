// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CharacterStatComponent.h"

#include <Data/StatDefine.h>
#include <Data/StatGrowth.h>
#include <SaveData/WarriorSaveData.h>


// Sets default values for this component's properties
UCharacterStatComponent::UCharacterStatComponent()
{
	// 스탯 정의 데이터 테이블 불러오기
	// 스탯 정의에는 초기값 및 min, max 값 있음
	static ConstructorHelpers::FObjectFinder<UDataTable> StatDefineTableAsset(
		TEXT("/Game/Blueprints/Data/DT_StatDefine.DT_StatDefine")
	);
	if (StatDefineTableAsset.Succeeded())
	{
		StatDefineTable = StatDefineTableAsset.Object;
	}

	// 스탯 성장 데이터 테이블 불러오기
	// 스탯 성장에는 레벨 별 증가치가 있음
	static ConstructorHelpers::FObjectFinder<UDataTable> StatGrowthTableAsset(
		TEXT("/Game/Blueprints/Data/DT_StatGrowth.DT_StatGrowth")
	);
	if (StatGrowthTableAsset.Succeeded())
	{
		StatGrowthTable = StatGrowthTableAsset.Object;
	}
}


// Called when the game starts
void UCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// 매개 변수에 PlayerState에서 가져온 Attribute 값을 넣어줄거
void UCharacterStatComponent::UpdateCharacterStat(const FWarriorAttributeData& Attributes)
{
	// 초기값 가져오기
	if (StatDefineTable)
	{
		MaxHP = GetStatInitialValue(HpKey);
		MaxFP = GetStatInitialValue(FpKey);
		MaxStamina = GetStatInitialValue(StaminaKey);
		EquipLoad = GetStatInitialValue(EquipLoadKey);
		Poise = GetStatInitialValue(PoiseKey);
		AttackPower = GetStatInitialValue(AttackPowerKey);
		Defense = GetStatInitialValue(DefenseKey);
		Resistance = GetStatInitialValue(ResistanceKey);
		MaxWalkSpeed = GetStatInitialValue(WalkSpeedKey);
		OverloadedSpeed = GetStatInitialValue(OverloadedSpeedKey);
	}

	// Todo: 초기값에서 InSaveData의 Attribute 값과
	// 스탯 성장의 레벨 별 증가치를 계산해서 각 변수에 넣어주기
	// ex) MaxHp += InSaveData.Vigor * 10.f;
}

float UCharacterStatComponent::GetStatInitialValue(const FName& StatName) const
{
	if (!StatDefineTable)
		return 0.f;

	// 행의 제일 앞에 열 값으로 행 찾기
	// 행의 제일 앞 열이 key가 돼
	const FStatDefine* StatRow 
		= StatDefineTable->FindRow<FStatDefine>(StatName, TEXT("InitialValue Loaded"));

	return StatRow ? StatRow->InitialValue : 0.f;
}
