// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "EnemySpawnSubsystem.generated.h"

// 전방 선언
class AEnemySpawner;
class AEnemyBase;

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API UEnemySpawnSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
protected:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	virtual void Deinitialize() override;

public:
	// Spawner 등록 및 해제
	void RegisterSpawner(AEnemySpawner* Spawner);
	void UnregisterSpawner(AEnemySpawner* Spawner);

	// 등록된 Spawner 개수
	int32_t GetSpawnerCount() const;

	// 생성된 Enemy 조회
	void GetAllEnemies(TArray<AEnemyBase*>& OutEnemies) const;

	void ResetAllEnemies();

protected:
	// 모든 Enemy Active 여부 갱신
	void UpdateEnemyActive();

private:
	// Spawner 참조
	TArray<TWeakObjectPtr<AEnemySpawner>> RegisteredSpawners;

	// 거리 검사 Timer
	FTimerHandle ActiveTimerHandle;

	// AI 활성화 거리
	UPROPERTY()
	float ActiveDistance = 3500.0f;

	// AI 비활성화 거리
	UPROPERTY()
	float DeactiveDistance = 4500.0f;

	// 검사 간격
	UPROPERTY()
	float Interval = 0.5f;
};
