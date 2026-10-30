// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/AI/BTTask/BTTask_SelectCombatAction.h"
#include <Enemy/AI/EnemyAIBlackboardKey.h>

#include <AIController.h>
#include <BehaviorTree/BehaviorTreeComponent.h>
#include <BehaviorTree/BlackboardComponent.h>

UBTTask_SelectCombatAction::UBTTask_SelectCombatAction()
{
	NodeName = TEXT("Select Combat Action");
}

EBTNodeResult::Type UBTTask_SelectCombatAction::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Blackboard 가져오기
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (nullptr == BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}

	// 가중치가 모두 0이면 건너뛰기
	if (AttackWeight <= 0 && OtherWeight <= 0)
	{
		return EBTNodeResult::Failed;
	}

	const ECombatAction SelectedCombatAction = SelectCombatAction();

	// Blackboard에 저장
	BlackboardComp->SetValueAsEnum(BBKEY_COMBATACTION, static_cast<uint8>(SelectedCombatAction));

	return EBTNodeResult::Succeeded;
}

ECombatAction UBTTask_SelectCombatAction::SelectCombatAction()
{
	// 음수 방지
	const int64 Attack = FMath::Max(0, AttackWeight);
	const int64 Other = FMath::Max(0, OtherWeight);

	// 전체 가중치
	const int64 TotalWeight = Attack + Other;
	if (TotalWeight <= 0)
	{
		return ECombatAction::Attack;
	}

	// 0 이상 합산가중치 미만의 난수
	const double RandomValue = FMath::FRand() * static_cast<double>(TotalWeight);

	// 가중치에 따라 상태 선택
	if (RandomValue < static_cast<double>(Attack))
	{
		return ECombatAction::Attack;
	}

	return ECombatAction::Other;
}
