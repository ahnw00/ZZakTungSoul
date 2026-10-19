// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Animation/EnemyBaseAnimInstance.h"
#include <Enemy/EnemyBase.h>

#include <GameFramework/CharacterMovementComponent.h>

UEnemyBaseAnimInstance::UEnemyBaseAnimInstance()
{
	
}

void UEnemyBaseAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// 자신을 소유하고 있는 Enemy 저장
	Owner = Cast<AEnemyBase>(TryGetPawnOwner());
	
	// 캐릭터 무브먼트 저장
	if (Owner)
	{
		Movement = Owner->GetCharacterMovement();
	}
}

void UEnemyBaseAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (nullptr == Movement)
	{
		return;
	}

	// 현재 속도 저장
	Velocity = Movement->Velocity;

	// 지상에서 이동하는 속력
	GroundSpeed = Velocity.Size2D();

	// 전후좌우 이동하는 속력
	FVector LocalVelocity = Owner->GetActorTransform().InverseTransformVectorNoScale(Velocity);
	
	ForwardSpeed = LocalVelocity.X;
	RightSpeed = LocalVelocity.Y;

	// Enemy 현재 상태 초기화
	CurrentState = Owner->GetEnemyState();

	// 이동/정지 상태 설정
	bIsIdle = CurrentState == EEnemyState::Idle;

	// 간보기 상태 설정
	bIsStrafe = CurrentState == EEnemyState::Strafe;
}