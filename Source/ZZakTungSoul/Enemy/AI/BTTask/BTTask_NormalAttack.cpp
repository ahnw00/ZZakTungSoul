// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/AI/BTTask/BTTask_NormalAttack.h"
#include <Enemy/EnemyBase.h>
#include <Enemy/Data/EnemyAnimationData.h>

#include <AIController.h>
#include <BehaviorTree/BehaviorTreeComponent.h>
#include <Animation/AnimInstance.h>
#include <Animation/AnimMontage.h>

UBTTask_NormalAttack::UBTTask_NormalAttack()
{
	NodeName = TEXT("Normal Attack");

	// AI마다 독립적인 인스턴스 사용
	bCreateNodeInstance = true;

	// Task 완료 시 OnTaskFinished 호출
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_NormalAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// AIController 유효성 검사
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!IsValid(AIController))
	{
		return EBTNodeResult::Failed;
	}

	// AI가 조종하고 있는 Pawn 가져오기
	AEnemyBase* Enemy = Cast<AEnemyBase>(AIController->GetPawn());
	if (!IsValid(Enemy) || Enemy->IsDead())
	{
		return EBTNodeResult::Failed;
	}

	// Enemy의 Animation Data 가져오기
	const UEnemyAnimationData* AnimationData = Enemy->GetEnemyAnimationData();
	if (!IsValid(AnimationData))
	{
		return EBTNodeResult::Failed;
	}

	// 일반 공격 Montage 가져오기
	AttackMontage = AnimationData->NormalAttack.Montage.Get();
	if (!IsValid(AttackMontage))
	{
		return EBTNodeResult::Failed;
	}

	// AnimInstance 가져오기
	UAnimInstance* AnimInstance = Enemy->GetMesh()->GetAnimInstance();
	if (!IsValid(AnimInstance))
	{
		return EBTNodeResult::Failed;
	}

	// 공격 데미지 데이터 보내기
	Enemy->SetCurrentDamageInfo(AnimationData->NormalAttack.DamageInfo);

	// Montage 재생
	if (AnimInstance->Montage_Play(AttackMontage.Get()) <= 0.0f)
	{
		return EBTNodeResult::Failed;
	}

	// 공격 실행 중 필요한 참조 저장
	OwnerBTComp = &OwnerComp;
	OwnerAnimInstance = AnimInstance;

	// Montage 종료 Delegate 설정
	FOnMontageEnded MontageEndedDelegate;
	MontageEndedDelegate.BindUObject(this, &UBTTask_NormalAttack::OnAttackMontageEnded);

	AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, AttackMontage.Get());

	// Montage 종료까지 대기
	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UBTTask_NormalAttack::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// 실행 중인 Montage와 Delegate 초기화
	CleanUp(true);

	// 즉시 중단
	return EBTNodeResult::Aborted;
}

void UBTTask_NormalAttack::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);

	// 성공 이외의 종료에서는 Montage 중지
	CleanUp(TaskResult != EBTNodeResult::Succeeded);
}

void UBTTask_NormalAttack::CleanUp(bool bStopMontage)
{
	UAnimInstance* AnimInstance = OwnerAnimInstance.Get();
	UAnimMontage* Montage = AttackMontage.Get();

	if (AnimInstance && Montage)
	{
		// Delegate 해제
		FOnMontageEnded EmptyDelegate;
		AnimInstance->Montage_SetEndDelegate(EmptyDelegate, Montage);

		// Montage 정지
		if (bStopMontage && AnimInstance->Montage_IsPlaying(Montage))
		{
			AnimInstance->Montage_Stop(0.2f, Montage);
		}
	}

	OwnerBTComp.Reset();
	OwnerAnimInstance.Reset();
}

void UBTTask_NormalAttack::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!OwnerBTComp.IsValid())
	{
		return;
	}

	UBehaviorTreeComponent* BTComp = OwnerBTComp.Get();

	CleanUp(false);

	// Task 완료
	FinishLatentTask(*BTComp, bInterrupted ? EBTNodeResult::Failed : EBTNodeResult::Succeeded);
}
