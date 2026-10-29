// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAIController.h"
#include <Enemy/EnemyBase.h>
#include <Enemy/Data/EnemyAIData.h>
#include <Enemy/AI/EnemyAIBlackboardKey.h>

#include <BehaviorTree/BehaviorTree.h>
#include <BehaviorTree/BlackboardData.h>
#include <BehaviorTree/BlackboardComponent.h>

AEnemyAIController::AEnemyAIController()
{
	// Blackboard 설정
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BlackboardAssetRef(TEXT("/Game/BehaviorTree/BB_EnemyBase.BB_EnemyBase"));
	if (BlackboardAssetRef.Succeeded())
	{
		BlackboardAsset = BlackboardAssetRef.Object;
	}
}

void AEnemyAIController::UpdateAIFocus(EEnemyState PreviousState, EEnemyState NewState)
{
	switch (NewState)
	{
	case EEnemyState::Idle:
		ClearFocus(EAIFocusPriority::Gameplay);
		break;

	case EEnemyState::Patrol:
		ClearFocus(EAIFocusPriority::Gameplay);
		break;

	case EEnemyState::Chase:
		ClearFocus(EAIFocusPriority::Gameplay);
		break;

	case EEnemyState::Strafe:
		ToggleFocus();
		break;

	case EEnemyState::Attack:
		ClearFocus(EAIFocusPriority::Gameplay);
		break;

	case EEnemyState::Search:
		ClearFocus(EAIFocusPriority::Gameplay);
		break;

	case EEnemyState::Hit:
		ClearFocus(EAIFocusPriority::Gameplay);
		break;

	case EEnemyState::Dead:
		ClearFocus(EAIFocusPriority::Gameplay);
		break;

	default:
		break;
	}
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// Enemy 검사
	AEnemyBase* Enemy = Cast<AEnemyBase>(InPawn);
	if (!Enemy)
	{
		return;
	}

	// DataAsset 가져오기
	const UEnemyAIData* AIData = Enemy->GetEnemyAIData();
	if (nullptr == AIData || nullptr == AIData->MainBT)
	{
		return;
	}

	BehaviorTreeAsset = AIData->MainBT;
	// BehaviorTree 검사
	if (!BehaviorTreeAsset)
	{
		return;
	}

	// Blackboard 컴포넌트 가져오기
	UBlackboardComponent* BlackboardComponent = GetBlackboardComponent();
	// Blackboard 검사
	if (nullptr == BlackboardAsset || !UseBlackboard(BlackboardAsset, BlackboardComponent))		// 사용할 블랙보드 지정
	{
		return;
	}

	// Blackboard 상태 설정
	BlackboardComponent->ClearValue(BBKEY_TARGETACTOR);
	BlackboardComponent->SetValueAsBool(BBKEY_HASLINEOFSIGHT, false);
	BlackboardComponent->SetValueAsVector(BBKEY_HOMELOCATION, Enemy->GetActorLocation());

	// Behavior Tree 실행
	RunBehaviorTree(BehaviorTreeAsset);
}

void AEnemyAIController::ToggleFocus()
{
	UBlackboardComponent* BlackboardComp = GetBlackboardComponent();

	if (nullptr == BlackboardComp)
	{
		ClearFocus(EAIFocusPriority::Gameplay);
		return;
	}

	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(BBKEY_TARGETACTOR));
	if (IsValid(TargetActor))
	{
		SetFocus(TargetActor);
	}
	else
	{
		ClearFocus(EAIFocusPriority::Gameplay);
	}
}