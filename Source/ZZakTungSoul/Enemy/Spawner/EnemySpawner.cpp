// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Spawner/EnemySpawner.h"
#include <Enemy/EnemyBase.h>
#include <Enemy/EnemyAIController.h>
#include <Enemy/NormalEnemy/NormalEnemyBase.h>
#include <Enemy/Spawner/EnemySpawnSubsystem.h>
#include <Enemy/AI/EnemyAIBlackboardKey.h>

#include <Engine/World.h>
#include <Engine/TargetPoint.h>
#include <BehaviorTree/BlackboardComponent.h>

// Sets default values
AEnemySpawner::AEnemySpawner()
{

}

// Called when the game starts or when spawned
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	
	// 배열 크기 SpawnData의 개수에 맞춰서 할당
	SpawnedEnemies.SetNum(SpawnDataList.Num());

	// 배열 크기 SpawnData의 개수에 맞춰서 할당 및 모든 요소 초기화
	DeadFlags.Init(false, SpawnDataList.Num());

	SpawnEnemies();

	// Subsystem에 Spawner 등록
	if (UWorld* World = GetWorld())
	{
		if (UEnemySpawnSubsystem* SpawnSubsystem = World->GetSubsystem<UEnemySpawnSubsystem>())
		{
			SpawnSubsystem->RegisterSpawner(this);
		}
	}
}

void AEnemySpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Subsystem에 Spawner 등록 해제
	if (UWorld* World = GetWorld())
	{
		if (UEnemySpawnSubsystem* SpawnSubsystem = World->GetSubsystem<UEnemySpawnSubsystem>())
		{
			SpawnSubsystem->UnregisterSpawner(this);
		}
	}

	Super::EndPlay(EndPlayReason);
}

void AEnemySpawner::SpawnEnemies()
{
	for (int32 ix = 0; ix < SpawnDataList.Num(); ++ix)
	{
		SpawnEnemy(ix);
	}
}

AEnemyBase* AEnemySpawner::SpawnEnemy(int32 Index)
{
	// 예외 처리
	if (false == SpawnDataList.IsValidIndex(Index) || false == SpawnedEnemies.IsValidIndex(Index))
	{
		return nullptr;
	}
	
	// 사망 상태 건너뛰기
	if (DeadFlags[Index])
	{
		return nullptr;
	}

	// 중복 방지
	if (IsValid(SpawnedEnemies[Index].Get()))
	{
		return nullptr;
	}

	// 특정 Enemy 스폰 데이터 저장
	const FEnemySpawnData& SpawnData = SpawnDataList[Index];
	if (nullptr == SpawnData.EnemyClass)
	{
		return nullptr;
	}

	UWorld* World = GetWorld();
	if (nullptr == World)
	{
		return nullptr;
	}

	if (false == IsValid(SpawnData.SpawnPoint))
	{
		return nullptr;
	}

	// 상대 Transform -> 월드 Transform 변환
	const FTransform WorldTransform = SpawnData.SpawnPoint->GetActorTransform();

	// Spawn Actor Deferred
	AEnemyBase* NewEnemy = World->SpawnActorDeferred<AEnemyBase>(
		SpawnData.EnemyClass,
		WorldTransform,
		this,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);

	if (false == IsValid(NewEnemy))
	{
		return nullptr;
	}

	// NormalEnemy일 경우 Patrol 설정
	if (ANormalEnemyBase* NormalEnemy = Cast<ANormalEnemyBase>(NewEnemy))
	{
		NormalEnemy->SetPatrolPath(SpawnData.PatrolPath.Get());
	}

	// 생성 완료
	NewEnemy->FinishSpawning(WorldTransform);

	NewEnemy->OnEnemyDied.AddUObject(this, &AEnemySpawner::HandleEnemyDied, Index);

	// 생성된 Actor 저장
	SpawnedEnemies[Index] = NewEnemy;

	USkeletalMeshComponent* Mesh = NewEnemy->GetMesh();

	return NewEnemy;
}

AEnemyBase* AEnemySpawner::GetSpawnEnemy(int32 Index) const
{
	if (false == SpawnedEnemies.IsValidIndex(Index))
	{
		return nullptr;
	}

	AEnemyBase* Enemy = SpawnedEnemies[Index].Get();

	return IsValid(Enemy) ? Enemy : nullptr;
}

void AEnemySpawner::ResetEnemies()
{
	for (int32 Index = 0; Index < SpawnDataList.Num(); ++Index)
	{
		if (false == SpawnedEnemies.IsValidIndex(Index) || false == DeadFlags.IsValidIndex(Index))
		{
			continue;
		}

		AEnemyBase* Enemy = SpawnedEnemies[Index].Get();

		// 사망한 적은 Destroy 후 재생성
		if (DeadFlags[Index])
		{
			if (IsValid(Enemy))
			{
				if (false == Enemy->Destroy())
				{
					continue;
				}
			}

			SpawnedEnemies[Index] = nullptr;

			DeadFlags[Index] = false;

			AEnemyBase* NewEnemy = SpawnEnemy(Index);
			if (false == IsValid(NewEnemy))
			{
				UE_LOG(LogTemp, Warning, TEXT("Enemy Respawn Failed!~!! : %d"), Index);
			}

			continue;
		}

		// 생존 적 초기화
		if (IsValid(Enemy))
		{
			AEnemyAIController* AIController = Cast<AEnemyAIController>(Enemy->GetController());
			if (IsValid(AIController))
			{
				AIController->EnemyContollerReset();
			}

			ATargetPoint* SpawnPoint = SpawnDataList[Index].SpawnPoint.Get();
			if (false == IsValid(SpawnPoint))
			{
				continue;
			}

			const FTransform SpawnTransform = SpawnPoint->GetActorTransform();

			Enemy->ResetEnemy(SpawnTransform);

			if (IsValid(AIController))
			{
				UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
				if (IsValid(BlackboardComp))
				{
					BlackboardComp->SetValueAsVector(BBKEY_HOMELOCATION, SpawnTransform.GetLocation());
				}
			}
		}
	}
}

void AEnemySpawner::HandleEnemyDied(AEnemyBase* DeadEnemy, int32 Index)
{
	if (SpawnedEnemies[Index].Get() != DeadEnemy)
	{
		return;
	}

	DeadFlags[Index] = true;
}