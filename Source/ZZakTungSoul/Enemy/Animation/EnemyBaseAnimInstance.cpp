// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Animation/EnemyBaseAnimInstance.h"
#include <Enemy/EnemyBase.h>

#include <GameFramework/CharacterMovementComponent.h>

UEnemyBaseAnimInstance::UEnemyBaseAnimInstance()
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage> StunMontageRef(
		TEXT("/Game/Blueprints/Anim/SwordAndShield/Warrior_SwordAndShield_Stun2.Warrior_SwordAndShield_Stun2")
	);
	if (StunMontageRef.Succeeded())
	{
		StunMontage = StunMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> GetFatalAttackMontageRef(
		TEXT("/Game/Blueprints/Anim/UnArmed/Warrior_GetFrontFatalAttack1_Montage.Warrior_GetFrontFatalAttack1_Montage")
	);
	if (GetFatalAttackMontageRef.Succeeded())
	{
		GetFrontFatalAttackMontage = GetFatalAttackMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> GetBackFatalAttackMontageRef(
		TEXT("/Game/Blueprints/Anim/UnArmed/Warrior_GetBackFatalAttack_Montage.Warrior_GetBackFatalAttack_Montage")
	);
	if (GetBackFatalAttackMontageRef.Succeeded())
	{
		GetBackFatalAttackMontage = GetBackFatalAttackMontageRef.Object;
	}
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

	if (nullptr == Movement || nullptr == Owner)
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
	CurrentMoveState = Owner->GetEnemyMoveState();

	// 이동/정지 상태 설정
	//bIsIdle = CurrentState == EEnemyState::Idle;
	bIsIdle = CurrentMoveState == EEnemyMoveState::Idle;

	// 간보기 상태 설정
	//bIsStrafe = CurrentState == EEnemyState::Strafe;
	bIsStrafe = CurrentMoveState == EEnemyMoveState::Strafe;
}

void UEnemyBaseAnimInstance::PlayStunMontage()
{
	StopAllMontages(1.f);
	Montage_Play(StunMontage);

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &UEnemyBaseAnimInstance::OnStunMontageEnded);

	Montage_SetEndDelegate(EndDelegate, StunMontage);
}

void UEnemyBaseAnimInstance::PlayGetFrontFatalAttackMontage()
{
	StopAllMontages(1.f);
	Montage_Play(GetFrontFatalAttackMontage);
}

void UEnemyBaseAnimInstance::PlayGetBackFatalAttackMontage()
{
	StopAllMontages(1.f);
	Montage_Play(GetBackFatalAttackMontage);

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &UEnemyBaseAnimInstance::OnFatalMontageEnded);

	Montage_SetEndDelegate(EndDelegate, GetBackFatalAttackMontage);
}

void UEnemyBaseAnimInstance::OnFatalMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (bInterrupted)
	{
		return;
	}

	AEnemyBase* Enemy = Cast<AEnemyBase>(TryGetPawnOwner());
	if (false == IsValid(Enemy))
	{
		return;
	}

	Enemy->OnFatalAttackFinished();
}

void UEnemyBaseAnimInstance::OnStunMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (bInterrupted)
	{
		return;
	}

	AEnemyBase* Enemy = Cast<AEnemyBase>(TryGetPawnOwner());
	if (false == IsValid(Enemy))
	{
		return;
	}

	Enemy->OnStunFinished();
}
