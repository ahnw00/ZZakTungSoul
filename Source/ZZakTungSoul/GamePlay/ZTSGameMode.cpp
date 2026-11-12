// Fill out your copyright notice in the Description page of Project Settings.


#include "GamePlay/ZTSGameMode.h"
#include <Kismet/GameplayStatics.h>
#include <Warrior/WarriorPlayerState.h>
#include "Warrior/WarriorController.h"
#include "Enemy/Boss/Encounter/BossRoomSubsystem.h"
#include "Engine/GameInstance.h"
#include "Enemy/Boss/Encounter/BossRoom.h"
#include "EngineUtils.h"
#include <Interactable/BonFire.h>
#include <GamePlay/MusicSubsystem.h>
#include <EngineUtils.h>

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

	static ConstructorHelpers::FObjectFinder<USoundBase> GamePlayBGMRef(
		TEXT("/Game/Assets/Sound/BGM/BGM1.BGM1")
	);
	if (GamePlayBGMRef.Succeeded())
	{
		GamePlayBGM = GamePlayBGMRef.Object;
	}
}

void AZTSGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	// 레벨 열릴 때 LoadGame 옵션 붙어 있는 지 검사.
	if (UGameplayStatics::HasOption(Options, TEXT("LoadGame")))
	{
		bShouldLoadOnStart = true;
		UE_LOG(LogTemp, Log, TEXT("[GameMode] LoadGame 옵션 감지. BeginPlay에서 세이브 로드합니다."));
	}

	// 컨트롤러로 조종하는 Pawn 세팅
	//DefaultPawnClass = LoadClass<APawn>()
	//PawnClass = TSoftClassPtr<APawn>(FSoftObjectPath(TEXT("/Game/Blueprints/Warrior/BP_Warrior.BP_Warrior_C")));
	//if (PawnClass.IsPending())
	//{
	//	PawnClass.LoadSynchronous();
	//	DefaultPawnClass = PawnClass.Get();
	//}
}

void AZTSGameMode::BeginPlay()
{
	Super::BeginPlay();

	PlayGameplayBGM();

	// 타이틀에서 [LoadGame]으로 들어왔다면 자동으로 세이브 로드.
	if (bShouldLoadOnStart)
	{
		LoadGame();
	}
}

void AZTSGameMode::PlayGameplayBGM(float FadeInDuration)
{
	if (UMusicSubsystem* Music = GetGameInstance()->GetSubsystem<UMusicSubsystem>())
	{
		Music->PlayBGMWithFade(GamePlayBGM, FadeInDuration);
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

	// 플레이어 저장 데이터 복사
	FWarriorSaveData SaveData = WarriorPlayerState->GetSaveData();

	// 현재 보스방 이력도 저장 데이터에 포함
	if (UBossRoomSubsystem* Progress = GetGameInstance()->GetSubsystem<UBossRoomSubsystem>())
	{
		SaveData.BossRoomProgress = Progress->GetProgressForSave();
	}

	// 완성된 데이터를 SaveGame 객체에 전달
	WarriorSaveGame->SetSaveData(SaveData);
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
			if (UBossRoomSubsystem* Progress = GetGameInstance()->GetSubsystem<UBossRoomSubsystem>())
			{
				Progress->LoadProgressFromSave(WarriorLoadGame->GetSaveData().BossRoomProgress);

				for (TActorIterator<ABossRoom> It(GetWorld()); It; ++It)
				{
					It->ApplySavedProgress();
				}
			}

			// WarriorPlayerState 가져오기
			if(AWarriorPlayerState* WarriorPlayerState = 
				GetWorld()->GetFirstPlayerController()->GetPlayerState<AWarriorPlayerState>())
			{
				// PlayerState로 정보 로드
				WarriorPlayerState->LoadSaveDataFrom(WarriorLoadGame->GetSaveData());
				UE_LOG(LogTemp, Log, TEXT("Warrior Data Loaded"));

				for (TActorIterator<ABonfire> It(GetWorld()); It; ++It)
				{
					It->SyncLitState(WarriorPlayerState);
				}

				// 마지막 상호작용 화톳불로 순간이동.
				const FName LastBonfireID = WarriorPlayerState->GetLastBonfireID();
				if (!LastBonfireID.IsNone())
				{
					if (AWarriorController* PC = Cast<AWarriorController>(GetWorld()->GetFirstPlayerController()))
					{
						PC->TeleportToBonfire(LastBonfireID);
						UE_LOG(LogTemp, Log, TEXT("[GameMode] 마지막 화톳불(%s) 위치로 이동 완료."), *LastBonfireID.ToString());
					}
				}
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


