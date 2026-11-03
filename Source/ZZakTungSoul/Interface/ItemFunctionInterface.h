// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include <Warrior/Warrior.h>
#include "ItemFunctionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UItemFunctionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ZZAKTUNGSOUL_API IItemFunctionInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void ItemFunction(AWarrior* User) = 0;
};
