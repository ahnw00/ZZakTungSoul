// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Animation/NormalEnemyAnimInstance.h"
#include <Enemy/NormalEnemy/NormalEnemyBase.h>
#include <Enemy/Data/EnemyAnimationMontageData.h>

#include <Animation/AnimMontage.h>

void UNormalEnemyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// 자신을 소유한 Owner 설정
	NormalEnemyOwner = Cast<ANormalEnemyBase>(TryGetPawnOwner());

	// Montage 초기화
	InitActionMontage();
}

bool UNormalEnemyAnimInstance::PlayActionMontage()
{
	if (false == IsValid(NormalEnemyOwner))
	{
		return false;
	}

	EEnemyActionState CurrentEnemyActionState = NormalEnemyOwner->GetEnemyActionState();

	switch (CurrentEnemyActionState)
	{
	case EEnemyActionState::None:
		break;

	case EEnemyActionState::Attack:
		switch (NormalEnemyOwner->GetEnemySelectAttack())
		{
		case EEnemySelectAttack::Attack01:
			if (IsValid(Attack01Montage))
			{
				return Montage_Play(Attack01Montage) > 0.0f;
			}
			break;

		case EEnemySelectAttack::Attack02:
			if (IsValid(Attack02Montage))
			{
				return Montage_Play(Attack02Montage) > 0.0f;
			}
			break;

		default:
			break;
		}

		break;

	case EEnemyActionState::Hit:
		if (IsValid(HitMontage))
		{
			return Montage_Play(HitMontage) > 0.0f;
		}
		break;

	case EEnemyActionState::Avoid:
		if (IsValid(AvoidMontage))
		{
			return Montage_Play(AvoidMontage) > 0.0f;
		}
		break;

	case EEnemyActionState::Block:
		break;

	case EEnemyActionState::Groggy:
		if (IsValid(GroggyMontage))
		{
			return Montage_Play(GroggyMontage) > 0.0f;
		}
		break;

	case EEnemyActionState::Dead:
		if (IsValid(DeadMontage))
		{
			return Montage_Play(DeadMontage) > 0.0f;
		}
		break;

	default:
		break;
	}

	return false;
}

void UNormalEnemyAnimInstance::InitActionMontage()
{
	if (false == IsValid(NormalEnemyOwner))
	{
		return;
	}

	const UEnemyAnimationMontageData* MontageData = NormalEnemyOwner->GetActionMontageData();
	if (false == IsValid(MontageData))
	{
		return;
	}

	AvoidMontage = MontageData->AvoidMontage;

	Attack01Montage = MontageData->ComboAttack01.Montage;
	Attack02Montage = MontageData->ComboAttack02.Montage;

	HitMontage = MontageData->HitMontage;
	GroggyMontage = MontageData->GroggyMontage;
	DeadMontage = MontageData->DeadMontage;
}
