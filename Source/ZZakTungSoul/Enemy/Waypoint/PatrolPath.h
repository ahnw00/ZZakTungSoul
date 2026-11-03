// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PatrolPath.generated.h"

UCLASS()
class ZZAKTUNGSOUL_API APatrolPath : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APatrolPath();

	// Waypoint 개수 반환 함수
	int32 GetWaypointCount() const;

	// Waypoint의 위치 반환 함수
	bool GetWaypointLocation(int32 Index, FVector& OutLocation) const;

protected:
	// Spline Component
	UPROPERTY(VisibleAnywhere, Category = "Patrol")
	TObjectPtr <class USplineComponent> PatrolSplineComp;
};
