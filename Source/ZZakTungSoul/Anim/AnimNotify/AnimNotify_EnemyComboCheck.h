// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_EnemyComboCheck.generated.h"

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API UAnimNotify_EnemyComboCheck : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference);

protected:
	// 목표 감지 범위
	UPROPERTY(EditAnywhere, Category = "ComboCheck|Range")
	float DetectRange = 200.0f;

	// 다음 Montage Section
	UPROPERTY(EditAnywhere, Category = "ComboCheck|Section")
	FName NextSection;
};
