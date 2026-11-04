#pragma once

#include <CoreMinimal.h>
#include <Engine/DataAsset.h>
#include <Enemy/Data/EnemyAttackData.h>
#include "BossClownAnimationData.generated.h"

class AActor;
class UTexture2D;
class UStaticMesh;

UENUM(BlueprintType)
enum class EBossClownAttack : uint8
{
	// Phase 1
	HammerSweep, 
	HammerSlam, 
	FastChaseSlam, 
	DashSweep, 
	TripleBalls, 
	ClapWave, 
	SpawnCards,
	CubeBarrage,

	// Phase 2
	Yabawi,

};

UENUM(BlueprintType)
enum class EClownAttackStyle : uint8
{
	Normal, Immediate, DelayedPursuit UMETA(Hidden)
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

	// 같은 거리 / 행동 역할 안에서 직전 몽타주와 다른 동작 선택.
	UPROPERTY(EditAnywhere)
	TArray<FEnemyAttackData> AttackVariants;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0"))
	float RecoveryTime = 1.0f;

	// 타격 전 준비 자세 유지 시간. 판정 Notify는 정지한 동안 실행하지 않음.
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0", ClampMax = "2"))
	float AnticipationTime = 0.55f;

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

	//---------------------------------------------------------------
	// 행동 분기 / 거리 / 확률 / 쿨다운은 BT에서 설정.
	//
	//---------------------------------------------------------------

	UPROPERTY(EditAnywhere, Category = "Clown|Phase1|Behavior")
	bool bEnableTacticalActions = true;

	UPROPERTY(EditAnywhere, Category = "Clown|Phase1|Behavior", meta = (ClampMin = "1"))
	float BackstepDistance = 220.0f;

	UPROPERTY(EditAnywhere, Category = "Clown|Phase1|Behavior", meta = (ClampMin = "1"))
	float BackjumpDistance = 650.0f;

	UPROPERTY(EditAnywhere, Category = "Clown|Phase1|Behavior", meta = (ClampMin = "1"))
	float BackjumpHeight = 130.0f;

	UPROPERTY(EditAnywhere, Category = "Clown|Phase1|Behavior", meta = (ClampMin = "1"))
	float SideEvadeDistance = 230.0f;

	UPROPERTY(EditAnywhere, Category = "Clown|Phase1|Behavior", meta = (ClampMin = "1"))
	float TacticalLeashRadius = 1800.0f;

	UPROPERTY(EditAnywhere, Category = "Clown|Phase1|Behavior")
	TObjectPtr<UAnimMontage> BackstepMontage;

	UPROPERTY(EditAnywhere, Category = "Clown|Phase1|Behavior")
	TObjectPtr<UAnimMontage> BackjumpMontage;

	UPROPERTY(EditAnywhere, Category = "Clown|Phase1|Behavior")
	TObjectPtr<UAnimMontage> EvadeLeftMontage;

	UPROPERTY(EditAnywhere, Category = "Clown|Phase1|Behavior")
	TObjectPtr<UAnimMontage> EvadeRightMontage;


	UPROPERTY(EditAnywhere, Category = "Clown|Phase1|Balls", meta = (ClampMin = "1", ClampMax = "16"))
	int32 BallCount = 5;

	UPROPERTY(EditAnywhere, Category = "Clown|Phase1")
	TArray<FClownPhaseOnePattern> PhaseOnePatterns;

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
	// 빠른 추격 달리기와 지휘 박수 패턴 설정.
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
	TSubclassOf<AActor> BallClass;

	UPROPERTY(EditAnywhere, Category = "Clown|Phase1|Cube")
	TSubclassOf<AActor> CubeClass;

	// 원본 큐브를 실제 3 x 3 x 3으로 절단한 조각. Z / Y / X 순서.
	UPROPERTY(EditAnywhere, Category = "Clown|Phase1|Cube")
	TArray<TObjectPtr<UStaticMesh>> CubePieces;

	UPROPERTY(EditAnywhere, Category = "Clown|Phase1|Clap", meta = (ClampMin = "0"))
	float ClapImpactTime = 1.1f;

	UPROPERTY(EditAnywhere, Category = "Clown|Phase1|Clap", meta = (ClampMin = "1"))
	float ClapWaveSpeed = 900.0f;

	UPROPERTY(EditAnywhere, Category = "Clown|Phase1|Clap", meta = (ClampMin = "1"))
	float ClapWaveRange = 1100.0f;

	UPROPERTY(EditAnywhere, Category = "Clown|Phase1|Clap")
	TSubclassOf<AActor> ClapWaveClass;

	UPROPERTY(EditAnywhere, Category = "Clown|Phase1|Cards", meta = (ClampMin = "1", ClampMax = "16"))
	int32 CardCount = 8;

	UPROPERTY(EditAnywhere, Category = "Clown|Phase1|Cards", meta = (ClampMin = "0"))
	float CardFirstLaunchTime = 1.2f;

	UPROPERTY(EditAnywhere, Category = "Clown|Phase1|Cards")
	TSubclassOf<AActor> CardClass;

	UPROPERTY(EditAnywhere, Category = "Clown|Phase1|Cards", meta = (ClampMin = "600", ClampMax = "900"))
	float CardRadius = 600.0f;

	UPROPERTY(EditAnywhere, Category = "Clown|Phase1|Cards", meta = (ClampMin = "100"))
	float CardHeight = 650.0f;

	UPROPERTY(EditAnywhere, Category = "Clown|Phase1|Cards", meta = (ClampMin = "0.1"))
	float CardActiveTime = 0.7f;

	UPROPERTY(EditAnywhere, Category = "Clown|Phase1|Cards", meta = (ClampMin = "350", ClampMax = "550"))
	float CardWidth = 550.0f;

	UPROPERTY(EditAnywhere, Category = "Clown|Phase1|Cards", meta = (ClampMin = "0.3"))
	float CardFallTime = 0.65f;

	UPROPERTY(EditAnywhere, Category = "Clown|Phase1|Cards")
	TArray<TObjectPtr<UTexture2D>> CardFaces;

	UPROPERTY(EditAnywhere, Category = "Clown|Phase1|Cards")
	TArray<TObjectPtr<UTexture2D>> HeartFaces;

};
