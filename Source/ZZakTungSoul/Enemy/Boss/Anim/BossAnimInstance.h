// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BossAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API UBossAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UBossAnimInstance();

protected:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character", Meta = (AllowPrivateAccess = true))
	TObjectPtr<class AEnemyBase> Owner;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character", Meta = (AllowPrivateAccess = true))
	FVector Velocity;	

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character", Meta = (AllowPrivateAccess = true))
	float Direction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character", Meta = (AllowPrivateAccess = true))
	float Speed;
};
