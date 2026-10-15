// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/WarriorAnimInstance.h"

#include "Warrior/Warrior.h"

UWarriorAnimInstance::UWarriorAnimInstance()
{
	CurrentPawnSpeed = 0.f;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> RollMontageRef(
		TEXT("/Game/RetargetAnimations/Warrior_Dodge_Montage.Warrior_Dodge_Montage")
	);
	if (RollMontageRef.Succeeded())
	{
		RollMontage = RollMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> BackStepMontageRef(
		TEXT("/Game/RetargetAnimations/Warrior_BackStep_Montage.Warrior_BackStep_Montage")
	);
	if (BackStepMontageRef.Succeeded())
	{
		BackStepMontage = BackStepMontageRef.Object;
	}
}

void UWarriorAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();
	if (Pawn)
	{
		// CurrentPawnSpeed를 ABP에서 사용하기 위해 AnimInstance에서 관리
		CurrentPawnSpeed = Pawn->GetVelocity().Size();
		AWarrior* Warrior = Cast<AWarrior>(Pawn);
		if (!Warrior) return;

		MoveState = Warrior->GetMoveState();
		ActionState = Warrior->GetActionState();
		OverlayState = Warrior->GetOverlayState();

		if (!Pawn->GetVelocity().IsNearlyZero())
		{
			FRotator MoveRot = Pawn->GetVelocity().Rotation();

			FRotator PawnRot;
			if (Warrior->IsLockOn())
				PawnRot = Pawn->GetControlRotation();
			else
				PawnRot = Pawn->GetActorRotation();

			Direction = FMath::FindDeltaAngleDegrees(PawnRot.Yaw, MoveRot.Yaw);
		}
	}
}

void UWarriorAnimInstance::AnimNotify_ActionEnd()
{
	auto Pawn = TryGetPawnOwner();

	if (AWarrior* Warrior = Cast<AWarrior>(Pawn))
	{
		UE_LOG(LogTemp, Log, TEXT("Action Finished"));
		Warrior->EndAction();
	}
}

void UWarriorAnimInstance::AnimNotify_ComboCheck()
{
	UE_LOG(LogTemp, Log, TEXT("ComboCheck"));
	auto Pawn = TryGetPawnOwner();

	if (AWarrior* Warrior = Cast<AWarrior>(Pawn))
	{
		if (Warrior->GetCombo() < 1)
			StopAttackMontage();

		Warrior->ResetCombo();
	}
}

void UWarriorAnimInstance::PlayMontage(const EActionState CurrentActionState)
{
}

void UWarriorAnimInstance::PlayActionMontage()
{
	auto Pawn = TryGetPawnOwner();
	if (!Pawn)
		return;

	AWarrior* Warrior = Cast<AWarrior>(Pawn);
	if (!Warrior)
		return;

	EActionState CurState = Warrior->GetActionState();

	switch (CurState)
	{
	case EActionState::None:
		break;
	case EActionState::Roll:
		Montage_Play(RollMontage);
		break;
	case EActionState::BackStep:
		Montage_Play(BackStepMontage);
		break;
	case EActionState::Attack:
		Montage_Play(AttackMontage);
		break;
	case EActionState::Hit:
		//Montage_Play();
		break;
	default:
		break;
	}
}

void UWarriorAnimInstance::PlayRollMontage()
{
}

void UWarriorAnimInstance::PlayBackStepMontage()
{
}

void UWarriorAnimInstance::PlayAttackMontage()
{
}

void UWarriorAnimInstance::StopMontage(UAnimMontage* Montage, float BlendOutTime)
{
	Montage_Stop(BlendOutTime, Montage);
}

void UWarriorAnimInstance::StopAttackMontage()
{
}
