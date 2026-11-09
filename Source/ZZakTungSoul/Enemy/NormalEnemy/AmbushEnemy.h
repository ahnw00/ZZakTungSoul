// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/NormalEnemy/NormalEnemyBase.h"
#include "AmbushEnemy.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EAmbushState : uint8
{
	Waiting		/*대기*/,
	Trigger		/*기습*/,
	Active		/*활성화*/
};

UCLASS()
class ZZAKTUNGSOUL_API AAmbushEnemy : public ANormalEnemyBase
{
	GENERATED_BODY()
	
public:
	AAmbushEnemy();

public:
	// Behavior Tree 자동 실행 끄기
	virtual bool ShouldAutoStartAI() const override { return false; }

	FORCEINLINE EAmbushState GetAmbushState() const { return CurrentAmbushState; }

public:
	// 매복 발동
	bool ActiveAmbush(AActor* TargetActor);

	// 체크포인트 초기화
	virtual void ResetEnemy(const FTransform& SpawnTransform) override;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Enemy|Ambush")
	EAmbushState CurrentAmbushState = EAmbushState::Waiting;
};
