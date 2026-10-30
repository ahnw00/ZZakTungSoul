// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/AnimNotify/AnimNotify_SetNormalAttackDamageInfo.h"
#include <Interface/AIDataInterface.h>
#include <Enemy/Data/EnemyAnimationData.h>

void UAnimNotify_SetNormalAttackDamageInfo::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (nullptr == MeshComp)
	{
		return;
	}

	// 인터페이스 전달
	IAIDataInterface* AIPawn = Cast<IAIDataInterface>(MeshComp->GetOwner());
	if (AIPawn)
	{
		// DamageInfo 전달
		UEnemyAnimationData* AnimationData = AIPawn->GetAnimationData();
		if (AnimationData)
		{
			AIPawn->SetCurrentDamageInfoInterface(AnimationData->NormalAttack.DamageInfo[Index]);
		}
	}
}