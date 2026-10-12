// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterStatComponent.generated.h"

// 1. 델리게이트 선언
// 스탯이 바뀔 때(현재값, 최대값) 2개의 float 데이터를 방송하는 델리게이트 타입 정의.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStatChangedDelegate, float, CurrentValue, float, MaxValue);

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


	// 2. [이벤트 방송 변수]: UI나 다른 시스템이 바인딩(구독)할 수 있는 델리게이트.
	UPROPERTY(BlueprintAssignable, Category = "Stat|Events")
	FOnStatChangedDelegate OnHPChanged;

	UPROPERTY(BlueprintAssignable, Category = "Stat|Events")
	FOnStatChangedDelegate OnFPChanged;

	UPROPERTY(BlueprintAssignable, Category = "Stat|Events")
	FOnStatChangedDelegate OnStaminaChanged;

	// 3. [Getter].
	FORCEINLINE float GetCurrentHP() const { return CurrentHP; }
	FORCEINLINE float GetMaxHP() const { return MaxHP; }
	FORCEINLINE float GetCurrentFP() const { return CurrentFP; }
	FORCEINLINE float GetMaxFP() const { return MaxFP; }
	FORCEINLINE float GetCurrentStamina() const { return CurrentStamina; }
	FORCEINLINE float GetMaxStamina() const { return MaxStamina; }

	// 4. [비율 Getter].
	UFUNCTION(BlueprintPure, Category = "Stat")
	float GetHPRatio() const { return MaxHP > 0.f ? FMath::Clamp(CurrentHP / MaxHP, 0.f, 1.f) : 0.f; }

	UFUNCTION(BlueprintPure, Category = "Stat")
	float GetFPRatio() const { return MaxFP > 0.f ? FMath::Clamp(CurrentFP / MaxFP, 0.f, 1.f) : 0.f; }
	
	UFUNCTION(BlueprintPure, Category = "Stat")
	float GetStaminaRatio() const { return MaxStamina > 0.f ? FMath::Clamp(CurrentStamina / MaxStamina, 0.f, 1.f) : 0.f; }

	// 5. [스탯 제어 및 소모 함수].
	UFUNCTION(BlueprintCallable, Category = "Stat")
	void SetCurrentHP(float NewHP);

	UFUNCTION(BlueprintCallable, Category = "Stat")
	void ApplyDamage(float InDamage);

	UFUNCTION(BlueprintCallable, Category = "Stat")
	void SetCurrentFP(float NewFP);

	UFUNCTION(BlueprintCallable, Category = "Stat")
	bool UseFP(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Stat")
	void SetCurrentStamina(float NewStamina);

	// 스테미너 소모(스테미너가 충분하면 깎고, true, 부족하면 false 반환).
	UFUNCTION(BlueprintCallable, Category = "Stat")
	bool UseStamina(float Amount);

	// 게임 시작 시 UI 초기화를 위해 현재 스텟을 즉시 한 번 방송하는 함수.
	UFUNCTION(BlueprintCallable, Category = "Stat")
	void BroadcastAllStats();

private:
	//6. [스테미너 자연 회복 파라미터].
	UPROPERTY(EditAnywhere, Category = "Stat|Stamina")
	float StaminaRegenRate = 30.f; // 초당 30 회복.

	UPROPERTY(EditAnywhere, Category = "Stat|Stamina")
	float StaminaRegenDelay = 1.f; // 스테미너 사용 후 회복 시작까지 대기할 지연 시간.

	float LastStaminaUseTime = 0.f; // 마지막으로 스테미너를 쓴 시각.

	void RegenerateStamina(float DeltaTime);

private:
	// 능력치로부터 계산되는 최대치
	UPROPERTY(VisibleAnywhere, Category = "Stat")
	float MaxHP = 100.f;

	UPROPERTY(VisibleAnywhere, Category = "Stat")
	float MaxFP = 100.f;

	UPROPERTY(VisibleAnywhere, Category = "Stat")
	float MaxStamina = 100.f;

	UPROPERTY(VisibleAnywhere, Category = "CurrentStat")
	float CurrentHP = 100.f;
	
	UPROPERTY(VisibleAnywhere, Category = "CurrentStat")
	float CurrentFP = 100.f;
	
	UPROPERTY(VisibleAnywhere, Category = "CurrentStat")
	float CurrentStamina = 100.f;

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
