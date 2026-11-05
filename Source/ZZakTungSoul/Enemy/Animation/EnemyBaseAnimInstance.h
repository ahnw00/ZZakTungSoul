// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include <Enemy/EnemyStates.h>
#include "EnemyBaseAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API UEnemyBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UEnemyBaseAnimInstance();

protected:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:
	// 일단은 스턴용만 만들어둠(일반화해둔 함수가 필요함)
	void PlayStunMontage();

	void PlayGetFrontFatalAttackMontage();
	void PlayGetBackFatalAttackMontage();

protected:
	// AnimInstance를 소유하는 오너
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character")
	TObjectPtr<class AEnemyBase> Owner;

	// 캐릭터 무브먼트
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character")
	TObjectPtr<class UCharacterMovementComponent> Movement;

	// 캐릭터 속도 값 (애니메이션 상태 전환 시 사용)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character")
	FVector Velocity;

	// 이동 빠르기 (Blend Space)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character")
	float GroundSpeed = 0.0f;

	// 전후 이동 빠르기 (Strafe Blend Space)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character")
	float ForwardSpeed = 0.0f;

	// 좌우 이동 빠르기 (Strafe Blend Space)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character")
	float RightSpeed = 0.0f;

	// Enemy 현재 상태
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character")
	EEnemyState CurrentState = EEnemyState::Idle;

	// 이동 중인지 확인할 변수 값
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character")
	uint32 bIsIdle : 1;

	// 간보기 중인지 확인할 변수 값
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character")
	uint32 bIsStrafe : 1 = false;

protected:
	UPROPERTY()
	TObjectPtr<UAnimMontage> StunMontage;

	UPROPERTY()
	TObjectPtr<UAnimMontage> GetFrontFatalAttackMontage;

	UPROPERTY()
	TObjectPtr<UAnimMontage> GetBackFatalAttackMontage;
};
