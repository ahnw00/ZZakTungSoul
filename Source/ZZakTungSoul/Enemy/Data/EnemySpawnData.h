#pragma once

#include "CoreMinimal.h"
#include "EnemySpawnData.generated.h"

USTRUCT(BlueprintType)
struct FEnemySpawnData
{
	GENERATED_BODY()

	// 생성할 Enemy 클래스
	UPROPERTY(EditAnywhere, Category = "Spawn")
	TSubclassOf<class AEnemyBase> EnemyClass;

	// 스폰 위치
	UPROPERTY(EditAnywhere, Category = "Spawn")
	TObjectPtr<class ATargetPoint> SpawnPoint = nullptr;

	// 생성된 Enemy에게 전달할 순찰 경로
	UPROPERTY(EditAnywhere, Category = "Spawn")
	TObjectPtr<class APatrolPath> PatrolPath = nullptr;
};