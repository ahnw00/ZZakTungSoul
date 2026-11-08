// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
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
	// Getter
	FORCEINLINE bool IsAIActive() const { return bAIActive; }

public:
	// 컨트롤 하고 있는 Enemy의 상태에 따라 컨트롤러의 Focus를 관리하는 함수
	virtual void UpdateAIFocus(EEnemyState PreviousState, EEnemyState NewState);

	virtual void UpdateAIFocusState();

	// 거리 기반 AI 상태 초기화
	virtual void ActiveInitialize(bool bActive);

	// AI 중지 함수
	void StopAIController();

	void SetAIWait(bool bIsWait);

	// 거리 기반 AI On/Off
	void SetAIActive(bool bActive);

	// Reset
	void EnemyContollerReset();

protected:
	virtual void OnPossess(APawn* InPawn) override;

	void ToggleFocus();

protected:
	// BehaviorTree
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<class UBehaviorTree> BehaviorTreeAsset;

	// Blackboard
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<class UBlackboardData> BlackboardAsset;

	// AI활성화 플래그
	UPROPERTY(VisibleAnywhere, Category = "AI")
	uint32 bAIActive : 1 = true;
};
