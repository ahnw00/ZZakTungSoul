#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BossHUDWidget.generated.h"

class UTextBlock;
class UProgressBar;
class ABoss;
class UEnemyStatComponent;

UCLASS()
class ZZAKTUNGSOUL_API UBossHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 보스 이름 표시
	UFUNCTION(BlueprintCallable, Category = "Boss|UI")
	void SetBossName(FText InBossName);

	// 현재 HP와 최대 HP를 받아 체력바 갱신
	UFUNCTION(BlueprintCallable, Category = "Boss|UI")
	void UpdateHealth(float CurrentHp, float MaxHp);

	// 표시할 실제 보스 연결
	UFUNCTION(BlueprintCallable, Category = "Boss|UI")
	void BindBoss(ABoss* InBoss);
protected:
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	void AddDamageDisplay(float Damage);
	void ResetDamageDisplay();

protected:

	// 보스 이름
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_BossName;

	// 누적 데미지 숫자
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Damage;

	// 노란 잔상 체력바
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> ProgressBar_Delay;

	// 빨간 실제 체력바
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> ProgressBar_HP;

	// 마지막 피격 후 숫자를 유지할 시간
	UPROPERTY(EditDefaultsOnly, Category = "Boss|UI|Damage",
		meta = (ClampMin = "0.1"))
	float DamageDisplayDuration = 2.5f;

	// 노란 바가 감소하기 전 대기 시간
	UPROPERTY(EditDefaultsOnly, Category = "Boss|UI|Health",
		meta = (ClampMin = "0.0"))
	float HpDelayDuration = 0.35f;

	// 초당 감소할 비율: 0.6이면 초당 전체 길이의 60%
	UPROPERTY(EditDefaultsOnly, Category = "Boss|UI|Health",
		meta = (ClampMin = "0.01"))
	float HpDecreaseSpeed = 0.6f;


private:
	// 현재 연결된 보스의 스탯 컴포넌트
	UPROPERTY(Transient)
	TWeakObjectPtr<UEnemyStatComponent> BoundStatComponent;

	bool bHasInitialHealth = false;

	float PreviousHp = 0.f;
	float PreviousMaxHp = 0.f;

	float TargetHpPercent = 0.f;
	float DelayedHpPercent = 0.f;
	float HpDelayRemaining = 0.f;



	float AccumulatedDamage = 0.f;
	float DamageDisplayRemaining = 0.f;


};