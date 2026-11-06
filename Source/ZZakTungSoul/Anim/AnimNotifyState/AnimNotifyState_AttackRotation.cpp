// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/AnimNotifyState/AnimNotifyState_AttackRotation.h"
#include <Enemy/EnemyBase.h>
#include <Enemy/AI/EnemyAIBlackboardKey.h>

#include <AIController.h>
#include <BehaviorTree/BlackboardComponent.h>

void UAnimNotifyState_AttackRotation::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (false == IsValid(MeshComp))
	{
		return;
	}

	AEnemyBase* Enemy = Cast<AEnemyBase>(MeshComp->GetOwner());
	if (false == IsValid(Enemy))
	{
		return;
	}

	// 행동 상태 검사
	if (Enemy->GetEnemyActionState() != EEnemyActionState::Attack)
	{
		return;
	}

	// AIController
	AAIController* AIController = Cast<AAIController>(Enemy->GetController());
	if (false == IsValid(AIController))
	{
		return;
	}

	// Blackboard
	UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
	if (false == IsValid(BlackboardComp))
	{
		return;
	}

	// 목표 가져오기
	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(BBKEY_TARGETACTOR));
	if (false == IsValid(TargetActor))
	{
		return;
	}

	// 회전 처리
	Enemy->RotateToTarget(TargetActor, FrameDeltaTime, RotationSpeed, MaxAngle);
}
