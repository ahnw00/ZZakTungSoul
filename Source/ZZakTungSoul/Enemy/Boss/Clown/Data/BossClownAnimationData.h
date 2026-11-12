#pragma once

#include <CoreMinimal.h>
#include <Engine/DataAsset.h>
#include <Engine/EngineTypes.h>
#include <Enemy/Data/EnemyAttackData.h>
#include "BossClownAnimationData.generated.h"

class UGeometryCollection;
class UChaosCacheCollection;

class AActor;
class UTexture2D;
class UStaticMesh;
class AClownYabawiBowl;
class UMaterialInterface;
class UNiagaraSystem;
class USoundBase;

UENUM(BlueprintType)
enum class EBossClownAttack : uint8
{
	HammerSweep,
	HammerSlam,
	FastChaseSlam,
	DashSweep,
	TripleBalls,
	ClapWave,
	SpawnCards,
	CubeBarrage,
	Yabawi,
};

USTRUCT(BlueprintType, meta = (DisplayName = "Clown Attack Pattern"))
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

	UPROPERTY(EditAnywhere, Category = "Attack")
	FEnemyAttackData Attack;

	UPROPERTY(EditAnywhere, Category = "Attack")
	TArray<FEnemyAttackData> AttackVariants;

	UPROPERTY(EditAnywhere, Category = "Timing", meta = (ClampMin = "0"))
	float RecoveryTime = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Timing",
		meta = (EditCondition = "AttackType != EBossClownAttack::Yabawi && AttackType != EBossClownAttack::DashSweep", EditConditionHides, ClampMin = "0"))
	float TrackingTime = 0.25f;
};

USTRUCT()
struct FClownPatternEffects
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> Sound;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "Sound != nullptr", EditConditionHides, ClampMin = "0"))
	float Volume = 1.0f;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "Sound != nullptr", EditConditionHides, ClampMin = "0.1"))
	float Pitch = 1.0f;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> NiagaraSystem;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "NiagaraSystem != nullptr", EditConditionHides, ClampMin = "0.01"))
	float NiagaraScale = 1.0f;

	void Play(const UObject* WorldContext, const FVector& Location, const FRotator& Rotation) const;
	void PlaySound(const UObject* WorldContext, const FVector& Location) const;
	void PlayVisual(const UObject* WorldContext, const FVector& Location, const FRotator& Rotation) const;
};

USTRUCT()
struct FClownSoundEffect
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> Sound;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "Sound != nullptr", EditConditionHides, ClampMin = "0"))
	float Volume = 1.0f;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "Sound != nullptr", EditConditionHides, ClampMin = "0.1"))
	float Pitch = 1.0f;

	void Play(const UObject* WorldContext, const FVector& Location) const;
};

USTRUCT()
struct FClownVisualEffect
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> NiagaraSystem;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "NiagaraSystem != nullptr", EditConditionHides, ClampMin = "0.01"))
	float NiagaraScale = 1.0f;

	void Play(const UObject* WorldContext, const FVector& Location, const FRotator& Rotation) const;
};

USTRUCT()
struct FClownBallSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0"))
	float FirstLaunchTime = 1.0f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.1"))
	float LaunchInterval = 0.6f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "1"))
	float Speed = 700.0f;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraSystem> SpinEffect;

	UPROPERTY(EditAnywhere)
	FClownPatternEffects GroundImpact;

	UPROPERTY(EditAnywhere)
	FClownPatternEffects LaunchEffects;
};

USTRUCT()
struct FClownCardSettings
{
	GENERATED_BODY()

	static constexpr float MinStandTime = 1.5f;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "3", ClampMax = "10"))
	int32 Count = 8;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "Stand Time", ClampMin = "1.5", Units = "s",
		ToolTip = "카드가 다 올라온 뒤 하트 카드가 넘어지기까지 걸리는 시간. 다른 카드는 0.55초 뒤에 넘어집니다."))
	float FirstLaunchTime = MinStandTime;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.3"))
	float FallTime = 0.65f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "600", ClampMax = "900"))
	float Radius = 600.0f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "350", ClampMax = "550"))
	float Width = 550.0f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "100"))
	float Height = 650.0f;

	UPROPERTY(EditAnywhere)
	TArray<TObjectPtr<UTexture2D>> CardFaces;

	UPROPERTY(EditAnywhere)
	TArray<TObjectPtr<UTexture2D>> HeartFaces;

	UPROPERTY(EditAnywhere)
	FClownPatternEffects SpawnEffects;
};

USTRUCT()
struct FClownCubeSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditAnywhere)
	TArray<TObjectPtr<UStaticMesh>> Pieces;

	UPROPERTY(EditAnywhere)
	FClownPatternEffects AssemblyEffects;
};

USTRUCT()
struct FClownClapSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> WaveClass;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0"))
	float ImpactTime = 1.1f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "1"))
	float WaveSpeed = 900.0f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "1"))
	float WaveRange = 1100.0f;

	UPROPERTY(EditAnywhere)
	FClownPatternEffects WaveEffects;
};

USTRUCT(BlueprintType)
struct FClownYabawiSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AClownYabawiBowl> BowlClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<ECollisionChannel> AttackTraceChannel = ECC_Pawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (ClampMin = "2",
			ClampMax = "8"))
	int32 BowlCount = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMesh> BowlMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMesh> BallMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (ClampMin = "100"))
	float BowlDiameter = 180.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (ClampMin = "200"))
	float BowlSpacing = 280.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (ClampMin = "50"))
	float BowlHitHeight = 160.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (ClampMin = "0.5"))
	float RevealTime = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (ClampMin = "0.2"))
	float CoverTime = 0.6f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (ClampMin = "1",
			ClampMax = "32"))
	int32 ShuffleCount = 12;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (ClampMin = "0.12",
			ClampMax = "1"))
	float ShuffleStepTime = 0.22f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (ClampMin = "1"))
	float SelectionTime = 5.0f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "1"))
	float ResultDisplayTime = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (ClampMin = "0"))
	float Cooldown = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (ClampMin = "100"))
	float ExplosionRadius = 2500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDamageInfo ExplosionDamage;

	UPROPERTY(EditAnywhere)
	FClownSoundEffect CoverSound;

	UPROPERTY(EditAnywhere)
	FClownPatternEffects FailureEffects;
};

USTRUCT()
struct FClownTacticalMovement
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	bool bEnableTacticalActions = true;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "1"))
	float LeashRadius = 1800.0f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "1"))
	float BackstepDistance = 220.0f;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> BackstepMontage;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "1"))
	float BackjumpDistance = 650.0f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "1"))
	float BackjumpHeight = 130.0f;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> BackjumpMontage;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "1"))
	float SideEvadeDistance = 230.0f;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> EvadeLeftMontage;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> EvadeRightMontage;
};

USTRUCT()
struct FClownFastChaseSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (ClampMin = "1"))
	float Speed = 850.0f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0"))
	float StopDistance = 90.0f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.1"))
	float MaxTime = 4.0f;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> Montage;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.1"))
	float AnimationRate = 1.0f;
};

USTRUCT()
struct FClownDashSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (ClampMin = "1"))
	float Speed = 1200.0f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0"))
	float StartTime = 0.45f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.01"))
	float Duration = 0.4f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "1"))
	float MaxDistance = 480.0f;
};

USTRUCT()
struct FClownMeleeEffects
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FClownPatternEffects GroundImpact;

	UPROPERTY(EditAnywhere)
	FClownPatternEffects HeavyGroundImpact;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInterface> GroundCrackMaterial;

	UPROPERTY(EditAnywhere)
	FClownVisualEffect CeilingDust;

	UPROPERTY(EditAnywhere)
	TMap<EBossClownAttack, FClownSoundEffect> SwingSounds;
};

UCLASS(PrioritizeCategories = ("Attacks", "Special Patterns", "Movement", "Reactions", "Fracture"))
class ZZAKTUNGSOUL_API UBossClownAnimationData : public UDataAsset
{
	GENERATED_BODY()

public:
	UBossClownAnimationData();

	UPROPERTY(EditAnywhere, Category = "Attacks", meta = (DisplayName = "Phase 1 Opening Attack"))
	FEnemyAttackData OpeningAttack;

	UPROPERTY(EditAnywhere, Category = "Attacks", meta = (DisplayName = "Phase 2 Opening Attack"))
	FEnemyAttackData Phase2OpeningAttack;

	UPROPERTY(EditAnywhere, Category = "Attacks",
		meta = (DisplayName = "Attack Patterns", TitleProperty = "AttackType"))
	TArray<FClownPhaseOnePattern> PhaseOnePatterns;

	UPROPERTY(EditAnywhere, Category = "Attacks")
	FClownMeleeEffects Melee;

	UPROPERTY(EditAnywhere, Category = "Special Patterns")
	FClownBallSettings Balls;

	UPROPERTY(EditAnywhere, Category = "Special Patterns")
	FClownCardSettings Cards;

	UPROPERTY(EditAnywhere, Category = "Special Patterns")
	FClownCubeSettings Cubes;

	UPROPERTY(EditAnywhere, Category = "Special Patterns")
	FClownClapSettings Clap;

	UPROPERTY(EditAnywhere, Category = "Special Patterns")
	FClownYabawiSettings Yabawi;

	UPROPERTY(EditAnywhere, Category = "Movement", meta = (ClampMin = "0"))
	float AttackTurnSpeed = 150.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	FClownTacticalMovement TacticalMovement;

	UPROPERTY(EditAnywhere, Category = "Movement")
	FClownFastChaseSettings FastChase;

	UPROPERTY(EditAnywhere, Category = "Movement")
	FClownDashSettings Dash;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reactions")
	TObjectPtr<UAnimMontage> GroggyMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reactions", meta = (ClampMin = "1"))
	float GroggyDuration = 4.5f;

	UPROPERTY(EditAnywhere, Category = "Reactions")
	TObjectPtr<UAnimMontage> FrontCriticalReaction;

	UPROPERTY(EditAnywhere, Category = "Reactions")
	TObjectPtr<UAnimMontage> BackCriticalReaction;

	UPROPERTY(EditAnywhere, Category = "Reactions")
	TObjectPtr<UAnimMontage> DeathReaction;

	UPROPERTY(EditAnywhere, Category = "Fracture")
	TObjectPtr<UChaosCacheCollection> FractureCaches;

	UPROPERTY(EditAnywhere, Category = "Fracture")
	TMap<FName, TObjectPtr<UGeometryCollection>> Fractures;
};
