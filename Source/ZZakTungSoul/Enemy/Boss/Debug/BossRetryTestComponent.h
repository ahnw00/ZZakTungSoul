#pragma once

#include <CoreMinimal.h>
#include <Components/ActorComponent.h>
#include <TimerManager.h>
#include "BossRetryTestComponent.generated.h"

class ABossRoom;
class AWarrior;
class APlayerController;
class UInputComponent;

UCLASS(ClassGroup = (Boss), meta = (BlueprintSpawnableComponent))
class ZZAKTUNGSOUL_API UBossRetryTestComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBossRetryTestComponent();

	//---------------------------------------------------------------
	// 복귀 위치가 막혀 자동 복귀에 실패했을 때 재시도.
	//
	//---------------------------------------------------------------
	UFUNCTION(BlueprintCallable, Category = "BossRetryTest")
	void RetryNow();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void StartRetry();
	void RestorePlayer();
	void LockPlayer();
	void UnlockPlayer();
	void StopPlayerActions();
	void CancelRetry();

protected:
	//---------------------------------------------------------------
	// 테스트 기능 사용 여부.
	// Shipping 빌드에서는 실행하지 않음.
	//
	//---------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BossRetryTest|Settings")
	bool bEnableRetryTest = true;

	//---------------------------------------------------------------
	// 사망 감지 후 복귀까지 대기 시간.
	//
	//---------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BossRetryTest|Settings", meta = (ClampMin = "0.1"))
	float RetryDelay = 2.0f;

	//---------------------------------------------------------------
	// 레벨에 배치한 복귀 지점.
	// 플레이어 캡슐 중심 높이로 배치.
	//
	//---------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BossRetryTest|Settings")
	TObjectPtr<AActor> RetryPoint;

private:
	UPROPERTY(Transient)
	TObjectPtr<UInputComponent> InputBlocker;

	TWeakObjectPtr<ABossRoom> BossRoom;
	TWeakObjectPtr<AWarrior> TestPlayer;
	TWeakObjectPtr<APlayerController> LockedController;

	FTimerHandle RetryTimerHandle;

	uint8 SavedMovementMode = 0;
	uint8 SavedCustomMovementMode = 0;

	bool bAttemptObserved = false;
	bool bRetryPending = false;
	bool bInputLocked = false;
	bool bMovementLocked = false;
};
