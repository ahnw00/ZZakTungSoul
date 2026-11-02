#pragma once

#include <CoreMinimal.h>
#include <GameFramework/Actor.h>
#include <Interface/InteractableInterface.h>
#include "BossRoom.generated.h"

class ABoss;
class AWarrior;
class APlayerController;
class ALevelSequenceActor;
class ULevelSequence;
class ULevelSequencePlayer;
class USceneComponent;
class UBoxComponent;
class UArrowComponent;
class UStaticMeshComponent;
class UBossRoomSubsystem;

UENUM(BlueprintType)
enum class EBossRoomState : uint8
{
	Ready, Intro, Fighting, Cleared
};

UCLASS()
class ZZAKTUNGSOUL_API ABossRoom : public AActor, public IInteractableInterface
{
	GENERATED_BODY()

public:
	ABossRoom();

	virtual void Interact(AActor* detectedActor) override;

	//---------------------------------------------------------------
	// 플레이어 사망 등 외부 실패 처리에서도 호출 가능.
	//
	//---------------------------------------------------------------
	UFUNCTION(BlueprintCallable, Category = "BossRoom")
	void FailAttempt();

	//---------------------------------------------------------------
	// 입장 컷신 스킵.
	//
	//---------------------------------------------------------------
	UFUNCTION(BlueprintCallable, Category = "BossRoom")
	void SkipIntro();

	//---------------------------------------------------------------
	// 방 이력과 보스를 초기화하고 첫 입장 상태로 복구.
	//
	//---------------------------------------------------------------
	UFUNCTION(BlueprintCallable, Category = "BossRoom|Debug")
	void DebugResetRoom();

	UFUNCTION(BlueprintPure, Category = "BossRoom")
	EBossRoomState GetRoomState() const { return RoomState; }

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UBossRoomSubsystem* GetRoomSubsystem() const;

	bool IsFirstEntry() const;
	bool IsPlayerAlive(AWarrior* Player) const;
	bool TryEnter(AWarrior* Player, bool bFromFog);

	void BeginBattle();
	void FinishClear();
	void RefreshGate();

	void StopIntro();
	void LockPlayer(AWarrior* Player);
	void UnlockPlayer();

	UFUNCTION()
	void OnIntroFinished();

	UFUNCTION()
	void OnFirstEntryBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnInteractionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnInteractionEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	//---------------------------------------------------------------
	// 보스방 컴포넌트.
	//
	//---------------------------------------------------------------
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BossRoom|Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BossRoom|Components")
	TObjectPtr<UBoxComponent> FirstEntryTrigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BossRoom|Components")
	TObjectPtr<UBoxComponent> InteractionTrigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BossRoom|Components")
	TObjectPtr<UBoxComponent> GateBlocker;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BossRoom|Components")
	TObjectPtr<UStaticMeshComponent> FogMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BossRoom|Components")
	TObjectPtr<UArrowComponent> InsidePoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BossRoom|Components")
	TObjectPtr<UArrowComponent> OutsidePoint;

	//---------------------------------------------------------------
	// 레벨에 배치한 방마다 고유 ID와 보스 지정.
	//
	//---------------------------------------------------------------
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "BossRoom|Settings")
	FName RoomID = NAME_None;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "BossRoom|Settings")
	TObjectPtr<ABoss> BossActor;

	//---------------------------------------------------------------
	// 첫 입장 컷신.
	//
	//---------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BossRoom|Presentation")
	TObjectPtr<ULevelSequence> IntroSequence;

	//---------------------------------------------------------------
	// 실패 후에도 첫 입장처럼 안개 없이 컷신 반복.
	//
	//---------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BossRoom|Debug")
	bool bDebugAlwaysFirstEntry = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "BossRoom|State")
	EBossRoomState RoomState = EBossRoomState::Ready;

private:
	//---------------------------------------------------------------
	// 실행 중 생성한 시퀀스 플레이어와 액터.
	//
	//---------------------------------------------------------------
	UPROPERTY(Transient)
	TObjectPtr<ULevelSequencePlayer> IntroPlayer;

	UPROPERTY(Transient)
	TObjectPtr<ALevelSequenceActor> IntroActor;

	//---------------------------------------------------------------
	// 클리어 후에도 디버그 재생성이 가능하도록 보스 정보 저장.
	//
	//---------------------------------------------------------------
	UPROPERTY(Transient)
	TSubclassOf<ABoss> InitialBossClass;

	FTransform BossInitialTransform = FTransform::Identity;
	TWeakObjectPtr<AActor> InitialBossArena;

	TWeakObjectPtr<AWarrior> EncounterPlayer;
	TWeakObjectPtr<APlayerController> LockedController;
	TWeakObjectPtr<AActor> PreviousViewTarget;

	bool bPlayerInputLocked = false;
	bool bChangingState = false;
};
