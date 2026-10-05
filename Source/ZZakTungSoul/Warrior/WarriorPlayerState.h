// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

	UPROPERTY()
	int32 Level;

	UPROPERTY()
	int32 Souls;

	//UPROPERTY()

};
