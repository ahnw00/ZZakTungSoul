#pragma once

#include "CoreMinimal.h"
#include <Data/DamageInfo.h>
#include "EnemyAttackData.generated.h"

USTRUCT(BlueprintType)
struct FEnemyAttackData
{
	GENERATED_BODY()

public:
	// 공격 애니메이션
	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> Montage = nullptr;

	// 공격 데이터 수치
	UPROPERTY(EditAnywhere)
	TArray<FDamageInfo> DamageInfo;
};