// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyAIController.h"
#include <Perception/AIPerceptionTypes.h>
#include "NormalEnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API ANormalEnemyAIController : public AEnemyAIController
{
	GENERATED_BODY()
	
public:
	ANormalEnemyAIController();

protected:
	// 타겟 인식 정보 갱신 함수
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	// 타겟 정보 갱신 함수
	void UpdateTargetInfo();

	virtual void ActiveInitialize(bool bActive) override;

	// Blackboard 초기화
	void ClearTargetInfo();

public:
	// Hit시 플레이어 인지
	void RecognizeTarget(AActor* NewTarget);

protected:
	// 관측 타이머
	FTimerHandle TargetInfoUpdateTimerHandle;

protected:
	// AI 인식 컴포넌트
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<class UAIPerceptionComponent> EnemyPerceptionComponent;

	// 시야 감지 설정
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<class UAISenseConfig_Sight> SightConfig;

	// 타겟 정보 갱신 주기
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float UpdateInterval = 0.2f;

	// 판정할 최소 이동 속도 -> 속도가 0이 되어 방향을 못 구하는 것을 방지하기 위함
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float MinTargetSpeed = 10.0f;
};
