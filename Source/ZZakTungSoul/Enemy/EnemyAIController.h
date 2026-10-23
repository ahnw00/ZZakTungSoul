// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include <Perception/AIPerceptionTypes.h>
#include <Enemy/EnemyStates.h>
#include "EnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AEnemyAIController();

public:
	// 컨트롤 하고 있는 Enemy의 상태에 따라 컨트롤러의 Focus를 관리하는 함수
	void UpdateAIFocus(EEnemyState PreviousState, EEnemyState NewState);

protected:
	virtual void OnPossess(APawn* InPawn) override;

	void ToggleFocus();

	// 타겟 인식 정보 갱신 함수
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

protected:
	// BehaviorTree
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<class UBehaviorTree> BehaviorTreeAsset;

	// Blackboard
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<class UBlackboardData> BlackboardAsset;

	// AI 인식 컴포넌트
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<class UAIPerceptionComponent> EnemyPerceptionComponent;

	// 시야 감지 설정
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<class UAISenseConfig_Sight> SightConfig;
};
