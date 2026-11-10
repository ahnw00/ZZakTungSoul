// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/AnimNotify/AnimNotify_PlaySoundCue.h"
#include <Components/SkeletalMeshComponent.h>
#include <Kismet/GameplayStatics.h>
#include <Sound/SoundCue.h>

void UAnimNotify_PlaySoundCue::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (nullptr == MeshComp || nullptr == Sound)
	{
		return;
	}

	UGameplayStatics::PlaySoundAtLocation(this, Sound, MeshComp->GetComponentLocation());
}
