#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "StatGrowth.generated.h"


USTRUCT(BlueprintType)
struct ZZAKTUNGSOUL_API FStatGrowth : public FTableRowBase
{
	GENERATED_BODY()


public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName StatID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName Target;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MinLevel = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MaxLevel = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Value = 0;
};