#pragma once

#include <CoreMinimal.h>
#include <Enemy/Boss/Boss.h>
#include <Enemy/Boss/Clown/Data/BossClownAnimationData.h>
#include <TimerManager.h>
#include "BossClown.generated.h"

class UAnimInstance;
class UAnimMontage;
class UAnimSequence;
class AChaosCacheManager;
class UGeometryCollectionComponent;
class APointLight;
class UNiagaraComponent;
class UNiagaraSystem;
class UPointLightComponent;
class UAnimSequenceBase;
class UClownProjectileComponent;
class UClownYabawiComponent;
class UPrimitiveComponent;
class UProjectileMovementComponent;
class AStaticMeshActor;
struct FCollisionQueryParams;

enum class EBossClownAttackResult : uint8
{
	Running,
	Succeeded,
	Failed,
	Aborted
};

UCLASS()
class ZZAKTUNGSOUL_API ABossClown : public ABoss
{
	GENERATED_BODY()

public:
	ABossClown();
	bool CanStartClownYabawi() const;
	void EnterYabawiGroggy();
	void CompleteClownYabawi(bool bCorrect);
	void FinishClownAttack(EBossClownAttackResult Result, bool bStopMontage, bool bRestoreState = true);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Clown|Phase2")
	TObjectPtr<UClownYabawiComponent> YabawiComponent;

	UPROPERTY(EditInstanceOnly, Category = "Clown|Arena")
	TObjectPtr<AActor> CubeArena;
	virtual void Tick(float DeltaTime) override;

	const UBossClownAnimationData* GetClownAnimationData() const;
	virtual const FEnemyAttackData* GetOpeningAttackData() const override;
	// 광대 전용 공격 실행과 중단.
	bool StartClownAttack(EBossClownAttack AttackType, AActor* Target, int32& OutExecutionID, float RecoveryScale = 1.0f);
	void CancelClownAttack(int32 ExecutionID);
	EBossClownAttackResult GetClownAttackResult(int32 ExecutionID) const;
	// 광대 전용 Notify에서 현재 공격의 타격 데미지 설정.
	void SetClownAttackDamage(int32 DamageIndex, UAnimSequenceBase* SourceAnimation);

	const FClownPhaseOnePattern* GetAttackPattern(EBossClownAttack AttackType) const;
	const FEnemyAttackData* GetClownAttackData(EBossClownAttack Type) const;
	bool EvaluateClownCondition(AActor* Target, bool bAttack, EBossClownAttack Type, float MinDistance, float MaxDistance, float Chance, float FacingDot) const;
	void PreferClownRangedAttack();
	bool FitClownCubePath(FVector& Start, FVector& End) const;
	virtual AActor* GetEncounterArena() const override
	{
		return CubeArena.Get();
	}

	virtual void SetEncounterArena(AActor* Arena) override
	{
		CubeArena = Arena;
	}

	void RefreshClownMovement();
	bool IsClownCombatPhase() const;
	bool CanKeepClownProjectiles() const;
	void FaceClownTarget(AActor* Target, float DeltaTime);

protected:
	virtual void BeginPlay() override;
	virtual void HandleAttackTraceHit(const FHitResult& Hit) override;
	virtual void ResetBossGimmicks() override;
	// 일반 적 데이터와 분리한 광대 전용 애니메이션 데이터.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Clown|Data")
	TObjectPtr<UBossClownAnimationData> ClownAnimationData;

	UPROPERTY(EditDefaultsOnly, Category = "Clown|Attack", meta = (ClampMin = "0.1"))
	float AttackTimeout = 15.0f;

private:
	UPROPERTY(EditInstanceOnly, Category = "Clown|Phase2 Transition")
	TObjectPtr<UAnimSequence> Phase2DisappearAnimation;

	UPROPERTY(EditInstanceOnly, Category = "Clown|Phase2 Transition")
	TObjectPtr<UAnimSequence> Phase2EmergenceAnimation;

	UPROPERTY(EditInstanceOnly, Category = "Clown|Phase2 Transition")
	TObjectPtr<UNiagaraSystem> Phase2DisappearEffect;

	UPROPERTY(EditInstanceOnly, Category = "Clown|Phase2 Transition", meta = (ClampMin = "0.01"))
	float Phase2DisappearEffectScale = 1.0f;

	UPROPERTY(EditInstanceOnly, Category = "Clown|Phase2 Transition")
	TObjectPtr<AChaosCacheManager> Phase2WallCache;

	// Time in the emergence animation to start the wall cache, including its lead-in.
	UPROPERTY(EditInstanceOnly, Category = "Clown|Phase2 Transition", meta = (ClampMin = "0.0"))
	float Phase2WallPlaybackTime = 0.0f;

	// Animation time of the final punch. Zero uses the first movement from the wall cache.
	UPROPERTY(EditInstanceOnly, Category = "Clown|Phase2 Transition", meta = (ClampMin = "0.0"))
	float Phase2WallImpactTime = 0.0f;

	UPROPERTY(EditInstanceOnly, Category = "Clown|Phase2 Transition")
	TObjectPtr<AActor> Phase2CombatPoint;

	UPROPERTY(EditInstanceOnly, Category = "Clown|Phase2 Transition")
	TObjectPtr<UStaticMesh> Phase2WallChipMesh;

	UPROPERTY(EditInstanceOnly, Category = "Clown|Phase2 Transition")
	TObjectPtr<AActor> Phase2WallCrack;

	UPROPERTY(EditInstanceOnly, Category = "Clown|Phase2 Transition")
	TObjectPtr<AActor> Phase2WallOpening;

	UPROPERTY(EditInstanceOnly, Category = "Clown|Phase2 Transition")
	TObjectPtr<AActor> Phase2BrokenWall;

	UPROPERTY(EditInstanceOnly, Category = "Clown|Phase2 Transition")
	bool bShowPhase2ChaosFracture = false;

	UPROPERTY(EditInstanceOnly, Category = "Clown|Phase2 Transition")
	TObjectPtr<AActor> Phase2WallExitBlocker;

	UPROPERTY(VisibleAnywhere, Category = "Clown|Phase2 Transition")
	TObjectPtr<UStaticMeshComponent> Phase2WallFog;

	// Animation times of the two smaller hits before the wall opens.
	UPROPERTY(EditInstanceOnly, Category = "Clown|Phase2 Transition")
	FVector2D Phase2WallChipTimes = FVector2D::ZeroVector;

	// Animation time at which the clown moves through the broken wall.
	UPROPERTY(EditInstanceOnly, Category = "Clown|Phase2 Transition", meta = (ClampMin = "0.0"))
	float Phase2AdvanceStartTime = 0.0f;

	UPROPERTY(EditInstanceOnly, Category = "Clown|Phase2 Transition")
	TObjectPtr<APointLight> Phase2SearchLight;

	UPROPERTY(VisibleAnywhere, Category = "Clown|Phase2 Transition")
	TObjectPtr<UPointLightComponent> Phase2RevealLight;

	UFUNCTION()
	void PlayPhase2Stage(ABoss* Boss, EBossPhase2TransitionStage Stage, int32 TransitionID);

	UFUNCTION()
	void StopPhase2Presentation(ABoss* Boss, int32 TransitionID, bool bCancelled);

	TWeakObjectPtr<UAnimMontage> Phase2StageMontage;
	TWeakObjectPtr<UNiagaraComponent> Phase2Smoke;
	TOptional<bool> Phase2WeaponVisibility;
	TOptional<FVector> Phase2AdvanceOrigin;
	TOptional<float> Phase2AdvanceStartedAt;
	TWeakObjectPtr<UGeometryCollectionComponent> Phase2WallComponent;
	TArray<FTransform3f> Phase2IntactWallTransforms;
	TOptional<ECollisionEnabled::Type> Phase2WallCollision;
	FTimerHandle Phase2WallDebrisTimer;
	float Phase2RevealElapsed = 0.0f;
	bool bPhase2WallPlayed = false;
	bool bPhase2WallMoved = false;
	bool bPhase2WallBreakShown = false;
	int32 NextPhase2WallChip = 0;
	TArray<TWeakObjectPtr<AActor>> Phase2WallChips;
	TArray<TWeakObjectPtr<UProjectileMovementComponent>> FallingPhase2WallBricks;

	struct FPhase2DebrisFade
	{
		int32 Bone;
		FVector FloorPivot;
		float Delay;
		float Duration;
	};
	TArray<FPhase2DebrisFade> Phase2FadingDebris;
	TArray<FTransform> Phase2WallFadePose;
	TOptional<float> Phase2DebrisFadeElapsed;

	struct FPhase2WallBrickFade
	{
		TWeakObjectPtr<AStaticMeshActor> Brick;
		FTransform RestPose;
		FVector FloorPivot = FVector::ZeroVector;
		float Delay = 0.0f;
		float Duration = 0.0f;
	};
	TArray<FPhase2WallBrickFade> Phase2LargeWallBricks;

	struct FPhase2BrickRoll
	{
		TWeakObjectPtr<AStaticMeshActor> Brick;
		FVector Start;
		FVector Step;
		FQuat StartRotation;
		FQuat RestRotation;
		double FloorZ = 0.0;
		float Elapsed = 0.0f;
	};
	TArray<FPhase2BrickRoll> RollingPhase2WallBricks;

	void TickPhase2Emergence();
	void SpawnPhase2WallChips(int32 HitIndex);
	void SettlePhase2WallBricks(float DeltaTime);
	void ClearPhase2WallChips();
	UGeometryCollectionComponent* FindPhase2Wall() const;
	void ShowPhase2WallOpening(AActor* Opening);
	void ShowPhase2WallBreak();
	void BeginPhase2WallDebrisFade();
	void PreparePhase2ChaosDebrisFade();
	void TickPhase2WallDebrisFade(float DeltaTime);
	void ShowPhase2WallFog(const FVector& BreachLocation);
	void RestorePhase2Wall();
	void CheckPhase2WallBreak();
	void TickClownAttack(float DeltaTime);
	void LaunchClapWave(const UBossClownAnimationData& Data, const FDamageInfo& Damage, float Range, float Speed);
	bool PlayCurrentClownMontage();
	void BeginClownApproach();
	void BeginClownRecovery();
	void StopFastChaseAnimation();
	void RestoreClownMovement();

	void OnClownAttackEnded(UAnimMontage* Montage, bool bInterrupted, int32 ExecutionID);
	void OnClownAttackTimeout();

	// BT 분기와 실제 공격 시작이 같은 패턴 조건을 사용한다.
	void CollectClownAttackChoices(EBossClownAttack Type, TArray<const FEnemyAttackData*>& Choices) const;
	bool CanStartClownAttack(EBossClownAttack Type, AActor* Target) const;
	float GetClownPatternSelectionScale(EBossClownAttack Type) const;
	bool ShouldPrioritizeClownCube() const;
	bool PrefersClownRangedAttack() const;
	void RecordClownAttackStarted(EBossClownAttack Type);
	void RecordClownAttackFinished(EBossClownAttack Type);
	void ResetClownAttackSelection();

	// 특수 패턴 배치와 생성물 정리.
	static bool UsesSpecialPresentation(EBossClownAttack Type);
	static bool UsesLoopingMontage(EBossClownAttack Type);
	bool PrepareClownPattern();
	bool TickClownSpecial(float DeltaTime, float MontageTime);
	void FindEncounterFloor();
	bool IsEncounterFloorHit(const FHitResult& Hit) const;
	bool FindClownPatternFloor(const FVector& Position, const FCollisionQueryParams& Query, FVector& Ground) const;
	bool FindClownPatternGround(const FVector& Position, const FVector& Center, float Width, float Height, FVector& Ground) const;
	bool PrepareClownBalls();
	struct FClownCardLayout
	{
		FVector Center = FVector::ZeroVector;
		TArray<FVector> Positions;
		float Width = 0.0f;
		float Height = 0.0f;
	};
	bool FindClownCardLayout(FClownCardLayout& Layout) const;
	bool SpawnClownCards(const FClownCardLayout& Layout);
	bool PrepareClownCubeMarch();
	bool PrepareClownYabawi();
	UClownProjectileComponent* CreateClownProjectile(TSubclassOf<AActor> Class, const FVector& Location, const FRotator& Rotation, float Speed, float LifeTime, const FDamageInfo* DamageOverride = nullptr);
	void ClearClownProjectiles(bool bIncludeLaunched);
	bool HasFinishedLaunchingBalls() const { return BallsLaunched >= BallCount; }

private:
	UPROPERTY(Transient)
	FEnemyAttackData CurrentClownAttack;

	TWeakObjectPtr<UAnimMontage> ChaseMontage;
	bool bWeaponWasVisible = false;
	bool bHidWeapon = false;

	TWeakObjectPtr<UAnimInstance> AttackAnimInstance;
	TWeakObjectPtr<UAnimMontage> LastPlayedAttackMontage;
	TWeakObjectPtr<UAnimMontage> ActiveGroggyMontage;
	float YabawiGroggyDuration = 0.0f;
	FTimerHandle AttackTimeoutHandle;

	// 현재 광대 공격의 타격 데미지 인덱스.
	int32 CurrentDamageIndex = INDEX_NONE;

	int32 AttackSerial = 0;
	int32 ActiveAttackID = INDEX_NONE;
	int32 LastAttackID = INDEX_NONE;

	EBossClownAttackResult LastAttackResult = EBossClownAttackResult::Aborted;

	// 접근, 몽타주, 후딜을 하나의 공격 실행 번호로 관리.
	// 취소와 재도전 시 이동 설정 및 선택 쿨타임 함께 초기화.
	enum class EClownAttackStage : uint8
	{
		None,
		Approach,
		Montage,
		Recovery,
		Yabawi,
		Groggy
	};

	TWeakObjectPtr<AActor> ClownAttackTarget;
	EBossClownAttack CurrentAttackType = EBossClownAttack::HammerSweep;
	EClownAttackStage ClownAttackStage = EClownAttackStage::None;

	FVector DashDirection = FVector::ZeroVector;
	FVector DashStartLocation = FVector::ZeroVector;
	float AttackStageTime = 0.0f;
	float CurrentRecoveryScale = 1.0f;

	bool bSavedOrientToMovement = false;
	bool bSavedControllerDesiredRotation = false;
	bool bSavedControllerYaw = false;
	bool bHasSavedMovement = false;
	bool bDashStarted = false;
	bool bDashEnded = false;
	double NextChasePathTime = 0.0;

	TArray<EBossClownAttack> RecentPatterns;
	TMap<EBossClownAttack, double> LastFinishedByPattern;
	double LastPatternTime = -1000.0;
	double LastCubeTime = -1000.0;
	double RangedPreferenceUntil = 0.0;

	static constexpr int32 BallCount = 16;
	TWeakObjectPtr<UPrimitiveComponent> EncounterFloor;
	float EncounterFloorZ = 0.0f;
	FBox EncounterFloorBounds = FBox(ForceInit);
	TArray<TWeakObjectPtr<AActor>> Projectiles;

	FVector BallOrbitCenter = FVector::ZeroVector;
	FVector BallOrbitAxis = FVector::ForwardVector;
	int32 BallsLaunched = 0;
	bool bClapReleased = false;
};
