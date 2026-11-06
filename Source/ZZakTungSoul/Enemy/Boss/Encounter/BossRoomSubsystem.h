#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SaveData/BossRoomSaveData.h"
#include "BossRoomSubsystem.generated.h"

UCLASS()
class ZZAKTUNGSOUL_API UBossRoomSubsystem
	: public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	bool HasEntered(FName RoomID) const;
	bool IsCleared(FName RoomID) const;

	void MarkEntered(FName RoomID);
	void MarkCleared(FName RoomID);
	void ResetRoom(FName RoomID);

	// 저장할 보스방 이력 가져오기
	const TMap<FName, FBossRoomProgress>& GetProgressForSave() const;

	// 저장된 보스방 이력 복원
	void LoadProgressFromSave(
		const TMap<FName, FBossRoomProgress>& SavedProgress
	);


private:
	UPROPERTY()
	TMap<FName, FBossRoomProgress> RoomProgress;
};