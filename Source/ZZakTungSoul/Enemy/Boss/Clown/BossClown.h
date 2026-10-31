#pragma once

#include <CoreMinimal.h>
#include <Enemy/Boss/Boss.h>
#include <Enemy/Boss/Clown/Data/BossClownAnimationData.h>
#include <TimerManager.h>
#include "BossClown.generated.h"

class UAnimInstance;
class UAnimMontage;
class UAnimSequenceBase;

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
	bool IsClownCombatPhase() const;
	const FEnemyAttackData* GetClownAttackData(EBossClownAttack AttackType) const;
	void FinishClownAttack(EBossClownAttackResult Result, bool bStopMontage, bool bRestoreState = true);
	void OnClownAttackEnded(UAnimMontage* Montage, bool bInterrupted, int32 ExecutionID);
	void OnClownAttackTimeout();

private:
	UPROPERTY(Transient)
	FEnemyAttackData CurrentClownAttack;

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
};
