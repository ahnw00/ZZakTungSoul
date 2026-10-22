// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CharacterBase.h"
#include "Data/StateEnum.h"
#include "Interface/CharacterStatInterface.h"
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
	public ICharacterStatInterface
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

	class ACharacter* LockOnTarget;
	bool bLockOn = false;
	// 락온 시에 카메라가 대상에 고정이 되도록(Tick에서 호출될 함수)
	void UpdateLockOnCamera(float DeltaTime);
	void UpdateLook(float DeltaTime);

	bool bIsSprint = false;
	bool bIsAttacking = false;

	EMoveState MoveState = EMoveState::Idle;
	EActionState ActionState = EActionState::None;
	EOverlayState OverlayState = EOverlayState::None;

	int32 AttackCombo = 0;

	class UWarriorAnimInstance* AnimInstance;

	UFUNCTION()
	void OnActionMontageEnded(UAnimMontage* Montage, bool bInterrupted);

protected:
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Data", meta = (AllowPrivateAccess = "true"))
	//class UDataTable* CharacterDataTable; // 블루프린트에서 데이터 테이블을 직접 참조
	//struct FCharacterStatData* CharacterStats; // 데이터 테이블에서 단일 행을 참조해 캐릭터 스탯으로 사용
	void UpdateCharacterStats();

	UPROPERTY(VisibleAnywhere, Category = "Stat")
	TObjectPtr<UCharacterStatComponent> CharacterStatComponent;

public:

	void SprintStart();
	void SprintEnd();
	FORCEINLINE bool IsSprint() const { return bIsSprint; }

	void BlockStart();
	void BlockEnd();

	void Attack();
	FORCEINLINE bool IsAttacking() const { return bIsAttacking; }

	void Dodge(FVector ControlDir);

	UPROPERTY()
	TArray<FOverlapResult> OverlapResults;

	void LockOn();
	void ChangeLockOn(bool flag);
	bool GetOverlaps();
	FORCEINLINE bool IsLockOn() const { return bLockOn; }

	// Getter/Setter
	
	void SetLockOnTarget(ACharacter* Target);
	void ResetLockOnTarget();
	FORCEINLINE ACharacter* GetLockOnTarget() { return LockOnTarget; }

	FORCEINLINE EMoveState GetMoveState() const { return MoveState; }
	void SetMoveState(EMoveState NewState);

	FORCEINLINE EActionState GetActionState() const { return ActionState; }
	void SetActionState(EActionState NewState);

	FORCEINLINE EOverlayState GetOverlayState() const { return OverlayState; }
	void SetOverlayState(EOverlayState NewState);

	// AnimMontage 관련
	FORCEINLINE void EndAction() { ActionState = EActionState::None; }

	FORCEINLINE int32 GetCombo() const { return AttackCombo; }
	FORCEINLINE void ResetCombo() { AttackCombo = 0; }

	//FORCEINLINE UArrowComponent GetLockOnPoint() { return LockOnPoint; }


	// [상호작용]
	void Interact();
	void SetCurrentInteractable(class IInteractionInterface* InInteractable) { CurrentInteractable = InInteractable; }
	class IInteractionInterface* GetCurrentInteractable() const { return CurrentInteractable; }

private:
	class IInteractionInterface* CurrentInteractable = nullptr;

public:
	// [캐릭터 스탯].
	virtual UCharacterStatComponent* GetCharacterStatComponent() const override { return CharacterStatComponent; }
	// [세이브 데이터].
	virtual const struct FWarriorSaveData* GetSaveData() const override;
	
};
