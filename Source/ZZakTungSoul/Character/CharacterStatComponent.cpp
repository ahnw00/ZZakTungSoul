// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CharacterStatComponent.h"

#include <Data/StatDefine.h>
#include <Data/LevelRequiredSouls.h>
#include <Data/StatGrowth.h>
#include <SaveData/WarriorSaveData.h>


// Sets default values for this component's properties
UCharacterStatComponent::UCharacterStatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// 스탯 정의 데이터 테이블 불러오기.
	static ConstructorHelpers::FObjectFinder<UDataTable> StatDefineTableAsset(
		TEXT("/Game/Data/DT_StatDefine.DT_StatDefine")
	);
	if (StatDefineTableAsset.Succeeded())
	{
		StatDefineTable = StatDefineTableAsset.Object;
	}
	// 스탯 성장에 필요한 소울 데이터 테이블 불러오기.
	static ConstructorHelpers::FObjectFinder<UDataTable> LevelRequiredSoulsTableAsset(
		TEXT("/Game/Data/DT_LevelRequiredSouls.DT_LevelRequiredSouls")
	);
	if (LevelRequiredSoulsTableAsset.Succeeded())
	{
		LevelRequiredSoulsTable = LevelRequiredSoulsTableAsset.Object;
	}
	// 스탯 성장 데이터 테이블 불러오기.
	static ConstructorHelpers::FObjectFinder<UDataTable> StatGrowthTableAsset(
		TEXT("/Game/Data/DT_StatGrowth.DT_StatGrowth")
	);
	if (StatGrowthTableAsset.Succeeded())
	{
		StatGrowthTable = StatGrowthTableAsset.Object;
	}
}

// [UI]
// 스탯 계산
// 기본값.
int32 UCharacterStatComponent::GetStatInitialValue(const FName& StatName) const
{
	if (!StatDefineTable)
	{
		return 0.f;
	}

	static const FString ContextString(TEXT("GetStatInitialValue"));
	// GetAllRows는 모든 행을 순서 없는 배열로 한꺼번에 긁어온다.
	// FindRow는 행 순서와 무관하게 데이터를 이름으로 정확히 조회할 수 있다.
	if (const FStatDefine* Row 
		= StatDefineTable->FindRow<FStatDefine>(StatName, ContextString)) 
	{
		return static_cast<float>(Row->InitialValue);
	}

	UE_LOG(
		LogTemp, 
		Warning, 
		TEXT("[%s] StatDefineTable에서 행을 찾지 못함: %s"), 
		*GetName(), *StatName.ToString()
	);
	
	return 0.f;
}
// 투자 포인트로 성장값 계산.
int32 UCharacterStatComponent::CalculateGrowthValue(
	const FName& InStatID, 
	const FName& InTarget, 
	int32 AddedPoints) const
{
	if (!StatGrowthTable || AddedPoints <= 0)
	{
		return 0.f;
	}

	static const FString ContextString(TEXT("CalculateGrowthValue"));
	TArray<FStatGrowth*> Rows;
	StatGrowthTable->GetAllRows<FStatGrowth>(ContextString, Rows);

	int32 TotalBonus = 0;

	for (const FStatGrowth* GrowthRow : Rows)
	{
		if (GrowthRow->StatID != InStatID 
			|| GrowthRow->Target != InTarget 
			|| AddedPoints < GrowthRow->MinLevel)
		{
			continue;
		}

		const int32 EffectivePoints 
			= FMath::Min(AddedPoints, GrowthRow->MaxLevel) 
			- GrowthRow->MinLevel + 1;
		TotalBonus += EffectivePoints * GrowthRow->Value;
	}


	return static_cast<float>(TotalBonus);
}
// 최종 스탯 결과값.
void UCharacterStatComponent::UpdateCharacterStat(const FWarriorAttributeData& Attributes)
{
	// Attribute 기본 수치 갱신.
	Vigor = GetStatInitialValue(VigorKey) + Attributes.Vigor;
	Attunement = GetStatInitialValue(AttunementKey) + Attributes.Attunement;
	Endurance = GetStatInitialValue(EnduranceKey) + Attributes.Endurance;
	Vitality = GetStatInitialValue(VitalityKey) + Attributes.Vitality;
	Strength = GetStatInitialValue(StrengthKey) + Attributes.Strength;
	Dexterity = GetStatInitialValue(DexterityKey) + Attributes.Dexterity;
	// Pool 최종값 계산.
	const FStatPreview NewStats = CalculateDerivedStats(Attributes);
	MaxHP = NewStats.MaxHP;
	MaxFP = NewStats.MaxFP;
	MaxStamina = NewStats.MaxStamina;
	// Stat 최종값 계산.
	Defense = NewStats.Defense;
	EquipLoad = NewStats.EquipLoad;
	Resistance = NewStats.Resistance;
	AttackPower = NewStats.AttackPower;

	if (CurrentHP <= 0.f)
	{
		CurrentHP = MaxHP;
	}
	else
	{
		CurrentHP = FMath::Clamp(CurrentHP, 0.f, MaxHP);
	}
	if (CurrentFP <= 0.f)
	{
		CurrentFP = MaxFP;
	}
	else
	{
		CurrentFP = FMath::Clamp(CurrentFP, 0.f, MaxFP);
	}
	
	
	CurrentStamina = MaxStamina;

	BroadcastAllStats();
}


// Called when the game starts
void UCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();

	// 게임 시작 시 테이블로부터 기본 스탯 로드.
	LoadCharacterStat();

	// 게임 시작 시 현재 HP, FP, Stamina 스탯을 UI에 방송.
	BroadcastAllStats();
}


// 기본 스탯 로딩.
void UCharacterStatComponent::LoadCharacterStat()
{
	if (!StatDefineTable)
	{
		UE_LOG(LogTemp, Error, TEXT("[StatComponent] 유효하지 않은 CSV 파일."));
		return;
	}

	// 1. 최대치(Max) 스탯 로드
	MaxHP = GetStatInitialValue(HpKey);
	MaxFP = GetStatInitialValue(FpKey);
	MaxStamina = GetStatInitialValue(StaminaKey);
	// 2. 현재값(Current) 초기화
	CurrentHP = MaxHP;
	CurrentFP = MaxFP;
	CurrentStamina = MaxStamina;
	// 3. 전투 및 이동 스탯 로드
	EquipLoad = GetStatInitialValue(EquipLoadKey);
	Poise = GetStatInitialValue(PoiseKey);
	AttackPower = GetStatInitialValue(AttackPowerKey);
	Defense = GetStatInitialValue(DefenseKey);
	Resistance = GetStatInitialValue(ResistanceKey);
	MaxWalkSpeed = GetStatInitialValue(WalkSpeedKey);
	OverloadedSpeed = GetStatInitialValue(OverloadedSpeedKey);
}

int32 UCharacterStatComponent::GetRequiredSouls(int32 Level) const
{
	if (!LevelRequiredSoulsTable || Level <= 0)
	{
		return 0;
	}

	const FName RowName = FName(*FString::Printf(TEXT("Level%d"), Level));
	static const FString ContextString(TEXT("GetRequiredSouls"));

	if (
		const FLevelRequiredSouls* Row 
		= LevelRequiredSoulsTable->FindRow<FLevelRequiredSouls>(RowName, ContextString))
	{
		return Row->RequiredSouls;
	}

	return 0;
}

int32 UCharacterStatComponent::GetTotalRequiredSouls(int32 FromLevel, int32 ToLevel) const
{
	if (FromLevel >= ToLevel)
	{
		return 0;
	}

	int32 TotalSouls = 0;
	for (int32 L = FromLevel; L < ToLevel; ++L)
	{
		TotalSouls += GetRequiredSouls(L);
	}
	return TotalSouls;
}

FStatPreview UCharacterStatComponent::CalculateDerivedStats(const FWarriorAttributeData& InAttributes) const
{
	FStatPreview OutPreview;

	// 1. Pool 스탯 가상 계산.
	OutPreview.MaxHP = GetStatInitialValue(HpKey) + CalculateGrowthValue(VigorKey, HpKey, InAttributes.Vigor);
	OutPreview.MaxFP = GetStatInitialValue(FpKey) + CalculateGrowthValue(AttunementKey, FpKey, InAttributes.Attunement);
	OutPreview.MaxStamina = GetStatInitialValue(StaminaKey) + CalculateGrowthValue(EnduranceKey, StaminaKey, InAttributes.Endurance);

	// 2. 전투 스탯 가상 계산.
	OutPreview.Defense = GetStatInitialValue(DefenseKey) + CalculateGrowthValue(EnduranceKey, DefenseKey, InAttributes.Endurance);
	OutPreview.EquipLoad = GetStatInitialValue(EquipLoadKey) + CalculateGrowthValue(VitalityKey, EquipLoadKey, InAttributes.Vitality);
	OutPreview.Resistance = GetStatInitialValue(ResistanceKey) + CalculateGrowthValue(VitalityKey, ResistanceKey, InAttributes.Vitality);

	// 3. 공격 가상 계산.
	OutPreview.AttackPower = GetStatInitialValue(AttackPowerKey) + CalculateGrowthValue(StrengthKey, AttackPowerKey, InAttributes.Strength)
		+ CalculateGrowthValue(DexterityKey, AttackPowerKey, InAttributes.Dexterity);

	return OutPreview;
}



// Called every frame
void UCharacterStatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 매 프레임 스테미너 자연 회복 체크.
	RegenerateStamina(DeltaTime);
}


// [스탯 제어 함수]
	// 범위 제한.
	// 이벤트 구독(Bind)하고 있는 UI에게 방송.
void UCharacterStatComponent::SetCurrentHP(float NewHP)
{
	CurrentHP = FMath::Clamp(NewHP, 0.f, MaxHP);
	OnHPChanged.Broadcast(CurrentHP, MaxHP);
}
void UCharacterStatComponent::SetCurrentFP(float NewFP)
{
	CurrentFP = FMath::Clamp(NewFP, 0.f, MaxFP);
	OnFPChanged.Broadcast(CurrentFP, MaxFP);
}
void UCharacterStatComponent::SetCurrentStamina(float NewStamina)
{
	CurrentStamina = FMath::Clamp(NewStamina, 0.f, MaxStamina);
	OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
}
// 스탯 방송.
void UCharacterStatComponent::BroadcastAllStats()
{
	OnHPChanged.Broadcast(CurrentHP, MaxHP);
	OnFPChanged.Broadcast(CurrentFP, MaxFP);
	OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
}


// [스탯 소모 함수]
void UCharacterStatComponent::ApplyDamage(float InDamage)
{
	if (InDamage <= 0.f) { return; }
	SetCurrentHP(CurrentHP - InDamage);
}
bool UCharacterStatComponent::UseFP(float Amount)
{
	if (Amount <= 0.f) { return false; }
	if (CurrentFP < Amount) { return false; }
	
	SetCurrentFP(CurrentFP - Amount);
	
	return true;
}
bool UCharacterStatComponent::UseStamina(float Amount)
{
	if (Amount <= 0.f) { return false; }
	if (CurrentStamina < Amount) { return false; }

	SetCurrentStamina(CurrentStamina - Amount);
	
	if (UWorld* World = GetWorld())
	{
		// GetTimeSeconds()는 게임 시작부터 몇초가 흘렀는지 알려준다.
		// 게임 시작 후 10초인 순간 구르기를 쓴다. LastStaminaUseTime = 10초.
		LastStaminaUseTime = World->GetTimeSeconds();
	}
	
	return true;
}

//	// TODO: 다크 소울 형식 고려.
//	// 1. 간단 버전.
//bool UCharacterStatComponent::UseStamina(float Amount)
//{
//	//if (CurrentStamina <= 0.f)
//	if (CurrentStamina <= Amount)
//	{
//		return false;
//	}
//	SetCurrentStamina(CurrentStamina - Amount);
//	
//	if (UWorld* World = GetWorld())
//	{
//		// GetTimeSeconds()는 게임 시작부터 몇초가 흘렀는지 알려준다.
//		// 게임 시작 후 10초인 순간 구르기를 쓴다. LastStaminaUseTime = 10초.
//		LastStaminaUseTime = World->GetTimeSeconds();
//	}
//	
//	return true;
//}
	// 2. 어려운 버전.
	// 실제 다크소울은 스테미나가 음수까지 떨어지고 차오른다.
//bool UCharacterStatComponent::UseStamina(float NewStamina)
//{
//	// UI의 GetStaminaRatio는 Clamp로 하한이 0.f기 때문에 프로그레스 바는 망가지지 않음.
//	CurrentStamina = FMath::Clamp(NewStamina, -50.f, MaxStamina);
//	OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
//}


void UCharacterStatComponent::RegenerateStamina(float DeltaTime)
{
	// 이미 스테미너가 꽉 차 있다면 회복할 필요 없음.
	if (CurrentStamina >= MaxStamina) { return; }
	UWorld* World = GetWorld();
	if (!World) { return; }

	// 마지막 스테미너 쓴 시간으로부터 딜레이(1초) 안 지났으면 대기.
	float CurrentTime = World->GetTimeSeconds();
	if (CurrentTime - LastStaminaUseTime < StaminaRegenDelay) { return; }

	// 딜레이가 지나면 초당 회복량만큼 부드럽게 회복.
	SetCurrentStamina(CurrentStamina + StaminaRegenRate * DeltaTime);
}
