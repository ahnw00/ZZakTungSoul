// Fill out your copyright notice in the Description page of Project Settings.


#include "GamePlay/MusicSubsystem.h"

#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

void UMusicSubsystem::PlayBGM(USoundBase* BGM)
{
    PlayBGMWithFade(BGM, 0.f);
}

void UMusicSubsystem::PlayBGMWithFade(USoundBase* BGM, float FadeInDuration)
{
    if (!BGM)
    {
        return;
    }

    if (CurrentBGM && CurrentBGM->Sound == BGM)
    {
        if (CurrentBGM->IsPlaying())
        {
            return;
        }
    }
    else
    {
        StopBGM();
        CurrentBGM = UGameplayStatics::CreateSound2D(GetGameInstance(), BGM, 1.f, 1.f, 0.f, nullptr, true, false);
    }

    if (CurrentBGM)
    {
        if (FadeInDuration > 0.f)
        {
            CurrentBGM->FadeIn(FadeInDuration, 1.f, 0.f, EAudioFaderCurve::Linear);
        }
        else
        {
            CurrentBGM->Play();
        }
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
