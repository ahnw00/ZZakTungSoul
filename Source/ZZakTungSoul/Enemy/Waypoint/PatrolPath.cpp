// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Waypoint/PatrolPath.h"

#include <Components/SplineComponent.h>

// Sets default values
APatrolPath::APatrolPath()
{
	PatrolSplineComp = CreateDefaultSubobject<USplineComponent>(TEXT("PatrolSpline"));

	RootComponent = PatrolSplineComp;

	// Spline 첫 위치와 마지막위치를 자동으로 연결하지 않음
	PatrolSplineComp->SetClosedLoop(false);
}

int32 APatrolPath::GetWaypointCount() const
{
	if (false == IsValid(PatrolSplineComp))
	{
		return 0;
	}

	// 현재 Spline의 Point 개수를 반환
	return PatrolSplineComp->GetNumberOfSplinePoints();
}

bool APatrolPath::GetWaypointLocation(int32 Index, FVector& OutLocation) const
{
	if (false == IsValid(PatrolSplineComp))
	{
		return false;
	}
	
	const int32 WaypointCount = GetWaypointCount();

	// 유효성 검사
	if (Index < 0 || Index >= WaypointCount)
	{
		return false;
	}

	// Spline point의 위치 가져오기
	OutLocation = PatrolSplineComp->GetLocationAtSplinePoint(Index, ESplineCoordinateSpace::World);

	return true;
}

