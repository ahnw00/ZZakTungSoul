#pragma once

#include <CoreMinimal.h>
#include <Engine/DataAsset.h>
#include <Enemy/Data/EnemyAttackData.h>
#include "BossClownAnimationData.generated.h"

class UMaterialInterface;

UENUM(BlueprintType)
enum class EBossClownAttack : uint8
{
	HammerSweep, HammerSlam, FastChaseSlam, DashSweep, TripleBalls, ClapWave
};

USTRUCT(BlueprintType)
struct FClownPhaseOnePattern
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EBossClownAttack AttackType = EBossClownAttack::HammerSweep;

	UPROPERTY(EditAnywhere)
	bool bEnabled = true;

	UPROPERTY(EditAnywhere)
	FEnemyAttackData Attack;

	// 캡슐 표면 사이의 수평 거리. 보스 크기가 바뀌어도 동일한 기준 사용.
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0"))
	float MinDistance = 0.0f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0"))
	float MaxDistance = 180.0f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0"))
	float Weight = 1.0f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0"))
	float Cooldown = 3.0f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0"))
	float RecoveryTime = 1.0f;

	// 몽타주 시작 후 이 시간이 지나면 타깃 추적 회전 종료.
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0"))
	float TrackingTime = 0.25f;
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

	//---------------------------------------------------------------
	// 광대 1페이즈 패턴 선택과 이동 설정.
	// 기존 첫 공격과 일반 적 데이터는 변경하지 않음.
	//
	//---------------------------------------------------------------
	UPROPERTY(EditAnywhere, Category = "Clown|Phase1")
	bool bUseNativePhaseOne = false;

	UPROPERTY(EditAnywhere, Category = "Clown|Phase1")
	TArray<FClownPhaseOnePattern> PhaseOnePatterns;

	UPROPERTY(EditAnywhere, Category = "Clown|Phase1|Movement", meta = (ClampMin = "0"))
	float MovementAttackCooldown = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Clown|Phase1|Movement", meta = (ClampMin = "1"))
	float FastChaseSpeed = 850.0f;

	UPROPERTY(EditAnywhere, Category = "Clown|Phase1|Movement", meta = (ClampMin = "0"))
	float FastChaseStopDistance = 90.0f;

	UPROPERTY(EditAnywhere, Category = "Clown|Phase1|Movement", meta = (ClampMin = "0.1"))
	float FastChaseMaxTime = 4.0f;

	UPROPERTY(EditAnywhere, Category = "Clown|Phase1|Movement", meta = (ClampMin = "1"))
	float DashSpeed = 1200.0f;

	UPROPERTY(EditAnywhere, Category = "Clown|Phase1|Movement", meta = (ClampMin = "0"))
	float DashStartTime = 0.45f;

	UPROPERTY(EditAnywhere, Category = "Clown|Phase1|Movement", meta = (ClampMin = "0.01"))
	float DashDuration = 0.4f;

	UPROPERTY(EditAnywhere, Category = "Clown|Phase1|Movement", meta = (ClampMin = "1"))
	float DashMaxDistance = 480.0f;

	UPROPERTY(EditAnywhere, Category = "Clown|Phase1|Movement", meta = (ClampMin = "0"))
	float AttackTurnSpeed = 150.0f;

	//---------------------------------------------------------------
	// 빠른 추격 달리기와 지휘 / 박수 패턴 설정.
	//
	//---------------------------------------------------------------
	UPROPERTY(EditAnywhere, Category = "Clown|Phase1|Movement")
	TObjectPtr<UAnimMontage> FastChaseMontage;

	UPROPERTY(EditAnywhere, Category = "Clown|Phase1|Movement", meta = (ClampMin = "0.1"))
	float FastChaseAnimationRate = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Clown|Phase1|Balls", meta = (ClampMin = "0"))
	float BallFirstLaunchTime = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Clown|Phase1|Balls", meta = (ClampMin = "0.1"))
	float BallLaunchInterval = 0.6f;

	UPROPERTY(EditAnywhere, Category = "Clown|Phase1|Balls", meta = (ClampMin = "1"))
	float BallSpeed = 700.0f;

	UPROPERTY(EditAnywhere, Category = "Clown|Phase1|Balls")
	TObjectPtr<UMaterialInterface> BallMaterial;

	UPROPERTY(EditAnywhere, Category = "Clown|Phase1|Clap", meta = (ClampMin = "0"))
	float ClapImpactTime = 1.1f;

	UPROPERTY(EditAnywhere, Category = "Clown|Phase1|Clap", meta = (ClampMin = "1"))
	float ClapWaveSpeed = 900.0f;

	UPROPERTY(EditAnywhere, Category = "Clown|Phase1|Clap", meta = (ClampMin = "1"))
	float ClapWaveRange = 1100.0f;

	UPROPERTY(EditAnywhere, Category = "Clown|Phase1|Clap")
	TObjectPtr<UMaterialInterface> ClapWaveMaterial;

	UFUNCTION(CallInEditor, Category = "Clown|Setup")
	void BuildAdditionalPatterns();

	//---------------------------------------------------------------
	// 원본 애니메이션에서 1페이즈 몽타주와 판정 Notify 생성.
	// 에디터에서만 실행. 이미 생성된 몽타주는 덮어쓰지 않음.
	//
	//---------------------------------------------------------------
	UFUNCTION(CallInEditor, Category = "Clown|Setup")
	void BuildPhaseOneMontages();
};
