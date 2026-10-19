// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/AI/BTTask/BTTask_FindStrafeLocation.h"
#include <Enemy/AI/EnemyAIBlackboardKey.h>

#include <AIController.h>
#include <BehaviorTree/BlackboardComponent.h>
#include <NavigationSystem.h>

UBTTask_FindStrafeLocation::UBTTask_FindStrafeLocation()
{
	NodeName = TEXT("Find Strafe Location");
}

EBTNodeResult::Type UBTTask_FindStrafeLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Blackboard 가져오기
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (nullptr == BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}

	// StrafeLocation 값 초기화
	BlackboardComp->ClearValue(BlackboardComp->GetKeyID(BBKEY_STRAFELOCATION));

	// AIController가 조종하는 폰 가져오기
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();

	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(BBKEY_TARGETACTOR));

	// 조종하는 Pawn이나 TargetActor가 존재하지 않거나 파괴 중이면 값 초기화 후 건너뛰기
	if (!IsValid(ControllingPawn) || !IsValid(TargetActor))
	{
		return EBTNodeResult::Failed;
	}

	// 4방향 가중치가 모두 0인지 확인
	if (ForwardWeight <= 0 && BackwardWeight <= 0 && LeftWeight <= 0 && RightWeight <= 0)
	{
		return EBTNodeResult::Failed;
	}

	// NavigationSystem 가져오기
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(ControllingPawn->GetWorld());
	if (nullptr == NavSystem)
	{
		return EBTNodeResult::Failed;
	}

	const FVector ControllingPawnLocation = ControllingPawn->GetActorLocation();
	const FVector TargetActorLocation = TargetActor->GetActorLocation();

	// Enemy에서 목표까지의 수평 방향
	FVector ForwardDirection = TargetActorLocation - ControllingPawnLocation;

	// 방향 벡터 정규화
	ForwardDirection.Z = 0.0f;
	ForwardDirection.Normalize();

	// 전방 벡터 기준 오른쪽 방향 벡터
	FVector RightDirection = FVector::CrossProduct(FVector::UpVector, ForwardDirection);
	RightDirection.Normalize();

	// 가중치로 방향 선택
	FVector MoveDirection;
	const EStrafeDirection SelectDirection = SelectStrafeDirection();

	// 선택된 방향에 따라 이동 벡터 결정
	switch (SelectDirection)
	{
	case EStrafeDirection::Forward:
		MoveDirection = ForwardDirection;
		break;

	case EStrafeDirection::Backward:
		MoveDirection = -ForwardDirection;
		break;

	case EStrafeDirection::Right:
		MoveDirection = RightDirection;
		break;

	case EStrafeDirection::Left:
		MoveDirection = -RightDirection;
		break;

	default:
		return EBTNodeResult::Failed;
	}

	// 최소 / 최대 이동거리 보정
	const float MinDistance = FMath::Max(0.0f, MinStrafeDistance);
	const float MaxDistance = FMath::Max(MinDistance, MaxStrafeDistance);

	// 랜덤 이동거리 선택
	const float RandomDistance = FMath::FRandRange(MinDistance, MaxDistance);

	// 이동 목적지 계산
	const FVector DesiredLocation = ControllingPawnLocation + MoveDirection * RandomDistance;

	// NavMesh에 위치 보정
	FNavLocation NavLocation;
	if (!NavSystem->ProjectPointToNavigation(DesiredLocation, NavLocation))
	{
		return EBTNodeResult::Failed;
	}

	// Blackboard에 위치 저장
	BlackboardComp->SetValueAsVector(BBKEY_STRAFELOCATION, NavLocation.Location);

	return EBTNodeResult::Succeeded;
}

EStrafeDirection UBTTask_FindStrafeLocation::SelectStrafeDirection()
{
	const int32 Weights[4] =
	{
		FMath::Max(0, ForwardWeight),
		FMath::Max(0, BackwardWeight),
		FMath::Max(0, LeftWeight),
		FMath::Max(0, RightWeight)
	};

	// 전체 가중치 합산
	//const int64 TotalWeight = static_cast<int64>(Weights[0]) + Weights[1] + Weights[2] + Weights[3];
	int64 TotalWeight = 0;
	for (int32 Weight : Weights)
	{
		TotalWeight += Weight;
	}

	// 유효한 가중치가 없으면 기본값 반환
	if (TotalWeight <= 0)
	{
		return EStrafeDirection::Forward;
	}

	// 0 이상 합산가중치 미만의 난수
	const double RandomValue = FMath::FRand() * static_cast<double>(TotalWeight);

	// 누적 가중치 변수
	double AccumulatedWeight = 0.0f;

	for (int32 i = 0; i < 4; ++i)
	{
		AccumulatedWeight += Weights[i];

		if (RandomValue < AccumulatedWeight)
		{
			return static_cast<EStrafeDirection>(i);
		}
	}

	return EStrafeDirection::Right;
}
