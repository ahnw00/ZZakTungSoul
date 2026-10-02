#pragma once

#include "CoreMinimal.h"
#include "StateEnum.generated.h"

UENUM(BlueprintType)
enum class EMoveState : uint8
{
	Idle,
	Walk,
	Run,
	Block,
	LockOn
};

UENUM(BlueprintType)
enum class EActionState : uint8
{
	None,
	Roll,
	BackStep,
	Attack,
	Hit
};

UENUM(BlueprintType)
enum class EOverlayState : uint8
{
	None,
	Block,
	TwoHand
};