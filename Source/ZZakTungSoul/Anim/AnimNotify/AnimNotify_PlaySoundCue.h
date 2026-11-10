// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_PlaySoundCue.generated.h"

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API UAnimNotify_PlaySoundCue : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Sound")
	TObjectPtr<class USoundCue> Sound;
};
