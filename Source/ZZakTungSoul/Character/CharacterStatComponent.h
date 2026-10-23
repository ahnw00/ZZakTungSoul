// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterStatComponent.generated.h"

// 레벨업 및 스택 시뮬레이션용 미리보기 구조체.
USTRUCT(BlueprintType)
struct FStatPreview
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float MaxHP = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float MaxFP = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float MaxStamina = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float AttackPower = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float Defense = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float EquipLoad = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float Resistance = 0.f;
};


// [델리게이트 선언]
// 스탯이 바뀔 때(현재값, 최대값) 2개의 float 데이터를 방송하는 델리게이트 타입 정의.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStatChangedDelegate, float, CurrentValue, float, MaxValue);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZZAKTUNGSOUL_API UCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCharacterStatComponent();

public:
	// PlayerState의 세이브데이터를 받아 최종 스탯 계산 및 UI 동기화.
	UFUNCTION(BlueprintCallable, Category = "Stat")
	void UpdateCharacterStat(const struct FWarriorAttributeData& Attributes);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	void LoadCharacterStat();
	int32 GetStatInitialValue(const FName& StatName) const;
	int32 CalculateGrowthValue(const FName& InStatID, const FName& InTarget, int32 AddedPoints) const;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	// [이벤트 방송 변수]: UI나 다른 시스템이 바인딩(구독)할 수 있는 델리게이트.
	UPROPERTY(BlueprintAssignable, Category = "Stat|Events")
	FOnStatChangedDelegate OnHPChanged;

	UPROPERTY(BlueprintAssignable, Category = "Stat|Events")
	FOnStatChangedDelegate OnFPChanged;

	UPROPERTY(BlueprintAssignable, Category = "Stat|Events")
	FOnStatChangedDelegate OnStaminaChanged;

public:
	// ==============================================================================================
	// [Getter].
	// ==============================================================================================
	// Pool.
	UFUNCTION(BlueprintPure, Category = "Stat")
	FORCEINLINE float GetCurrentHP() const { return CurrentHP; }
	UFUNCTION(BlueprintPure, Category = "Stat")
	FORCEINLINE float GetMaxHP() const { return MaxHP; }
	UFUNCTION(BlueprintPure, Category = "Stat")
	FORCEINLINE float GetCurrentFP() const { return CurrentFP; }
	UFUNCTION(BlueprintPure, Category = "Stat")
	FORCEINLINE float GetMaxFP() const { return MaxFP; }
	UFUNCTION(BlueprintPure, Category = "Stat")
	FORCEINLINE float GetCurrentStamina() const { return CurrentStamina; }
	UFUNCTION(BlueprintPure, Category = "Stat")
	FORCEINLINE float GetMaxStamina() const { return MaxStamina; }
	// Stat.
	UFUNCTION(BlueprintPure, Category = "Stat")
	FORCEINLINE float GetAttackPower() const { return AttackPower; }
	UFUNCTION(BlueprintPure, Category = "Stat")
	FORCEINLINE float GetDefense() const { return Defense; }
	UFUNCTION(BlueprintPure, Category = "Stat")
	FORCEINLINE float GetPoise() const { return Poise; }
	UFUNCTION(BlueprintPure, Category = "Stat")
	FORCEINLINE float GetEquipLoad() const { return EquipLoad; }
	UFUNCTION(BlueprintPure, Category = "Stat")
	FORCEINLINE float GetResistance() const { return Resistance; }
	UFUNCTION(BlueprintPure, Category = "Stat")
	FORCEINLINE float GetMaxWalkSpeed() const { return MaxWalkSpeed; }
	// RequiredSouls
	UFUNCTION(BlueprintPure, Category = "Stat|Attributes")
	int32 GetRequiredSouls(int32 Level) const;

	UFUNCTION(BlueprintPure, Category = "Stat|Attributes")
	int32 GetTotalRequiredSouls(int32 FromLevel, int32 ToLevel) const;

	// 가상 수치 계산.
	UFUNCTION(BlueprintPure, Category = "Stat|Simulation")
	FStatPreview CalculateDerivedStats(const struct FWarriorAttributeData& InAttributes) const;
	// Attributes
	UFUNCTION(BlueprintPure, Category = "Stat|Attributes")
	FORCEINLINE float GetVigor() const { return Vigor; }
	UFUNCTION(BlueprintPure, Category = "Stat|Attributes")
	FORCEINLINE float GetAttunement() const { return Attunement; }
	UFUNCTION(BlueprintPure, Category = "Stat|Attributes")
	FORCEINLINE float GetEndurance() const { return Endurance; }
	UFUNCTION(BlueprintPure, Category = "Stat|Attributes")
	FORCEINLINE float GetVitality() const { return Vitality; }
	UFUNCTION(BlueprintPure, Category = "Stat|Attributes")
	FORCEINLINE float GetStrength() const { return Strength; }
	UFUNCTION(BlueprintPure, Category = "Stat|Attributes")
	FORCEINLINE float GetDexterity() const { return Dexterity; }

	// [비율 Getter].
	UFUNCTION(BlueprintPure, Category = "Stat")
	float GetHPRatio() const { return MaxHP > 0.f ? FMath::Clamp(CurrentHP / MaxHP, 0.f, 1.f) : 0.f; }
	UFUNCTION(BlueprintPure, Category = "Stat")
	float GetFPRatio() const { return MaxFP > 0.f ? FMath::Clamp(CurrentFP / MaxFP, 0.f, 1.f) : 0.f; }
	UFUNCTION(BlueprintPure, Category = "Stat")
	float GetStaminaRatio() const { return MaxStamina > 0.f ? FMath::Clamp(CurrentStamina / MaxStamina, 0.f, 1.f) : 0.f; }

	




	// [스탯 제어 및 소모 함수].
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
	UFUNCTION(BlueprintCallable, Category = "Stat")
	bool UseStamina(float Amount);

	// 게임 시작 시 UI 초기화를 위해 현재 스텟을 즉시 한 번 방송하는 함수.
	UFUNCTION(BlueprintCallable, Category = "Stat")
	void BroadcastAllStats();

private:
	// [스테미너 자연 회복 파라미터].
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

	// 적이나 보스 등 다른 객체에도 사용할 것이기 때문에 CurrentHP, CurrentFP 추가.
	UPROPERTY(VisibleAnywhere, Category = "CurrentStat")
	float CurrentHP = 100.f;
	UPROPERTY(VisibleAnywhere, Category = "CurrentStat")
	float CurrentFP = 100.f;
	UPROPERTY(VisibleAnywhere, Category = "CurrentStat")
	float CurrentStamina = 100.f;

	// Attribute
	UPROPERTY(VisibleAnywhere, Category = "Stat|Attributes")
	float Vigor = 10.f;
	UPROPERTY(VisibleAnywhere, Category = "Stat|Attributes")
	float Attunement= 10.f;
	UPROPERTY(VisibleAnywhere, Category = "Stat|Attributes")
	float Endurance = 10.f;
	UPROPERTY(VisibleAnywhere, Category = "Stat|Attributes")
	float Vitality = 10.f;
	UPROPERTY(VisibleAnywhere, Category = "Stat|Attributes")
	float Strength = 10.f;
	UPROPERTY(VisibleAnywhere, Category = "Stat|Attributes")
	float Dexterity= 10.f;

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
	TObjectPtr<UDataTable> LevelRequiredSoulsTable;
	UPROPERTY()
	TObjectPtr<UDataTable> StatGrowthTable;

	// [CSV 탐색용 정적키]
	// Attributes
	inline static const FName VigorKey = TEXT("Vigor");
	inline static const FName AttunementKey = TEXT("Attunement");
	inline static const FName EnduranceKey = TEXT("Endurance");
	inline static const FName VitalityKey = TEXT("Vitality");
	inline static const FName StrengthKey = TEXT("Strength");
	inline static const FName DexterityKey = TEXT("Dexterity");

	// Pool
	inline static const FName HpKey = TEXT("HP");
	inline static const FName FpKey = TEXT("FP");
	inline static const FName StaminaKey = TEXT("Stamina");

	// Stat
	inline static const FName EquipLoadKey = TEXT("EquipLoad");
	inline static const FName PoiseKey = TEXT("Poise");
	inline static const FName AttackPowerKey = TEXT("AttackPower");
	inline static const FName DefenseKey = TEXT("Defense");
	inline static const FName ResistanceKey = TEXT("Resistance");
	inline static const FName WalkSpeedKey = TEXT("MaxWalkSpeed");
	inline static const FName OverloadedSpeedKey = TEXT("OverloadedSpeed");
};
