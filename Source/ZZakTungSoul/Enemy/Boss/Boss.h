#pragma once

#include <CoreMinimal.h>
#include <Enemy/EnemyBase.h>
#include "Boss.generated.h"

class UBossAIData;
struct FEnemyAttackData;
class UAnimMontage;

UENUM(BlueprintType)
enum class EBossPhase : uint8
{
	Intro, OpeningAttack, Phase1, Phase2, Dead, Waiting, PhaseTransition
};

UENUM(BlueprintType)
enum class EBossPresentation : uint8
{
	None, Intro, Phase2Transition
};

//---------------------------------------------------------------
// 보스전 진행 단계는 FSM으로 관리.
// 이동과 일반 공격의 행동 선택은 기존 BT 사용.
//
//---------------------------------------------------------------
UCLASS()
class ZZAKTUNGSOUL_API ABoss : public AEnemyBase
{
	GENERATED_BODY()

public:
	ABoss();

	//---------------------------------------------------------------
	// 입장 연출과 첫 공격이 끝나기 전에는 BT 자동 실행 차단.
	//
	//---------------------------------------------------------------
	virtual bool ShouldAutoStartAI() const override { return false; }

	//---------------------------------------------------------------
	// 보스방 진입 시 타깃을 전달하고 전투 시작 요청.
	//
	//---------------------------------------------------------------
	UFUNCTION(BlueprintCallable, Category = "Boss|Encounter")
	bool StartEncounter(AActor* Target);

	//---------------------------------------------------------------
	// 실패한 보스전을 재도전 대기 상태로 초기화.
	// 이미 처치한 보스는 초기화하지 않음.
	//
	//---------------------------------------------------------------
	UFUNCTION(BlueprintCallable, Category = "Boss|Encounter")
	void ResetEncounter();

	//---------------------------------------------------------------
	// 정상 종료 또는 스킵 시 해당 연출 완료.
	//
	//---------------------------------------------------------------
	UFUNCTION(BlueprintCallable, Category = "Boss|Presentation")
	void CompletePresentation(int32 PresentationID);

	//---------------------------------------------------------------
	// 해당 연출을 취소하고 보스전 초기화.
	//
	//---------------------------------------------------------------
	UFUNCTION(BlueprintCallable, Category = "Boss|Presentation")
	void CancelPresentation(int32 PresentationID);

	//---------------------------------------------------------------
	// 보스 데이터 반환.
	//
	//---------------------------------------------------------------
	const UBossAIData* GetBossAIData() const { return BossAIData; }
	virtual const FEnemyAttackData* GetOpeningAttackData() const;

	//---------------------------------------------------------------
	// 현재 보스 페이즈 반환.
	//
	//---------------------------------------------------------------
	UFUNCTION(BlueprintPure, Category = "Boss|Phase")
	EBossPhase GetBossPhase() const { return BossPhase; }

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void ReceiveDamage(const FDamageInfo& DamageInfo, const FHitResult& Hit) override;
	virtual void HandleAttackTraceHit(const FHitResult& Hit) override;
	virtual void InitializeWeapon() override;
	virtual void UpdateMovementSpeed() override;

	//---------------------------------------------------------------
	// 보스 AI 데이터에서 전투 거리 반환.
	//
	//---------------------------------------------------------------
	virtual float GetCombatRangeData() override;
	virtual float GetAttackRangeData() override;
	virtual float GetMinMaintainDistanceData() override;

	//---------------------------------------------------------------
	// 연출 실행 요청.
	// C++ 기본 구현은 연출 없이 즉시 완료.
	//
	//---------------------------------------------------------------
	UFUNCTION(BlueprintNativeEvent, Category = "Boss|Presentation")
	void OnPresentationStarted(EBossPresentation Presentation, int32 PresentationID);
	virtual void OnPresentationStarted_Implementation(EBossPresentation Presentation, int32 PresentationID);

	//---------------------------------------------------------------
	// 완료 또는 취소 시 연출 자원 정리.
	// 카메라, 입력, 시퀀스 등 연출에서 변경한 상태 복구.
	//
	//---------------------------------------------------------------
	UFUNCTION(BlueprintNativeEvent, Category = "Boss|Presentation")
	void OnPresentationStopped(EBossPresentation Presentation, int32 PresentationID, bool bCancelled);
	virtual void OnPresentationStopped_Implementation(EBossPresentation Presentation, int32 PresentationID, bool bCancelled);

	//---------------------------------------------------------------
	// 보스별 기믹과 타이머 정리.
	// 필요한 보스 자식 클래스에서 재정의.
	//
	//---------------------------------------------------------------
	virtual void ResetBossGimmicks();

private:
	bool ValidateEncounter() const;
	void HandleHpChanged(float CurrentHp, float MaxHp);
	void EnterCombatPhase(EBossPhase NewPhase);

	void StartPresentation(EBossPresentation Presentation);
	void StopPresentation();

	void BeginOpeningAttack();
	void OnOpeningEnded(UAnimMontage* Montage, bool bInterrupted, int32 InEncounterID);

	void StopCombat();
	void StopOpening();
	void StopEncounterActions();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boss|Data")
	TObjectPtr<UBossAIData> BossAIData;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Boss|Phase")
	EBossPhase BossPhase = EBossPhase::Waiting;

private:
	TWeakObjectPtr<AActor> EncounterTarget;

	//---------------------------------------------------------------
	// 재도전 시 복구할 보스 시작 위치와 회전.
	//
	//---------------------------------------------------------------
	FTransform InitialTransform = FTransform::Identity;

	//---------------------------------------------------------------
	// 이전 실행의 지연 콜백을 무시하기 위한 실행 번호.
	//
	//---------------------------------------------------------------
	int32 PresentationSerial = 0;
	int32 ActivePresentationID = INDEX_NONE;
	int32 EncounterID = 0;

	EBossPresentation ActivePresentation = EBossPresentation::None;

	bool bEncounterStarted = false;
	bool bLifecycleChanging = false;
};
