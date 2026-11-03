// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "GeometryCollectionComponentBase.generated.h"

/**
 * 
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class ZZAKTUNGSOUL_API UGeometryCollectionComponentBase : public UGeometryCollectionComponent
{
	GENERATED_BODY()
	
private:
	virtual void NotifyBreak(const FChaosBreakEvent& Event) override;
};
