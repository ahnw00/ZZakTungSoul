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

	// 맵 열릴 때 LoadGame=1 수신.
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

protected:
	virtual void BeginPlay() override;

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

	// 시작시 세이브 로드해야 하는 지 여부 플래그.
	bool bShouldLoadOnStart = false;

	UPROPERTY()
	TObjectPtr<USoundBase> GamePlayBGM;

	//UPROPERTY()
	//TSoftClassPtr<APawn> PawnClass;
};
