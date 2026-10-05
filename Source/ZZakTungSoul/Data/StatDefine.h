#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "StatDefine.generated.h"


USTRUCT(BlueprintType)
struct ZZAKTUNGSOUL_API FStatDefine : public FTableRowBase
{
	GENERATED_BODY()


public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 InitialValue = 10;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MinValue = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MaxValue = 99;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bUpgradeable = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Describes;
};