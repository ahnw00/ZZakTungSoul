// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/AI/BTTask/BTTask_FindPatrolLocation.h"
#include <Enemy/AI/EnemyAIBlackboardKey.h>
#include <Interface/AIDataInterface.h>
#include <Enemy/Waypoint/PatrolPath.h>

#include <AIController.h>
#include <BehaviorTree/BlackboardComponent.h>

UBTTask_FindPatrolLocation::UBTTask_FindPatrolLocation()
{
	NodeName = TEXT("Find Patrol Location");
}

EBTNodeResult::Type UBTTask_FindPatrolLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Blackboard 가져오기
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (nullptr == BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}

	// 전의 값 초기화
	BlackboardComp->ClearValue(BBKEY_PATROLLOCATION);

	// AIController 가져오기
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (false == IsValid(AIController))
	{
		return EBTNodeResult::Failed;
	}

	// Pawn 가져오기
	APawn* ControllingPawn = AIController->GetPawn();
	if (false == IsValid(ControllingPawn))
	{
		return EBTNodeResult::Failed;
	}

	// 인터페이스 형변환
	IAIDataInterface* AIPawn = Cast<IAIDataInterface>(ControllingPawn);
	if (nullptr == AIPawn)
	{
		return EBTNodeResult::Failed;
	}

	// PatrolPath 가져오기
	APatrolPath* PatrolPath = AIPawn->GetPatrolPathData();
	if (false == IsValid(PatrolPath))
	{
		return EBTNodeResult::Failed;
	}

	// Waypoint의 개수
	const int32 WaypointCount = PatrolPath->GetWaypointCount();
	if (WaypointCount <= 0)
	{
		return EBTNodeResult::Failed;
	}

	// Waypoint Index
	const int32 PatrolIndex = BlackboardComp->GetValueAsInt(BBKEY_PATROLINDEX);
	if (PatrolIndex < 0 || PatrolIndex >= WaypointCount)
	{
		return EBTNodeResult::Failed;
	}

	// Waypoint 위치 가져오기
	FVector WaypointLocation;
	if (false == PatrolPath->GetWaypointLocation(PatrolIndex, WaypointLocation))
	{
		return EBTNodeResult::Failed;
	}

	// 값 저장
	BlackboardComp->SetValueAsVector(BBKEY_PATROLLOCATION, WaypointLocation);

	// Waypoint Index 값 증가
	const int32 NextIndex = (PatrolIndex + 1) % WaypointCount;

	BlackboardComp->SetValueAsInt(BBKEY_PATROLINDEX, NextIndex);

	return EBTNodeResult::Succeeded;
}
