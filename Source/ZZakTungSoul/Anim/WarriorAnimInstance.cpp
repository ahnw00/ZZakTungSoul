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
	{
		if (!Warrior)
			return false;

		//AActor* Owner = GetOwningActor();

		//if (!Owner)
		//	return false;

		UEquipmentComponent* Equipment = Warrior->FindComponentByClass<UEquipmentComponent>();

		UInventoryComponent* Inventory = Warrior->FindComponentByClass<UInventoryComponent>();

		if (!Equipment || !Inventory)
			return false;

		FGuid ItemID = Equipment->GetEquippedItem(EEquipmentSlot::RightHand);

		if (!ItemID.IsValid())
		{
			return Montage_Play(RollMontage) > 0.f;
		}

		const FInventoryItem* FoundItem = Inventory->GetItems().FindByPredicate([&ItemID](const FInventoryItem& Item)
		{
			return Item.InstanceID == ItemID;
		});

		if (!FoundItem)
			return false;

		UItemDataAsset* ItemData = Inventory->GetItemData(*FoundItem);

		UWeaponDataAsset* WeaponData = Cast<UWeaponDataAsset>(ItemData);

		if (!WeaponData)
			return false;

		const TSoftObjectPtr<UAnimMontage>& RollMontageEquip = Warrior->IsTwoHanded() ? WeaponData->TwoHandRollMontage : WeaponData->OneHandRollMontage;

		UAnimMontage* Montage = RollMontageEquip.LoadSynchronous();

		if (!Montage)
			return false;

		return Montage_Play(Montage) > 0.f;
	}
		break;
	case EActionState::BackStep:
	{
		if (!Warrior)
			return false;

		//AActor* Owner = GetOwningActor();

		//if (!Owner)
		//	return false;

		UEquipmentComponent* Equipment = Warrior->FindComponentByClass<UEquipmentComponent>();

		UInventoryComponent* Inventory = Warrior->FindComponentByClass<UInventoryComponent>();

		if (!Equipment || !Inventory)
			return false;

		FGuid ItemID = Equipment->GetEquippedItem(EEquipmentSlot::RightHand);

		if (!ItemID.IsValid())
		{
			return Montage_Play(BackStepMontage) > 0.f;
		}

		const FInventoryItem* FoundItem = Inventory->GetItems().FindByPredicate([&ItemID](const FInventoryItem& Item)
		{
			return Item.InstanceID == ItemID;
		});

		if (!FoundItem)
			return false;

		UItemDataAsset* ItemData = Inventory->GetItemData(*FoundItem);

		UWeaponDataAsset* WeaponData = Cast<UWeaponDataAsset>(ItemData);

		if (!WeaponData)
			return false;

		const TSoftObjectPtr<UAnimMontage>& BackStepMontageEquip = Warrior->IsTwoHanded() ? WeaponData->TwoHandBackStepMontage : WeaponData->OneHandBackStepMontage;

		UAnimMontage* Montage = BackStepMontageEquip.LoadSynchronous();

		if (!Montage)
			return false;

		return Montage_Play(Montage) > 0.f;
	}
		break;
	case EActionState::Attack:
	{
		if (!Warrior)
			return false;

		//AActor* Owner = GetOwningActor();

		//if (!Owner)
		//	return false;

		UEquipmentComponent* Equipment = Warrior->FindComponentByClass<UEquipmentComponent>();
		UInventoryComponent* Inventory = Warrior->FindComponentByClass<UInventoryComponent>();

		if (!Equipment || !Inventory)
			return false;

		FGuid ItemID = Equipment->GetEquippedItem(EEquipmentSlot::RightHand);

		if (!ItemID.IsValid())
		{
			const int32 ComboIndex = Warrior->GetCombo();

			if (!AttackMontage.IsValidIndex(ComboIndex))
			{
				return false;
			}

			return Montage_Play(AttackMontage[ComboIndex]) > 0.f;;
		}

		const FInventoryItem* FoundItem = Inventory->GetItems().FindByPredicate(
			[&ItemID](const FInventoryItem& Item)
		{
			return Item.InstanceID == ItemID;
		});

		if (!FoundItem)
			return false;

		UWeaponDataAsset* WeaponData = Cast<UWeaponDataAsset>(Inventory->GetItemData(*FoundItem));

		if (!WeaponData)
			return false;

		const int32 ComboIndex = Warrior->GetCombo();
		//const EWeaponState CurWeaponState = Warrior->GetWeaponState();

		const auto& AttackMontageEquip =
			Warrior->IsTwoHanded()
			? WeaponData->TwoHandAttackMontages
			: WeaponData->OneHandAttackMontages;

		if (!AttackMontageEquip.IsValidIndex(ComboIndex))
			return false;

		UAnimMontage* Montage = AttackMontageEquip[ComboIndex].LoadSynchronous();

		if (!Montage)
			return false;

		return Montage_Play(Montage) > 0.f;
	}
		break;
	case EActionState::Hit:
		//Montage_Play();
	{
		if (!Warrior)
			return false;

		//AActor* Owner = GetOwningActor();

		//if (!Owner)
		//	return false;

		UEquipmentComponent* Equipment = Warrior->FindComponentByClass<UEquipmentComponent>();

		UInventoryComponent* Inventory = Warrior->FindComponentByClass<UInventoryComponent>();

		if (!Equipment || !Inventory)
			return false;

		FGuid ItemID = Equipment->GetEquippedItem(EEquipmentSlot::RightHand);

		if (!ItemID.IsValid())
		{
			return Montage_Play(HitMontage) > 0.f;
		}

		const FInventoryItem* FoundItem = Inventory->GetItems().FindByPredicate([&ItemID](const FInventoryItem& Item)
		{
			return Item.InstanceID == ItemID;
		});

		if (!FoundItem)
			return false;

		UItemDataAsset* ItemData = Inventory->GetItemData(*FoundItem);

		UWeaponDataAsset* WeaponData = Cast<UWeaponDataAsset>(ItemData);

		if (!WeaponData)
			return false;

		const TSoftObjectPtr<UAnimMontage>& HitMontageEquip = Warrior->IsTwoHanded() ? WeaponData->TwoHandHitMontage : WeaponData->OneHandHitMontage;

		UAnimMontage* Montage = HitMontageEquip.LoadSynchronous();

		if (!Montage)
			return false;

		return Montage_Play(Montage) > 0.f;
	}
		break;
	case EActionState::Stun:
		//Montage_Play();
	{
		if (!Warrior)
			return false;

		//AActor* Owner = GetOwningActor();

		//if (!Owner)
		//	return false;

		UEquipmentComponent* Equipment = Warrior->FindComponentByClass<UEquipmentComponent>();

		UInventoryComponent* Inventory = Warrior->FindComponentByClass<UInventoryComponent>();

		if (!Equipment || !Inventory)
			return false;

		FGuid ItemID = Equipment->GetEquippedItem(EEquipmentSlot::RightHand);

		if (!ItemID.IsValid())
		{
			return Montage_Play(StunMontage) > 0.f;
		}

		const FInventoryItem* FoundItem = Inventory->GetItems().FindByPredicate([&ItemID](const FInventoryItem& Item)
		{
			return Item.InstanceID == ItemID;
		});

		if (!FoundItem)
			return false;

		UItemDataAsset* ItemData = Inventory->GetItemData(*FoundItem);

		UWeaponDataAsset* WeaponData = Cast<UWeaponDataAsset>(ItemData);

		if (!WeaponData)
			return false;

		const TSoftObjectPtr<UAnimMontage>& StunMontageEquip = Warrior->IsTwoHanded() ? WeaponData->TwoHandStunMontage : WeaponData->OneHandStunMontage;

		UAnimMontage* Montage = StunMontageEquip.LoadSynchronous();

		if (!Montage)
			return false;

		return Montage_Play(Montage) > 0.f;
	}
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
