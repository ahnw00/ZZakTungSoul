// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/AnimNotifyState_ComboAttack.h"
#include <Warrior/Warrior.h>

void UAnimNotifyState_ComboWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, 
	UAnimSequenceBase* Animation, 
	float TotalDuration, 
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (MeshComp)
	{
		AWarrior* Player = Cast<AWarrior>(MeshComp->GetOwner());

		if (Player)
		{
			UE_LOG(
				LogTemp,
				Warning,
				TEXT("Attack Window Open")
			);

			Player->SetComboWindow(true);
		}


	}
}

void UAnimNotifyState_ComboWindow::NotifyEnd(USkeletalMeshComponent * MeshComp,
	UAnimSequenceBase * Animation, 
	const FAnimNotifyEventReference & EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (MeshComp)
	{
		AWarrior* Player = Cast<AWarrior>(MeshComp->GetOwner());

		if (Player)
		{
			UE_LOG(
				LogTemp,
				Warning,
				TEXT("Attack Window Close")
			);
			
			Player->SetComboWindow(false);
		}
	}
}
