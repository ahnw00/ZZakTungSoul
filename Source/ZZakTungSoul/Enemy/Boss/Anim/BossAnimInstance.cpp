// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Boss/Anim/BossAnimInstance.h"
#include <Enemy/Boss/Boss.h>

#include <GameFramework/CharacterMovementComponent.h>


UBossAnimInstance::UBossAnimInstance()
{

}

void UBossAnimInstance::NativeInitializeAnimation()
{
	Owner = Cast<ABoss>(TryGetPawnOwner());
}

void UBossAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (!Owner)
	{
		return;
	}

	Velocity = Owner->GetVelocity();

	//---------------------------------------------------------------
	// 정지 상태에서도 속도와 방향을 갱신하여 이전 값이 남지 않도록 처리.
	//
	//---------------------------------------------------------------
	Speed = Velocity.Size2D();
	Direction = 0.0f;
	if (!Velocity.IsNearlyZero())
	{
		FRotator MoveRot = Velocity.Rotation();

		FRotator PawnRot = Owner->GetActorRotation();

		Direction = FMath::FindDeltaAngleDegrees(PawnRot.Yaw, MoveRot.Yaw);
	}
}
