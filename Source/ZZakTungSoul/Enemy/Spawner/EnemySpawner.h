// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Enemy/Data/EnemySpawnData.h>
#include "EnemySpawner.generated.h"

UCLASS()
class ZZAKTUNGSOUL_API AEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	FORCEINLINE int32 GetSpawnEnemyCount() const { return SpawnedEnemies.Num(); }

public:	
	// 설정된 Enemy 스폰하기전 Data 순회
	void SpawnEnemies();

	// 지정된 Enemy 스폰
	AEnemyBase* SpawnEnemy(int32 Index);

	// 생성된 Enemy 가져오기
	AEnemyBase* GetSpawnEnemy(int32 Index) const;

	// Reset Enemies
	void ResetEnemies();

protected:
	// Enemy 사망 알림
	void HandleEnemyDied(AEnemyBase* DeadEnemy, int32 Index);

protected:
	// 스폰시 필요한 데이터
	UPROPERTY(EditInstanceOnly, Category = "Spawner|Enemy")
	TArray<FEnemySpawnData> SpawnDataList;

	// 스폰된 Enemy 참조
	UPROPERTY(Transient)
	TArray<TObjectPtr<class AEnemyBase>> SpawnedEnemies;

	// Enemy 사망 상태 저장
	UPROPERTY(Transient)
	TArray<bool> DeadFlags;

};
