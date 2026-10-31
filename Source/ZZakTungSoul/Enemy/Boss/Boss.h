#pragma once
#include <CoreMinimal.h>
#include <Enemy/EnemyBase.h>
#include "Boss.generated.h"

class UBossAIData;
class UAnimMontage;

UENUM(BlueprintType)
enum class EBossPhase : uint8
{
    Intro, OpeningAttack, Phase1, Phase2, Dead
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
    // 컷씬과 첫 공격이 끝난 뒤 BT를 시작하도록 자동 실행 차단.
    //
    //---------------------------------------------------------------
    virtual bool ShouldAutoStartAI() const override { return false; }

    //---------------------------------------------------------------
    // 보스방 진입 시 타깃을 전달하고 전투 시작 요청.
    //
    //---------------------------------------------------------------
    UFUNCTION(BlueprintCallable, Category = "Boss")
    bool StartEncounter(AActor* Target);

    //---------------------------------------------------------------
    // 컷씬 정상 종료 또는 스킵 시 호출.
    //
    //---------------------------------------------------------------
    UFUNCTION(BlueprintCallable, Category = "Boss")
    void FinishIntro();

    //---------------------------------------------------------------
    // 블루프린트에서 컷씬 연출을 연결할 이벤트.
    //
    //---------------------------------------------------------------
    UFUNCTION(BlueprintImplementableEvent, Category = "Boss")
    void OnIntroStarted();

    //---------------------------------------------------------------
    // 보스 AI 데이터 게터 추가.
    //
    //---------------------------------------------------------------
    const UBossAIData* GetBossAIData() const { return BossAIData; }

    //---------------------------------------------------------------
    // 현재 보스 페이즈 반환.
    //
    //---------------------------------------------------------------
    UFUNCTION(BlueprintPure, Category = "Boss|Phase")
    EBossPhase GetBossPhase() const { return BossPhase; }

    virtual void UpdateMovementSpeed() override;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void ReceiveDamage(const FDamageInfo& DamageInfo, const FHitResult& Hit) override;
    virtual void HandleAttackTraceHit(const FHitResult& Hit) override;
    virtual void InitializeWeapon() override;

private:
    bool ValidateEncounter() const;
    void HandleHpChanged(float CurrentHp, float MaxHp);
    void EnterCombatPhase(EBossPhase NewPhase);
    void StopCombat();
    void OnOpeningEnded(UAnimMontage* Montage, bool bInterrupted);
    void StopOpening();

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boss|Data")
    TObjectPtr<UBossAIData> BossAIData;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Boss|Phase")
    EBossPhase BossPhase = EBossPhase::Intro;

private:
    TWeakObjectPtr<AActor> EncounterTarget;
    bool bEncounterStarted = false;



};
