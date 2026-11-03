// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/AnimNotifyState/AnimNotifyState_Parry.h"
#include <Components/CapsuleComponent.h>


void UAnimNotifyState_Parry::NotifyBegin(
	USkeletalMeshComponent* MeshComp, 
	UAnimSequenceBase* Animation, 
	float TotalDuration, 
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		UCapsuleComponent* CapsuleComponent 
			= MeshComp->GetOwner()->FindComponentByTag<UCapsuleComponent>(ParryComponentTagName);

		if (!CapsuleComponent)
		{
			return;
		}

		CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
}

void UAnimNotifyState_Parry::NotifyEnd(
	USkeletalMeshComponent* MeshComp, 
	UAnimSequenceBase* Animation, 
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		UCapsuleComponent* CapsuleComponent
			= MeshComp->GetOwner()->FindComponentByTag<UCapsuleComponent>(ParryComponentTagName);

		if (!CapsuleComponent)
		{
			return;
		}

		CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}
