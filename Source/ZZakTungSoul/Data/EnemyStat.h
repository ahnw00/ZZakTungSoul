#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "EnemyStat.generated.h"

USTRUCT(BlueprintType)
struct ZZAKTUNGSOUL_API FEnemyStat : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Hp = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Poise = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Stance = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Defense = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Resistance = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Soul = 500.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float WalkSpeed = 200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ChaseSpeed = 250.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float StrafeSpeed = 150.0f;
};