// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Data/WarriorPlayerSaveGame.h>
#include "GameFramework/GameMode.h"
#include "ZTSGameMode.generated.h"

// 전방 선언
class UWarriorPlayerSaveGame;

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API AZTSGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	AZTSGameMode();

protected:
	// Getter(만약 없으면 생성해서 반환)
	UWarriorPlayerSaveGame* GetSaveGame();

public:
	// PlayerState -> SaveData로 덮어씌우고 저장
	void SaveGame();

	// SaveData -> PlayerState로 덮어씌우기
	void LoadGame();

	void DeleteGame();


private:
	const FString SlotName = TEXT("SaveSlot");
};
