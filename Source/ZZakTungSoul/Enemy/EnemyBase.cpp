// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyBase.h"
#include <Enemy/EnemyAIController.h>
#include <Enemy/Data/EnemyAIData.h>

#include <GameFramework/CharacterMovementComponent.h>

AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = false;

	// 이동 방향 회전 설정
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	// 초기 이동 속도 설정
	//GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;

	// AIController 설정
	AIControllerClass = AEnemyAIController::StaticClass();

	// AI제어 타입 설정
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	// 초기 상태에 따른 이동속도 설정
	UpdateMovementSpeed();
}

void AEnemyBase::UpdateMovementSpeed()
{
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	if (!Movement || !EnemyAIData)
	{
		return;
	}

	switch (CurrentState)
	{
	case EEnemyState::Idle:
		Movement->MaxWalkSpeed = 0.0f;
		break;

	case EEnemyState::Patrol:
		Movement->MaxWalkSpeed = EnemyAIData->PatrolSpeed;
		break;

	case EEnemyState::Chase:
		Movement->MaxWalkSpeed = EnemyAIData->ChaseSpeed;
		break;

	case EEnemyState::Strafe:
		Movement->MaxWalkSpeed = EnemyAIData->StrafeSpeed;
		break;

	case EEnemyState::Attack:
		break;
	case EEnemyState::Hit:
		break;

	case EEnemyState::Dead:
		Movement->MaxWalkSpeed = 0.0f;
		break;

	default:
		break;
	}
}

void AEnemyBase::SetEnemyState(EEnemyState NewState)
{
	// 상태 검사
	if (IsDead())
	{
		return;
	}

	if (CurrentState == NewState)
	{
		return;
	}

	// 현재 상태 변경
	CurrentState = NewState;

	// 변경된 상태에 따라 이동속도 갱신
	UpdateMovementSpeed();
}

void AEnemyBase::SetStrafeMode(bool bEnable)
{
	// 현재 Strafe 상태와 설정할 상태가 같으면 건너뜀
	if (bIsStrafeMode == bEnable)
	{
		return;
	}

	bIsStrafeMode = bEnable;

	if (bEnable)
	{
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}
	else
	{
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}
}
