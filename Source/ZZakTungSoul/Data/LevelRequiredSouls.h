#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "LevelRequiredSouls.generated.h"


USTRUCT(BlueprintType)
struct ZZAKTUNGSOUL_API FLevelRequiredSouls : public FTableRowBase
{
	GENERATED_BODY()


public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 RequiredSouls = 1;
};