// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyBase.h"
#include <Enemy/Waypoint/PatrolPath.h>
#include "NormalEnemyBase.generated.h"

// 전방 선언
//class APatrolPath;
class UEnemyAnimationMontageData;
class AItemObject;
class AEnemySpawner;

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API ANormalEnemyBase : public AEnemyBase
{
	GENERATED_BODY()
	
public:
	// Getter / Setter
	// Patrol Path
	FORCEINLINE APatrolPath* GetPatrolPath() const { return PatrolPath.Get(); }

	// Patrol Path 설정
	FORCEINLINE void SetPatrolPath(APatrolPath* InPatrolPath) { PatrolPath = InPatrolPath; }

	// 행동 Montage Data Getter
	FORCEINLINE const UEnemyAnimationMontageData* GetActionMontageData() const { return ActionMontageData.Get(); }

	// 선택한 공격 정보 Getter
	FORCEINLINE EEnemySelectAttack GetEnemySelectAttack() const { return CurrentSelectAttack; }

	// 선택한 공격 정보 Setter
	FORCEINLINE void SetEnemySelectAttack(EEnemySelectAttack NewSelectAttack) { CurrentSelectAttack = NewSelectAttack; }

public:
	// 상위 클래스의 Interface
	FORCEINLINE  virtual APatrolPath* GetPatrolPathData() const override { return PatrolPath.Get(); }

	virtual bool GetActionDamageInfo(int32 Index, FDamageInfo& DamageInfo) const override;

	// Spawner Getter
	void SetEnemySpawner(AEnemySpawner* InSpawner);

protected:
	virtual void DeadStart() override;

	virtual void HitStart() override;

	void EnemyDropItem();

protected:
	TWeakObjectPtr<AEnemySpawner> EnemySpawner;

protected:
	// PatrolPath 참조
	UPROPERTY(EditInstanceOnly, Category = "Enemy|Patrol")
	TObjectPtr<APatrolPath> PatrolPath;

	// Animation Montage AssetData
	UPROPERTY(EditAnywhere, Category = "Enemy|ActionMontage")
	TObjectPtr<UEnemyAnimationMontageData> ActionMontageData;

	// 공격 선택
	UPROPERTY(VisibleAnywhere, Category = "Enemy|ActionMontage|SelectAttack")
	EEnemySelectAttack CurrentSelectAttack = EEnemySelectAttack::Attack01;

	// 드랍 아이템
	UPROPERTY(EditDefaultsOnly, Category = "Enemy|DropItem")
	TSubclassOf<AItemObject> DropItemClass;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy|DropItem")
	float DropPercent = 10.0f;

	// 중복 드랍 방지
	UPROPERTY(VisibleAnywhere, Category = "Enemy|DropItem")
	uint32 bIsDrop : 1 = false;
};
