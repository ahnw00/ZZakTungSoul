#include <Enemy/Boss/Encounter/BossRoom.h>
#include <Enemy/Boss/Encounter/BossRoomSubsystem.h>
#include <Enemy/Boss/Boss.h>
#include <Enemy/Boss/Clown/BossClown.h>
#include <Warrior/Warrior.h>
#include <Animation/AnimInstance.h>
#include <Animation/AnimMontage.h>
#include <Components/SceneComponent.h>
#include <Components/BoxComponent.h>
#include <Components/ArrowComponent.h>
#include <Components/AudioComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Components/CapsuleComponent.h>
#include <Components/SkeletalMeshComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <GameFramework/PlayerController.h>
#include <Engine/GameInstance.h>
#include <Engine/World.h>
#include <Kismet/GameplayStatics.h>
#include <Materials/MaterialInstanceDynamic.h>
#include <UObject/ConstructorHelpers.h>
#include <LevelSequence.h>
#include <LevelSequenceActor.h>
#include <LevelSequencePlayer.h>
#include <MovieScene.h>
#include <MovieSceneSequencePlaybackSettings.h>
#include <Sound/SoundAttenuation.h>
#include <Sound/SoundBase.h>
#include "UI/BossHUDWidget.h"
#include "Components/InputComponent.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Engine/LocalPlayer.h"
#include "UI/Notify/NotificationSubsystem.h"
#include "GamePlay/ZTSGameMode.h"
#include <GamePlay/MusicSubsystem.h>

ABossRoom::ABossRoom()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.1f;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> FogEntry(
		TEXT("/Game/Blueprints/Enemy/Boss/Clown/Animation/Encounter/AM_ClownFogEnter_Warrior.AM_ClownFogEnter_Warrior"));
	ClownFogEntryMontage = FogEntry.Object;
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> FogOpening(
		TEXT("/Game/Blueprints/Enemy/Boss/Clown/VFX/M_ClownFogOpening.M_ClownFogOpening"));
	ClownFogOpeningMaterial = FogOpening.Object;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	// 방 안쪽 첫 입장 감지.
	FirstEntryTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("FirstEntryTrigger"));
	FirstEntryTrigger->SetupAttachment(SceneRoot);
	FirstEntryTrigger->SetBoxExtent(FVector(75, 200, 150));
	FirstEntryTrigger->SetRelativeLocation(FVector(250, 0, 150));
	FirstEntryTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	FirstEntryTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	FirstEntryTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	FirstEntryTrigger->SetGenerateOverlapEvents(true);

	// 방 밖 안개문 상호작용 감지.
	InteractionTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionTrigger"));
	InteractionTrigger->SetupAttachment(SceneRoot);
	InteractionTrigger->SetBoxExtent(FVector(100, 200, 150));
	InteractionTrigger->SetRelativeLocation(FVector(-150, 0, 150));
	InteractionTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	InteractionTrigger->SetGenerateOverlapEvents(true);

	// 안개문 통행 차단.
	// 아래쪽이 액터 원점과 일치하도록 배치.
	GateBlocker = CreateDefaultSubobject<UBoxComponent>(TEXT("GateBlocker"));
	GateBlocker->SetupAttachment(SceneRoot);
	GateBlocker->SetBoxExtent(FVector(20, 200, 150));
	GateBlocker->SetRelativeLocation(FVector(0, 0, 150));
	GateBlocker->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GateBlocker->SetCollisionResponseToAllChannels(ECR_Ignore);
	GateBlocker->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	GateBlocker->SetGenerateOverlapEvents(false);

	OhterSideGateBlocker = CreateDefaultSubobject<UBoxComponent>(TEXT("OhterSideGateBlocker"));
	OhterSideGateBlocker->SetupAttachment(SceneRoot);
	OhterSideGateBlocker->SetBoxExtent(FVector(20, 200, 150));
	OhterSideGateBlocker->SetRelativeLocation(FVector(0, 0, 150));
	OhterSideGateBlocker->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	OhterSideGateBlocker->SetCollisionResponseToAllChannels(ECR_Ignore);
	OhterSideGateBlocker->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	OhterSideGateBlocker->SetGenerateOverlapEvents(false);

	OtherSideFirstEntryTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("OtherSideFirstEntryTrigger"));
	OtherSideFirstEntryTrigger->SetupAttachment(OhterSideGateBlocker);
	OtherSideFirstEntryTrigger->SetBoxExtent(FVector(75, 200, 150));
	OtherSideFirstEntryTrigger->SetRelativeLocation(FVector(-250, 0, 0));
	OtherSideFirstEntryTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	OtherSideFirstEntryTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	OtherSideFirstEntryTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	OtherSideFirstEntryTrigger->SetGenerateOverlapEvents(true);

	OtherSideInteractionTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("OtherSideInteractionTrigger"));
	OtherSideInteractionTrigger->SetupAttachment(OhterSideGateBlocker);
	OtherSideInteractionTrigger->SetBoxExtent(FVector(100, 200, 150));
	OtherSideInteractionTrigger->SetRelativeLocation(FVector(150, 0, 0));
	OtherSideInteractionTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	OtherSideInteractionTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	OtherSideInteractionTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	OtherSideInteractionTrigger->SetGenerateOverlapEvents(true);

	OtherSideInsidePoint = CreateDefaultSubobject<UArrowComponent>(TEXT("OtherSideInsidePoint"));
	OtherSideInsidePoint->SetupAttachment(OhterSideGateBlocker);
	OtherSideInsidePoint->SetRelativeLocation(FVector(-400, 0, -50));
	OtherSideInsidePoint->SetRelativeRotation(FRotator(0, 180, 0));

	// 안개 외형.
	// 기본 Plane 기준으로 문 아래쪽을 원점에 맞춤.
	FogMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FogMesh"));
	FogMesh->SetupAttachment(SceneRoot);
	FogMesh->SetRelativeLocation(FVector(0, 0, 150));

	FogMesh->SetRelativeScale3D(FVector(3, 4, 1));
	FogMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FogMesh->SetGenerateOverlapEvents(false);
	FogMesh->SetVisibility(false);

	OhterSideFogMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OhterSideFogMesh"));
	OhterSideFogMesh->SetupAttachment(OhterSideGateBlocker);

	OhterSideFogMesh->SetRelativeScale3D(FVector(3, 4, 1));
	OhterSideFogMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	OhterSideFogMesh->SetGenerateOverlapEvents(false);
	OhterSideFogMesh->SetVisibility(false);

	// 입장과 디버그 복귀 위치.
	// 캡슐 중심 높이로 배치.
	InsidePoint = CreateDefaultSubobject<UArrowComponent>(TEXT("InsidePoint"));
	InsidePoint->SetupAttachment(SceneRoot);
	InsidePoint->SetRelativeLocation(FVector(400, 0, 100));

	ClownApproachMusic = CreateDefaultSubobject<UAudioComponent>(TEXT("ClownApproachMusic"));
	ClownApproachMusic->SetupAttachment(InsidePoint);
	ClownApproachMusic->SetAutoActivate(false);

	static ConstructorHelpers::FObjectFinder<USoundBase> ApproachSong(TEXT(
		"/Game/Assets/Sound/Clown/SFX_ApproachSong.SFX_ApproachSong"));
	ClownApproachMusic->SetSound(ApproachSong.Object);
	ClownApproachMusic->SetVolumeMultiplier(0.18f);

	FSoundAttenuationSettings Attenuation;
	Attenuation.AttenuationShapeExtents = FVector(400.f, 0.f, 0.f);
	Attenuation.FalloffDistance = 2100.f;
	Attenuation.bAttenuateWithLPF = true;
	Attenuation.LPFRadiusMin = 450.f;
	Attenuation.LPFRadiusMax = 2000.f;
	Attenuation.LPFFrequencyAtMin = 7000.f;
	Attenuation.LPFFrequencyAtMax = 1600.f;
	ClownApproachMusic->AdjustAttenuation(Attenuation);

	EncounterMusic = CreateDefaultSubobject<UAudioComponent>(TEXT("EncounterMusic"));
	EncounterMusic->SetupAttachment(SceneRoot);
	EncounterMusic->SetAutoActivate(false);
	EncounterMusic->bAllowSpatialization = false;
}

void ABossRoom::PostLoad()
{
	Super::PostLoad();

	if (EncounterMusicVolume >= 0.f)
	{
		Phase1CombatMusicVolume = EncounterMusicVolume;
		Phase2SearchMusicVolume = EncounterMusicVolume;
		Phase2CombatMusicVolume = EncounterMusicVolume;
		EncounterMusicVolume = -1.f;
	}
}

void ABossRoom::PostRegisterAllComponents()
{
	Super::PostRegisterAllComponents();

	// 기존 레벨에 저장된 부모와 오프셋도 차단 박스 기준으로 맞춤.
	if (OhterSideFogMesh->GetAttachParent() != OhterSideGateBlocker)
	{
		OhterSideFogMesh->AttachToComponent(OhterSideGateBlocker, FAttachmentTransformRules::KeepRelativeTransform);
	}

	OhterSideFogMesh->SetRelativeLocationAndRotation(FVector::ZeroVector, FRotator::ZeroRotator);

	// 안개 메시의 세로 범위가 차단 박스보다 작으면 바닥까지 늘림.
	const auto FitFogHeight = [](UStaticMeshComponent* Fog, const UBoxComponent* Blocker)
	{
		const double FogHalfHeight = Fog->CalcBounds(Fog->GetComponentTransform()).BoxExtent.Z;
		const double GateHalfHeight = Blocker->GetScaledBoxExtent().Z;
		if (FogHalfHeight > UE_KINDA_SMALL_NUMBER && FogHalfHeight < GateHalfHeight)
		{
			FVector Scale = Fog->GetRelativeScale3D();
			Scale.Z *= GateHalfHeight / FogHalfHeight;
			Fog->SetRelativeScale3D(Scale);
		}
	};

	FitFogHeight(FogMesh, GateBlocker);
	FitFogHeight(OhterSideFogMesh, OhterSideGateBlocker);
}

void ABossRoom::BeginPlay()
{
	Super::BeginPlay();

	FirstEntryTrigger->OnComponentBeginOverlap.AddDynamic(this, &ABossRoom::OnFirstEntryBeginOverlap);
	InteractionTrigger->OnComponentBeginOverlap.AddDynamic(this, &ABossRoom::OnInteractionBeginOverlap);
	InteractionTrigger->OnComponentEndOverlap.AddDynamic(this, &ABossRoom::OnInteractionEndOverlap);

	OtherSideFirstEntryTrigger->OnComponentBeginOverlap.AddDynamic(this, &ABossRoom::OnFirstEntryBeginOverlap);
	OtherSideInteractionTrigger->OnComponentBeginOverlap.AddDynamic(this, &ABossRoom::OnInteractionBeginOverlap);
	OtherSideInteractionTrigger->OnComponentEndOverlap.AddDynamic(this, &ABossRoom::OnInteractionEndOverlap);

	if (RoomID.IsNone())
	{
		UE_LOG(LogTemp, Error, TEXT("BossRoom: unique RoomID is required."));
	}

	if (IsValid(BossActor.Get()))
	{
		InitialBossClass = BossActor->GetClass();
		BossInitialTransform = BossActor->GetActorTransform();
		InitialBossArena = BossActor->GetEncounterArena();

		const UMovieScene* MovieScene = IntroSequence ? IntroSequence->GetMovieScene() : nullptr;
		if (MovieScene && BossActor->IsA<ABossClown>())
		{
			const int32 CutIndex = MovieScene->FindMarkedFrameByLabel(TEXT("ClownApproachMusicCut"));
			if (CutIndex != INDEX_NONE)
			{
				ClownApproachMusicCutFrame = MovieScene->GetMarkedFrames()[CutIndex].FrameNumber;
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BossRoom: BossActor is required."));
	}

	bRoomInitialized = true;
	ApplySavedProgress();
}

void ABossRoom::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bFogEntryActive)
	{
		TickClownFogEntry(DeltaSeconds);
		return;
	}

	if (bChangingState ||
		(RoomState != EBossRoomState::Intro &&
			RoomState != EBossRoomState::Fighting))
	{
		return;
	}

	// 동시 사망이면 보스 처치 성공을 우선
	if (IsValid(BossActor.Get()) &&
		BossActor->GetBossPhase() == EBossPhase::Dead)
	{
		FinishClear();
		return;
	}

	// 보스가 없어졌거나 플레이어가 사망한 경우
	if (!IsValid(BossActor.Get()) ||
		!IsPlayerAlive(EncounterPlayer.Get()))
	{
		FailAttempt();
		return;
	}

	// 보스가 내부적으로 초기화됐다면 방도 실패 처리
	if (RoomState == EBossRoomState::Fighting &&
		BossActor->GetBossPhase() == EBossPhase::Waiting)
	{
		FailAttempt();
		return;
	}

	if (RoomState == EBossRoomState::Fighting)
	{
		UpdateEncounterMusic();
	}
}

void ABossRoom::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	EndClownFogEntry(false);

	if (IsValid(BossActor.Get()))
	{
		BossActor->OnPhase2StageRequested.RemoveDynamic(this, &ABossRoom::UpdateBossHUDForPhase2);
	}

	bChangingState = true;
	StopIntro();
	SetEncounterMusic(nullptr);
	HideBossHUD();

	// 플레이어의 상호작용 목록에서 제거.
	TArray<AActor*> Players;
	InteractionTrigger->GetOverlappingActors(Players, AWarrior::StaticClass());
	TArray<AActor*> OtherSidePlayers;
	OtherSideInteractionTrigger->GetOverlappingActors(OtherSidePlayers, AWarrior::StaticClass());
	for (AActor* Actor : OtherSidePlayers) Players.AddUnique(Actor);

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

	const UBossRoomSubsystem* Progress = GetRoomSubsystem();

	return Progress && !Progress->HasEntered(RoomID);
}

bool ABossRoom::IsPlayerAlive(AWarrior* Player) const
{
	if (!IsValid(Player) || !Player->IsPlayerControlled())
	{
		return false;
	}

	return !Player->IsDead();
}

void ABossRoom::OnFirstEntryBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AWarrior* Player = Cast<AWarrior>(OtherActor);

	const bool bOtherSide = OverlappedComp == OtherSideFirstEntryTrigger;

	if (Player && OtherComp == Player->GetCapsuleComponent() && IsFirstEntry() && (!bOtherSide || bEnableOtherSideGate))
	{
		TryEnter(Player, false, bOtherSide);
	}
}

void ABossRoom::OnInteractionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AWarrior* Player = Cast<AWarrior>(OtherActor))
	{
		if (OtherComp == Player->GetCapsuleComponent() && (OverlappedComp != OtherSideInteractionTrigger || bEnableOtherSideGate))
		{
			Player->AddDetectedObject(this);
		}
	}
}

void ABossRoom::OnInteractionEndOverlap(
	UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AWarrior* Player = Cast<AWarrior>(OtherActor))
	{
		if (OtherComp == Player->GetCapsuleComponent() && !IsPlayerAtGate(Player))
		{
			Player->RemoveDetectedObject(this);
		}
	}
}

void ABossRoom::UpdateBossHUDForPhase2(ABoss* Boss, EBossPhase2TransitionStage Stage, int32 TransitionID)
{
	if (bChangingState || RoomState != EBossRoomState::Fighting || Boss != BossActor.Get() || !IsValid(BossHUDWidget.Get()) ||
		Boss->GetPhase2TransitionID() != TransitionID || Boss->GetPhase2TransitionStage() != Stage)
	{
		return;
	}

	const ESlateVisibility Visibility = Stage == EBossPhase2TransitionStage::Searching ? ESlateVisibility::Hidden : ESlateVisibility::HitTestInvisible;
	BossHUDWidget->SetVisibility(Visibility);
}

void ABossRoom::ShowBossHUD()
{
	GetWorldTimerManager().ClearTimer(BossHUDHideTimer);

	if (!IsValid(BossActor.Get()))
	{
		return;
	}

	if (!BossHUDWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("BossRoom: BossHUDWidgetClass is missing."));
		return;
	}

	if (!IsValid(BossHUDWidget.Get()))
	{
		BossHUDWidget = CreateWidget<UBossHUDWidget>(
			GetWorld(),
			BossHUDWidgetClass
		);
	}

	if (!IsValid(BossHUDWidget.Get()))
	{
		return;
	}

	if (!BossHUDWidget->IsInViewport())
	{
		BossHUDWidget->AddToViewport(1);
	}

	BossHUDWidget->SetVisibility(ESlateVisibility::HitTestInvisible);

	// 이 방에 지정된 실제 보스 연결
	BossHUDWidget->BindBoss(BossActor.Get());
}

void ABossRoom::HideBossHUD()
{
	GetWorldTimerManager().ClearTimer(BossHUDHideTimer);

	if (!IsValid(BossHUDWidget.Get()))
	{
		return;
	}

	BossHUDWidget->BindBoss(nullptr);
	BossHUDWidget->RemoveFromParent();
	BossHUDWidget = nullptr;
}

void ABossRoom::Interact(AActor* detectedActor)
{
	AWarrior* Player = Cast<AWarrior>(detectedActor);

	if (!IsPlayerAtGate(Player))
	{
		return;
	}

	// 첫 입장은 자동 감지.
	// 재시도는 안개문 앞 상호작용.
	if (!IsFirstEntry())
	{
		const bool bOtherSide = !InteractionTrigger->IsOverlappingComponent(Player->GetCapsuleComponent());
		TryEnter(Player, true, bOtherSide);
	}
}

bool ABossRoom::IsPlayerAtGate(const AWarrior* Player) const
{
	return IsValid(Player) && (InteractionTrigger->IsOverlappingComponent(Player->GetCapsuleComponent()) ||
		(bEnableOtherSideGate && OtherSideInteractionTrigger->IsOverlappingComponent(Player->GetCapsuleComponent())));
}

void ABossRoom::SetOtherSideGateEnabled(bool bEnabled)
{
	if (bEnableOtherSideGate == bEnabled)
	{
		return;
	}

	TArray<AActor*> Players;
	OtherSideInteractionTrigger->GetOverlappingActors(Players, AWarrior::StaticClass());
	bEnableOtherSideGate = bEnabled;

	if (!HasActorBegunPlay())
	{
		return;
	}
	RefreshGate();

	for (AActor* Actor : Players)
	{
		if (AWarrior* Player = Cast<AWarrior>(Actor))
		{
			if (!IsPlayerAtGate(Player))
			{
				Player->RemoveDetectedObject(this);
			}
		}
	}
}

bool ABossRoom::TryEnter(AWarrior* Player, bool bFromFog, bool bOtherSide)
{
	if (bChangingState || bFogEntryActive || RoomState != EBossRoomState::Ready || RoomID.IsNone() || !GetRoomSubsystem() ||
		!IsPlayerAlive(Player) || !IsValid(BossActor.Get()) || BossActor->GetBossPhase() != EBossPhase::Waiting)
	{
		return false;
	}

	const bool bFirst = IsFirstEntry();

	// 첫 입장은 자동 트리거만 허용.
	// 재시도는 안개문 상호작용만 허용.
	if ((bFirst && bFromFog) || (!bFirst && !bFromFog))
	{
		return false;
	}

	if (bFromFog && BossActor->IsA<ABossClown>())
	{
		return BeginClownFogEntry(Player, bOtherSide);
	}

	if (bFirst)
	{
		if (!IntroSequence)
		{
			UE_LOG(LogTemp, Error, TEXT("BossRoom: IntroSequence is required for first entry."));

			return false;
		}

		// UE 5.8 시퀀스 재생 설정.
		FMovieSceneSequencePlaybackSettings Settings;
		Settings.bAutoPlay = false;
		Settings.LoopCount.Value = 0;
		Settings.bPauseAtEnd = false;
		Settings.FinishCompletionStateOverride = EMovieSceneCompletionModeOverride::ForceRestoreState;

		// 출력 인자는 일반 포인터로 받은 뒤 TObjectPtr에 보관.
		ALevelSequenceActor* CreatedIntroActor = nullptr;

		IntroPlayer = ULevelSequencePlayer::CreateLevelSequencePlayer( GetWorld(), IntroSequence.Get(), Settings, CreatedIntroActor);
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

	// 재입장은 문 안쪽 안전 위치로 이동.
	if (bFromFog)
	{
		Player->GetCharacterMovement()->StopMovementImmediately();

		const UArrowComponent* EntryPoint = bOtherSide ? OtherSideInsidePoint.Get() : InsidePoint.Get();
		if (!Player->TeleportTo(EntryPoint->GetComponentLocation(), EntryPoint->GetComponentRotation()))
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

	// 최초 입장 시에만 자동 저장
	if (bFirst)
	{
		if (AZTSGameMode* GM = Cast<AZTSGameMode>(GetWorld()->GetAuthGameMode()))
		{
			GM->SaveGame();
		}
	}

	RefreshGate();

	bChangingState = false;

	if (bFirst)
	{
		// 해당 태그가 있는 시퀀스에만 실제 액터 연결.
		// 카메라 전용 시퀀스에서는 바인딩 생략.
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

		if (ClownApproachMusicCutFrame.IsSet())
		{
			IntroPlayer->OnSequenceUpdated().AddUObject(this, &ABossRoom::OnIntroSequenceUpdated);
		}

		IntroPlayer->OnFinished.AddDynamic(this, &ABossRoom::OnIntroFinished);
		IntroPlayer->Play();
	}
	else
	{
		BeginBattle();
	}

	return true;
}

bool ABossRoom::BeginClownFogEntry(AWarrior* Player, bool bOtherSide)
{
	UCharacterMovementComponent* Movement = Player->GetCharacterMovement();
	UAnimInstance* Anim = Player->GetMesh()->GetAnimInstance();
	const APlayerController* Controller = Cast<APlayerController>(Player->GetController());
	if (!ClownFogEntryMontage || !Anim || !Movement->IsMovingOnGround() ||
		Player->GetActionState() != EActionState::None || !Controller)
	{
		return false;
	}

	UBoxComponent* Gate = bOtherSide ? OhterSideGateBlocker.Get() : GateBlocker.Get();
	const UArrowComponent* Inside = bOtherSide ? OtherSideInsidePoint.Get() : InsidePoint.Get();
	const FVector GateCenter = Gate->GetComponentLocation();
	FVector Inward = Gate->GetForwardVector().GetSafeNormal2D();
	if (FVector::DotProduct(Inside->GetComponentLocation() - GateCenter, Inward) < 0.0)
	{
		Inward *= -1.0;
	}

	const UCapsuleComponent* Capsule = Player->GetCapsuleComponent();
	const FVector Extent = Gate->GetScaledBoxExtent();
	const double Radius = Capsule->GetScaledCapsuleRadius();
	const FVector Across = FVector::CrossProduct(FVector::UpVector, Inward);
	const double SideLimit = FMath::Max(Extent.Y - Radius - 6.0, 0.0);
	FogEntryStart = Player->GetActorLocation();
	const double Side = FMath::Clamp(FVector::DotProduct(FogEntryStart - GateCenter, Across), -SideLimit, SideLimit);
	FVector GatePath = GateCenter + Across * Side;
	GatePath.Z = FogEntryStart.Z;
	FogEntryEnd = GatePath + Inward * (Extent.X + Radius + 36.0);
	FogEntryStartRotation = Player->GetActorQuat();
	FogEntryFacing = Inward.ToOrientationQuat();
	FogEntryStartControlRotation = Controller->GetControlRotation();

	FogEntryDuration = Anim->Montage_Play(ClownFogEntryMontage, 1.0f, EMontagePlayReturnType::Duration);
	if (FogEntryDuration <= 0.0f)
	{
		return false;
	}

	EncounterPlayer = Player;
	TraversedFogGate = Gate;
	if (ClownFogOpeningMaterial)
	{
		UStaticMeshComponent* Visual = bOtherSide ? OhterSideFogMesh.Get() : FogMesh.Get();
		TraversedFogVisual = Visual;
		FogEntryOriginalMaterial = Visual->GetMaterial(0);
		FogEntryOpening = Visual->CreateDynamicMaterialInstance(0, ClownFogOpeningMaterial);
	}
	FogEntryElapsed = 0.0f;
	bFogEntryActive = true;
	FogEntryPreviousTickInterval = GetActorTickInterval();
	SetActorTickInterval(0.0f);
	LockPlayer(Player);
	if (Player->IsLockOn())
	{
		Player->ChangeLockOn(false);
	}
	Movement->DisableMovement();
	Player->GetCapsuleComponent()->IgnoreComponentWhenMoving(Gate, true);

	for (USceneComponent* Child : Player->GetMesh()->GetAttachChildren())
	{
		USkeletalMeshComponent* HeldMesh = Cast<USkeletalMeshComponent>(Child);
		if (HeldMesh && (HeldMesh->GetFName() == TEXT("Weapon") || HeldMesh->GetFName() == TEXT("LeftHandWeapon")))
		{
			FogEntryWeaponVisibility.Add(HeldMesh, HeldMesh->IsVisible());
			HeldMesh->SetVisibility(false);
		}
	}

	return true;
}

void ABossRoom::TickClownFogEntry(float DeltaSeconds)
{
	AWarrior* Player = EncounterPlayer.Get();
	if (!IsPlayerAlive(Player) || !TraversedFogGate.IsValid() ||
		!IsValid(BossActor.Get()) || BossActor->GetBossPhase() != EBossPhase::Waiting)
	{
		EndClownFogEntry(false);
		return;
	}

	const UAnimInstance* Anim = Player->GetMesh()->GetAnimInstance();
	if (FogEntryElapsed < FogEntryDuration * 0.82f && (!Anim || !Anim->Montage_IsActive(ClownFogEntryMontage)))
	{
		EndClownFogEntry(false);
		return;
	}

	FogEntryElapsed += DeltaSeconds;
	const float Alpha = FMath::Clamp(FogEntryElapsed / FogEntryDuration, 0.0f, 1.0f);
	const float TurnAlpha = FMath::Min(Alpha / 0.25f, 1.0f);
	if (APlayerController* Controller = LockedController.Get())
	{
		const float CameraAlpha = FMath::SmoothStep(0.0f, 1.0f, TurnAlpha);
		FRotator ControlRotation = FogEntryStartControlRotation;
		ControlRotation.Yaw += FMath::FindDeltaAngleDegrees(ControlRotation.Yaw, FogEntryFacing.Rotator().Yaw) * CameraAlpha;
		Controller->SetControlRotation(ControlRotation);
	}

	const float Cross = FMath::InterpEaseInOut(0.0f, 1.0f, Alpha, 1.25f);
	FVector Desired = FMath::Lerp(FogEntryStart, FogEntryEnd, Cross);
	const double HalfHeight = Player->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const FCollisionQueryParams FloorQuery(SCENE_QUERY_STAT(ClownFogEntryFloor), false, Player);
	FHitResult Floor;
	if (GetWorld()->LineTraceSingleByChannel(Floor, Desired + FVector(0, 0, 35), Desired - FVector(0, 0, HalfHeight + 70),
		ECC_WorldStatic, FloorQuery) && Floor.ImpactNormal.Z > 0.6f)
	{
		Desired.Z = Floor.ImpactPoint.Z + HalfHeight + 2.0;
	}

	const FQuat Rotation = FQuat::Slerp(FogEntryStartRotation, FogEntryFacing, TurnAlpha);
	Player->SetActorLocationAndRotation(Desired, Rotation, true);
	if (FogEntryOpening && TraversedFogVisual.IsValid())
	{
		const FVector Hand = Player->GetMesh()->GetSocketLocation(TEXT("hand_r"));
		const FVector Normal = FogEntryFacing.GetForwardVector();
		const FVector FogCenter = TraversedFogVisual->GetComponentLocation();
		const double HandDepth = FVector::DotProduct(Hand - FogCenter, Normal);
		const FVector OpeningCenter = Hand - Normal * HandDepth;
		const float Reach = FMath::SmoothStep(0.12f, 0.29f, Alpha) * (1.0f - FMath::SmoothStep(0.71f, 0.94f, Alpha));
		const float Strength = Reach * (1.0f - FMath::SmoothStep(15.0, 95.0, FMath::Abs(HandDepth)));
		FogEntryOpening->SetVectorParameterValue(TEXT("FogOpeningHand"),
			FLinearColor(OpeningCenter.X, OpeningCenter.Y, OpeningCenter.Z, 1.0f));
		FogEntryOpening->SetScalarParameterValue(TEXT("FogOpeningAmount"), Strength);
	}

	if (Alpha >= 1.0f)
	{
		const UBoxComponent* Gate = TraversedFogGate.Get();
		const double Clearance = Gate->GetScaledBoxExtent().X + Player->GetCapsuleComponent()->GetScaledCapsuleRadius();
		const double Depth = FVector::DotProduct(Player->GetActorLocation() - Gate->GetComponentLocation(), FogEntryFacing.GetForwardVector());
		EndClownFogEntry(Depth > Clearance);
	}
}

void ABossRoom::EndClownFogEntry(bool bReachedInside)
{
	if (!bFogEntryActive)
	{
		return;
	}

	bFogEntryActive = false;
	SetActorTickInterval(FogEntryPreviousTickInterval);
	AWarrior* Player = EncounterPlayer.Get();
	if (Player)
	{
		if (UBoxComponent* Gate = TraversedFogGate.Get())
		{
			Player->GetCapsuleComponent()->IgnoreComponentWhenMoving(Gate, false);
		}
		if (UAnimInstance* Anim = Player->GetMesh()->GetAnimInstance())
		{
			Anim->Montage_Stop(0.15f, ClownFogEntryMontage);
		}
		if (IsPlayerAlive(Player))
		{
			Player->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		}
	}
	TraversedFogGate.Reset();
	if (UStaticMeshComponent* Visual = TraversedFogVisual.Get())
	{
		Visual->SetMaterial(0, FogEntryOriginalMaterial);
	}
	TraversedFogVisual.Reset();
	FogEntryOriginalMaterial = nullptr;
	FogEntryOpening = nullptr;

	for (const auto& Item : FogEntryWeaponVisibility)
	{
		if (USkeletalMeshComponent* HeldMesh = Item.Key.Get())
		{
			HeldMesh->SetVisibility(Item.Value);
		}
	}
	FogEntryWeaponVisibility.Reset();
	UnlockPlayer();

	if (bReachedInside && IsPlayerAlive(Player))
	{
		RoomState = EBossRoomState::Intro;
		BeginBattle();
	}
	else
	{
		EncounterPlayer.Reset();
		RefreshGate();
	}
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

	// 시퀀스 동안 보스방이 입력을 먼저 처리
	InputPriority = 100;
	bBlockInput = true;

	EnableInput(Controller);

	if (InputComponent)
	{
		// 재입장 시 키 바인딩 중복 방지
		InputComponent->KeyBindings.Reset();

		// 이 입력 컴포넌트 아래의 일반 게임 입력 차단
		InputComponent->bBlockInput = true;

	}

	// 현재 표시 중인 UI 숨기기
	HideSequenceUI();
}

void ABossRoom::UnlockPlayer()
{
	if (APlayerController* Controller = LockedController.Get())
	{
		// 시퀀스용 입력 차단 해제
		DisableInput(Controller);

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

	RestoreSequenceUI();
}

void ABossRoom::OnIntroSequenceUpdated(const UMovieSceneSequencePlayer& Player, FFrameTime CurrentTime, FFrameTime)
{
	if (!ClownApproachMusic->IsPlaying())
	{
		return;
	}

	const FFrameRate TickResolution = IntroSequence->GetMovieScene()->GetTickResolution();
	const FFrameTime SequenceTime = FFrameRate::TransformTime(CurrentTime, Player.GetCurrentTime().Rate, TickResolution);
	if (SequenceTime >= FFrameTime(ClownApproachMusicCutFrame.GetValue()))
	{
		ClownApproachMusic->Stop();
	}
}

void ABossRoom::StopIntro()
{
	// 시퀀스 중단 전에 완료 콜백부터 해제.
	if (IsValid(IntroPlayer.Get()))
	{
		IntroPlayer->OnSequenceUpdated().RemoveAll(this);
		IntroPlayer->OnFinished.RemoveDynamic(this, &ABossRoom::OnIntroFinished);
		IntroPlayer->Stop();
	}

	ClownApproachMusic->Stop();
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
	if (UMusicSubsystem* Music =
		GetGameInstance()->GetSubsystem<UMusicSubsystem>())
	{
		Music->StopBGM();
	}

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
		return;
	}

	BossActor->OnPhase2StageRequested.AddUniqueDynamic(this, &ABossRoom::UpdateBossHUDForPhase2);
	ShowBossHUD();
	UpdateEncounterMusic();
}

void ABossRoom::UpdateEncounterMusic()
{
	switch (BossActor->GetBossPhase())
	{
	case EBossPhase::OpeningAttack:
	case EBossPhase::Phase1:
		SetEncounterMusic(Phase1CombatMusic, Phase1CombatMusicVolume);
		break;

	case EBossPhase::PhaseTransition:
		if (BossActor->GetPhase2TransitionStage() == EBossPhase2TransitionStage::Searching)
		{
			SetEncounterMusic(Phase2SearchMusic, Phase2SearchMusicVolume);
		}
		break;

	case EBossPhase::Phase2:
		SetEncounterMusic(Phase2CombatMusic, Phase2CombatMusicVolume);
		break;

	default:
		break;
	}
}

void ABossRoom::SetEncounterMusic(USoundBase* Music, float Volume)
{
	const bool bMusicChanged = EncounterMusic->Sound != Music;
	if (bMusicChanged)
	{
		EncounterMusic->Stop();
		EncounterMusic->SetSound(Music);
	}

	if (Music && (bMusicChanged || EncounterMusic->VolumeMultiplier != Volume))
	{
		EncounterMusic->SetVolumeMultiplier(Volume);
		if (Volume > 0.f && !EncounterMusic->IsPlaying())
		{
			EncounterMusic->Play();
		}
	}
}

void ABossRoom::FailAttempt()
{
	EndClownFogEntry(false);

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
	SetEncounterMusic(nullptr);
	HideBossHUD();

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
	SetEncounterMusic(nullptr);
	RoomState = EBossRoomState::Cleared;

	// 마지막 체력 감소와 데미지 숫자를 보여준 뒤 체력바 제거.
	if (BossHUDHideDelay > 0.f)
	{
		GetWorldTimerManager().SetTimer(BossHUDHideTimer, this, &ABossRoom::HideBossHUD, BossHUDHideDelay, false);
	}
	else
	{
		HideBossHUD();
	}

	// 보스방 클리어 이력 기록.
	if (UBossRoomSubsystem* Progress = GetRoomSubsystem())
	{
		Progress->MarkCleared(RoomID);

		if (AZTSGameMode* GM = Cast<AZTSGameMode>(GetWorld()->GetAuthGameMode()))
		{
			GM->SaveGame();
		}
	}

	// 기존 보스 처치 COMPLETE 알림 표시.
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
		{
			if (UNotificationSubsystem* Notifications = LocalPlayer->GetSubsystem<UNotificationSubsystem>())
			{
				Notifications->ShowNotification(ENotificationType::BossClear);
			}
		}
	}

	EncounterPlayer.Reset();
	bChangingState = false;

	RefreshGate();
}

void ABossRoom::RefreshGate()
{
	if (IsValid(IntroWaitingStandIn))
	{
		const bool bShowStandIn = RoomState == EBossRoomState::Ready && IsFirstEntry() && IsValid(BossActor.Get());
		IntroWaitingStandIn->SetActorHiddenInGame(!bShowStandIn);

		if (bShowStandIn != bIntroWaitingStandInActive && IsValid(BossActor.Get()))
		{
			if (bShowStandIn)
			{
				bIntroWaitingBossWasHidden = BossActor->IsHidden();
			}
			BossActor->SetActorHiddenInGame(bShowStandIn || bIntroWaitingBossWasHidden);
		}
		bIntroWaitingStandInActive = bShowStandIn;
	}

	// 첫 접근은 열림.
	// 입장 연출과 전투 중에는 닫힘.
	// 재시도 대기는 닫힘.
	// 클리어 이후는 열림.
	const bool bActive = RoomState == EBossRoomState::Intro || RoomState == EBossRoomState::Fighting;
	const bool bRetry = RoomState == EBossRoomState::Ready && !IsFirstEntry();
	const bool bClosed = RoomState != EBossRoomState::Cleared && (bActive || bRetry);

	const bool bShowFog = bClosed && RoomState != EBossRoomState::Intro;
	const bool bOtherSideClosed = bEnableOtherSideGate && bClosed;

	if (BossActor && BossActor->IsA<ABossClown>())
	{
		// The fog gate stops the player capsule but must not collapse the spring arm during entry.
		GateBlocker->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
		OhterSideGateBlocker->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	}

	FogMesh->SetVisibility(bShowFog, true);
	OhterSideFogMesh->SetVisibility(bEnableOtherSideGate && bShowFog, true);
	GateBlocker->SetCollisionEnabled(bClosed ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
	OhterSideGateBlocker->SetCollisionEnabled(
		bOtherSideClosed ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
	OtherSideFirstEntryTrigger->SetCollisionEnabled(bEnableOtherSideGate ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
	OtherSideInteractionTrigger->SetCollisionEnabled(bEnableOtherSideGate ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
}

void ABossRoom::HideSequenceUI()
{
	if (bSequenceUIHidden)
	{
		return;
	}

	bSequenceUIHidden = true;
	SavedUIVisibility.Reset();

	TArray<UUserWidget*> Widgets;

	// 현재 화면에 추가된 최상위 UI만 가져오기
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(
		GetWorld(),
		Widgets,
		UUserWidget::StaticClass(),
		true
	);

	for (UUserWidget* Widget : Widgets)
	{
		if (!IsValid(Widget))
		{
			continue;
		}

		// 원래 표시 상태 저장
		SavedUIVisibility.Add(
			TWeakObjectPtr<UUserWidget>(Widget),
			Widget->GetVisibility()
		);

		Widget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ABossRoom::RestoreSequenceUI()
{
	if (!bSequenceUIHidden)
	{
		return;
	}

	for (const auto& Entry : SavedUIVisibility)
	{
		UUserWidget* Widget = Entry.Key.Get();

		// 연출 중 제거된 UI는 건너뛰기
		if (IsValid(Widget) && Widget->IsInViewport())
		{
			Widget->SetVisibility(Entry.Value);
		}
	}

	SavedUIVisibility.Reset();
	bSequenceUIHidden = false;
}

void ABossRoom::ApplySavedProgress()
{
	// BeginPlay에서 초기 정보가 준비된 뒤 적용
	if (!bRoomInitialized || RoomID.IsNone())
	{
		return;
	}

	UBossRoomSubsystem* Progress = GetRoomSubsystem();

	if (!Progress || bChangingState)
	{
		return;
	}

	EndClownFogEntry(false);
	bChangingState = true;

	// 현재 진행 중인 전투와 연출 표시 정리
	StopIntro();
	SetEncounterMusic(nullptr);
	HideBossHUD();
	EncounterPlayer.Reset();

	if (Progress->IsCleared(RoomID))
	{
		// 저장본에서 이미 처치한 보스
		RoomState = EBossRoomState::Cleared;

		if (IsValid(BossActor.Get()))
		{
			BossActor->ResetEncounter();
			BossActor->Destroy();
		}

		BossActor = nullptr;
	}
	else
	{
		// 저장본에서 아직 처치하지 않은 보스
		RoomState = EBossRoomState::Ready;

		if (IsValid(BossActor.Get()) &&
			BossActor->GetBossPhase() != EBossPhase::Dead)
		{
			// 살아 있는 보스는 전투 초기화
			BossActor->ResetEncounter();
		}
		else if (InitialBossClass)
		{
			// 현재 보스가 죽었거나 제거됐다면 재생성
			FActorSpawnParameters Params;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			Params.TransformScaleMethod = ESpawnActorScaleMethod::OverrideRootScale;

			ABoss* NewBoss = GetWorld()->SpawnActor<ABoss>(
				InitialBossClass.Get(),
				BossInitialTransform,
				Params
			);

			if (IsValid(NewBoss))
			{
				if (IsValid(BossActor.Get()))
				{
					BossActor->Destroy();
				}

				NewBoss->SetEncounterArena(
					InitialBossArena.Get()
				);

				BossActor = NewBoss;
			}
			else
			{
				UE_LOG(
					LogTemp,
					Error,
					TEXT("BossRoom: failed to restore boss for %s."),
					*RoomID.ToString()
				);
			}
		}
		else
		{
			UE_LOG(
				LogTemp,
				Error,
				TEXT("BossRoom: initial boss class missing for %s."),
				*RoomID.ToString()
			);
		}
	}

	bChangingState = false;

	// 시청 이력과 클리어 상태에 맞춰 안개문 갱신
	RefreshGate();

	if (ClownApproachMusicCutFrame.IsSet() && ClownApproachMusic->Sound &&
		RoomState == EBossRoomState::Ready && IsFirstEntry())
	{
		ClownApproachMusic->Play();
	}
}
