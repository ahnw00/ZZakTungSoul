// Fill out your copyright notice in the Description page of Project Settings.


#include "GamePlay/ZTSGameMode.h"
#include <Kismet/GameplayStatics.h>
#include <Warrior/WarriorPlayerState.h>
#include "Warrior/WarriorController.h"

AZTSGameMode::AZTSGameMode()
{
	// 컨트롤러 세팅
	PlayerControllerClass = AWarriorController::StaticClass();
	PlayerStateClass = AWarriorPlayerState::StaticClass();

	// 컨트롤러로 조종하는 Pawn 세팅
	static ConstructorHelpers::FClassFinder<APawn> WarriorClassRef(
		TEXT("/Game/Blueprints/Warrior/BP_Warrior.BP_Warrior_C")
	);

	if (WarriorClassRef.Succeeded())
	{
		DefaultPawnClass = WarriorClassRef.Class;
	}
}

UWarriorPlayerSaveGame* AZTSGameMode::GetSaveGame()
{
	// 저장된 데이터가 있으면 그거 반환
	if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		UWarriorPlayerSaveGame* WarriorLoadGame =
			Cast<UWarriorPlayerSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));

		if (WarriorLoadGame)
			return WarriorLoadGame;
	}

	// 저장된 데이터가 없으면 생성해서 반환
	UWarriorPlayerSaveGame* WarriorNewGame =
		Cast<UWarriorPlayerSaveGame>(UGameplayStatics::CreateSaveGameObject(UWarriorPlayerSaveGame::StaticClass()));
	return WarriorNewGame;
}

void AZTSGameMode::SaveGame()
{
	UWarriorPlayerSaveGame* WarriorSaveGame = GetSaveGame();

	AWarriorPlayerState* WarriorPlayerState =
		GetWorld()->GetFirstPlayerController()->GetPlayerState<AWarriorPlayerState>();
	if (!WarriorPlayerState)
	{
		UE_LOG(LogTemp, Log, TEXT("No WarriorPlayerState"));
		return;
	}

	// PlayerState에서 저장 데이터 가져오기
	WarriorSaveGame->SetSaveData(WarriorPlayerState->GetSaveData());

	// 저장해주기
	if (!UGameplayStatics::SaveGameToSlot(WarriorSaveGame, SlotName, 0))
	{
		UE_LOG(LogTemp, Log, TEXT("Cannot save WarriorSaveGame"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("WarriorData Saved"));
}

void AZTSGameMode::LoadGame()
{
	// 저장 데이터 없으면 return
	if (!UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		UE_LOG(LogTemp, Log, TEXT("There is no data saved"));
		return;
	}

	// 저장 데이터가 존재한다면
	if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		// 저장 데이터 캐스팅
		if(UWarriorPlayerSaveGame* WarriorLoadGame =
			Cast<UWarriorPlayerSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0)))
		{
			// WarriorPlayerState 가져오기
			if(AWarriorPlayerState* WarriorPlayerState = 
				GetWorld()->GetFirstPlayerController()->GetPlayerState<AWarriorPlayerState>())
			{
				// PlayerState로 정보 로드
				WarriorPlayerState->LoadSaveDataFrom(WarriorLoadGame->GetSaveData());
				UE_LOG(LogTemp, Log, TEXT("Warrior Data Loaded"));
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("WarriorPlayerState not loaded"));
			}
		}
	}
}

void AZTSGameMode::DeleteGame()
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		UGameplayStatics::DeleteGameInSlot(SlotName, 0);
	}
}


