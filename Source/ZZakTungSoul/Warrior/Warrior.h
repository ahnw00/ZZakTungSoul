// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CharacterBase.h"
#include "Data/StateEnum.h"
#include <ItemData/ItemDataAsset.h>
#include <Item/ItemObject.h>
#include "Interface/CharacterStatInterface.h"
#include <Interface/ItemInterface.h>
#include "Warrior.generated.h"


// [전방 선언].
class UCharacterStatComponent;
class IInteractionInterface;

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API AWarrior 
	: public ACharacterBase,
	public ICharacterStatInterface,
	public IItemInterface
{
	GENERATED_BODY()
	
public:
	AWarrior();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	class USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	class UCameraComponent* Camera;
	UPROPERTY(VisibleAnywhere, Category = "LockOn")
	class UArrowComponent* LockOnPoint;

	UPROPERTY(VisibleAnywhere, Category = "Stat")
	TObjectPtr<UCharacterStatComponent> CharacterStatComponent;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	class USkeletalMeshComponent* Weapon;

	UPROPERTY(VisibleAnywhere, Category = "LeftHandWeapon")
	class USkeletalMeshComponent* LeftHandWeapon;

	UPROPERTY(VisibleAnywhere, Category = "LeftHandCapsule")
	class UCapsuleComponent* LeftHandCapsule;

	UPROPERTY(VisibleAnywhere, Category = "Equipment")
	class UEquipmentComponent* Equipment;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	class UInventoryComponent* Inventory;


	class ACharacter* LockOnTarget;
	bool bLockOn = false;
	// 락온 시에 카메라가 대상에 고정이 되도록(Tick에서 호출될 함수)
	void UpdateLockOnCamera(float DeltaTime);
	void UpdateLook(float DeltaTime);

	bool bIsSprint = false;
	bool bIsAttacking = false;
	uint8 bCanComboInput:1 = false;
	uint8 bComboQueued:1 = false;

	EMoveState MoveState = EMoveState::Idle;
	EActionState ActionState = EActionState::None;
	EWeaponState WeaponState = EWeaponState::None;

	int32 AttackCombo = 0;

	bool bIsTwoHanded : 1 = 0;

	UPROPERTY()
	class UWarriorAnimInstance* AnimInstance;

	UFUNCTION()
	void OnActionMontageEnded(UAnimMontage* Montage, bool bInterrupted);

protected:
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Data", meta = (AllowPrivateAccess = "true"))
	//class UDataTable* CharacterDataTable; // 블루프린트에서 데이터 테이블을 직접 참조
	//struct FCharacterStatData* CharacterStats; // 데이터 테이블에서 단일 행을 참조해 캐릭터 스탯으로 사용
	void UpdateCharacterStats();

public:

	void SprintStart();
	void SprintEnd();
	FORCEINLINE bool IsSprint() const { return bIsSprint; }

	void BlockStart();
	void BlockEnd();
	void Parry();

	void Attack();
	void FinishAttack();
	void SetComboWindow(bool bOpen);
	void OnComboTransition();
	void ChargeAttack();
	FORCEINLINE bool IsAttacking() const { return bIsAttacking; }

	FORCEINLINE  bool IsAttackKeyDown() const { return bIsAttacking; }

	FORCEINLINE  bool IsTwoHanded() const { return bIsTwoHanded; }

	FORCEINLINE  void SetTwoHanded(bool Value) {bIsTwoHanded = Value; }
	FORCENOINLINE  void ToggleTwoHanded() { bIsTwoHanded = !bIsTwoHanded; }

	void Dodge(FVector ControlDir);

	UPROPERTY()
	TArray<FOverlapResult> OverlapResults;

	void LockOn();
	void ChangeLockOn(bool flag);
	bool GetOverlaps();
	FORCEINLINE bool IsLockOn() const { return bLockOn; }

	// 상호작용
	void Interact();

	// Getter/Setter
	
	void SetLockOnTarget(ACharacter* Target);
	void ResetLockOnTarget();
	FORCEINLINE ACharacter* GetLockOnTarget() { return LockOnTarget; }

	FORCEINLINE EMoveState GetMoveState() const { return MoveState; }
	void SetMoveState(EMoveState NewState);

	FORCEINLINE EActionState GetActionState() const { return ActionState; }
	void SetActionState(EActionState NewState);

	FORCEINLINE EWeaponState GetWeaponState() const { return WeaponState; }
	void SetWeaponState(EWeaponState NewState);

	// AnimMontage 관련
	FORCEINLINE void EndAction() { ActionState = EActionState::None; }

	FORCEINLINE int32 GetCombo() const { return AttackCombo; }
	FORCEINLINE void ResetCombo() { AttackCombo = 0; }

	//FORCEINLINE UArrowComponent GetLockOnPoint() { return LockOnPoint; }

private:
	class IInteractionInterface* CurrentInteractable = nullptr;

public:
	// [캐릭터 스탯].
	virtual UCharacterStatComponent* GetCharacterStatComponent() const override { return CharacterStatComponent; }
	// [세이브 데이터].
	virtual const struct FWarriorSaveData* GetSaveData() const override;
	// [인벤토리]
	FORCEINLINE UInventoryComponent* GetInventory() const { return Inventory; }
	// [장비 컴포넌트]
	FORCEINLINE class UEquipmentComponent* GetEquipment() const { return Equipment; }

protected:
	// 데미지 처리 판정 -> 상위 클래스에서 인터페이스 사용함
	virtual void ReceiveDamage(const struct FDamageInfo& DamageInfo, const FHitResult& Hit) override;

	// Trace에 걸린 액터 처리 함수
	void HandleAttackTraceHit(const FHitResult& Hit);

protected:
	// 공격 판정을 위한 Trace Component
	UPROPERTY(VisibleAnywhere, Category = "Player|Combat")
	TObjectPtr<class UAttackTraceComponent> AttackTraceComponent;

	UPROPERTY(VisibleAnywhere, Category = "Player|Combat")
	TObjectPtr<class UAttackTraceComponent> ParryTraceComponent;
	

	// 아이템 관련 함수
public:
	void EquipWeapon(const FGuid& InID, EEquipmentSlot Slot);

	// Inherited via IItemInterface
	void ObtainItem(class UItemDataAsset* detectedItem) override;

	FORCEINLINE void AddDetectedObject(AActor* Detected) { DetectedActors.AddUnique(Detected); }
	FORCEINLINE void RemoveDetectedObject(AActor* Detected) { DetectedActors.Remove(Detected); }

	// 상호작용 관련 변수
private:
	// 상호작용할 대상 저장 변수
	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> DetectedActors;

public:
	//virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	float TakeDamaged(float Damage, float PoiseDamage, float StanceDamage, bool bUnblockable, bool bIgnoreInvincibility);
	float TakeDamageTest_1(float Damage, float PoiseDamage, float StanceDamage, bool bUnblockable, bool bIgnoreInvincibility);
	float TakeDamageTest_2(float Damage, float PoiseDamage, float StanceDamage, bool bUnblockable, bool bIgnoreInvincibility);
	void ReceiveDamageTest_1(const struct FDamageInfo& DamageInfo, const FHitResult& Hit);
	void ReceiveDamageTest_2(const struct FDamageInfo& DamageInfo, const FHitResult& Hit);
};
