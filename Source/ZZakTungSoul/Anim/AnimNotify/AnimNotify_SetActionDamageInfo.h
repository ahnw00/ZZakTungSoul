// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_SetActionDamageInfo.generated.h"

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API UAnimNotify_SetActionDamageInfo : public UAnimNotify
{
	GENERATED_BODY()

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
protected:
	UPROPERTY(EditAnywhere, Category = "DamageInfo|ComboNumber")
	int32 Index = 0;
};
