// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterStatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZZAKTUNGSOUL_API UCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCharacterStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// 초기값에서 Attribute 값을 계산하여 최종 스탯 로드해주기
	void UpdateCharacterStat(const struct FWarriorAttributeData& Attributes);

private:
	// 초기값 csv에서 읽어온 데이터 테이블 값으로 초기화해주기
	float GetStatInitialValue(const FName& StatName) const;

private:
	// 능력치로부터 계산되는 최대치
	UPROPERTY(VisibleAnywhere, Category = "Stat")
	float MaxHP = 0.f;

	UPROPERTY(VisibleAnywhere, Category = "Stat")
	float MaxFP = 0.f;

	UPROPERTY(VisibleAnywhere, Category = "Stat")
	float MaxStamina = 0.f;

	UPROPERTY(VisibleAnywhere, Category = "CurrentStat")
	float CurrentStamina = 0.f;

	// 계산된 전투 스탯
	UPROPERTY(VisibleAnywhere, Category = "Stat")
	float EquipLoad = 0.f;

	UPROPERTY(VisibleAnywhere, Category = "Stat")
	float Poise = 0.f;

	UPROPERTY(VisibleAnywhere, Category = "Stat")
	float AttackPower = 0.f;

	UPROPERTY(VisibleAnywhere, Category = "Stat")
	float Defense = 0.f;

	UPROPERTY(VisibleAnywhere, Category = "Stat")
	float Resistance = 0.f;

	// 이동 속도 값
	UPROPERTY(VisibleAnywhere, Category = "Stat")
	float MaxWalkSpeed = 0.f;

	UPROPERTY(VisibleAnywhere, Category = "Stat")
	float OverloadedSpeed = 0.f;

private:
	UPROPERTY()
	TObjectPtr<UDataTable> StatDefineTable;

	UPROPERTY()
	TObjectPtr<UDataTable> StatGrowthTable;

	// 키 값 설정
	inline static const FName HpKey = TEXT("HP");
	inline static const FName FpKey = TEXT("FP");
	inline static const FName StaminaKey = TEXT("Stamina");
	inline static const FName EquipLoadKey = TEXT("EquipLoad");
	inline static const FName PoiseKey = TEXT("Poise");
	inline static const FName AttackPowerKey = TEXT("AttackPower");
	inline static const FName DefenseKey = TEXT("Defense");
	inline static const FName ResistanceKey = TEXT("Resistance");
	inline static const FName WalkSpeedKey = TEXT("MaxWalkSpeed");
	inline static const FName OverloadedSpeedKey = TEXT("OverloadedSpeed");
};
