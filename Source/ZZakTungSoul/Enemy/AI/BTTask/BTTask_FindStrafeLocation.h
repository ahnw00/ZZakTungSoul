// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindStrafeLocation.generated.h"

// 간보기 이동방향
enum class EStrafeDirection : uint8
{
	Forward,
	Backward,
	Left,
	Right
};

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API UBTTask_FindStrafeLocation : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_FindStrafeLocation();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	// 가중치 기반의 방향 선택 함수
	EStrafeDirection SelectStrafeDirection();

protected:		// 이동 방향 가중치 섹션
	UPROPERTY(EditAnywhere, Category = "Strafe|Weight")
	int32 ForwardWeight = 10;

	UPROPERTY(EditAnywhere, Category = "Strafe|Weight")
	int32 BackwardWeight = 20;

	UPROPERTY(EditAnywhere, Category = "Strafe|Weight")
	int32 LeftWeight = 35;

	UPROPERTY(EditAnywhere, Category = "Strafe|Weight")
	int32 RightWeight = 35;

protected:		// 이동 거리 섹션
	UPROPERTY(EditAnywhere, Category = "Strafe|Distance")
	float MinStrafeDistance = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Strafe|Distance")
	float MaxStrafeDistance = 200.0f;
};
