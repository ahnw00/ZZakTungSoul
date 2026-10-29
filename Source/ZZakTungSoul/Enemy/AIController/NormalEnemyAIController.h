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

protected:
	// AI 인식 컴포넌트
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<class UAIPerceptionComponent> EnemyPerceptionComponent;

	// 시야 감지 설정
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<class UAISenseConfig_Sight> SightConfig;
};
