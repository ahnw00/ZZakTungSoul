// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Enemy/EnemyBase.h>
#include "EnemyBoss.generated.h"

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API AEnemyBoss : public AEnemyBase
{
	GENERATED_BODY()
public:
	AEnemyBoss();
	
protected:
	virtual void BeginPlay() override;

	// 데미지 처리 판정 -> 상위 클래스에서 인터페이스 사용함
	virtual void ReceiveDamage(const struct FDamageInfo& DamageInfo, const FHitResult& Hit) override;

	// 상태에 따라 이동속도 변경 함수
	virtual void UpdateMovementSpeed() override;

	// 상태에 따라 회전 상태 변경 함수
	virtual void UpdateRotationMode() override;

	// Trace에 걸린 액터 처리 함수
	virtual void HandleAttackTraceHit(const FHitResult& Hit) override;

	// 무기 초기화 함수
	virtual void InitializeWeapon() override;

protected:

};
