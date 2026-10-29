#pragma once

#include "CoreMinimal.h"
#include "DamageInfo.generated.h"

USTRUCT(BlueprintType)
struct FDamageInfo
{
	GENERATED_BODY()

public:
	// 전달할 데미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackDamage = 0.0f;

	// 경직 피해 수치
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PoiseDamage = 0.0f;

	// 자세 무너짐(그로기) 피해 수치
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StanceDamage = 0.0f;

	// 가드 무시 여부 플래그
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint32 bUnblockable : 1 = false;

	// 무적 무시 여부 플래그
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint32 bIgnoreInvincibility : 1 = false;
};