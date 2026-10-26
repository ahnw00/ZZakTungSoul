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

	UFUNCTION(BlueprintCallable, Category = "PlayerState|Stat")
	bool UpgradeStat(const FName& StatName, int32 RequiredSouls);

	// 가상 포인트 검증 및 일괄 적용 함수.
	UFUNCTION(BlueprintCallable, Category = "PlayerState|Stat")
	bool ApplyLevelUp(const struct FWarriorAttributeData& PendingPoints);

	// 화톳불 
	UFUNCTION(BlueprintCallable, Category = "PlayerState|Bonfire")
	bool RegisterLitBonfire(const FName & BonfireID);
	UFUNCTION(BlueprintPure, Category = "PlayerState|Bonfire")
	bool IsBonfireLit(const FName& BonfireID) const;

	FORCEINLINE const TArray<FName>& GetLitBonfireIds() const { return SaveData.LitBonfireIDs; }
private:
	// 실제 게임 플레이 중 값을 변화하게할 대상
	UPROPERTY()
	FWarriorSaveData SaveData;
};
