// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/AI/BTTask/BTTask_FindSearchLocation.h"
#include <Enemy/AI/EnemyAIBlackboardKey.h>
#include "BTTask_FindSearchLocation.h"

#include <AIController.h>
#include <BehaviorTree/BehaviorTreeComponent.h>
#include <BehaviorTree/BlackboardComponent.h>
#include <NavigationSystem.h>
#include <NavigationPath.h>

UBTTask_FindSearchLocation::UBTTask_FindSearchLocation()
{
	NodeName = TEXT("Find Search Location");
}

EBTNodeResult::Type UBTTask_FindSearchLocation::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// Blackboard 가져오기
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (nullptr == BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}

	// 기존 값 초기화
	BlackboardComp->ClearValue(BBKEY_SEARCHLOCATION);

	// Controller와 Pawn 가져오기
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (nullptr == AIController)
	{
		return EBTNodeResult::Failed;
	}

	APawn* ControllingPawn = AIController->GetPawn();
	if (nullptr == ControllingPawn)
	{
		return EBTNodeResult::Failed;
	}

	// 마지막 관측 위치 확인
	if (false == BlackboardComp->IsVectorValueSet(BBKEY_LASTKNOWNLOCATION))
	{
		return EBTNodeResult::Failed;
	}

	// 마지막 관측 위치
	const FVector LastKnownLocation = BlackboardComp->GetValueAsVector(BBKEY_LASTKNOWNLOCATION);

	// 마지막 관측 이동 방향
	FVector LastKnownDirection = BlackboardComp->GetValueAsVector(BBKEY_LASTKNOWNDIRECTION);
	LastKnownDirection.Z = 0.0f;

	// 방향이 없으면 실패
	if (!LastKnownDirection.Normalize())
	{
		return EBTNodeResult::Failed;
	}

	// Navigation System 가져오기
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(ControllingPawn->GetWorld());
	if (nullptr == NavSystem)
	{
		return EBTNodeResult::Failed;
	}

	const float MinDistance = FMath::Max(0.0f, MinSearchDistance);
	const float MaxDistance = FMath::Max(0.0f, MaxSearchDistance);
	const float HalfAngle = FMath::Clamp(SearchHalfAngle, 0.0f, 180.0f);

	// 최대 탐색 후보군
	const int32 Candidates = FMath::Max(1, MaxSearchCandidate);

	// 이동 시작 위치
	const FVector StartLocation = ControllingPawn->GetActorLocation();

	// Navigation Data
	const ANavigationData* NavData = NavSystem->GetDefaultNavDataInstance(FNavigationSystem::DontCreate);
	if (!NavData)
	{
		return EBTNodeResult::Failed;
	}

	// 수색 후보 생성
	for (int32 ix = 0; ix < Candidates; ++ix)
	{
		// 마지막 이동 방향 기준 좌우 랜덤 각도
		const float RandomAngle = FMath::FRandRange(-HalfAngle, HalfAngle);

		// 수색 거리
		const float RandomDistance = FMath::FRandRange(MinDistance, MaxDistance);

		// 방향 회전
		const FVector SearchDirection = LastKnownDirection.RotateAngleAxis(RandomAngle, FVector::UpVector);

		// 수색 위치
		const FVector DesiredLocation = LastKnownLocation + SearchDirection * RandomDistance;

		// NavMesh 위로 위치 보정
		FNavLocation NavLocation;

		const bool bProjected = NavSystem->ProjectPointToNavigation(DesiredLocation, NavLocation, NavProjectionExtent, NavData);
		if (!bProjected)
		{
			continue;
		}

		// 실제 이동 가능한 경로인지 검사
		UNavigationPath* Path = NavSystem->FindPathToLocationSynchronously(
			ControllingPawn->GetWorld(),
			StartLocation,
			NavLocation.Location,
			ControllingPawn
		);

		// 경로가 없거나 도달 불가능한 경우 제외
		if (!IsValid(Path) || !Path->IsValid() || Path->IsPartial())
		{
			continue;
		}

		// 수색 위치 저장
		BlackboardComp->SetValueAsVector(BBKEY_SEARCHLOCATION, NavLocation.Location);

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
