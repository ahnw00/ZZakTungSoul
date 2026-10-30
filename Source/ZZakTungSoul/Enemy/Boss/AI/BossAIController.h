// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyAIController.h"
#include "BossAIController.generated.h"

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API ABossAIController : public AEnemyAIController
{
	GENERATED_BODY()
public:
	ABossAIController();

	virtual void UpdateAIFocus(EEnemyState PreviousState, EEnemyState NewState) override;

};
