// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <SaveData/WarriorSaveData.h>
#include "GameFramework/PlayerState.h"
#include "WarriorPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API AWarriorPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	AWarriorPlayerState();

	// Getter/Setter
	FORCEINLINE const FWarriorSaveData& GetSaveData() const { return SaveData; }
	FORCEINLINE void LoadSaveDataFrom(const FWarriorSaveData& NewSaveData) { SaveData = NewSaveData; }

private:
	// 실제 게임 플레이 중 값을 변화하게할 대상
	UPROPERTY()
	FWarriorSaveData SaveData;
};
