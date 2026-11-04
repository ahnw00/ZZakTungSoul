// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/AI/BTTask/BTTask_PlayNormalEnemyMontage.h"
#include <Enemy/NormalEnemy/NormalEnemyBase.h>
#include <Enemy/Animation/NormalEnemyAnimInstance.h>

#include <AIController.h>
#include <BehaviorTree/BehaviorTreeComponent.h>
#include <Animation/AnimMontage.h>


UBTTask_PlayNormalEnemyMontage::UBTTask_PlayNormalEnemyMontage()
{
	NodeName = TEXT("Play Montage : Normal Enemy");

	bCreateNodeInstance = true;

	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_PlayNormalEnemyMontage::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (false == IsValid(AIController))
	{
		return EBTNodeResult::Failed;
	}

	ANormalEnemyBase* Enemy = Cast<ANormalEnemyBase>(AIController->GetPawn());
	if (false == IsValid(Enemy) || Enemy->IsDeadState())
	{
		return EBTNodeResult::Failed;
	}

	UNormalEnemyAnimInstance* AnimInstance = Cast<UNormalEnemyAnimInstance>(Enemy->GetMesh()->GetAnimInstance());
	if (false == IsValid(AnimInstance))
	{
		return EBTNodeResult::Failed;
	}

	// 상태에 맞는 Montage 재생
	if (false == AnimInstance->PlayActionMontage())
	{
		Enemy->SetEnemyActionState(EEnemyActionState::None);
		return EBTNodeResult::Failed;
	}

	// 지금 재생중인 Montge 가져오기
	UAnimMontage* Montage = AnimInstance->GetCurrentActiveMontage();
	if (false == IsValid(Montage))
	{
		Enemy->SetEnemyActionState(EEnemyActionState::None);
		return EBTNodeResult::Failed;
	}

	OwnerBTComp = &OwnerComp;
	OwnerAnimInstance = AnimInstance;
	OwnerEnemy = Enemy;
	PlayingMontage = Montage;

	// Montage 종료 Delegate 바인딩
	FOnMontageEnded EndDelegate;

	EndDelegate.BindUObject(this, &UBTTask_PlayNormalEnemyMontage::OnMontageEnded);

	AnimInstance->Montage_SetEndDelegate(EndDelegate, Montage);

	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UBTTask_PlayNormalEnemyMontage::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UNormalEnemyAnimInstance* AnimInstance = OwnerAnimInstance.Get();

	UAnimMontage* Montage = PlayingMontage.Get();
	ANormalEnemyBase* Enemy = OwnerEnemy.Get();

	// 정리
	CleanUp();

	// Montage 중단
	if (IsValid(AnimInstance) && IsValid(Montage))
	{
		if (AnimInstance->Montage_IsPlaying(Montage))
		{
			AnimInstance->Montage_Stop(0.2f, Montage);
		}
	}

	if (IsValid(Enemy))
	{
		const EEnemyActionState State = Enemy->GetEnemyActionState();
		if (State == EEnemyActionState::Attack || State == EEnemyActionState::Avoid)
		{
			Enemy->SetEnemyActionState(EEnemyActionState::None);
		}

	}

	return EBTNodeResult::Aborted;
}

void UBTTask_PlayNormalEnemyMontage::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	CleanUp();

	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}

void UBTTask_PlayNormalEnemyMontage::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (false == OwnerBTComp.IsValid())
	{
		return;
	}

	// 현재 Task가 실행중인 Montage만 처리
	if (PlayingMontage != Montage)
	{
		return;
	}

	UBehaviorTreeComponent* BTComp = OwnerBTComp.Get();

	// 정리 함수 실행
	CleanUp();

	// 정상 종료면 성공 아니면 실패
	FinishLatentTask(*BTComp, bInterrupted ? EBTNodeResult::Failed : EBTNodeResult::Succeeded);
}

void UBTTask_PlayNormalEnemyMontage::CleanUp()
{
	UNormalEnemyAnimInstance* AnimInstance = OwnerAnimInstance.Get();

	UAnimMontage* Montage = PlayingMontage.Get();

	if (IsValid(AnimInstance) && IsValid(Montage))
	{
		FOnMontageEnded EmptyDelegate;

		AnimInstance->Montage_SetEndDelegate(EmptyDelegate, Montage);
	}

	OwnerBTComp.Reset();
	OwnerAnimInstance.Reset();
	OwnerEnemy.Reset();
	PlayingMontage.Reset();
}
