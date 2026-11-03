// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CharacterBase.h"
#include "EnemyStates.h"
#include <Data/DamageInfo.h>
#include <Interface/AIDataInterface.h>
#include <Interface/FatalAttackableInterface.h>
#include "EnemyBase.generated.h"

// 전방 선언
class UEnemyAIData;
class UEnemyAnimationData;
class UCharacterStatComponent;
class APatrolPath;

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API AEnemyBase 
	: public ACharacterBase, public IAIDataInterface, public IFatalAttackableInterface
{
	GENERATED_BODY()
	
public:
	AEnemyBase();

protected:
	virtual void BeginPlay() override;

	// 데미지 처리 판정 -> 상위 클래스에서 인터페이스 사용함
	virtual void ReceiveDamage(const struct FDamageInfo& DamageInfo, const FHitResult& Hit) override;

	// 상태에 따라 이동속도 변경 함수
	virtual void UpdateMovementSpeed();

	// 상태에 따라 회전 상태 변경 함수
	virtual void UpdateRotationMode();

	// Trace에 걸린 액터 처리 함수
	virtual void HandleAttackTraceHit(const FHitResult& Hit);

	// 무기 초기화 함수
	virtual void InitializeWeapon();

	// 사망 처리 함수
	virtual void SetDead() override;

	// 사망 시 실제 실행될 로직 함수 -> 자식 클래스에서 각자 구현
	virtual void DeadStart();

public:
	// Getter / Setter
	// 현재 적 상태 반환
	FORCEINLINE EEnemyState GetEnemyState() const { return CurrentState; }
	FORCEINLINE EEnemyMoveState GetEnemyMoveState() const { return CurrentMoveState; }
	FORCEINLINE EEnemyActionState GetEnemyActionState() const { return CurrentActionState; }

	void SetEnemyMoveState(EEnemyMoveState NewMoveState);
	void SetEnemyActionState(EEnemyActionState NewActionState);

	//---------------------------------------------------------------
	// Possess 시 BT 자동 실행 여부 반환.
	//
	//---------------------------------------------------------------
	virtual bool ShouldAutoStartAI() const { return true; }

	// 사망 여부
	bool IsDead() const { return CurrentState == EEnemyState::Dead; }

	// Strafe 상태 여부
	bool IsStrafeMode() const { return CurrentState == EEnemyState::Strafe; }

	// 적 애셋 데이터 반환
	const UEnemyAIData* GetEnemyAIData() const { return EnemyAIData; }
	const UEnemyAnimationData* GetEnemyAnimationData() const { return EnemyAnimationData; }

	// 적 현재 데미지 데이터 설정
	void SetCurrentDamageInfo(const FDamageInfo& NewDamageInfo) { CurrentDamageInfo = NewDamageInfo; }

public:
	// 적 상태 변경
	void SetEnemyState(EEnemyState NewState);

	// Strafe 상태일 때 캐릭터 매쉬 회전 관련 플래그 설정 함수
	void SetStrafeMode(bool bEnable);

private:
	// 패링 당했을 때 호출될 함수(델리게이트에 바인드해줄 함수)
	void OnGetParried();

////// 인터페이스 섹션
protected:
	// Inherited via IAIDataInterface
	virtual float GetCombatRangeData() override;
	virtual float GetAttackRangeData() override;
	virtual float GetMinMaintainDistanceData() override;

	virtual UEnemyAnimationData* GetAnimationData() override;

	virtual void SetCurrentDamageInfoInterface(const struct FDamageInfo& NewDamageInfo) override;

	virtual APatrolPath* GetPatrolPathData() const override;

protected:
	// 공격할 때 사용할 데미지 정보 저장 변수
	FDamageInfo CurrentDamageInfo;

protected:
	// 적의 현재 상태
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|State")
	EEnemyState CurrentState = EEnemyState::Idle;

	UPROPERTY(VisibleAnywhere, Category = "Enemy|MovementState")
	EEnemyMoveState CurrentMoveState = EEnemyMoveState::None;

	UPROPERTY(VisibleAnywhere, Category = "Enemy|ActionState")
	EEnemyActionState CurrentActionState = EEnemyActionState::None;

	// AI Data -> Behavior Tree / Movement Speed
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Data")
	TObjectPtr<UEnemyAIData> EnemyAIData;

	// Animation Data -> 공격 관련 Montage
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Data")
	TObjectPtr<UEnemyAnimationData> EnemyAnimationData;

	// 무기 Static Mesh
	UPROPERTY(EditDefaultsOnly, Category = "Enemy|Weapon")
	TObjectPtr<class USkeletalMesh> WeaponMesh;

	// 무기 Mesh Component
	UPROPERTY(VisibleAnywhere, Category = "Enemy|Weapon")
	TObjectPtr<class USkeletalMeshComponent> WeaponMeshComponent;

	// 무기 부착 Socket 이름
	UPROPERTY(EditDefaultsOnly, Category = "Enemy|Weapon")
	FName WeaponAttachSocketName = TEXT("hand_rSocket");

	// 앞잡 또는 뒤잡을 위한 박스 컴포넌트
	UPROPERTY(EditDefaultsOnly, Category = "Enemy|FatalAttackBox")
	TObjectPtr<class UBoxComponent> FatalAttackBox;

	// Enemy Stat Component
	UPROPERTY(VisibleAnywhere, Category = "Enemy|Stat")
	TObjectPtr<class UEnemyStatComponent> EnemyStatComponet;

	// 공격 판정을 위한 Trace Component
	UPROPERTY(VisibleAnywhere, Category = "Enemy|Combat")
	TObjectPtr<class UAttackTraceComponent> AttackTraceComponent;

	// Strafe 여부 플래그
	UPROPERTY(VisibleAnywhere, Category = "Enemy|Combat")
	uint32 bIsStrafeMode : 1 = false;

	// Inherited via IFatalAttackableInterface
	bool CanBeFatalAttacked(AActor* Attacker) const override;
	void ReceiveFatalAttack(const FDamageInfo DamageInfo) override;
};
