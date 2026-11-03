// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/AI/BTTask/BTTask_FinishSearch.h"
#include <Enemy/AI/EnemyAIBlackboardKey.h>

#include <AIController.h>
#include <BehaviorTree/BlackboardComponent.h>

UBTTask_FinishSearch::UBTTask_FinishSearch()
{
	NodeName = TEXT("Finish Search");
}

EBTNodeResult::Type UBTTask_FinishSearch::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (nullptr == BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}

	// 수색 정보 초기화
	BlackboardComp->ClearValue(BBKEY_LASTKNOWNLOCATION);
	BlackboardComp->ClearValue(BBKEY_LASTKNOWNDIRECTION);
	BlackboardComp->ClearValue(BBKEY_SEARCHLOCATION);
	BlackboardComp->ClearValue(BBKEY_TARGETACTOR);

	return EBTNodeResult::Succeeded;
}
