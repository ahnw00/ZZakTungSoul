// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/NormalEnemy/AmbushEnemy.h"
#include <Enemy/EnemyAIController.h>

AAmbushEnemy::AAmbushEnemy()
{
	CurrentAmbushState = EAmbushState::Waiting;
}

bool AAmbushEnemy::ActiveAmbush(AActor* TargetActor)
{
	// 이미 활성화 된 경우 건너뛰기
	if (CurrentAmbushState != EAmbushState::Waiting)
	{
		return false;
	}

	// 유효성 검사
	if (IsDeadState() || false == IsValid(TargetActor))
	{
		return false;
	}

	AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController());
	if (false == IsValid(AIController))
	{
		return false;
	}

	CurrentAmbushState = EAmbushState::Trigger;
	if (false == AIController->StartAIFirstAction(TargetActor))
	{
		CurrentAmbushState = EAmbushState::Waiting;
		return false;
	}

	CurrentAmbushState = EAmbushState::Active;

	return true;
}

void AAmbushEnemy::ResetEnemy(const FTransform& SpawnTransform)
{
	Super::ResetEnemy(SpawnTransform);

	if (IsDeadState())
	{
		return;
	}

	CurrentAmbushState = EAmbushState::Waiting;
}
