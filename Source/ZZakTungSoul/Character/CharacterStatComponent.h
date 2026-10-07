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

	void LoadCharacterStat();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

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
};
