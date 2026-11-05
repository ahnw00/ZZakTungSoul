#pragma once

#include "CoreMinimal.h"
#include <Data/DamageInfo.h>
#include "EnemyAttackData.generated.h"

USTRUCT(BlueprintType)
struct FEnemyAttackData
{
	GENERATED_BODY()

public:
	// Used by the clown's combat montage selection. Both enabled preserves existing behavior.
	UPROPERTY(EditAnywhere, Category = "Boss Phase Usage", meta = (DisplayName = "Allow In Phase 1"))
	bool bAllowInPhase1 = true;

	UPROPERTY(EditAnywhere, Category = "Boss Phase Usage", meta = (DisplayName = "Allow In Phase 2"))
	bool bAllowInPhase2 = true;

	// 공격 애니메이션
	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> Montage = nullptr;

	// 공격 데이터 수치
	UPROPERTY(EditAnywhere)
	TArray<FDamageInfo> DamageInfo;
};
