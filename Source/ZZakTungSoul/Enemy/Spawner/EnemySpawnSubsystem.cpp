// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Spawner/EnemySpawnSubsystem.h"
#include <Enemy/Spawner/EnemySpawner.h>
#include <Enemy/EnemyAIController.h>
#include <Enemy/EnemyBase.h>

#include <Engine/World.h>
#include <TimerManager.h>
#include <GameFramework/PlayerController.h>

void UEnemySpawnSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	InWorld.GetTimerManager().SetTimer(
		ActiveTimerHandle,
		this,
		&UEnemySpawnSubsystem::UpdateEnemyActive,
		Interval,
		true
	);
}

void UEnemySpawnSubsystem::Deinitialize()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ActiveTimerHandle);
	}

	Super::Deinitialize();
}

void UEnemySpawnSubsystem::RegisterSpawner(AEnemySpawner* Spawner)
{
	// 유효성 검사
	if (false == IsValid(Spawner))
	{
		return;
	}

	// 중복 방지
	RegisteredSpawners.AddUnique(Spawner);

	UE_LOG(LogTemp, Warning, TEXT("EnemySpawnSubsystem Registered: %s | Count: %d"), *GetNameSafe(Spawner), GetSpawnerCount());
}

void UEnemySpawnSubsystem::UnregisterSpawner(AEnemySpawner * Spawner)
{
	int32 Index = RegisteredSpawners.Num() - 1;

	for (Index; Index >= 0; --Index)
	{
		const TWeakObjectPtr<AEnemySpawner>& RemoveSpawner = RegisteredSpawners[Index];

		if (nullptr == RemoveSpawner || RemoveSpawner.Get() == Spawner)
		{
			RegisteredSpawners.RemoveAt(Index);
		}
	}
}

int32_t UEnemySpawnSubsystem::GetSpawnerCount() const
{
	int32 Count = 0;

	for (const TWeakObjectPtr<AEnemySpawner>& Spawner : RegisteredSpawners)
	{
		if (Spawner.IsValid())
		{
			++Count;
		}
	}

	return Count;
}

void UEnemySpawnSubsystem::GetAllEnemies(TArray<AEnemyBase*>& OutEnemies) const
{
	OutEnemies.Reset();

	for (const TWeakObjectPtr<AEnemySpawner>& SpawnerPtr : RegisteredSpawners)
	{
		AEnemySpawner* Spawner = SpawnerPtr.Get();
		if (false == IsValid(Spawner))
		{
			continue;
		}

		for (int32 Index = 0; Index < Spawner->GetSpawnEnemyCount(); ++Index)
		{
			AEnemyBase* Enemy = Spawner->GetSpawnEnemy(Index);
			if (IsValid(Enemy))
			{
				OutEnemies.Add(Enemy);
			}
		}
	}
}

void UEnemySpawnSubsystem::ResetAllEnemies()
{
	for (const TWeakObjectPtr<AEnemySpawner>& SpawnerPtr : RegisteredSpawners)
	{
		AEnemySpawner* Spawner = SpawnerPtr.Get();
		if (false == IsValid(Spawner))
		{
			continue;
		}

		Spawner->ResetEnemies();
	}

	// 초기화 후 상태 갱신
	UpdateEnemyActive();
}

void UEnemySpawnSubsystem::UpdateEnemyActive()
{
	UWorld* World = GetWorld();
	if (false == IsValid(World))
	{
		return;
	}

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (false == IsValid(PlayerController))
	{
		return;
	}

	APawn* Player = PlayerController->GetPawn();
	if (false == IsValid(Player))
	{
		return;
	}

	const FVector PlayerLocation = Player->GetActorLocation();

	const float ActiveDistanceSquare = FMath::Square(ActiveDistance);
	const float DeactiveDistanceSquare = FMath::Square(DeactiveDistance);

	TArray<AEnemyBase*> Enemies;
	GetAllEnemies(Enemies);

	for (AEnemyBase* Enemy : Enemies)
	{
		// 유효성 검사
		if (false == IsValid(Enemy) || Enemy->IsDeadState())
		{
			continue;
		}

		// BT를 자동실행 안하면 건너뛰기
		if (false == Enemy->ShouldAutoStartAI())
		{
			continue;
		}

		AEnemyAIController* AIController = Cast<AEnemyAIController>(Enemy->GetController());
		if (false == IsValid(AIController))
		{
			continue;
		}

		const float Distance = FVector::DistSquared(PlayerLocation, Enemy->GetActorLocation());

		if (false == AIController->IsAIActive())
		{
			// AI On
			if (Distance <= ActiveDistanceSquare)
			{
				AIController->SetAIActive(true);
			}
		}
		else
		{
			// AI On
			if (Distance >= DeactiveDistanceSquare)
			{
				AIController->SetAIActive(false);
			}
		}
	}
}
