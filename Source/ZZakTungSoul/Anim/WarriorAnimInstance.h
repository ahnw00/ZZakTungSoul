// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/StateEnum.h"
#include "Animation/AnimInstance.h"
#include "WarriorAnimInstance.generated.h"

//---------------------------------------------------------------
// TMap에서 Value값으로 TArray 사용 불가. -> 직렬화 관련 이유.
// Wrapper를 통해서 TArray 사용 불가 우회.
// 
//---------------------------------------------------------------
USTRUCT(BlueprintType)
struct FAnimMontageArrayWrapper
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<TObjectPtr<UAnimMontage>> Entries;
};

UCLASS()
class ZZAKTUNGSOUL_API UWarriorAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UWarriorAnimInstance();
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UPROPERTY()
	UAnimMontage* RollMontage;
	UPROPERTY()
	UAnimMontage* BackStepMontage;
	//UPROPERTY()
	UPROPERTY()
	TMap<EOverlayState, FAnimMontageArrayWrapper> AttackMontage;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	float CurrentPawnSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	float Direction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	EMoveState MoveState = EMoveState::Idle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	EActionState ActionState = EActionState::None;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	EOverlayState OverlayState = EOverlayState::None;

public:
	UFUNCTION()
	void AnimNotify_ActionEnd();
	UFUNCTION()
	void AnimNotify_ComboCheck();

	void PlayMontage(const EActionState CurrentActionState);
	bool PlayActionMontage();

	void PlayRollMontage();
	void PlayBackStepMontage();
	void PlayAttackMontage();

	void StopMontage(UAnimMontage* Montage, float BlendOutTime);
	void StopAttackMontage();

	//void PlayMontage(UAnimMontage* AnimMontage);
	//void StopMontage(UAnimMontage* AnimMontage);
};
