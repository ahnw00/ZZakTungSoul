#pragma once

#include <CoreMinimal.h>
#include <Subsystems/GameInstanceSubsystem.h>
#include "BossRoomSubsystem.generated.h"

//---------------------------------------------------------------
// 보스방별 진행 이력.
//
//---------------------------------------------------------------
USTRUCT()
struct FBossRoomProgress
{
	GENERATED_BODY()

	UPROPERTY()
	bool bEntered = false;

	UPROPERTY()
	bool bCleared = false;
};

UCLASS()
class ZZAKTUNGSOUL_API UBossRoomSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	bool HasEntered(FName RoomID) const;
	bool IsCleared(FName RoomID) const;

	void MarkEntered(FName RoomID);
	void MarkCleared(FName RoomID);
	void ResetRoom(FName RoomID);

private:
	UPROPERTY()
	TMap<FName, FBossRoomProgress> RoomProgress;
};