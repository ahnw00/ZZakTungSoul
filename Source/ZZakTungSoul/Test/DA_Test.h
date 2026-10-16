// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DA_Test.generated.h"

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API UDA_Test : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	TArray<TSoftObjectPtr<class UBehaviorTree>> Tests;
};
