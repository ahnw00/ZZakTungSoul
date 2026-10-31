#pragma once

#include <CoreMinimal.h>
#include <Engine/DataAsset.h>
#include <Enemy/Data/EnemyAttackData.h>
#include "BossClownAnimationData.generated.h"

UENUM(BlueprintType)
enum class EBossClownAttack : uint8
{
	HammerSweep, HammerSlam
};

UCLASS()
class ZZAKTUNGSOUL_API UBossClownAnimationData : public UDataAsset
{
	GENERATED_BODY()

public:
	UBossClownAnimationData();

	//---------------------------------------------------------------
	// 첫 공격과 광대 전용 공격 데이터.
	//
	//---------------------------------------------------------------
	UPROPERTY(EditAnywhere, Category = "Montage|Clown|Opening")
	FEnemyAttackData OpeningAttack;

	UPROPERTY(EditAnywhere, Category = "Montage|Clown|Attack")
	FEnemyAttackData HammerSweep;

	UPROPERTY(EditAnywhere, Category = "Montage|Clown|Attack")
	FEnemyAttackData HammerSlam;
};
