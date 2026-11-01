// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Components/EnemyStatComponent.h"
#include <Data/EnemyStat.h>
#include <Data/DamageInfo.h>

// Sets default values for this component's properties
UEnemyStatComponent::UEnemyStatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UEnemyStatComponent::BeginPlay()
{
	Super::BeginPlay();

	// 스탯 초기화
	InitializeEnemyStat();
}


// Called every frame
void UEnemyStatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UEnemyStatComponent::SetCurrentHp(float NewHp)
{
	CurrentHp = FMath::Clamp(NewHp, 0.0f, MaxHp);
	OnEnemyHpChanged.Broadcast(CurrentHp, MaxHp);
}

void UEnemyStatComponent::SetCurrentStance(float NewStance)
{
	CurrentStance = FMath::Clamp(NewStance, 0.0f, MaxStance);
}

void UEnemyStatComponent::ApplyDamage(const FDamageInfo& DamageInfo)
{
	// Hp가 이미 0인 경우
	if (CurrentHp <= 0.0f)
	{
		return;
	}

	// Damage Info 받아오기
	float InDamage = DamageInfo.AttackDamage;
	float InPoiseDamage = DamageInfo.PoiseDamage;
	float InStanceDamage = DamageInfo.StanceDamage;
	if (InDamage < 0.0f || InPoiseDamage < 0.0f || InStanceDamage < 0.0f)
	{
		return;
	}

	SetCurrentHp(CurrentHp - InDamage);
	SetCurrentStance(CurrentStance + InStanceDamage);

	bIsPoiseOver = Poise < InPoiseDamage;
	bIsStanceOver = MaxStance <= CurrentStance;

	// Hp가 0이 되었는지 확인
	if (CurrentHp <= 0.0f)
	{
		OnEnemyHpZero.Broadcast();
	}

	UE_LOG(LogTemp, Log, TEXT("Current HP: %f"), CurrentHp);
}

void UEnemyStatComponent::InitializeEnemyStat()
{
	// Enemy Stat DataTable Row 가져오기
	const FEnemyStat* Row = EnemyStatRow.GetRow<FEnemyStat>(TEXT("EnemyStat"));
	if (nullptr == Row)
	{
		return;
	}

	// Enemy Stat 수치 초기화
	MaxHp = Row->Hp;
	CurrentHp = MaxHp;

	Poise = Row->Poise;

	MaxStance = Row->Stance;
	CurrentStance = 0.0f;

	Defense = Row->Defense;
	Resistance = Row->Resistance;

	WalkSpeed = Row->WalkSpeed;
	ChaseSpeed = Row->ChaseSpeed;
	StrafeSpeed = Row->StrafeSpeed;
}