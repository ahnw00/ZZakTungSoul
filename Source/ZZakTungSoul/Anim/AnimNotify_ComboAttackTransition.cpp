// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/AnimNotify_ComboAttackTransition.h"
#include <Warrior/Warrior.h>

void UAnimNotify_ComboAttackTransition::Notify(USkeletalMeshComponent* MeshComp, 
	UAnimSequenceBase* Animation, 
	const FAnimNotifyEventReference& EventReference)
{
	if (MeshComp)
	{
		AWarrior* Player = Cast<AWarrior>(MeshComp->GetOwner());

		if (Player)
		{
			Player->OnComboTransition();
			//UE_LOG(
			//	LogTemp,
			//	Warning,
			//	TEXT("Notify()== Attack Combo Trasition")
			//);
		}
	}
}
