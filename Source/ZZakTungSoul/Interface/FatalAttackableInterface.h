// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include <Data/DamageInfo.h>
#include "FatalAttackableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UFatalAttackableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ZZAKTUNGSOUL_API IFatalAttackableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// 페이탈 어택을 당할 수 있는 상태인지 확인
	virtual bool CanBeFatalAttacked(AActor* Attacker) const = 0;

	// 페이탈 어택 당하는 함수
	virtual void ReceiveFatalAttack(const FDamageInfo DamageInfo, bool bIsFront) = 0;
};
