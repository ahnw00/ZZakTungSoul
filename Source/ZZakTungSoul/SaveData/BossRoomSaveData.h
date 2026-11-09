#pragma once

#include "CoreMinimal.h"
#include "BossRoomSaveData.generated.h"

USTRUCT()
struct ZZAKTUNGSOUL_API FBossRoomProgress
{
	GENERATED_BODY()

	// 첫 시퀀스 시청 이력
	UPROPERTY()
	bool bEntered = false;

	// 보스 처치 여부
	UPROPERTY()
	bool bCleared = false;
};