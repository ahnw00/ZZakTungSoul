// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/AnimNotify/AnimNotify_FatalAttack_Shake.h"

#include <Warrior/Warrior.h>
#include <GameFramework/PlayerController.h>
#include <Camera/PlayerCameraManager.h>

void UAnimNotify_FatalAttack_Shake::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (false == IsValid(MeshComp) || nullptr == ShakeClass)
	{
		return;
	}
	AWarrior* Warrior = Cast<AWarrior>(MeshComp->GetOwner());
	if (false == IsValid(Warrior))
	{
		return;
	}
	APlayerController* PC = Cast<APlayerController>(Warrior->GetController());
	if (false == IsValid(PC) || false == IsValid(PC->PlayerCameraManager))
	{
		return;
	}

	PC->PlayerCameraManager->StartCameraShake(ShakeClass, Scale);
}
