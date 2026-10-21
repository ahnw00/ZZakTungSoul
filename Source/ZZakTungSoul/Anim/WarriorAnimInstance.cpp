// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/WarriorAnimInstance.h"

#include "Warrior/Warrior.h"

UWarriorAnimInstance::UWarriorAnimInstance()
{
	CurrentPawnSpeed = 0.f;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> RollMontageRef(
		TEXT("/Game/RetargetAnimations/SwordAndShield/Warrior_Dodge_Montage.Warrior_Dodge_Montage")
	);
	if (RollMontageRef.Succeeded())
	{
		RollMontage = RollMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> BackStepMontageRef(
		TEXT("/Game/RetargetAnimations/SwordAndShield/Warrior_BackStep_Montage.Warrior_BackStep_Montage")
	);
	if (BackStepMontageRef.Succeeded())
	{
		BackStepMontage = BackStepMontageRef.Object;
	}

	//None						// 아무런 무기를 안든 상태
	//Block,					// 무기를 든 채로 방패로 막기
	//BlockWithoutWeapon,		// 무기를 안 든 채로 방패로 막기
	//OneHand,					// 방패 든 채로 무기 들고 있는 상태		
	//OneHandWithoutShield,		// 방패 안 든 채로 무기 들고 있는 상태	
	//TwoHand					// 두 손으로 무기 들고 있는 상태

	//if (!HasAnyFlags(RF_ClassDefaultObject))
	//{
	//	return;
	//}

	//None,	아무런 무기를 안든 상태
	const int32 NoneAttackMontageMaxCount = 4;
	AttackMontage.Add(EOverlayState::None);

	for (int32 i = 1; i <= NoneAttackMontageMaxCount; i++)
	{
		FString ObjectPath = FString::Printf(
			TEXT("/Game/RetargetAnimations/UnArmed/Warrior_Unarmed_Attack_Montage_%d.Warrior_Unarmed_Attack_Montage_%d"), i, i);

		ConstructorHelpers::FObjectFinder<UAnimMontage> NoneAttackMontageRef(*ObjectPath);

		if (NoneAttackMontageRef.Succeeded())
		{
			AttackMontage[EOverlayState::None].Entries.Add(NoneAttackMontageRef.Object);
		}
	}

	//OneHand, 방패 든 채로 무기 들고 있는 상태
	const int32 OneHandAttackMontageMaxCount = 4;
	AttackMontage.Add(EOverlayState::OneHand);

	for (int32 i = 1; i <= OneHandAttackMontageMaxCount; i++)
	{
		FString ObjectPath = FString::Printf(
			TEXT("/Game/RetargetAnimations/SwordAndShield/Warrior_SwordAndShield_Attack_Montage_%d.Warrior_SwordAndShield_Attack_Montage_%d"), i, i);
		ConstructorHelpers::FObjectFinder<UAnimMontage> OneHandAttackMontageRef(*ObjectPath);

		if (OneHandAttackMontageRef.Succeeded())
		{
			AttackMontage[EOverlayState::OneHand].Entries.Add(OneHandAttackMontageRef.Object);
		}
	}

	//TwoHand, 두 손으로 무기 들고 있는 상태
	const int32 TwoHandAttackMontageMaxCount = 4;
	AttackMontage.Add(EOverlayState::TwoHand);

	for (int32 i = 1; i <= TwoHandAttackMontageMaxCount; i++)
	{
		FString ObjectPath = FString::Printf(
			TEXT("/Game/RetargetAnimations/TwoHanded/Warrior_TwoHand_Attack1_Montage_%d.Warrior_TwoHand_Attack1_Montage_%d"), i, i);
		ConstructorHelpers::FObjectFinder<UAnimMontage> TwoHandAttackMontageRef(*ObjectPath);

		if (TwoHandAttackMontageRef.Succeeded())
		{
			AttackMontage[EOverlayState::TwoHand].Entries.Add(TwoHandAttackMontageRef.Object);
		}
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
		{
			StopAttackMontage();
		}

		Warrior->ResetCombo();
	}
}

void UWarriorAnimInstance::PlayMontage(const EActionState CurrentActionState)
{

}

bool UWarriorAnimInstance::PlayActionMontage()
{
	//UE_LOG(LogTemp, Warning, TEXT("PlayActionMontage()== Start "));
	auto Pawn = TryGetPawnOwner();

	if (!Pawn)
	{
		return false;
	}

	AWarrior* Warrior = Cast<AWarrior>(Pawn);

	if (!Warrior)
	{
		return false;
	}

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
	{
		EOverlayState CurOverlayState = Warrior->GetOverlayState();
		const int32 ComboIndex = Warrior->GetCombo();
		
		const auto Montages = AttackMontage.Find(CurOverlayState);

		if (!Montages)
		{
			return false;
		}

		if (!Montages->Entries.IsValidIndex(ComboIndex))
		{
			return false;
		}

		//UE_LOG(LogTemp, Warning, TEXT("PlayActionMontage()== Montage_Play ComboIndex: %d"), ComboIndex);
		float Time = Montage_Play(Montages->Entries[ComboIndex]);

		//UE_LOG(LogTemp, Warning, TEXT("PlayActionMontage()== Montage_Play Montages->Entries[ComboIndex]: %s"), *Montages->Entries[ComboIndex]->GetName());
		//UE_LOG(LogTemp, Warning, TEXT("PlayActionMontage()== Montage_Play Time: %f"), Time);
		//UE_LOG(LogTemp, Warning, TEXT("PlayActionMontage()== Montage_Play ComboIndex: %d"), ComboIndex);

		return Time > 0.f;
		//Montage_Play(AttackMontage[CurOverlayState].Entries[ComboIndex]);
	}
		break;
	case EActionState::Hit:
		//Montage_Play();
		break;
	default:
		break;
	}

	return true;
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
