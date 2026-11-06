// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_AttackRotation.generated.h"

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API UAnimNotifyState_AttackRotation : public UAnimNotifyState
{
	GENERATED_BODY()
	
protected:
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;

protected:
	// 초당 회전 속도
	UPROPERTY(EditAnywhere, Category = "Rotation")
	float RotationSpeed = 5.0f;

	// 최대 추적 각도
	UPROPERTY(EditAnywhere, Category = "Rotation")
	float MaxAngle = 180.0f;

};
