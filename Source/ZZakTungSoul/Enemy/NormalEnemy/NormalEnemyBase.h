// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyBase.h"
#include <Enemy/Waypoint/PatrolPath.h>
#include "NormalEnemyBase.generated.h"

// 전방 선언
//class APatrolPath;

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

public:
	// 상위 클래스의 Interface
	FORCEINLINE  virtual APatrolPath* GetPatrolPathData() const override { return PatrolPath.Get(); }

protected:
	// PatrolPath 참조
	UPROPERTY(EditInstanceOnly, Category = "Enemy|Patrol")
	TObjectPtr<APatrolPath> PatrolPath;
};
