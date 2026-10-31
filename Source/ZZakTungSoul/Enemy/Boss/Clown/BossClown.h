#pragma once

#include <CoreMinimal.h>
#include <Enemy/Boss/Boss.h>
#include <Enemy/Boss/Clown/Data/BossClownAnimationData.h>
#include <TimerManager.h>
#include "BossClown.generated.h"

class UAnimInstance;
class UAnimMontage;
class UAnimSequenceBase;
class ABossClownProjectile;

enum class EBossClownAttackResult : uint8
{
	Running, Succeeded, Failed, Aborted
};

UCLASS()
class ZZAKTUNGSOUL_API ABossClown : public ABoss
{
	GENERATED_BODY()

public:
	ABossClown();
	virtual void Tick(float DeltaTime) override;

	const UBossClownAnimationData* GetClownAnimationData() const;
	virtual const FEnemyAttackData* GetOpeningAttackData() const override;

	//---------------------------------------------------------------
	// 광대 전용 공격 실행과 중단.
	//
	//---------------------------------------------------------------
	bool StartClownAttack(EBossClownAttack AttackType, AActor* Target, int32& OutExecutionID);
	void CancelClownAttack(int32 ExecutionID);
	EBossClownAttackResult GetClownAttackResult(int32 ExecutionID) const;

	//---------------------------------------------------------------
	// 광대 전용 Notify에서 현재 공격의 타격 데미지 설정.
	//
	//---------------------------------------------------------------
	void SetClownAttackDamage(int32 DamageIndex, UAnimSequenceBase* SourceAnimation);

protected:
	virtual bool StartBossCombatBehavior(AActor* Target) override;
	virtual void HandleAttackTraceHit(const FHitResult& Hit) override;
	virtual void ResetBossGimmicks() override;

	//---------------------------------------------------------------
	// 일반 적 데이터와 분리한 광대 전용 애니메이션 데이터.
	//
	//---------------------------------------------------------------
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Clown|Data")
	TObjectPtr<UBossClownAnimationData> ClownAnimationData;

	UPROPERTY(EditDefaultsOnly, Category = "Clown|Attack", meta = (ClampMin = "0.1"))
	float AttackTimeout = 15.0f;

private:
	const FClownPhaseOnePattern* GetPhaseOnePattern(EBossClownAttack AttackType) const;
	float GetTargetSurfaceDistance(const AActor* Target) const;
	void TickPhaseOneSelection(float DeltaTime);
	void TickClownAttack(float DeltaTime);
	bool PlayCurrentClownMontage();
	void BeginClownRecovery();
	void StopFastChaseAnimation();
	void PrepareClownBalls();
	void TickClownSpecial(float MontageTime);
	void ClearClownProjectiles(bool bIncludeLaunched);
	void RestoreClownMovement();
	void FaceClownTarget(AActor* Target, float DeltaTime);

	bool IsClownCombatPhase() const;
	const FEnemyAttackData* GetClownAttackData(EBossClownAttack AttackType) const;
	void FinishClownAttack(EBossClownAttackResult Result, bool bStopMontage, bool bRestoreState = true);
	void OnClownAttackEnded(UAnimMontage* Montage, bool bInterrupted, int32 ExecutionID);
	void OnClownAttackTimeout();

private:
	UPROPERTY(Transient)
	FEnemyAttackData CurrentClownAttack;

	TArray<TWeakObjectPtr<ABossClownProjectile>> ClownProjectiles;
	TWeakObjectPtr<UAnimMontage> ChaseMontage;
	int32 BallsLaunched = 0;
	bool bClapReleased = false;
	bool bWeaponWasVisible = false;
	bool bHidWeapon = false;

	TWeakObjectPtr<UAnimInstance> AttackAnimInstance;
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
		None, Approach, Montage, Recovery
	};

	TWeakObjectPtr<AActor> ClownCombatTarget;
	TWeakObjectPtr<AActor> ClownAttackTarget;
	TMap<EBossClownAttack, double> PatternReadyTimes;
	EBossClownAttack CurrentAttackType = EBossClownAttack::HammerSweep;
	EBossClownAttack PreviousAttackType = EBossClownAttack::HammerSweep;
	EClownAttackStage ClownAttackStage = EClownAttackStage::None;

	FVector DashDirection = FVector::ZeroVector;
	FVector DashStartLocation = FVector::ZeroVector;
	float AttackStageTime = 0.0f;
	float SavedWalkSpeed = 0.0f;
	bool bSavedOrientToMovement = false;
	bool bSavedControllerDesiredRotation = false;
	bool bSavedControllerYaw = false;
	bool bHasSavedMovement = false;
	bool bNativePhaseOneActive = false;
	bool bHasPreviousAttack = false;
	bool bDashStarted = false;
	bool bDashEnded = false;
	double NextPatternDecisionTime = 0.0;
	double MovementPatternReadyTime = 0.0;
	double NextChasePathTime = 0.0;
};
