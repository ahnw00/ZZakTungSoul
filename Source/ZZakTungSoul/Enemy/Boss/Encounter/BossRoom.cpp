#include <Enemy/Boss/Encounter/BossRoom.h>
#include <Enemy/Boss/Encounter/BossRoomSubsystem.h>
#include <Enemy/Boss/Boss.h>
#include <Warrior/Warrior.h>
#include <Character/CharacterStatComponent.h>
#include <Components/SceneComponent.h>
#include <Components/BoxComponent.h>
#include <Components/ArrowComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Components/CapsuleComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <GameFramework/PlayerController.h>
#include <Engine/GameInstance.h>
#include <Engine/World.h>
#include <Kismet/GameplayStatics.h>
#include <LevelSequence.h>
#include <LevelSequenceActor.h>
#include <LevelSequencePlayer.h>
#include <MovieSceneSequencePlaybackSettings.h>

ABossRoom::ABossRoom()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.1f;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	//---------------------------------------------------------------
	// 방 안쪽 첫 입장 감지.
	//
	//---------------------------------------------------------------
	FirstEntryTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("FirstEntryTrigger"));
	FirstEntryTrigger->SetupAttachment(SceneRoot);
	FirstEntryTrigger->SetBoxExtent(FVector(75, 200, 150));
	FirstEntryTrigger->SetRelativeLocation(FVector(250, 0, 150));
	FirstEntryTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	FirstEntryTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	FirstEntryTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	FirstEntryTrigger->SetGenerateOverlapEvents(true);

	//---------------------------------------------------------------
	// 방 밖 안개문 상호작용 감지.
	//
	//---------------------------------------------------------------
	InteractionTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionTrigger"));
	InteractionTrigger->SetupAttachment(SceneRoot);
	InteractionTrigger->SetBoxExtent(FVector(100, 200, 150));
	InteractionTrigger->SetRelativeLocation(FVector(-150, 0, 150));
	InteractionTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	InteractionTrigger->SetGenerateOverlapEvents(true);

	//---------------------------------------------------------------
	// 안개문 통행 차단.
	// 아래쪽이 액터 원점과 일치하도록 배치.
	//
	//---------------------------------------------------------------
	GateBlocker = CreateDefaultSubobject<UBoxComponent>(TEXT("GateBlocker"));
	GateBlocker->SetupAttachment(SceneRoot);
	GateBlocker->SetBoxExtent(FVector(20, 200, 150));
	GateBlocker->SetRelativeLocation(FVector(0, 0, 150));
	GateBlocker->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GateBlocker->SetCollisionResponseToAllChannels(ECR_Ignore);
	GateBlocker->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	GateBlocker->SetGenerateOverlapEvents(false);

	//---------------------------------------------------------------
	// 안개 외형.
	// 기본 Plane 기준으로 문 아래쪽을 원점에 맞춤.
	//
	//---------------------------------------------------------------
	FogMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FogMesh"));
	FogMesh->SetupAttachment(SceneRoot);
	FogMesh->SetRelativeLocation(FVector(0, 0, 150));
	FogMesh->SetRelativeRotation(FRotator(90, 0, 0));
	FogMesh->SetRelativeScale3D(FVector(3, 4, 1));
	FogMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FogMesh->SetGenerateOverlapEvents(false);
	FogMesh->SetVisibility(false);

	//---------------------------------------------------------------
	// 입장과 디버그 복귀 위치.
	// 캡슐 중심 높이로 배치.
	//
	//---------------------------------------------------------------
	InsidePoint = CreateDefaultSubobject<UArrowComponent>(TEXT("InsidePoint"));
	InsidePoint->SetupAttachment(SceneRoot);
	InsidePoint->SetRelativeLocation(FVector(400, 0, 100));

	OutsidePoint = CreateDefaultSubobject<UArrowComponent>(TEXT("OutsidePoint"));
	OutsidePoint->SetupAttachment(SceneRoot);
	OutsidePoint->SetRelativeLocation(FVector(-400, 0, 100));
}

void ABossRoom::BeginPlay()
{
	Super::BeginPlay();

	FirstEntryTrigger->OnComponentBeginOverlap.AddDynamic(this, &ABossRoom::OnFirstEntryBeginOverlap);
	InteractionTrigger->OnComponentBeginOverlap.AddDynamic(this, &ABossRoom::OnInteractionBeginOverlap);
	InteractionTrigger->OnComponentEndOverlap.AddDynamic(this, &ABossRoom::OnInteractionEndOverlap);

	if (RoomID.IsNone())
	{
		UE_LOG(LogTemp, Error, TEXT("BossRoom: unique RoomID is required."));
	}

	if (IsValid(BossActor.Get()))
	{
		InitialBossClass = BossActor->GetClass();
		BossInitialTransform = BossActor->GetActorTransform();
		InitialBossArena = BossActor->GetEncounterArena();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BossRoom: BossActor is required."));
	}

	//---------------------------------------------------------------
	// 같은 게임 실행 중 유지되는 클리어 이력 적용.
	//
	//---------------------------------------------------------------
	if (UBossRoomSubsystem* Progress = GetRoomSubsystem())
	{
		if (!RoomID.IsNone() && Progress->IsCleared(RoomID))
		{
			RoomState = EBossRoomState::Cleared;

			if (IsValid(BossActor.Get()))
			{
				BossActor->Destroy();
				BossActor = nullptr;
			}
		}
	}

	RefreshGate();
}

void ABossRoom::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bChangingState || (RoomState != EBossRoomState::Intro && RoomState != EBossRoomState::Fighting))
	{
		return;
	}

	//---------------------------------------------------------------
	// 보스와 플레이어가 동시에 사망했다면 클리어 우선.
	//
	//---------------------------------------------------------------
	if (IsValid(BossActor.Get()) && BossActor->GetBossPhase() == EBossPhase::Dead)
	{
		FinishClear();
		return;
	}

	if (!IsValid(BossActor.Get()) || !IsPlayerAlive(EncounterPlayer.Get()))
	{
		FailAttempt();
		return;
	}

	//---------------------------------------------------------------
	// 보스 내부 초기화 또는 연출 취소와 방 상태 동기화.
	//
	//---------------------------------------------------------------
	if (RoomState == EBossRoomState::Fighting && BossActor->GetBossPhase() == EBossPhase::Waiting)
	{
		FailAttempt();
	}
}

void ABossRoom::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	bChangingState = true;
	StopIntro();

	//---------------------------------------------------------------
	// 플레이어의 상호작용 목록에서 제거.
	//
	//---------------------------------------------------------------
	TArray<AActor*> Players;
	InteractionTrigger->GetOverlappingActors(Players, AWarrior::StaticClass());

	for (AActor* Actor : Players)
	{
		if (AWarrior* Player = Cast<AWarrior>(Actor))
		{
			Player->RemoveDetectedObject(this);
		}
	}

	Super::EndPlay(EndPlayReason);
}

UBossRoomSubsystem* ABossRoom::GetRoomSubsystem() const
{
	UGameInstance* Instance = GetGameInstance();
	return Instance ? Instance->GetSubsystem<UBossRoomSubsystem>() : nullptr;
}

bool ABossRoom::IsFirstEntry() const
{
	if (bDebugAlwaysFirstEntry)
	{
		return true;
	}

	const UBossRoomSubsystem* Progress = GetRoomSubsystem();
	return Progress && !Progress->HasEntered(RoomID);
}

bool ABossRoom::IsPlayerAlive(AWarrior* Player) const
{
	if (!IsValid(Player) || !Player->IsPlayerControlled())
	{
		return false;
	}

	const UCharacterStatComponent* Stat = Player->GetCharacterStatComponent();
	return true;//Stat && Stat->GetCurrentHP() > 0.0f;
}

void ABossRoom::OnFirstEntryBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AWarrior* Player = Cast<AWarrior>(OtherActor);

	if (Player && OtherComp == Player->GetCapsuleComponent() && IsFirstEntry())
	{
		TryEnter(Player, false);
	}
}

void ABossRoom::OnInteractionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AWarrior* Player = Cast<AWarrior>(OtherActor))
	{
		if (OtherComp == Player->GetCapsuleComponent())
		{
			Player->AddDetectedObject(this);
		}
	}
}

void ABossRoom::OnInteractionEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AWarrior* Player = Cast<AWarrior>(OtherActor))
	{
		if (OtherComp == Player->GetCapsuleComponent())
		{
			Player->RemoveDetectedObject(this);
		}
	}
}

void ABossRoom::Interact(AActor* detectedActor)
{
	AWarrior* Player = Cast<AWarrior>(detectedActor);

	if (!Player || !InteractionTrigger->IsOverlappingActor(Player))
	{
		return;
	}

	//---------------------------------------------------------------
	// 첫 입장은 자동 감지.
	// 재시도는 안개문 앞 상호작용.
	//
	//---------------------------------------------------------------
	if (!IsFirstEntry())
	{
		TryEnter(Player, true);
	}
}

bool ABossRoom::TryEnter(AWarrior* Player, bool bFromFog)
{
	if (bChangingState || RoomState != EBossRoomState::Ready || RoomID.IsNone() ||
		!GetRoomSubsystem() || !IsPlayerAlive(Player) || !IsValid(BossActor.Get()) ||
		BossActor->GetBossPhase() != EBossPhase::Waiting)
	{
		return false;
	}

	const bool bFirst = IsFirstEntry();

	//---------------------------------------------------------------
	// 첫 입장은 자동 트리거만 허용.
	// 재시도는 안개문 상호작용만 허용.
	//
	//---------------------------------------------------------------
	if ((bFirst && bFromFog) || (!bFirst && !bFromFog))
	{
		return false;
	}

	if (bFirst)
	{
		if (!IntroSequence)
		{
			UE_LOG(LogTemp, Error, TEXT("BossRoom: IntroSequence is required for first entry."));
			return false;
		}

		//---------------------------------------------------------------
		// UE 5.8 시퀀스 재생 설정.
		//
		//---------------------------------------------------------------
		FMovieSceneSequencePlaybackSettings Settings;
		Settings.bAutoPlay = false;
		Settings.LoopCount.Value = 0;
		Settings.bPauseAtEnd = false;
		Settings.FinishCompletionStateOverride = EMovieSceneCompletionModeOverride::ForceRestoreState;

		//---------------------------------------------------------------
		// 출력 인자는 일반 포인터로 받은 뒤 TObjectPtr에 보관.
		//
		//---------------------------------------------------------------
		ALevelSequenceActor* CreatedIntroActor = nullptr;

		IntroPlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), IntroSequence.Get(), Settings, CreatedIntroActor);
		IntroActor = CreatedIntroActor;

		if (!IsValid(IntroPlayer.Get()) || !IsValid(IntroActor.Get()))
		{
			StopIntro();
			UE_LOG(LogTemp, Error, TEXT("BossRoom: failed to create intro player."));
			return false;
		}
	}

	bChangingState = true;
	EncounterPlayer = Player;
	RoomState = EBossRoomState::Intro;

	//---------------------------------------------------------------
	// 재입장은 문 안쪽 안전 위치로 이동.
	//
	//---------------------------------------------------------------
	if (bFromFog)
	{
		Player->GetCharacterMovement()->StopMovementImmediately();

		if (!Player->TeleportTo(InsidePoint->GetComponentLocation(), InsidePoint->GetComponentRotation()))
		{
			EncounterPlayer.Reset();
			RoomState = EBossRoomState::Ready;
			bChangingState = false;

			RefreshGate();

			UE_LOG(LogTemp, Warning, TEXT("BossRoom: inside entry point is blocked."));
			return false;
		}
	}

	GetRoomSubsystem()->MarkEntered(RoomID);
	RefreshGate();

	bChangingState = false;

	if (bFirst)
	{
		//---------------------------------------------------------------
		// 해당 태그가 있는 시퀀스에만 실제 액터 연결.
		// 카메라 전용 시퀀스에서는 바인딩 생략.
		//
		//---------------------------------------------------------------
		const FName PlayerTag = TEXT("Player");
		const FName BossTag = TEXT("Boss");

		if (IntroActor->FindNamedBindings(PlayerTag).Num() > 0)
		{
			TArray<AActor*> PlayerBindings;
			PlayerBindings.Add(Player);

			IntroActor->SetBindingByTag(PlayerTag, PlayerBindings, false);
		}

		if (IntroActor->FindNamedBindings(BossTag).Num() > 0)
		{
			TArray<AActor*> BossBindings;
			BossBindings.Add(BossActor.Get());

			IntroActor->SetBindingByTag(BossTag, BossBindings, false);
		}

		LockPlayer(Player);

		IntroPlayer->OnFinished.AddDynamic(this, &ABossRoom::OnIntroFinished);
		IntroPlayer->Play();
	}
	else
	{
		BeginBattle();
	}

	return true;
}

void ABossRoom::LockPlayer(AWarrior* Player)
{
	APlayerController* Controller = Player ? Cast<APlayerController>(Player->GetController()) : nullptr;

	if (!Controller || bPlayerInputLocked)
	{
		return;
	}

	LockedController = Controller;
	PreviousViewTarget = Controller->GetViewTarget();

	Player->GetCharacterMovement()->StopMovementImmediately();

	Controller->SetIgnoreMoveInput(true);
	Controller->SetIgnoreLookInput(true);

	bPlayerInputLocked = true;
}

void ABossRoom::UnlockPlayer()
{
	if (APlayerController* Controller = LockedController.Get())
	{
		if (bPlayerInputLocked)
		{
			Controller->SetIgnoreMoveInput(false);
			Controller->SetIgnoreLookInput(false);
		}

		if (PreviousViewTarget.IsValid())
		{
			Controller->SetViewTarget(PreviousViewTarget.Get());
		}
	}

	bPlayerInputLocked = false;
	LockedController.Reset();
	PreviousViewTarget.Reset();
}

void ABossRoom::StopIntro()
{
	//---------------------------------------------------------------
	// 시퀀스 중단 전에 완료 콜백부터 해제.
	//
	//---------------------------------------------------------------
	if (IsValid(IntroPlayer.Get()))
	{
		IntroPlayer->OnFinished.RemoveDynamic(this, &ABossRoom::OnIntroFinished);
		IntroPlayer->Stop();
	}

	UnlockPlayer();

	IntroPlayer = nullptr;

	if (IsValid(IntroActor.Get()))
	{
		IntroActor->Destroy();
	}

	IntroActor = nullptr;
}

void ABossRoom::OnIntroFinished()
{
	if (bChangingState || RoomState != EBossRoomState::Intro)
	{
		return;
	}

	StopIntro();
	BeginBattle();
}

void ABossRoom::SkipIntro()
{
	if (!bChangingState && RoomState == EBossRoomState::Intro && IsValid(IntroPlayer.Get()))
	{
		OnIntroFinished();
	}
}

void ABossRoom::BeginBattle()
{
	if (bChangingState || RoomState != EBossRoomState::Intro)
	{
		return;
	}

	if (!IsPlayerAlive(EncounterPlayer.Get()) || !IsValid(BossActor.Get()))
	{
		FailAttempt();
		return;
	}

	RoomState = EBossRoomState::Fighting;
	RefreshGate();

	if (!BossActor->StartEncounter(EncounterPlayer.Get()))
	{
		UE_LOG(LogTemp, Error, TEXT("BossRoom: boss encounter failed to start."));
		FailAttempt();
	}
}

void ABossRoom::FailAttempt()
{
	if (bChangingState || RoomState == EBossRoomState::Ready || RoomState == EBossRoomState::Cleared)
	{
		return;
	}

	if (IsValid(BossActor.Get()) && BossActor->GetBossPhase() == EBossPhase::Dead)
	{
		FinishClear();
		return;
	}

	bChangingState = true;

	StopIntro();

	if (IsValid(BossActor.Get()))
	{
		BossActor->ResetEncounter();
	}

	EncounterPlayer.Reset();
	RoomState = EBossRoomState::Ready;

	bChangingState = false;
	RefreshGate();
}

void ABossRoom::FinishClear()
{
	if (bChangingState || RoomState == EBossRoomState::Cleared)
	{
		return;
	}

	bChangingState = true;

	StopIntro();

	RoomState = EBossRoomState::Cleared;

	if (UBossRoomSubsystem* Progress = GetRoomSubsystem())
	{
		Progress->MarkCleared(RoomID);
	}

	EncounterPlayer.Reset();

	bChangingState = false;
	RefreshGate();
}

void ABossRoom::RefreshGate()
{
	//---------------------------------------------------------------
	// 첫 접근은 열림.
	// 입장 연출과 전투 중에는 닫힘.
	// 재시도 대기는 닫힘.
	// 클리어 이후는 열림.
	//
	//---------------------------------------------------------------
	const bool bActive = RoomState == EBossRoomState::Intro || RoomState == EBossRoomState::Fighting;
	const bool bRetry = RoomState == EBossRoomState::Ready && !IsFirstEntry();
	const bool bClosed = RoomState != EBossRoomState::Cleared && (bActive || bRetry);

	FogMesh->SetVisibility(bClosed, true);
	GateBlocker->SetCollisionEnabled(bClosed ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
}

void ABossRoom::DebugResetRoom()
{
#if !UE_BUILD_SHIPPING
	if (bChangingState || !GetRoomSubsystem() || RoomID.IsNone() || !InitialBossClass)
	{
		return;
	}

	bChangingState = true;

	StopIntro();

	AWarrior* Player = Cast<AWarrior>(UGameplayStatics::GetPlayerPawn(this, 0));

	//---------------------------------------------------------------
	// 기존 보스가 살아 있다면 먼저 전투와 기믹 정리.
	//
	//---------------------------------------------------------------
	if (IsValid(BossActor.Get()))
	{
		BossActor->ResetEncounter();
	}

	//---------------------------------------------------------------
	// 재생성은 보스 BP 클래스 기본값 사용.
	// 전투 영역은 다시 연결하고 배치한 스케일을 그대로 사용.
	//
	//---------------------------------------------------------------
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	Params.TransformScaleMethod = ESpawnActorScaleMethod::OverrideRootScale;

	ABoss* NewBoss = GetWorld()->SpawnActor<ABoss>(InitialBossClass.Get(), BossInitialTransform, Params);

	if (!IsValid(NewBoss))
	{
		bChangingState = false;

		UE_LOG(LogTemp, Error, TEXT("BossRoom: debug boss respawn failed."));
		return;
	}

	if (IsValid(BossActor.Get()))
	{
		BossActor->Destroy();
	}

	NewBoss->SetEncounterArena(InitialBossArena.Get());
	BossActor = NewBoss;

	GetRoomSubsystem()->ResetRoom(RoomID);

	RoomState = EBossRoomState::Ready;
	EncounterPlayer.Reset();

	//---------------------------------------------------------------
	// 초기화 중에는 입장 트리거가 다시 작동하지 않도록 유지.
	//
	//---------------------------------------------------------------
	if (IsPlayerAlive(Player))
	{
		Player->GetCharacterMovement()->StopMovementImmediately();

		if (!Player->TeleportTo(OutsidePoint->GetComponentLocation(), OutsidePoint->GetComponentRotation()))
		{
			UE_LOG(LogTemp, Warning, TEXT("BossRoom: outside debug point is blocked."));
		}
	}

	bChangingState = false;
	RefreshGate();
#endif
}
