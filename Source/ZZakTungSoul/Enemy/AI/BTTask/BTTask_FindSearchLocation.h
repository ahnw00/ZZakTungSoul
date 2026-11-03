// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindSearchLocation.generated.h"

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API UBTTask_FindSearchLocation : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_FindSearchLocation();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	// 최소 수색 거리
	UPROPERTY(EditAnywhere, Category = "Search|Distance")
	float MinSearchDistance = 200.0f;

	// 최대 수색 거리
	UPROPERTY(EditAnywhere, Category = "Search|Distance")
	float MaxSearchDistance = 500.0f;

	// 좌우 수색 각도
	UPROPERTY(EditAnywhere, Category = "Search|Angle")
	float SearchHalfAngle = 45.0f;

	// 탐색 후보 수
	UPROPERTY(EditAnywhere, Category = "Search|Location")
	int32 MaxSearchCandidate= 4;

	UPROPERTY(EditAnywhere, Category = "Search|Navigation")
	FVector NavProjectionExtent = FVector(100.0f, 100.0f, 200.0f);
};
