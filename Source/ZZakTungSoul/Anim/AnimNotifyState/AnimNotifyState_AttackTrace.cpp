// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/AnimNotifyState/AnimNotifyState_AttackTrace.h"
#include <Enemy/Components/AttackTraceComponent.h>

void UAnimNotifyState_AttackTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		UAttackTraceComponent* TraceComp = MeshComp->GetOwner()->FindComponentByTag<UAttackTraceComponent>(FindTraceComponentTagName);
		if (TraceComp)
		{
			TraceComp->BeginTrace();
		}
	}
}

void UAnimNotifyState_AttackTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		UAttackTraceComponent* TraceComp = MeshComp->GetOwner()->FindComponentByTag<UAttackTraceComponent>(FindTraceComponentTagName);
		if (TraceComp)
		{
			TraceComp->EndTrace();
		}
	}
}