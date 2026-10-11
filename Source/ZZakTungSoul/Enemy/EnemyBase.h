// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CharacterBase.h"
#include "EnemyStates.h"
#include "EnemyBase.generated.h"

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API AEnemyBase : public ACharacterBase
{
	GENERATED_BODY()
	
public:
	AEnemyBase();

protected:
	virtual void BeginPlay() override;

	// 상태에 따라 이동속도 변경 함수
	void UpdateMovementSpeed();

public:
	// Getter / Setter
	// 현재 적 상태 반환
	EEnemyState GetEnemyState() const { return CurrentState; }

	// 사망 여부
	bool IsDead() const { return CurrentState == EEnemyState::Dead; }

public:
	// 적 상태 변경
	void SetEnemyState(EEnemyState NewState);

protected:
	// 적의 현재 상태
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|State")
	EEnemyState CurrentState = EEnemyState::Idle;

	// 이동속도 =========================
	// Patrol(순찰) 이동속도
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Movement")
	float PatrolSpeed = 200.0f;

	// Chase(추격) 이동속도
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Movement")
	float ChaseSpeed = 450.0f;

	// Strafe(간보기) 이동속도
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Movement")
	float StarfeSpeed = 150.0f;
};
