// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MusicSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API UMusicSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable)
    void PlayBGM(USoundBase* BGM);

    void PlayBGMWithFade(USoundBase* BGM, float FadeInDuration);

    UFUNCTION(BlueprintCallable)
    void StopBGM(float FadeOutDuration = 0.5f);

public:
    UPROPERTY()
    TObjectPtr<UAudioComponent> CurrentBGM;
};
