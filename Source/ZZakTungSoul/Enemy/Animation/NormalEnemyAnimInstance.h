// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Animation/EnemyBaseAnimInstance.h"
#include <Enemy/EnemyStates.h>
#include "NormalEnemyAnimInstance.generated.h"

// 전방 선언
class ANormalEnemyBase;
class UAnimMontage;

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API UNormalEnemyAnimInstance : public UEnemyBaseAnimInstance
{
	GENERATED_BODY()
	
protected:
	virtual void NativeInitializeAnimation() override;

public:
	// 상태에 따른 Montage실행 함수
	bool PlayActionMontage();

private:
	// Montage Data 가져오기
	void InitActionMontage();

protected:
	UPROPERTY()
	TObjectPtr<ANormalEnemyBase> NormalEnemyOwner;

	// Montage Data -----------------------------------
	// 회피
	UPROPERTY()
	TObjectPtr<UAnimMontage> AvoidMontage;

	// 공격
	UPROPERTY()
	TObjectPtr<UAnimMontage> Attack01Montage;

	UPROPERTY()
	TObjectPtr<UAnimMontage> Attack02Montage;

	// 피격
	UPROPERTY()
	TObjectPtr<UAnimMontage> HitMontage;

	// 그로기
	UPROPERTY()
	TObjectPtr<UAnimMontage> GroggyMontage;

	// 사망
	UPROPERTY()
	TObjectPtr<UAnimMontage> DeadMontage;
};
