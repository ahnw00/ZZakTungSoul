// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/AI/BTTask/BTTask_SetStrafeMode.h"
#include <Enemy/EnemyBase.h>
#include <Enemy/AI/EnemyAIBlackboardKey.h>

#include <AIController.h>
#include <BehaviorTree/BlackboardComponent.h>

UBTTask_SetStrafeMode::UBTTask_SetStrafeMode()
{
	NodeName = TEXT("Set Strafe Mode");
}

EBTNodeResult::Type UBTTask_SetStrafeMode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Behavior Tree를 실행하고 있는 AIController 획득
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	// AIController가 제어하고 있는 Enemy 획득
	AEnemyBase* Enemy = Cast<AEnemyBase>(AIController->GetPawn());
	if (!Enemy || Enemy->IsDead())
	{
		return EBTNodeResult::Failed;
	}

	if (bEnableStrafe)
	{
		// Blackboard 획득
		UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
		if (!BlackboardComp)
		{
			return EBTNodeResult::Failed;
		}

		// 블랙보드에 저장된 캐릭터 가져오기
		APawn* TargetActor = Cast<APawn>(BlackboardComp->GetValueAsObject(BBKEY_TARGETACTOR));
		if (!IsValid(TargetActor))
		{
			return EBTNodeResult::Failed;
		}

		Enemy->SetStrafeMode(bEnableStrafe);

		AIController->SetFocus(TargetActor);
	}
	else
	{
		Enemy->SetStrafeMode(bEnableStrafe);

		AIController->ClearFocus(EAIFocusPriority::Gameplay);
	}

	return EBTNodeResult::Succeeded;
}
