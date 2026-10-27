// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CharacterBase.h"
#include "EnemyStates.h"
#include "EnemyBase.generated.h"

// 전방 선언
class UEnemyAIData;
class UEnemyAnimationData;
class UCharacterStatComponent;

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

	// 데미지 처리 판정
	virtual void ReceiveDamage(const struct FDamageInfo& DamageInfo, const FHitResult& Hit) override;

	// 상태에 따라 이동속도 변경 함수
	void UpdateMovementSpeed();

	// 상태에 따라 회전 상태 변경 함수
	void UpdateRotationMode();

	// Trace에 걸린 액터 처리 함수
	void HandleAttackTraceHit(const FHitResult& Hit);

	// 무기 초기화 함수
	void InitializeWeapon();

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
	const UEnemyAnimationData* GetEnemyAnimationData() const { return EnemyAnimationData; }

public:
	// 적 상태 변경
	void SetEnemyState(EEnemyState NewState);

	// Strafe 상태일 때 캐릭터 매쉬 회전 관련 플래그 설정 함수
	void SetStrafeMode(bool bEnable);

protected:
	// 타겟의 스탯 컴포넌트
	UPROPERTY()
	TWeakObjectPtr<UCharacterStatComponent> TargetStatComp;

	// Enemy Stat Component
	TObjectPtr<class UEnemyStatComponent> EnemyStatComponet;

protected:
	// 적의 현재 상태
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|State")
	EEnemyState CurrentState = EEnemyState::Idle;

	// AI Data -> Behavior Tree / Movement Speed
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Data")
	TObjectPtr<UEnemyAIData> EnemyAIData;

	// Animation Data -> 공격 관련 Montage
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Data")
	TObjectPtr<UEnemyAnimationData> EnemyAnimationData;

	// 무기 Static Mesh
	UPROPERTY(EditDefaultsOnly, Category = "Enemy|Weapon")
	TObjectPtr<class UStaticMesh> WeaponMesh;

	// 무기 Mesh Component
	UPROPERTY(VisibleAnywhere, Category = "Enemy|Weapon")
	TObjectPtr<class UStaticMeshComponent> WeaponMeshComponent;

	// 무기 부착 Socket 이름
	UPROPERTY(EditDefaultsOnly, Category = "Enemy|Weapon")
	FName WeaponAttachSocketName = TEXT("hand_rSocket");

	// 공격 판정을 위한 Trace Component
	UPROPERTY(VisibleAnywhere, Category = "Enemy|Combat")
	TObjectPtr<class UAttackTraceComponent> AttackTraceComponent;

	

	// Strafe 여부 플래그
	UPROPERTY(VisibleAnywhere, Category = "Enemy|Combat")
	uint32 bIsStrafeMode : 1 = false;
};
