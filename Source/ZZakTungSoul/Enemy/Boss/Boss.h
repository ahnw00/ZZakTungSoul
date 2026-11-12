#pragma once

#include <CoreMinimal.h>
#include <Enemy/EnemyBase.h>
#include <TimerManager.h>
#include "Boss.generated.h"

class UBossAIData;
struct FEnemyAttackData;
class UAnimMontage;

UENUM(BlueprintType)
enum class EBossPhase : uint8
{
	Intro,
	OpeningAttack,
	Phase1,
	Phase2,
	Dead,
	Waiting,
	PhaseTransition
};

UENUM(BlueprintType)
enum class EBossPresentation : uint8
{
	None,
	Intro,
	Phase2Transition
};

UENUM(BlueprintType)
enum class EBossPhase2TransitionStage : uint8
{
	None,
	Disappearing,
	Searching,
	Emerging
};

class ABoss;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FBossPhase2StageRequested, ABoss*, Boss, EBossPhase2TransitionStage, Stage, int32, TransitionID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FBossPhase2TransitionStopped, ABoss*, Boss, int32, TransitionID, bool, bCancelled);
// 보스전 진행 단계는 FSM으로 관리.
// 이동과 일반 공격의 행동 선택은 기존 BT 사용.
UCLASS()
class ZZAKTUNGSOUL_API ABoss : public AEnemyBase
{
	GENERATED_BODY()

public:
	ABoss();
	virtual void SetDead() override;
	AActor* GetEncounterTarget() const { return EncounterTarget.Get(); }
	// 입장 연출과 첫 공격이 끝나기 전에는 BT 자동 실행 차단.
	virtual bool ShouldAutoStartAI() const override
	{
		return false;
	}
	// 보스방 진입 시 타깃을 전달하고 전투 시작 요청.
	UFUNCTION(BlueprintCallable, Category = "Boss|Encounter")
	bool StartEncounter(AActor* Target);
	// 실패한 보스전을 재도전 대기 상태로 초기화.
	// 이미 처치한 보스는 초기화하지 않음.
	UFUNCTION(BlueprintCallable, Category = "Boss|Encounter")
	void ResetEncounter();
	// 정상 종료 또는 스킵 시 해당 연출 완료.
	UFUNCTION(BlueprintCallable, Category = "Boss|Presentation")
	void CompletePresentation(int32 PresentationID);
	// 해당 연출을 취소하고 보스전 초기화.
	UFUNCTION(BlueprintCallable, Category = "Boss|Presentation")
	void CancelPresentation(int32 PresentationID);

	// 첫 입장 인트로와 별개인 전환 흐름. BP는 이 이벤트에서 VFX/시퀀스만 재생.
	UPROPERTY(BlueprintAssignable, Category = "Boss|Phase2Transition")
	FBossPhase2StageRequested OnPhase2StageRequested;

	// 정상 완료/재도전/사망/종료 시 BP가 재생 중인 연출과 카메라를 정리할 연결점.
	UPROPERTY(BlueprintAssignable, Category = "Boss|Phase2Transition")
	FBossPhase2TransitionStopped OnPhase2TransitionStopped;

	UFUNCTION(BlueprintPure, Category = "Boss|Phase2Transition")
	EBossPhase2TransitionStage GetPhase2TransitionStage() const
	{
		return Phase2TransitionStage;
	}

	UFUNCTION(BlueprintPure, Category = "Boss|Phase2Transition")
	int32 GetPhase2TransitionID() const
	{
		return ActivePresentation == EBossPresentation::Phase2Transition ? ActivePresentationID : INDEX_NONE;
	}

	// 사라짐 시퀀스의 OnFinished에서 요청 당시 ID로 호출.
	UFUNCTION(BlueprintCallable, Category = "Boss|Phase2Transition")
	bool CompletePhase2Disappear(int32 TransitionID);

	// 수색 트리거의 OtherActor를 Finder로 전달. 현재 전투 상대만 등장 신호를 보낼 수 있음.
	UFUNCTION(BlueprintCallable, Category = "Boss|Phase2Transition")
	bool NotifyPhase2Found(AActor* Finder, int32 TransitionID, AActor* EmergencePoint = nullptr);

	// 벽 등장 시퀀스의 OnFinished에서 호출. 여기까지 끝나야 2페이즈 BT 시작.
	UFUNCTION(BlueprintCallable, Category = "Boss|Phase2Transition")
	bool CompletePhase2Emergence(int32 TransitionID);

	// 보스 액터 원점을 놓을 위치/회전. 벽/시퀀스를 만들지 않고 나중에 연결할 마커 슬롯.
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Boss|Phase2Transition")
	TObjectPtr<AActor> Phase2EmergencePoint;

	// 시간 기반 연출을 사용하는 런타임 설정. 끄면 BP/시퀀스 완료 신호를 기다린다. 수색은 항상 발견 신호로 완료한다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Boss|Phase2Transition", meta = (DisplayName = "Use Timed Phase2 Transition"))
	bool bUsePhase2PlaceholderTimings = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Phase2Transition",
		meta = (ClampMin = "0.01", EditCondition = "bUsePhase2PlaceholderTimings", DisplayName = "Phase2 Disappear Duration"))
	float Phase2DisappearPlaceholderTime = 0.35f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Phase2Transition",
		meta = (ClampMin = "0.01", EditCondition = "bUsePhase2PlaceholderTimings", DisplayName = "Phase2 Emergence Duration"))
	float Phase2EmergencePlaceholderTime = 0.35f;
	// 보스 데이터 반환.
	const UBossAIData* GetBossAIData() const
	{
		return BossAIData;
	}

	virtual const FEnemyAttackData* GetOpeningAttackData(EBossPhase CombatPhase) const;
	EBossPhase GetOpeningCombatPhase() const { return OpeningCombatPhase; }
	// 수평 중심 거리에서 양쪽 캡슐 반지름을 뺀 값. 겹치면 0.
	float GetTargetSurfaceDistance(const AActor* Target) const;
	virtual AActor* GetEncounterArena() const
	{
		return nullptr;
	}

	virtual void SetEncounterArena(AActor* Arena)
	{
	}
	// 현재 보스 페이즈 반환.
	UFUNCTION(BlueprintPure, Category = "Boss|Phase")
	EBossPhase GetBossPhase() const
	{
		return BossPhase;
	}

	UEnemyStatComponent* GetEnemyStatComponent() const
	{
		return EnemyStatComponet;
	}

	const FText& GetBossDisplayName() const
	{
		return BossDisplayName;
	}

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void ReceiveDamage(const FDamageInfo& DamageInfo, const FHitResult& Hit) override;
	virtual void HandleAttackTraceHit(const FHitResult& Hit) override;
	virtual void InitializeWeapon() override;
	virtual void UpdateMovementSpeed() override;
	// 보스 AI 데이터에서 전투 거리 반환.
	// 공통 AI 인터페이스 호환용. 보스는 전투범위 임계값을 사용하지 않음.
	virtual float GetCombatRangeData() override
	{
		return 0.0f;
	}

	virtual float GetAttackRangeData() override;
	virtual float GetMinMaintainDistanceData() override;
	// 연출 실행 요청.
	// 첫 입장 연출 연결점. 2페이즈 전환은 OnPhase2StageRequested에서만 관리.
	UFUNCTION(BlueprintNativeEvent, Category = "Boss|Presentation")
	void OnPresentationStarted(EBossPresentation Presentation, int32 PresentationID);
	virtual void OnPresentationStarted_Implementation(EBossPresentation Presentation, int32 PresentationID);
	// 완료 또는 취소 시 연출 자원 정리.
	// 카메라, 입력, 시퀀스 등 연출에서 변경한 상태 복구.
	UFUNCTION(BlueprintNativeEvent, Category = "Boss|Presentation")
	void OnPresentationStopped(EBossPresentation Presentation, int32 PresentationID, bool bCancelled);
	virtual void OnPresentationStopped_Implementation(
		EBossPresentation Presentation, int32 PresentationID, bool bCancelled);
	// 보스별 기믹과 타이머 정리.
	// 필요한 보스 자식 클래스에서 재정의.
	virtual void ResetBossGimmicks();
	virtual void TryStartPhase2Transition(float CurrentHp, float MaxHp);

private:
	void FinishPresentation(int32 PresentationID);
	void BeginPhase2Transition();
	void SetPhase2TransitionStage(EBossPhase2TransitionStage Stage);
	bool IsPhase2TransitionStep(int32 TransitionID, EBossPhase2TransitionStage Stage) const;
	void StopPhase2Transition(int32 TransitionID, bool bCancelled);
	bool ValidateEncounter() const;
	void EnterCombatPhase(EBossPhase NewPhase);

	void StartPresentation(EBossPresentation Presentation);
	void StopPresentation(bool bCancelled = true);

	void BeginOpeningAttack(EBossPhase CombatPhase);
	void OnOpeningEnded(UAnimMontage* Montage, bool bInterrupted, int32 InEncounterID);

	void StopCombat();
	void StopEncounterActions();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boss|Data")
	TObjectPtr<UBossAIData> BossAIData;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Boss|Phase")
	EBossPhase BossPhase = EBossPhase::Waiting;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boss|UI")
	FText BossDisplayName;
private:
	TWeakObjectPtr<AActor> EncounterTarget;
	EBossPhase OpeningCombatPhase = EBossPhase::Phase1;
	EBossPhase2TransitionStage Phase2TransitionStage = EBossPhase2TransitionStage::None;
	FTimerHandle Phase2StageTimer;
	bool bPhase2StateSaved = false;
	bool bPhase2SavedHidden = false;
	bool bPhase2SavedCollision = true;
	uint8 Phase2SavedMovementMode = 0;
	uint8 Phase2SavedCustomMovementMode = 0;
	// 재도전 시 복구할 보스 시작 위치와 회전.
	FTransform InitialTransform = FTransform::Identity;
	// 이전 실행의 지연 콜백을 무시하기 위한 실행 번호.
	int32 PresentationSerial = 0;
	int32 ActivePresentationID = INDEX_NONE;
	int32 EncounterID = 0;

	EBossPresentation ActivePresentation = EBossPresentation::None;

	bool bLifecycleChanging = false;

};
