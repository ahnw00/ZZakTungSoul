// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CharacterStatComponent.h"

#include <Data/StatDefine.h>


// Sets default values for this component's properties
UCharacterStatComponent::UCharacterStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UDataTable> StatDefineTableAsset(
		TEXT("/Game/Blueprints/Data/DT_StatDefine.DT_StatDefine")
	);
	if (StatDefineTableAsset.Succeeded())
	{
		StatDefineTable = StatDefineTableAsset.Object;
	}
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
		UE_LOG(LogTemp, Error, TEXT("{StatComponent] 유효하지 않은 CSV 파일."))
		return;
	}

	// Pool 스탯.
	// HP, FP, Stamina용 헬퍼 람다.
	auto LoadPoolStat = [this](const FName& RowName, float& OutMax, float& OutCurrent)
		{
			static const FString ContextString(TEXT("LoadCharacterStat"));
			if (const FStatDefine* Row = StatDefineTable->FindRow<FStatDefine>(RowName, ContextString))
			{
				OutMax = static_cast<float>(Row->InitialValue);
				OutCurrent = OutMax;
			}
			else
			{
				// CSV 행 누락시 기본 값 유지 및 경고 출력.
				UE_LOG(LogTemp, Warning,
					TEXT("[%s] StatDefineTable에서 행을 찾지 못함: %s (기존 Max 유지: %1.f)"),
					*GetName(), *RowName.ToString(), OutMax);
				OutCurrent = OutMax;
			}
		};
	// Pool 로드.
	LoadPoolStat(TEXT("HP"), MaxHP, CurrentHP);
	LoadPoolStat(TEXT("FP"), MaxFP, CurrentFP);
	LoadPoolStat(TEXT("Stamina"), MaxStamina, CurrentStamina);

	// 나머지 스탯.
	// 헬퍼 람다.
	auto LoadStat = [this](const FName& RowName, float& OutStat)
		{
			static const FString ContextString(TEXT("LoadCharacterStat"));
			if (const FStatDefine* Row = StatDefineTable->FindRow<FStatDefine>(RowName, ContextString))
			{
				OutStat = static_cast<float>(Row->InitialValue);
			}
			else
			{
				// CSV 행 누락시 기본 값 유지 및 경고 출력.
				UE_LOG(LogTemp, Warning,
					TEXT("[%s] StatDefineTable에서 행을 찾지 못함: %s (기존 Max 유지: %1.f)"),
					*GetName(), *RowName.ToString(), OutStat);
			}
		};
	// 스탯 로드.
	LoadStat(TEXT("EquipLoad"), EquipLoad);
	LoadStat(TEXT("Poise"), Poise);
	LoadStat(TEXT("AttackPower"), AttackPower);
	LoadStat(TEXT("Defense"), Defense);
	LoadStat(TEXT("Resistance"), Resistance);
	LoadStat(TEXT("MaxWalkSpeed"), MaxWalkSpeed);
	LoadStat(TEXT("OverloadedSpeed"), OverloadedSpeed);
}


// Called every frame
void UCharacterStatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 매 프레임 스테미너 자연 회복 체크.
	RegenerateStamina(DeltaTime);
}

// [스탯 제어 함수]
	// 형식
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
	if (InDamage <= 0.f)
	{
		return;
	}
	SetCurrentHP(CurrentHP - InDamage);
}
bool UCharacterStatComponent::UseFP(float Amount)
{
	if (CurrentFP < Amount)
	{
		return false;
	}
	SetCurrentFP(CurrentFP - Amount);
	return true;
}
bool UCharacterStatComponent::UseStamina(float Amount)
{
	if (CurrentStamina < Amount)
	{
		return false;
	}
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
	if (CurrentStamina >= MaxStamina)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	// 마지막 스테미너 쓴 시간으로부터 딜레이(1초) 안 지났으면 대기.
	float CurrentTime = World->GetTimeSeconds();
	if (CurrentTime - LastStaminaUseTime < StaminaRegenDelay)
	{
		return;
	}
	// 딜레이가 지나면 초당 회복량만큼 부드럽게 회복.
	SetCurrentStamina(CurrentStamina + StaminaRegenRate * DeltaTime);
}
