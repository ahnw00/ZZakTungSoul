// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/AnimNotify/AnimNotify_SetActionDamageInfo.h"
#include <Interface/AIDataInterface.h>
#include <Data/DamageInfo.h>
#include <Enemy/EnemyStates.h>
#include "AnimNotify_SetActionDamageInfo.h"

void UAnimNotify_SetActionDamageInfo::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (nullptr == MeshComp)
	{
		return;
	}

	IAIDataInterface* AIData = Cast<IAIDataInterface>(MeshComp->GetOwner());
	if (nullptr == AIData)
	{
		return;
	}

	if (AIData->GetActionStateInterface() != EEnemyActionState::Attack)
	{
		return;
	}

	FDamageInfo DamageInfo;

	if (false == AIData->GetActionDamageInfo(Index, DamageInfo))
	{
		return;
	}

	AIData->SetCurrentDamageInfoInterface(DamageInfo);
}