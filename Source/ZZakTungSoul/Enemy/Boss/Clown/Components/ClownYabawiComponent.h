#pragma once

#include <CoreMinimal.h>
#include <Components/ActorComponent.h>
#include <TimerManager.h>
#include <Enemy/Boss/Clown/Data/BossClownAnimationData.h>
#include "ClownYabawiComponent.generated.h"

class AClownYabawiBowl;
class UWidgetComponent;

UENUM(BlueprintType)
enum class EClownYabawiStage : uint8
{
	None,
	Reveal,
	Cover,
	Shuffle,
	Selection,
	Resolving
};

UCLASS(ClassGroup = (Boss), meta = (BlueprintSpawnableComponent))
class ZZAKTUNGSOUL_API UClownYabawiComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UClownYabawiComponent();
	bool CanStart() const;
	bool StartRound(AActor* Player, const TArray<FVector>& Positions);
	void CancelRound(bool bResetCooldown = false);
	virtual void TickComponent(
		float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	bool SelectBowl(AActor* Player, AClownYabawiBowl* Bowl, int32 InRoundID, const FHitResult& Hit);

	UFUNCTION(BlueprintPure, Category = "Clown|Yabawi")
	EClownYabawiStage GetStage() const
	{
		return Stage;
	}

	UFUNCTION(BlueprintPure, Category = "Clown|Yabawi")
	int32 GetRoundID() const
	{
		return RoundID;
	}

	UFUNCTION(BlueprintPure, Category = "Clown|Yabawi")
	float GetSelectionRemaining() const;

private:
	enum class ERoundResult : uint8
	{
		Correct,
		Wrong,
		Timeout
	};

	void SetStage(EClownYabawiStage Next);
	void StartSwap();
	void Resolve(ERoundResult Result);
	void UpdatePresentation();
	void CommitSelection();

	UPROPERTY(Transient)
	FClownPhaseOnePattern Settings;

	UPROPERTY(Transient)
	TObjectPtr<UWidgetComponent> SelectionPanel;

	UPROPERTY(Transient)
	TArray<TObjectPtr<AClownYabawiBowl>> Bowls;

	UPROPERTY(VisibleInstanceOnly, Category = "Clown|Yabawi")
	EClownYabawiStage Stage = EClownYabawiStage::None;

	TWeakObjectPtr<AActor> Target;
	int32 RoundSerial = 0;
	int32 RoundID = INDEX_NONE;
	int32 CorrectBowl = INDEX_NONE;

	int32 SwapA = INDEX_NONE;
	int32 SwapB = INDEX_NONE;
	int32 SwapsCompleted = 0;
	FVector SwapStartA = FVector::ZeroVector;
	FVector SwapStartB = FVector::ZeroVector;

	float StageTime = 0.0f;
	float SwapTime = 0.0f;
	double SelectionDeadline = 0.0;
	double ReadyTime = 0.0;

	ERoundResult RoundResult = ERoundResult::Wrong;
	int32 LastPresentedSeconds = INDEX_NONE;

	TWeakObjectPtr<AClownYabawiBowl> PendingBowl;
	FHitResult PendingHit;
	uint64 PendingFrame = 0;
	FTimerHandle SelectionCommitTimer;
};
