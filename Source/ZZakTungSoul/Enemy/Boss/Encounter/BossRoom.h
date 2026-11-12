#pragma once

#include <CoreMinimal.h>
#include <GameFramework/Actor.h>
#include <Interface/InteractableInterface.h>
#include <Misc/FrameTime.h>
#include "Blueprint/UserWidget.h"
#include "TimerManager.h"
#include "BossRoom.generated.h"

class ABoss;
class AWarrior;
class APlayerController;
class UAnimMontage;
class ALevelSequenceActor;
class ULevelSequence;
class ULevelSequencePlayer;
class UMovieSceneSequencePlayer;
class UMaterialInterface;
class UMaterialInstanceDynamic;
class USceneComponent;
class UBoxComponent;
class UArrowComponent;
class UAudioComponent;
class USoundBase;
class UStaticMeshComponent;
class USkeletalMeshComponent;
class UBossRoomSubsystem;
class UBossHUDWidget;
enum class EBossPhase2TransitionStage : uint8;

UENUM(BlueprintType)
enum class EBossRoomState : uint8
{
	Ready,
	Intro,
	Fighting,
	Cleared
};

UCLASS()
class ZZAKTUNGSOUL_API ABossRoom : public AActor, public IInteractableInterface
{
	GENERATED_BODY()

public:
	ABossRoom();

	virtual void Interact(AActor* detectedActor) override;

	void ApplySavedProgress();
	// 플레이어 사망 등 외부 실패 처리에서도 호출 가능.
	UFUNCTION(BlueprintCallable, Category = "BossRoom")
	void FailAttempt();
	// 입장 컷신 스킵.
	UFUNCTION(BlueprintCallable, Category = "BossRoom")
	void SkipIntro();

	UFUNCTION(BlueprintPure, Category = "BossRoom")
	EBossRoomState GetRoomState() const
	{
		return RoomState;
	}

	UFUNCTION(BlueprintCallable, Category = "BossRoom|OtherSide")
	void SetOtherSideGateEnabled(bool bEnabled);

protected:
	virtual void PostLoad() override;
	virtual void PostRegisterAllComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UBossRoomSubsystem* GetRoomSubsystem() const;

	bool IsFirstEntry() const;
	bool IsPlayerAlive(AWarrior* Player) const;
	bool TryEnter(AWarrior* Player, bool bFromFog, bool bOtherSide = false);
	bool IsPlayerAtGate(const AWarrior* Player) const;

	bool BeginClownFogEntry(AWarrior* Player, bool bOtherSide);
	void TickClownFogEntry(float DeltaSeconds);
	void EndClownFogEntry(bool bReachedInside);

	void BeginBattle();
	void FinishClear();
	void RefreshGate();

	void UpdateEncounterMusic();
	void SetEncounterMusic(USoundBase* Music, float Volume = 1.f);

	void StopIntro();
	void OnIntroSequenceUpdated(const UMovieSceneSequencePlayer& Player, FFrameTime CurrentTime, FFrameTime PreviousTime);
	void LockPlayer(AWarrior* Player);
	void UnlockPlayer();

	UFUNCTION()
	void OnIntroFinished();

	UFUNCTION()
	void OnFirstEntryBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnInteractionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnInteractionEndOverlap(
		UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void UpdateBossHUDForPhase2(ABoss* Boss, EBossPhase2TransitionStage Stage, int32 TransitionID);

	void ShowBossHUD();
	void HideBossHUD();

	void OpenCreditsMap();

	void HideSequenceUI();
	void RestoreSequenceUI();

protected:
	// 보스방 컴포넌트.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BossRoom|Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BossRoom|Components")
	TObjectPtr<UBoxComponent> FirstEntryTrigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BossRoom|Components")
	TObjectPtr<UBoxComponent> InteractionTrigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BossRoom|Components")
	TObjectPtr<UBoxComponent> GateBlocker;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "BossRoom|Components")
	TObjectPtr<UBoxComponent> OhterSideGateBlocker;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BossRoom|OtherSide")
	TObjectPtr<UBoxComponent> OtherSideFirstEntryTrigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BossRoom|OtherSide")
	TObjectPtr<UBoxComponent> OtherSideInteractionTrigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BossRoom|OtherSide")
	TObjectPtr<UArrowComponent> OtherSideInsidePoint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BossRoom|OtherSide")
	bool bEnableOtherSideGate = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BossRoom|Components")
	TObjectPtr<UStaticMeshComponent> FogMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BossRoom|Components")
	TObjectPtr<UStaticMeshComponent> OhterSideFogMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BossRoom|Components")
	TObjectPtr<UArrowComponent> InsidePoint;
	// 레벨에 배치한 방마다 고유 ID와 보스 지정.
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "BossRoom|Settings")
	FName RoomID = NAME_None;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "BossRoom|Settings")
	TObjectPtr<ABoss> BossActor;
	// 첫 입장 컷신.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BossRoom|Presentation")
	TObjectPtr<ULevelSequence> IntroSequence;

	UPROPERTY(EditInstanceOnly, Category = "BossRoom|Presentation")
	TObjectPtr<AActor> IntroWaitingStandIn;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "BossRoom|State")
	EBossRoomState RoomState = EBossRoomState::Ready;

	// 보스방 BP에서 표시할 위젯 클래스 지정
	UPROPERTY(EditDefaultsOnly, Category = "BossRoom|UI")
	TSubclassOf<UBossHUDWidget> BossHUDWidgetClass;

	// 보스 처치 후 체력바를 유지할 시간
	UPROPERTY(EditDefaultsOnly, Category = "BossRoom|UI",
		meta = (ClampMin = "0.0"))
	float BossHUDHideDelay = 2.5f;

	UPROPERTY(VisibleAnywhere, Category = "BossRoom|Audio")
	TObjectPtr<UAudioComponent> ClownApproachMusic;

	UPROPERTY(EditAnywhere, Category = "BossRoom|Audio")
	TObjectPtr<USoundBase> Phase1CombatMusic;

	UPROPERTY(EditAnywhere, Category = "BossRoom|Audio", meta = (ClampMin = "0.0", DisplayAfter = "Phase1CombatMusic"))
	float Phase1CombatMusicVolume = 1.f;

	UPROPERTY(EditAnywhere, Category = "BossRoom|Audio")
	TObjectPtr<USoundBase> Phase2SearchMusic;

	UPROPERTY(EditAnywhere, Category = "BossRoom|Audio", meta = (ClampMin = "0.0", DisplayAfter = "Phase2SearchMusic"))
	float Phase2SearchMusicVolume = 1.f;

	UPROPERTY(EditAnywhere, Category = "BossRoom|Audio")
	TObjectPtr<USoundBase> Phase2CombatMusic;

	UPROPERTY(EditAnywhere, Category = "BossRoom|Audio", meta = (ClampMin = "0.0", DisplayAfter = "Phase2CombatMusic"))
	float Phase2CombatMusicVolume = 1.f;

private:
	// 기존에 저장된 공통 볼륨을 읽기 위한 필드.
	UPROPERTY()
	float EncounterMusicVolume = -1.f;

	UPROPERTY()
	TObjectPtr<UAnimMontage> ClownFogEntryMontage;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> ClownFogOpeningMaterial;
	UPROPERTY()
	TObjectPtr<UAudioComponent> EncounterMusic;

	TOptional<FFrameNumber> ClownApproachMusicCutFrame;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInterface> FogEntryOriginalMaterial;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> FogEntryOpening;

	TWeakObjectPtr<UStaticMeshComponent> TraversedFogVisual;
	TWeakObjectPtr<UBoxComponent> TraversedFogGate;
	TMap<TWeakObjectPtr<USkeletalMeshComponent>, bool> FogEntryWeaponVisibility;
	FVector FogEntryStart = FVector::ZeroVector;
	FVector FogEntryEnd = FVector::ZeroVector;
	FQuat FogEntryStartRotation = FQuat::Identity;
	FQuat FogEntryFacing = FQuat::Identity;
	FRotator FogEntryStartControlRotation = FRotator::ZeroRotator;
	float FogEntryElapsed = 0.0f;
	float FogEntryDuration = 0.0f;
	float FogEntryPreviousTickInterval = 0.0f;
	bool bFogEntryActive = false;

	// 실행 중 생성한 시퀀스 플레이어와 액터.
	UPROPERTY(Transient)
	TObjectPtr<ULevelSequencePlayer> IntroPlayer;

	UPROPERTY(Transient)
	TObjectPtr<ALevelSequenceActor> IntroActor;
	// 저장본을 다시 불러올 때 보스를 복원하기 위한 초기 정보.
	UPROPERTY(Transient)
	TSubclassOf<ABoss> InitialBossClass;

	FTransform BossInitialTransform = FTransform::Identity;
	TWeakObjectPtr<AActor> InitialBossArena;

	TWeakObjectPtr<AWarrior> EncounterPlayer;
	TWeakObjectPtr<APlayerController> LockedController;
	TWeakObjectPtr<AActor> PreviousViewTarget;

	bool bPlayerInputLocked = false;
	bool bChangingState = false;
	bool bIntroWaitingStandInActive = false;
	bool bIntroWaitingBossWasHidden = false;

	// 이 보스방이 생성한 체력바
	UPROPERTY(Transient)
	TObjectPtr<UBossHUDWidget> BossHUDWidget;

	// 시퀀스 시작 전 각 위젯의 표시 상태
	TMap<TWeakObjectPtr<UUserWidget>, ESlateVisibility> SavedUIVisibility;

	bool bSequenceUIHidden = false;
	FTimerHandle BossHUDHideTimer;

	FTimerHandle CreditsTimer;

	bool bRoomInitialized = false;

	// 최종 보스 방 클리어 후 크레딧 맵 이동.
	UPROPERTY(EditInstanceOnly, Category = "BossRoom|Credits")
	bool bFinalBossRoom = false;
	// 보스 처치 후 크레딧 맵 이동 대기 시간.
	UPROPERTY(EditInstanceOnly, Category = "BossRoom|Credits", meta = (ClampMin = "0.0"))
	float CreditsDelay = 5.f;
	// 이동할 크레딧 맵 이름.
	UPROPERTY(EditInstanceOnly, Category = "BossRoom|Credits")
	FName CreditsMapName = TEXT("CreditMap");
};
