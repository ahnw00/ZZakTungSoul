// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <SaveData/WarriorSaveData.h>
#include <Interface/SoulStateInterface.h>
#include "GameFramework/PlayerState.h"
#include "WarriorPlayerState.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSoulChangedDelegate, int32, CurrentSouls);

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API AWarriorPlayerState : public APlayerState, public ISoulStateInterface
{
	GENERATED_BODY()
	
public:
	AWarriorPlayerState();

	// Getter/Setter
	FORCEINLINE FWarriorSaveData& GetSaveData() { return SaveData; }
	
	
	void LoadSaveDataFrom(const FWarriorSaveData& NewSaveData);

	UFUNCTION(BlueprintCallable, Category = "PlayerState|Stat")
	bool UpgradeStat(const FName& StatName, int32 RequiredSouls);

	// 가상 포인트 검증 및 일괄 적용 함수.
	UFUNCTION(BlueprintCallable, Category = "PlayerState|Stat")
	bool ApplyLevelUp(const struct FWarriorAttributeData& PendingPoints);

	// 소울 획득.
	UFUNCTION(BlueprintCallable, Category = "PlayerState|Souls")
	void AddSouls(int32 Amount);
	// 소울 변경 알림.
	UPROPERTY(BlueprintAssignable, Category = "PlayerState|Souls")
	FOnSoulChangedDelegate OnSoulChanged;

	// 화톳불 
	UFUNCTION(BlueprintCallable, Category = "PlayerState|Bonfire")
	bool RegisterLitBonfire(const FName & BonfireID);
	UFUNCTION(BlueprintPure, Category = "PlayerState|Bonfire")
	bool IsBonfireLit(const FName& BonfireID) const;

	FORCEINLINE const TArray<FName>& GetLitBonfireIds() const { return SaveData.LitBonfireIDs; }
	FORCEINLINE FName GetLastBonfireID() { return SaveData.LastBonfireID; }
	FORCEINLINE void SetLastBonfireID(const FName& IsBonfireID) { SaveData.LastBonfireID = IsBonfireID; }
private:
	// 실제 게임 플레이 중 값을 변화하게할 대상
	UPROPERTY()
	FWarriorSaveData SaveData;

	// Inherited via ISoulStateInterface
	virtual void TakeSoul(int32 Amount) override;
};
