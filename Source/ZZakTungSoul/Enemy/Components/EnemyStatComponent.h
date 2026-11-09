// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyStatComponent.generated.h"

// 델리게이트 선언
// Hp가 0이 되었을때 발행
DECLARE_MULTICAST_DELEGATE(FOnEnemyHpZeroDelegate);

// 스탯에 변화가 생겼을때 발행
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnEnemyHpChangedDelegate, float /*CurrentHp*/, float /*MaxHp*/);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZZAKTUNGSOUL_API UEnemyStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemyStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	// Getter
	// State Flag
	FORCEINLINE bool IsPoiseOver() const { return bIsPoiseOver; }
	FORCEINLINE bool IsStanceOver() const { return bIsStanceOver; }

	//---------------------------------------------------------------
	// 현재 HP와 최대 HP 게터 추가.
	//
	//---------------------------------------------------------------
	float GetCurrentHp() const { return CurrentHp; }
	float GetMaxHp() const { return MaxHp; }

	// 소울량 Getter
	FORCEINLINE float GetSoul() const { return Soul; }

	// Movement Speed
	FORCEINLINE float GetWalkSpeed() const { return WalkSpeed; }
	FORCEINLINE float GetChaseSpeed() const { return ChaseSpeed; }
	FORCEINLINE float GetStrafeSpeed() const { return StrafeSpeed; }

	// Setter
	// State Flag
	FORCEINLINE void SetPoiseFlag(bool NewFlag) { bIsPoiseOver = NewFlag; }
	FORCEINLINE void SetStanceFlag(bool NewFlag) { bIsStanceOver = NewFlag; }

public:
	void SetCurrentHp(float NewHp);
	void SetCurrentStance(float NewStance);

	void ApplyDamage(const struct FDamageInfo& DamageInfo);

	void ResetEnemyStat();

protected:
	// Enemy Stat 초기화 함수
	void InitializeEnemyStat();

public:
	// Delegate 변수
	FOnEnemyHpZeroDelegate OnEnemyHpZero;

	FOnEnemyHpChangedDelegate OnEnemyHpChanged;

	// 데이터 테이블 Row 지정 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|DataTable")
	FDataTableRowHandle EnemyStatRow;

private:
	// 상태 판단 플래그
	uint32 bIsPoiseOver : 1 = false;
	uint32 bIsStanceOver : 1 = false;

////// Stat Variable Section
private:
	// Hp
	UPROPERTY(VisibleAnywhere, Category = "Stat")
	float MaxHp = 100.0f;

	// 현재 Hp
	UPROPERTY(VisibleAnywhere, Category = "Stat")
	float CurrentHp = 100.0f;

	// 경직치 -> 움찔거림
	UPROPERTY(VisibleAnywhere, Category = "Stat")
	float Poise = 0.0f;

	// 그로기 수치 -> 자세 무너짐 수치
	UPROPERTY(VisibleAnywhere, Category = "Stat")
	float MaxStance = 50.0f;

	// 누적 그로기 수치
	UPROPERTY(VisibleAnywhere, Category = "Stat")
	float CurrentStance = 0.0f;

	// 방어력
	UPROPERTY(VisibleAnywhere, Category = "Stat")
	float Defense = 10.0f;

	// 저항
	UPROPERTY(VisibleAnywhere, Category = "Stat")
	float Resistance = 10.0f;

	UPROPERTY(VisibleAnywhere, Category = "Stat")
	float Soul = 500.0f;

	// 기본 이동속도
	UPROPERTY(VisibleAnywhere, Category = "Stat")
	float WalkSpeed = 200.0f;

	// 추격 이동속도
	UPROPERTY(VisibleAnywhere, Category = "Stat")
	float ChaseSpeed = 250.0f;

	// 간보기 이동속도
	UPROPERTY(VisibleAnywhere, Category = "Stat")
	float StrafeSpeed = 150.0f;
};
