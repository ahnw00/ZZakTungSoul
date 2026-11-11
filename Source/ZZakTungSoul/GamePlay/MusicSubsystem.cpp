// Fill out your copyright notice in the Description page of Project Settings.


#include "GamePlay/MusicSubsystem.h"

#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

void UMusicSubsystem::PlayBGM(USoundBase* BGM)
{
    if (!BGM)
    {
        return;
    }

    if (CurrentBGM && CurrentBGM->Sound == BGM)
    {
        if (!CurrentBGM->IsPlaying())
        {
            CurrentBGM->Play();
        }

        return;
    }

    if (CurrentBGM)
    {
        CurrentBGM->FadeOut(
            0.5f,
            0.f,
            EAudioFaderCurve::Linear);
    }

    CurrentBGM = UGameplayStatics::SpawnSound2D(
        GetGameInstance(),
        BGM,
        1.f,
        1.f,
        0.f,
        nullptr,
        true,
        false);

    if (CurrentBGM)
    {
        CurrentBGM->Play();
    }
}

void UMusicSubsystem::StopBGM(float FadeOutDuration)
{
    if (!CurrentBGM)
    {
        return;
    }

    CurrentBGM->FadeOut(
        FadeOutDuration,
        0.f,
        EAudioFaderCurve::Linear);

    CurrentBGM = nullptr;
}
