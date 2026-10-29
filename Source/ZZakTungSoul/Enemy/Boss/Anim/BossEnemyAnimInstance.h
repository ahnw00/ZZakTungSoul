// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BossEnemyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API UBossEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UBossEnemyAnimInstance();

protected:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character")
	TObjectPtr<class AEnemyBase> Owner;
};
