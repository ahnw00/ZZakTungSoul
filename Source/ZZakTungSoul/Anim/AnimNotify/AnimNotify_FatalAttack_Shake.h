// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_FatalAttack_Shake.generated.h"

class UCameraShakeBase;

/**
 * 처형 타격 순간에 화면 흔들림.
 */
UCLASS()
class ZZAKTUNGSOUL_API UAnimNotify_FatalAttack_Shake : public UAnimNotify
{
	GENERATED_BODY()

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	// 재생할 카메라 흔들림 에셋.
	UPROPERTY(EditAnywhere, Category = "FatalAttack|Shake")
	TSubclassOf<UCameraShakeBase> ShakeClass;
	// 흔들림 세기.
	UPROPERTY(EditAnywhere, Category = "FatalAttack|Shake")
	float Scale = 1.f;
	
};
