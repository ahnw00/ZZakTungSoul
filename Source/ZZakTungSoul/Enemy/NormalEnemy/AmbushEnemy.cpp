// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/NormalEnemy/AmbushEnemy.h"
#include <Enemy/EnemyAIController.h>
#include <Enemy/AIController/NormalEnemyAIController.h>

#include <Animation/AnimInstance.h>
#include <Animation/AnimMontage.h>
#include <Components/SkeletalMeshComponent.h>

AAmbushEnemy::AAmbushEnemy()
{
	CurrentAmbushState = EAmbushState::Waiting;
}

void AAmbushEnemy::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UAnimInstance* AmbushAnimInstance = GetMesh()->GetAnimInstance();
	if (IsValid(AmbushAnimInstance))
	{
		AmbushAnimInstance->OnMontageEnded.AddDynamic(this, &AAmbushEnemy::OnAmbushMontageEnded);
	}
}

bool AAmbushEnemy::ActiveAmbush(AActor* TargetActor)
{
	// 이미 활성화 된 경우 건너뛰기
	if (CurrentAmbushState != EAmbushState::Waiting)
	{
		return false;
	}

	// 유효성 검사
	if (IsDeadState() || false == IsValid(TargetActor))
	{
		return false;
	}

	AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController());
	if (false == IsValid(AIController))
	{
		return false;
	}

	if (false == IsValid(AmbushAttackData.Montage) || false == AmbushAttackData.DamageInfo.IsValidIndex(0))
	{
		return false;
	}

	UAnimInstance* AmbushAnimInstance = GetMesh()->GetAnimInstance();
	if (false == IsValid(AmbushAnimInstance))
	{
		return false;
	}

	CurrentAmbushState = EAmbushState::Trigger;
	AmbushTarget = TargetActor;

	SetCurrentDamageInfo(AmbushAttackData.DamageInfo[0]);

	// 기습 실행
	if (AmbushAnimInstance->Montage_Play(AmbushAttackData.Montage) <= 0.0f)
	{
		AmbushTarget.Reset();
		CurrentAmbushState = EAmbushState::Waiting;
		return false;
	}

	return true;
}

void AAmbushEnemy::ResetEnemy(const FTransform& SpawnTransform)
{
	Super::ResetEnemy(SpawnTransform);

	if (IsDeadState())
	{
		return;
	}

	CurrentAmbushState = EAmbushState::Waiting;
}

void AAmbushEnemy::OnAmbushMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != AmbushAttackData.Montage || CurrentAmbushState != EAmbushState::Trigger)
	{
		return;
	}

	if (IsDeadState())
	{
		AmbushTarget.Reset();
		return;
	}

	StartAmbushAI();
}

void AAmbushEnemy::StartAmbushAI()
{
	if (CurrentAmbushState != EAmbushState::Trigger || IsDeadState())
	{
		return;
	}

	AActor* TargetActor = AmbushTarget.Get();
	if (false == IsValid(TargetActor))
	{
		AmbushTarget.Reset();
		CurrentAmbushState = EAmbushState::Waiting;
		return;
	}

	AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController());
	if (false == IsValid(AIController))
	{
		AmbushTarget.Reset();
		CurrentAmbushState = EAmbushState::Waiting;
		return;
	}

	if (false == AIController->StartAIFirstAction(TargetActor))
	{
		AmbushTarget.Reset();
		CurrentAmbushState = EAmbushState::Waiting;
		return;
	}

	CurrentAmbushState = EAmbushState::Active;

	// 타겟 BT에 넘겨주기
	if (ANormalEnemyAIController* NormalAIController = Cast<ANormalEnemyAIController>(AIController))
	{
		NormalAIController->RecognizeTarget(TargetActor);
	}

	AmbushTarget.Reset();
}
