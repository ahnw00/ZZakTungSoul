#pragma once

#include <CoreMinimal.h>
#include <Enemy/Boss/Boss.h>
#include <Enemy/Boss/Clown/Data/BossClownAnimationData.h>
#include <TimerManager.h>
#include "BossClown.generated.h"

class UAnimInstance;
class UAnimMontage;
class UAnimSequenceBase;
class UClownProjectileComponent;
class UClownYabawiComponent;

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
	static constexpr int32 BallFormationCount = 16;
	bool CanStartClownYabawi() const;
	void EnterYabawiGroggy();
	void CompleteClownYabawi(bool bCorrect);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Clown|Phase2")
	TObjectPtr<UClownYabawiComponent> YabawiComponent;

	UPROPERTY(EditInstanceOnly, Category = "Clown|Arena")
	TObjectPtr<AActor> CubeArena;
	virtual void Tick(float DeltaTime) override;

	const UBossClownAnimationData* GetClownAnimationData() const;
	virtual const FEnemyAttackData* GetOpeningAttackData() const override;

	//---------------------------------------------------------------
	// 광대 전용 공격 실행과 중단.
	//
	//---------------------------------------------------------------
	bool StartClownAttack(EBossClownAttack AttackType, AActor* Target, int32& OutExecutionID,
		EClownAttackStyle Style = EClownAttackStyle::Normal, float RecoveryScale = 1.0f);
	void CancelClownAttack(int32 ExecutionID);
	EBossClownAttackResult GetClownAttackResult(int32 ExecutionID) const;

	//---------------------------------------------------------------
	// 광대 전용 Notify에서 현재 공격의 타격 데미지 설정.
	//
	//---------------------------------------------------------------
	void SetClownAttackDamage(int32 DamageIndex, UAnimSequenceBase* SourceAnimation);

	const FClownPhaseOnePattern* GetPhaseOnePattern(EBossClownAttack AttackType) const;
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
	void FaceClownTarget(AActor* Target, float DeltaTime);
	void PreferClownRangedAttack();
	bool PrefersClownRangedAttack() const;
	float GetClownPatternSelectionScale(EBossClownAttack AttackType) const;
	bool ShouldPrioritizeClownCube(const AActor* Target) const;

protected:
	virtual void BeginPlay() override;
	virtual void HandleAttackTraceHit(const FHitResult& Hit) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void ResetBossGimmicks() override;

	//---------------------------------------------------------------
	// 일반 적 데이터와 분리한 광대 전용 애니메이션 데이터.
	//
	//---------------------------------------------------------------
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Clown|Data")
	TObjectPtr<UBossClownAnimationData> ClownAnimationData;

	UPROPERTY(EditDefaultsOnly, Category = "Clown|Attack", meta = (ClampMin = "0.1"))
	float AttackTimeout = 15.0f;

public:
	void TickClownAttack(float DeltaTime);
	void UpdateClownTraceDebug();
	void ToggleClownTraceDebug();
	bool bTraceDebugInputReady = false;
	bool bDrawClownTraces = true;
	bool bTraceDebugDirty = true;
	bool PlayCurrentClownMontage();
	void BeginClownApproach();
	void BeginClownRecovery();
	void StopFastChaseAnimation();
	UClownProjectileComponent* CreateClownProjectile(
		TSubclassOf<AActor> Class, const FVector& Location, const FRotator& Rotation, float Speed, float LifeTime);
	bool PrepareClownBalls();
	bool PrepareClownCubeMarch();
	bool PrepareClownCards();
	bool PrepareClownYabawi();
	void TickClownSpecial(float MontageTime);
	void ClearClownProjectiles(bool bIncludeLaunched);
	void RestoreClownMovement();

	const FEnemyAttackData* GetClownAttackData(EBossClownAttack AttackType) const;
	void FinishClownAttack(EBossClownAttackResult Result, bool bStopMontage, bool bRestoreState = true);
	void OnClownAttackEnded(UAnimMontage* Montage, bool bInterrupted, int32 ExecutionID);
	void OnClownAttackTimeout();

private:
	void CollectClownAttackChoices(EBossClownAttack AttackType, TArray<const FEnemyAttackData*>& OutChoices) const;

	void FindEncounterFloor();
	TWeakObjectPtr<class UPrimitiveComponent> EncounterFloor;
	float EncounterFloorZ = 0.0f;
	FVector BallOrbitCenter = FVector::ZeroVector;
	FVector BallOrbitAxis = FVector::ForwardVector;
	double LastBallPatternTime = -1000.0;
	double LastCubeAttemptTime = -1000.0;
	bool FindClownCardGround(
		const FVector& Position, const FVector& Center, float Width, float Height, FVector& OutGround) const;
	TArray<EBossClownAttack> RecentClownPatterns;
	double LastClownPatternTime = 0.0;

	UPROPERTY(Transient)
	FEnemyAttackData CurrentClownAttack;

	TArray<TWeakObjectPtr<AActor>> ClownProjectiles;
	TWeakObjectPtr<UAnimMontage> ChaseMontage;
	int32 BallsLaunched = 0;
	bool bClapReleased = false;
	bool bWeaponWasVisible = false;
	bool bHidWeapon = false;

	TWeakObjectPtr<UAnimInstance> AttackAnimInstance;
	TWeakObjectPtr<UAnimMontage> LastPlayedAttackMontage;
	TWeakObjectPtr<UAnimMontage> ActiveGroggyMontage;
	float YabawiGroggyDuration = 0.0f;
	FTimerHandle AttackTimeoutHandle;

	//---------------------------------------------------------------
	// 현재 광대 공격의 타격 데미지 인덱스.
	//
	//---------------------------------------------------------------
	int32 CurrentDamageIndex = INDEX_NONE;

	int32 AttackSerial = 0;
	int32 ActiveAttackID = INDEX_NONE;
	int32 LastAttackID = INDEX_NONE;

	EBossClownAttackResult LastAttackResult = EBossClownAttackResult::Aborted;

	//---------------------------------------------------------------
	// 접근, 몽타주, 후딜을 하나의 공격 실행 번호로 관리.
	// 취소와 재도전 시 이동 설정 및 선택 쿨타임 함께 초기화.
	//
	//---------------------------------------------------------------
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
	double RangedPreferenceUntil = 0.0;
	bool bSavedOrientToMovement = false;
	bool bSavedControllerDesiredRotation = false;
	bool bSavedControllerYaw = false;
	bool bHasSavedMovement = false;
	bool bDashStarted = false;
	bool bDashEnded = false;
	double NextChasePathTime = 0.0;
};
