// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AmbushTrigger.generated.h"

UCLASS()
class ZZAKTUNGSOUL_API AAmbushTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAmbushTrigger();

protected:
	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	// Ambush Trigger
	UPROPERTY(VisibleAnywhere, Category = "Ambush|Trigger")
	TObjectPtr<class UBoxComponent> TriggerBox;

	// Spawner
	UPROPERTY(EditInstanceOnly, Category = "Ambush|Spawner")
	TObjectPtr<class AEnemySpawner> EnemySpawner;

	UPROPERTY(EditInstanceOnly, Category = "Ambush|Spawner")
	int32 SpawnIndex = 0;

};
