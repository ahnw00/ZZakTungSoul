// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/RestorableProp.h"
#include <GeometryCollection/GeometryCollectionComponent.h>
#include <Kismet/GameplayStatics.h>
#include <Sound/SoundBase.h>

// Sets default values
ARestorableProp::ARestorableProp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void ARestorableProp::BeginPlay()
{
	Super::BeginPlay();

	TArray<UGeometryCollectionComponent*> GCComps;
	GetComponents<UGeometryCollectionComponent>(GCComps);

	for (UGeometryCollectionComponent* GC : GCComps)
	{
		GC->SetNotifyBreaks(true);
		GC->OnChaosBreakEvent.AddDynamic(this, &ARestorableProp::OnPieceBroken);
	}
}

void ARestorableProp::OnPieceBroken(const FChaosBreakEvent& BreakEvent)
{
	if (!BreakEvent.Component)
	{
		return;
	}
	for (const FName& Tag : BreakEvent.Component->ComponentTags)
	{
		if (PlayedGroups.Contains(Tag))
		{
			continue;
		}
		const TObjectPtr<USoundBase>* Sound = BreakSounds.Find(Tag);
		if (!Sound || !*Sound)
		{
			continue;
		}
		PlayedGroups.Add(Tag);
		UGameplayStatics::PlaySoundAtLocation(this, *Sound, BreakEvent.Location);
	}
}


