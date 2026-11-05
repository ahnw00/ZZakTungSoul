#pragma once

#include <CoreMinimal.h>
#include <Engine/DataAsset.h>
#include <Engine/EngineTypes.h>
#include <Enemy/Data/EnemyAttackData.h>
#include "BossClownAnimationData.generated.h"

class AActor;
class UTexture2D;
class UStaticMesh;
class UMaterialInterface;
class AClownYabawiBowl;

UENUM(BlueprintType)
enum class EBossClownAttack : uint8
{
	// Attack behavior identifiers. Montage phase usage is configured per attack entry.
	HammerSweep,
	HammerSlam,
	FastChaseSlam,
	DashSweep,
	TripleBalls,
	ClapWave,
	SpawnCards,
	CubeBarrage,

	// Uses the same Attack / AttackVariants montage and phase settings as other attacks.
	Yabawi,
};

UENUM(BlueprintType)
enum class EClownAttackStyle : uint8
{
	Normal,
	Immediate,
	DelayedPursuit UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FClownPhaseOnePattern
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EBossClownAttack AttackType = EBossClownAttack::HammerSweep;

	UPROPERTY(EditAnywhere)
	bool bEnabled = true;

	UPROPERTY(EditAnywhere, Category = "Boss Phase Usage", meta = (DisplayName = "Allow In Phase 1"))
	bool bAllowInPhase1 = true;

	UPROPERTY(EditAnywhere, Category = "Boss Phase Usage", meta = (DisplayName = "Allow In Phase 2"))
	bool bAllowInPhase2 = true;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (EditCondition = "AttackType == EBossClownAttack::Yabawi", EditConditionHides))
	TSubclassOf<AClownYabawiBowl> BowlClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (EditCondition = "AttackType == EBossClownAttack::Yabawi", EditConditionHides))
	TEnumAsByte<ECollisionChannel> AttackTraceChannel = ECC_Pawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (EditCondition = "AttackType == EBossClownAttack::Yabawi", EditConditionHides, ClampMin = "2",
			ClampMax = "8"))
	int32 BowlCount = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (EditCondition = "AttackType == EBossClownAttack::Yabawi", EditConditionHides))
	TObjectPtr<UStaticMesh> BowlMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (EditCondition = "AttackType == EBossClownAttack::Yabawi", EditConditionHides))
	TObjectPtr<UStaticMesh> BallMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (EditCondition = "AttackType == EBossClownAttack::Yabawi", EditConditionHides, ClampMin = "100"))
	float BowlDiameter = 180.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (EditCondition = "AttackType == EBossClownAttack::Yabawi", EditConditionHides, ClampMin = "200"))
	float BowlSpacing = 280.0f;

	// The query box reaches above the bowl so ordinary weapon swings can hit it.
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (EditCondition = "AttackType == EBossClownAttack::Yabawi", EditConditionHides, ClampMin = "50"))
	float BowlHitHeight = 160.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (EditCondition = "AttackType == EBossClownAttack::Yabawi", EditConditionHides, ClampMin = "0.5"))
	float RevealTime = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (EditCondition = "AttackType == EBossClownAttack::Yabawi", EditConditionHides, ClampMin = "0.2"))
	float CoverTime = 0.6f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (EditCondition = "AttackType == EBossClownAttack::Yabawi", EditConditionHides, ClampMin = "1",
			ClampMax = "32"))
	int32 ShuffleCount = 12;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (EditCondition = "AttackType == EBossClownAttack::Yabawi", EditConditionHides, ClampMin = "0.12",
			ClampMax = "1"))
	float ShuffleStepTime = 0.22f;

	// 섞기를 마친 순간부터 선택 시간을 계산.
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (EditCondition = "AttackType == EBossClownAttack::Yabawi", EditConditionHides, ClampMin = "1"))
	float SelectionTime = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (EditCondition = "AttackType == EBossClownAttack::Yabawi", EditConditionHides, ClampMin = "0"))
	float Cooldown = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (EditCondition = "AttackType == EBossClownAttack::Yabawi", EditConditionHides, ClampMin = "100"))
	float ExplosionRadius = 2500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (EditCondition = "AttackType == EBossClownAttack::Yabawi", EditConditionHides))
	FDamageInfo ExplosionDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (EditCondition = "AttackType == EBossClownAttack::Yabawi", EditConditionHides))
	TObjectPtr<UMaterialInterface> ExplosionMaterial;
};

UCLASS()
class ZZAKTUNGSOUL_API UBossClownAnimationData : public UDataAsset
{
	GENERATED_BODY()

public:
	UBossClownAnimationData();
	virtual void PostLoad() override;

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

	UPROPERTY(EditAnywhere, Category = "Clown|Behavior")
	bool bEnableTacticalActions = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clown|Behavior", meta = (ClampMin = "1"))
	float GroggyDuration = 3.0f;

	UPROPERTY(EditAnywhere, Category = "Clown|Behavior", meta = (ClampMin = "1"))
	float BackstepDistance = 220.0f;

	UPROPERTY(EditAnywhere, Category = "Clown|Behavior", meta = (ClampMin = "1"))
	float BackjumpDistance = 650.0f;

	UPROPERTY(EditAnywhere, Category = "Clown|Behavior", meta = (ClampMin = "1"))
	float BackjumpHeight = 130.0f;

	UPROPERTY(EditAnywhere, Category = "Clown|Behavior", meta = (ClampMin = "1"))
	float SideEvadeDistance = 230.0f;

	UPROPERTY(EditAnywhere, Category = "Clown|Behavior", meta = (ClampMin = "1"))
	float TacticalLeashRadius = 1800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clown|Behavior")
	TObjectPtr<UAnimMontage> GroggyMontage;

	UPROPERTY(EditAnywhere, Category = "Clown|Behavior")
	TObjectPtr<UAnimMontage> BackstepMontage;

	UPROPERTY(EditAnywhere, Category = "Clown|Behavior")
	TObjectPtr<UAnimMontage> BackjumpMontage;

	UPROPERTY(EditAnywhere, Category = "Clown|Behavior")
	TObjectPtr<UAnimMontage> EvadeLeftMontage;

	UPROPERTY(EditAnywhere, Category = "Clown|Behavior")
	TObjectPtr<UAnimMontage> EvadeRightMontage;

	UPROPERTY(EditAnywhere, Category = "Clown|Balls", meta = (ClampMin = "1", ClampMax = "16"))
	int32 BallCount = 5;

	UPROPERTY(EditAnywhere, Category = "Clown|Attacks",
		meta = (DisplayName = "Attack Patterns", TitleProperty = "AttackType"))
	TArray<FClownPhaseOnePattern> PhaseOnePatterns;

	UPROPERTY(EditAnywhere, Category = "Clown|Movement", meta = (ClampMin = "1"))
	float FastChaseSpeed = 850.0f;

	UPROPERTY(EditAnywhere, Category = "Clown|Movement", meta = (ClampMin = "0"))
	float FastChaseStopDistance = 90.0f;

	UPROPERTY(EditAnywhere, Category = "Clown|Movement", meta = (ClampMin = "0.1"))
	float FastChaseMaxTime = 4.0f;

	UPROPERTY(EditAnywhere, Category = "Clown|Movement", meta = (ClampMin = "1"))
	float DashSpeed = 1200.0f;

	UPROPERTY(EditAnywhere, Category = "Clown|Movement", meta = (ClampMin = "0"))
	float DashStartTime = 0.45f;

	UPROPERTY(EditAnywhere, Category = "Clown|Movement", meta = (ClampMin = "0.01"))
	float DashDuration = 0.4f;

	UPROPERTY(EditAnywhere, Category = "Clown|Movement", meta = (ClampMin = "1"))
	float DashMaxDistance = 480.0f;

	UPROPERTY(EditAnywhere, Category = "Clown|Movement", meta = (ClampMin = "0"))
	float AttackTurnSpeed = 150.0f;

	//---------------------------------------------------------------
	// 빠른 추격 달리기와 지휘 박수 패턴 설정.
	//
	//---------------------------------------------------------------
	UPROPERTY(EditAnywhere, Category = "Clown|Movement")
	TObjectPtr<UAnimMontage> FastChaseMontage;

	UPROPERTY(EditAnywhere, Category = "Clown|Movement", meta = (ClampMin = "0.1"))
	float FastChaseAnimationRate = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Clown|Balls", meta = (ClampMin = "0"))
	float BallFirstLaunchTime = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Clown|Balls", meta = (ClampMin = "0.1"))
	float BallLaunchInterval = 0.6f;

	UPROPERTY(EditAnywhere, Category = "Clown|Balls", meta = (ClampMin = "1"))
	float BallSpeed = 700.0f;

	UPROPERTY(EditAnywhere, Category = "Clown|Balls")
	TSubclassOf<AActor> BallClass;

	UPROPERTY(EditAnywhere, Category = "Clown|Cube")
	TSubclassOf<AActor> CubeClass;

	// 원본 큐브를 실제 3 x 3 x 3으로 절단한 조각. Z / Y / X 순서.
	UPROPERTY(EditAnywhere, Category = "Clown|Cube")
	TArray<TObjectPtr<UStaticMesh>> CubePieces;

	UPROPERTY(EditAnywhere, Category = "Clown|Clap", meta = (ClampMin = "0"))
	float ClapImpactTime = 1.1f;

	UPROPERTY(EditAnywhere, Category = "Clown|Clap", meta = (ClampMin = "1"))
	float ClapWaveSpeed = 900.0f;

	UPROPERTY(EditAnywhere, Category = "Clown|Clap", meta = (ClampMin = "1"))
	float ClapWaveRange = 1100.0f;

	UPROPERTY(EditAnywhere, Category = "Clown|Clap")
	TSubclassOf<AActor> ClapWaveClass;

	UPROPERTY(EditAnywhere, Category = "Clown|Cards", meta = (ClampMin = "1", ClampMax = "16"))
	int32 CardCount = 8;

	UPROPERTY(EditAnywhere, Category = "Clown|Cards", meta = (ClampMin = "0"))
	float CardFirstLaunchTime = 1.2f;

	UPROPERTY(EditAnywhere, Category = "Clown|Cards")
	TSubclassOf<AActor> CardClass;

	UPROPERTY(EditAnywhere, Category = "Clown|Cards", meta = (ClampMin = "600", ClampMax = "900"))
	float CardRadius = 600.0f;

	UPROPERTY(EditAnywhere, Category = "Clown|Cards", meta = (ClampMin = "100"))
	float CardHeight = 650.0f;

	UPROPERTY(EditAnywhere, Category = "Clown|Cards", meta = (ClampMin = "0.1"))
	float CardActiveTime = 0.7f;

	UPROPERTY(EditAnywhere, Category = "Clown|Cards", meta = (ClampMin = "350", ClampMax = "550"))
	float CardWidth = 550.0f;

	UPROPERTY(EditAnywhere, Category = "Clown|Cards", meta = (ClampMin = "0.3"))
	float CardFallTime = 0.65f;

	UPROPERTY(EditAnywhere, Category = "Clown|Cards")
	TArray<TObjectPtr<UTexture2D>> CardFaces;

	UPROPERTY(EditAnywhere, Category = "Clown|Cards")
	TArray<TObjectPtr<UTexture2D>> HeartFaces;
};
