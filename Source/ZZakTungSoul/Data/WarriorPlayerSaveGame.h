// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <SaveData/WarriorSaveData.h>
#include "GameFramework/SaveGame.h"
#include "WarriorPlayerSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API UWarriorPlayerSaveGame : public ULocalPlayerSaveGame
{
	GENERATED_BODY()

public:
	FORCEINLINE const FWarriorSaveData& GetSaveData() const { return SaveData; }
	FORCEINLINE void SetSaveData(const FWarriorSaveData& NewSaveData) { SaveData = NewSaveData; }
	
private:
	// 저장될 대상
	// 저장할 때 PlayerState에 있는 값을 읽어오고
	// 로드할 때 PlayerState에 덮어씌워줄 값
	UPROPERTY()
	FWarriorSaveData SaveData;
};
