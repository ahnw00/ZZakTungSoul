#pragma once

#include "CoreMinimal.h"
#include "WarriorSaveData.generated.h"

USTRUCT(BlueprintType)
struct ZZAKTUNGSOUL_API FWarriorAttributeData
{
	GENERATED_BODY()


    // 플레이어가 성장/증가시킨 값
    UPROPERTY()
    int32 Vigor = 10;

    UPROPERTY()
    int32 Attunement = 10;

    UPROPERTY()
    int32 Endurance = 10;

    UPROPERTY()
    int32 Vitality = 10;

    UPROPERTY()
    int32 Strength = 10;

    UPROPERTY()
    int32 Dexterity = 10;
};

USTRUCT(BlueprintType)
struct ZZAKTUNGSOUL_API FWarriorSaveData
{
    GENERATED_BODY()


    // 실제 현재 상태
    UPROPERTY()
    int32 Level = 1;

    UPROPERTY()
    int32 Souls = 0;

    UPROPERTY()
    FWarriorAttributeData Attributes;

    UPROPERTY()
    float CurrentHP = 0.f;

    UPROPERTY()
    float CurrentFP = 0.f;
};