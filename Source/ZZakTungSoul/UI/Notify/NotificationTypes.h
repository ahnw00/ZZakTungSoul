#pragma once

#include "CoreMinimal.h"
#include "NotificationTypes.generated.h"

UENUM(BlueprintType)
enum class ENotificationType : uint8
{
	Death			UMETA(DisplayName = "Death"),
	BossClear		UMETA(DisplayName = "Boss Clear"),
	BornFireLit     UMETA(DisplayName = "Born Fire Lit"),
	Region			UMETA(DisplayName = "Region")
};