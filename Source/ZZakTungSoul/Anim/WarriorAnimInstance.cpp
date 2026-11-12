// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/WarriorAnimInstance.h"
#include "Warrior/Warrior.h"

// Item
#include <ItemData/WeaponDataAsset.h>
#include <Component/EquipmentComponent.h>
#include <Component/InventoryComponent.h>
#include <Engine/AssetManager.h>

UWarriorAnimInstance::UWarriorAnimInstance()
{
	CurrentPawnSpeed = 0.f;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> UnarmedRollMontageRef(
		TEXT("/Game/Blueprints/Anim/SwordAndShield/Warrior_SwordAndShield_Dodge_Montage.Warrior_SwordAndShield_Dodge_Montage")
	);
	if (UnarmedRollMontageRef.Succeeded())
	{
		RollMontage = UnarmedRollMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> UnarmedBackStepMontageRef(
		TEXT("/Game/Blueprints/Anim/SwordAndShield/Warrior_SwordAndShield_BackStep_Montage.Warrior_SwordAndShield_BackStep_Montage")
	);
	if (UnarmedBackStepMontageRef.Succeeded())
	{
		BackStepMontage = UnarmedBackStepMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> UnarmedHitMontageMontageRef(
		TEXT("/Game/Blueprints/Anim/SwordAndShield/Warrior_SwordAndShield_Hit_1.Warrior_SwordAndShield_Hit_1")
	);
	if (UnarmedHitMontageMontageRef.Succeeded())
	{
		HitMontage = UnarmedHitMontageMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> UnarmedStunMontageMontageRef(
		TEXT("/Game/Blueprints/Anim/SwordAndShield/Warrior_SwordAndShield_Stun.Warrior_SwordAndShield_Stun")
	);
	if (UnarmedStunMontageMontageRef.Succeeded())
	{
		StunMontage = UnarmedStunMontageMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> ParryMontageRef(
		TEXT("/Game/Blueprints/Anim/SwordAndShield/Warrior_SwordAndShield_Parry_Montage1.Warrior_SwordAndShield_Parry_Montage1")
	);
	if (ParryMontageRef.Succeeded())
	{
		ParryMontage = ParryMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> FrontFatalAttackMontageRef(
		TEXT("/Game/Blueprints/Anim/UnArmed/Warrior_FatalAttack_Montage.Warrior_FatalAttack_Montage")
	);
	if (FrontFatalAttackMontageRef.Succeeded())
	{
		FrontFatalAttackMontage = FrontFatalAttackMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> BackFatalAttackMontageRef(
		TEXT("/Game/Blueprints/Anim/UnArmed/Warrior_BackFatalAttack_Montage.Warrior_BackFatalAttack_Montage")
	);
	if (BackFatalAttackMontageRef.Succeeded())
	{
		BackFatalAttackMontage = BackFatalAttackMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> PickUpMontageRef(
		TEXT("/Game/Blueprints/Anim/UnArmed/Warrior_PickUp_Montage.Warrior_PickUp_Montage")
	);
	if (PickUpMontageRef.Succeeded())
	{
		PickUpMontage = PickUpMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> DrinkMontageRef(
		TEXT("/Game/Blueprints/Anim/UnArmed/Warrior_DrinkPotion_Montage.Warrior_DrinkPotion_Montage")
	);
	if (DrinkMontageRef.Succeeded())
	{
		DrinkMontage = DrinkMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> LightOnMontageRef(
		TEXT("/Game/Blueprints/Anim/UnArmed/Warrior_LightOn_Montage.Warrior_LightOn_Montage")
	);
	if (LightOnMontageRef.Succeeded())
	{
		LightOnMontage = LightOnMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> FirstLitMontageRef(
		TEXT("/Game/Blueprints/Anim/UnArmed/Warrior_FirstLit_Montage.Warrior_FirstLit_Montage")
	);
	if (FirstLitMontageRef.Succeeded())
	{
		FirstLitMontage = FirstLitMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> LitMontageRef(
		TEXT("/Game/Blueprints/Anim/UnArmed/Warrior_Lit_Montage.Warrior_Lit_Montage")
	);
	if (LitMontageRef.Succeeded())
	{
		LitMontage = LitMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> DeathMontageRef(
		TEXT("/Game/Blueprints/Anim/UnArmed/Warrior_Death_Montage.Warrior_Death_Montage")
	);
	if (DeathMontageRef.Succeeded())
	{
		DeathMontage = DeathMontageRef.Object;
	}

	const int32 NoneAttackMontageMaxCount = 4;

	for (int32 i = 1; i <= NoneAttackMontageMaxCount; i++)
	{
		FString ObjectPath = FString::Printf(
			TEXT("/Game/Blueprints/Anim/UnArmed/Warrior_Unarmed_Attack_Montage_%d.Warrior_Unarmed_Attack_Montage_%d"), i, i);

		ConstructorHelpers::FObjectFinder<UAnimMontage> UnarmedAttackMontageRef(*ObjectPath);

		if (UnarmedAttackMontageRef.Succeeded())
		{
			AttackMontage.Add(UnarmedAttackMontageRef.Object);
		}
	}

	//InitRollMontages();
	//InitStepMontages();
	//InitAttackMontages();

}

void UWarriorAnimInstance::InitRollMontages()
{
	////None
	////OneHand
	////OneHandWithShield
	//static ConstructorHelpers::FObjectFinder<UAnimMontage> NoneRollMontageRef(
	//	TEXT("/Game/RetargetAnimations/SwordAndShield/Warrior_Dodge_Montage.Warrior_Dodge_Montage")
	//);

	//if (NoneRollMontageRef.Succeeded())
	//{
	//	RollMontage.Add(EWeaponState::None, NoneRollMontageRef.Object);
	//	RollMontage.Add(EWeaponState::OneHand, NoneRollMontageRef.Object);
	//	RollMontage.Add(EWeaponState::OneHandWithShield, NoneRollMontageRef.Object);
	//}

	////TwoHand
	//static ConstructorHelpers::FObjectFinder<UAnimMontage> TwoHandRollMontageRef(
	//	TEXT("/Game/RetargetAnimations/SwordAndShield/Warrior_Dodge_Montage.Warrior_Dodge_Montage")
	//);

	//if (TwoHandRollMontageRef.Succeeded())
	//{
	//	RollMontage.Add(EWeaponState::TwoHand, TwoHandRollMontageRef.Object);
	//}

	////Katana
	//static ConstructorHelpers::FObjectFinder<UAnimMontage> KatanaRollMontageRef(
	//	TEXT("/Game/RetargetAnimations/SwordAndShield/Warrior_Dodge_Montage.Warrior_Dodge_Montage")
	//);

	//if (KatanaRollMontageRef.Succeeded())
	//{
	//	RollMontage.Add(EWeaponState::Katana, KatanaRollMontageRef.Object);
	//}

}

void UWarriorAnimInstance::InitStepMontages()
{
	////None
	////OneHand
	////OneHandWithShield
	//static ConstructorHelpers::FObjectFinder<UAnimMontage> NoneBackStepMontageRef(
	//	TEXT("/Game/RetargetAnimations/SwordAndShield/Warrior_BackStep_Montage.Warrior_BackStep_Montage")
	//);

	//if (NoneBackStepMontageRef.Succeeded())
	//{
	//	BackStepMontage.Add(EWeaponState::None, NoneBackStepMontageRef.Object);
	//	BackStepMontage.Add(EWeaponState::OneHand, NoneBackStepMontageRef.Object);
	//	BackStepMontage.Add(EWeaponState::OneHandWithShield, NoneBackStepMontageRef.Object);
	//}

	////TwoHand
	//static ConstructorHelpers::FObjectFinder<UAnimMontage> TwoHandBackStepMontageRef(
	//	TEXT("/Game/RetargetAnimations/SwordAndShield/Warrior_BackStep_Montage.Warrior_BackStep_Montage")
	//);

	//if (TwoHandBackStepMontageRef.Succeeded())
	//{
	//	BackStepMontage.Add(EWeaponState::TwoHand, TwoHandBackStepMontageRef.Object);
	//}

	////Katana
	//static ConstructorHelpers::FObjectFinder<UAnimMontage> KatanaBackStepMontageRef(
	//	TEXT("/Game/RetargetAnimations/SwordAndShield/Warrior_BackStep_Montage.Warrior_BackStep_Montage")
	//);

	//if (KatanaBackStepMontageRef.Succeeded())
	//{
	//	BackStepMontage.Add(EWeaponState::Katana, KatanaBackStepMontageRef.Object);
	//}

}

void UWarriorAnimInstance::InitAttackMontages()
{
	////None
	//const int32 NoneAttackMontageMaxCount = 4;
	//AttackMontage.Add(EWeaponState::None);

	//for (int32 i = 1; i <= NoneAttackMontageMaxCount; i++)
	//{
	//	FString ObjectPath = FString::Printf(
	//		TEXT("/Game/RetargetAnimations/UnArmed/Warrior_Unarmed_Attack_Montage_%d.Warrior_Unarmed_Attack_Montage_%d"), i, i);

	//	ConstructorHelpers::FObjectFinder<UAnimMontage> NoneAttackMontageRef(*ObjectPath);

	//	if (NoneAttackMontageRef.Succeeded())
	//	{
	//		AttackMontage[EWeaponState::None].Entries.Add(NoneAttackMontageRef.Object);
	//	}
	//}

	////OneHand
	////OneHandWithShield
	//const int32 OneHandAttackMontageMaxCount = 4;
	//AttackMontage.Add(EWeaponState::OneHand);
	//AttackMontage.Add(EWeaponState::OneHandWithShield);

	//for (int32 i = 1; i <= OneHandAttackMontageMaxCount; i++)
	//{
	//	FString ObjectPath = FString::Printf(
	//		TEXT("/Game/RetargetAnimations/SwordAndShield/Warrior_SwordAndShield_Attack_Montage_%d.Warrior_SwordAndShield_Attack_Montage_%d"), i, i);
	//	ConstructorHelpers::FObjectFinder<UAnimMontage> OneHandAttackMontageRef(*ObjectPath);

	//	if (OneHandAttackMontageRef.Succeeded())
	//	{
	//		AttackMontage[EWeaponState::OneHand].Entries.Add(OneHandAttackMontageRef.Object);
	//		AttackMontage[EWeaponState::OneHandWithShield].Entries.Add(OneHandAttackMontageRef.Object);
	//	}
	//}

	////OneHandWithShield
	////const int32 OneHandWithShieldAttackMontageMaxCount = 4;
	////AttackMontage.Add(EWeaponState::OneHandWithShield);

	////for (int32 i = 1; i <= OneHandWithShieldAttackMontageMaxCount; i++)
	////{
	////	FString ObjectPath = FString::Printf(
	////		TEXT("/Game/RetargetAnimations/SwordAndShield/Warrior_SwordAndShield_Attack_Montage_%d.Warrior_SwordAndShield_Attack_Montage_%d"), i, i);
	////	ConstructorHelpers::FObjectFinder<UAnimMontage> OneHandWithShieldAttackMontageRef(*ObjectPath);

	////	if (OneHandWithShieldAttackMontageRef.Succeeded())
	////	{
	////		AttackMontage[EWeaponState::OneHandWithShield].Entries.Add(OneHandWithShieldAttackMontageRef.Object);
	////	}
	////}

	////TwoHand
	//const int32 TwoHandAttackMontageMaxCount = 4;
	//AttackMontage.Add(EWeaponState::TwoHand);

	//for (int32 i = 1; i <= TwoHandAttackMontageMaxCount; i++)
	//{
	//	FString ObjectPath = FString::Printf(
	//		TEXT("/Game/RetargetAnimations/TwoHanded/Warrior_TwoHand_Attack1_Montage_%d.Warrior_TwoHand_Attack1_Montage_%d"), i, i);
	//	ConstructorHelpers::FObjectFinder<UAnimMontage> TwoHandAttackMontageRef(*ObjectPath);

	//	if (TwoHandAttackMontageRef.Succeeded())
	//	{
	//		AttackMontage[EWeaponState::TwoHand].Entries.Add(TwoHandAttackMontageRef.Object);
	//	}
	//}

	////Katana
	//const int32 KatanaAttackMontageMaxCount = 4;
	//AttackMontage.Add(EWeaponState::Katana);

	//for (int32 i = 1; i <= TwoHandAttackMontageMaxCount; i++)
	//{
	//	FString ObjectPath = FString::Printf(
	//		TEXT("/Game/RetargetAnimations/TwoHanded/Warrior_TwoHand_Attack1_Montage_%d.Warrior_TwoHand_Attack1_Montage_%d"), i, i);
	//	ConstructorHelpers::FObjectFinder<UAnimMontage> TwoHandAttackMontageRef(*ObjectPath);

	//	if (TwoHandAttackMontageRef.Succeeded())
	//	{
	//		AttackMontage[EWeaponState::TwoHand].Entries.Add(TwoHandAttackMontageRef.Object);
	//	}
	//}
}

void UWarriorAnimInstance::InitStunMontages()
{

}

void UWarriorAnimInstance::InitHitMontages()
{

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
		WeaponState = Warrior->GetWeaponState();

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

void UWarriorAnimInstance::AnimNotify_DrinkStart()
{
	auto Pawn = TryGetPawnOwner();
	if (AWarrior* Warrior = Cast<AWarrior>(Pawn))
	{
		Warrior->SetLeftHandPotionMesh();
	}
}

void UWarriorAnimInstance::PlayMontage(const EActionState CurrentActionState)
{

}

const UWeaponDataAsset* UWarriorAnimInstance::GetWeaponData(AWarrior* Warrior)
{
	if (!Warrior)
		return nullptr;

	UEquipmentComponent* Equipment = Warrior->FindComponentByClass<UEquipmentComponent>();
	UInventoryComponent* Inventory = Warrior->FindComponentByClass<UInventoryComponent>();

	if (!Equipment || !Inventory)
		return nullptr;

	const FGuid ItemID = Equipment->GetEquippedItem(EEquipmentSlot::RightHand);
	if (!ItemID.IsValid())
		return nullptr;

	const UItemDataAsset* ItemData = Inventory->FindItemDataByInstanceID(ItemID);
	if (!ItemData)
		return nullptr;

	return Cast<UWeaponDataAsset>(ItemData);
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
	{
		const UWeaponDataAsset* WeaponData = GetWeaponData(Warrior);
		if (!WeaponData)
			return Montage_Play(RollMontage) > 0.f;

		const TSoftObjectPtr<UAnimMontage>& RollMontageEquip 
			= Warrior->IsTwoHanded() ? WeaponData->TwoHandRollMontage : WeaponData->OneHandRollMontage;

		UAnimMontage* Montage = RollMontageEquip.LoadSynchronous();
		if (!Montage)
			return false;

		return Montage_Play(Montage) > 0.f;
	}

	case EActionState::BackStep:
	{
		const UWeaponDataAsset* WeaponData = GetWeaponData(Warrior);
		if (!WeaponData)
			return Montage_Play(BackStepMontage) > 0.f;

		const TSoftObjectPtr<UAnimMontage>& BackStepMontageEquip 
			= Warrior->IsTwoHanded() ? WeaponData->TwoHandBackStepMontage : WeaponData->OneHandBackStepMontage;

		UAnimMontage* Montage = BackStepMontageEquip.LoadSynchronous();
		if (!Montage)
			return false;

		return Montage_Play(Montage) > 0.f;
	}

	case EActionState::Attack:
	{
		const UWeaponDataAsset* WeaponData = GetWeaponData(Warrior);
		const int32 ComboIndex = Warrior->GetCombo();

		if (!WeaponData)
		{
			if (!AttackMontage.IsValidIndex(ComboIndex))
			{
				return false;
			}

			return Montage_Play(AttackMontage[ComboIndex]) > 0.f;;
		}

		const auto& AttackMontageEquip =
			Warrior->IsTwoHanded() ? WeaponData->TwoHandAttackMontages : WeaponData->OneHandAttackMontages;

		if (!AttackMontageEquip.IsValidIndex(ComboIndex))
			return false;

		UAnimMontage* Montage = AttackMontageEquip[ComboIndex].LoadSynchronous();
		if (!Montage)
			return false;

		return Montage_Play(Montage) > 0.f;
	}

	case EActionState::FrontFatalAttack:
	{
		return Montage_Play(FrontFatalAttackMontage) > 0.f;
	}

	case EActionState::BackFatalAttack:
	{
		return Montage_Play(BackFatalAttackMontage) > 0.f;
	}

	case EActionState::Parry:
	{
		return Montage_Play(ParryMontage) > 0.f;
	}

	case EActionState::Hit:
	{
		const UWeaponDataAsset* WeaponData = GetWeaponData(Warrior);
		if (!WeaponData)
			return Montage_Play(HitMontage) > 0.f;

		const TSoftObjectPtr<UAnimMontage>& HitMontageEquip 
			= Warrior->IsTwoHanded() ? WeaponData->TwoHandHitMontage : WeaponData->OneHandHitMontage;

		UAnimMontage* Montage = HitMontageEquip.LoadSynchronous();
		if (!Montage)
			return false;

		return Montage_Play(Montage) > 0.f;
	}

	case EActionState::Stun:
	{
		const UWeaponDataAsset* WeaponData = GetWeaponData(Warrior);
		if (!WeaponData)
			return Montage_Play(StunMontage) > 0.f;

		const TSoftObjectPtr<UAnimMontage>& StunMontageEquip 
			= Warrior->IsTwoHanded() ? WeaponData->TwoHandStunMontage : WeaponData->OneHandStunMontage;

		UAnimMontage* Montage = StunMontageEquip.LoadSynchronous();
		if (!Montage)
			return false;

		return Montage_Play(Montage) > 0.f;
	}

	case EActionState::PickUp:
	{
		return Montage_Play(PickUpMontage) > 0.f;
	}

	case EActionState::Drink:
	{
		return Montage_Play(DrinkMontage) > 0.f;
	}

	case EActionState::LightOn:
	{
		return Montage_Play(LightOnMontage) > 0.f;
	}

	case EActionState::FirstLit:
	{
		return Montage_Play(FirstLitMontage) > 0.f;
	}

	case EActionState::Lit:
	{
		return Montage_Play(LitMontage) > 0.f;
	}

	case EActionState::Dead:
	{
		return PlayDeathMontage() > 0.f;
	}

	default:
		break;
	}

	return false;
}

float UWarriorAnimInstance::PlayDeathMontage()
{
	return Montage_Play(DeathMontage, 1.0f, EMontagePlayReturnType::Duration);
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
