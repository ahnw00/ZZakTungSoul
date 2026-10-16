// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CharacterBase.h"
#include "EnemyStates.h"
#include "EnemyBase.generated.h"

// 전방 선언
class UEnemyAIData;

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

	// 상태에 따라 회전 상태 변경 함수
	void UpdateRotationMode();

public:
	// Getter / Setter
	// 현재 적 상태 반환
	EEnemyState GetEnemyState() const { return CurrentState; }

	// 사망 여부
	bool IsDead() const { return CurrentState == EEnemyState::Dead; }

	// Strafe 상태 여부
	bool IsStrafeMode() const { return CurrentState == EEnemyState::Strafe; }

	// 적 애셋 데이터 반환
	const UEnemyAIData* GetEnemyAIData() const { return EnemyAIData; }

public:
	// 적 상태 변경
	void SetEnemyState(EEnemyState NewState);

	// Strafe 상태일 때 캐릭터 매쉬 회전 관련 플래그 설정 함수
	void SetStrafeMode(bool bEnable);

protected:
	// 적의 현재 상태
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|State")
	EEnemyState CurrentState = EEnemyState::Idle;

	// AI Data -> Behavior Tree / Movement Speed
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|AIData")
	TObjectPtr<UEnemyAIData> EnemyAIData;

	// 현재 전투 상태 플래그
	UPROPERTY(VisibleAnywhere, Category = "Enemy|Combat")
	uint32 bIsCombat : 1;

	// Strafe 여부 플래그
	UPROPERTY(VisibleAnywhere, Category = "Enemy|Combat")
	uint32 bIsStrafeMode : 1;

	/* 이동속도 =========================
	// Patrol(순찰) 이동속도
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Movement")
	float PatrolSpeed = 200.0f;

	// Chase(추격) 이동속도
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Movement")
	float ChaseSpeed = 450.0f;

	// Strafe(간보기) 이동속도
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Movement")
	float StarfeSpeed = 150.0f;	//*/
};
