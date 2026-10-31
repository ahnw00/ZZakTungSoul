#include <Enemy/Boss/Debug/BossRetryTestComponent.h>
#include <Enemy/Boss/Encounter/BossRoom.h>
#include <Enemy/Components/AttackTraceComponent.h>
#include <Warrior/Warrior.h>
#include <Character/CharacterStatComponent.h>
#include <Components/InputComponent.h>
#include <Components/SkeletalMeshComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <GameFramework/PlayerController.h>
#include <Animation/AnimInstance.h>
#include <Kismet/GameplayStatics.h>
#include <Engine/World.h>

UBossRetryTestComponent::UBossRetryTestComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.05f;
}

void UBossRetryTestComponent::BeginPlay()
{
	Super::BeginPlay();

#if UE_BUILD_SHIPPING
	SetComponentTickEnabled(false);
	return;
#endif

	//---------------------------------------------------------------
	// 이 컴포넌트는 보스룸에 부착해서 사용.
	//
	//---------------------------------------------------------------
	BossRoom = Cast<ABossRoom>(GetOwner());

	if (!bEnableRetryTest)
	{
		SetComponentTickEnabled(false);
		return;
	}

	if (!BossRoom.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("BossRetryTest: component must be attached to BossRoom."));
		SetComponentTickEnabled(false);
		return;
	}

	if (!IsValid(RetryPoint.Get()))
	{
		UE_LOG(LogTemp, Error, TEXT("BossRetryTest: RetryPoint is required."));
		SetComponentTickEnabled(false);
		return;
	}

	//---------------------------------------------------------------
	// 대기 중 기존 입력 컴포넌트보다 우선하여 입력 차단.
	//
	//---------------------------------------------------------------
	InputBlocker = NewObject<UInputComponent>(GetOwner(), TEXT("BossRetryTestInputBlocker"));

	if (InputBlocker)
	{
		InputBlocker->bBlockInput = true;
		InputBlocker->Priority = 10000;
		InputBlocker->RegisterComponent();
	}
}

void UBossRetryTestComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	CancelRetry();

	if (IsValid(InputBlocker.Get()))
	{
		InputBlocker->DestroyComponent();
		InputBlocker = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}

void UBossRetryTestComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

#if UE_BUILD_SHIPPING
	return;
#endif

	if (!bEnableRetryTest)
	{
		CancelRetry();
		return;
	}

	if (!BossRoom.IsValid())
	{
		CancelRetry();
		return;
	}

	if (bRetryPending)
	{
		//---------------------------------------------------------------
		// 기존 시스템이 플레이어를 제거했다면 복귀 처리 중단.
		//
		//---------------------------------------------------------------
		if (!TestPlayer.IsValid())
		{
			CancelRetry();
		}

		return;
	}

	const EBossRoomState RoomState = BossRoom->GetRoomState();
	const bool bRoomActive = RoomState == EBossRoomState::Intro || RoomState == EBossRoomState::Fighting;

	if (bRoomActive)
	{
		AWarrior* Player = Cast<AWarrior>(UGameplayStatics::GetPlayerPawn(this, 0));

		if (!IsValid(Player))
		{
			return;
		}

		TestPlayer = Player;
		bAttemptObserved = true;
	}

	if (!bAttemptObserved || !TestPlayer.IsValid())
	{
		return;
	}

	UCharacterStatComponent* Stat = TestPlayer->GetCharacterStatComponent();
	if (!Stat)
	{
		return;
	}

	//---------------------------------------------------------------
	// 보스룸이 먼저 Ready로 초기화되어도 직전 전투의 사망 감지.
	//
	//---------------------------------------------------------------
	if (Stat->GetCurrentHP() <= 0.0f)
	{
		StartRetry();
		return;
	}

	if (!bRoomActive)
	{
		bAttemptObserved = false;
		TestPlayer.Reset();
	}
}

void UBossRetryTestComponent::StartRetry()
{
	if (bRetryPending || !BossRoom.IsValid() || !TestPlayer.IsValid())
	{
		return;
	}

	bRetryPending = true;
	bAttemptObserved = false;

	//---------------------------------------------------------------
	// 보스와 연출부터 정리.
	// 입장 이력은 유지하므로 재시도 안개문이 남음.
	//
	//---------------------------------------------------------------
	BossRoom->FailAttempt();

	StopPlayerActions();
	LockPlayer();

	//---------------------------------------------------------------
	// 테스트 복귀 예약.
	//
	//---------------------------------------------------------------
	GetWorld()->GetTimerManager().SetTimer(RetryTimerHandle, this, &UBossRetryTestComponent::RestorePlayer, FMath::Max(RetryDelay, 0.1f), false);

	UE_LOG(LogTemp, Log, TEXT("BossRetryTest: player HP reached zero. Retry scheduled."));
}

void UBossRetryTestComponent::StopPlayerActions()
{
	AWarrior* Player = TestPlayer.Get();
	if (!IsValid(Player))
	{
		return;
	}

	Player->GetCharacterMovement()->StopMovementImmediately();

	//---------------------------------------------------------------
	// 실행 중인 공격과 피격 몽타주 중단.
	//
	//---------------------------------------------------------------
	if (UAnimInstance* Anim = Player->GetMesh()->GetAnimInstance())
	{
		Anim->StopAllMontages(0.0f);
	}

	if (UAttackTraceComponent* Trace = Player->FindComponentByClass<UAttackTraceComponent>())
	{
		Trace->EndTrace();
	}

	//---------------------------------------------------------------
	// 공개 함수로 복구 가능한 행동 상태 정리.
	//
	//---------------------------------------------------------------
	Player->ResetLockOnTarget();
	Player->ResetCombo();
	Player->EndAction();
}

void UBossRetryTestComponent::LockPlayer()
{
	AWarrior* Player = TestPlayer.Get();
	if (!IsValid(Player))
	{
		return;
	}

	if (!bMovementLocked)
	{
		UCharacterMovementComponent* Movement = Player->GetCharacterMovement();

		SavedMovementMode = static_cast<uint8>(Movement->MovementMode);
		SavedCustomMovementMode = Movement->CustomMovementMode;

		Movement->DisableMovement();
		bMovementLocked = true;
	}

	APlayerController* Controller = Cast<APlayerController>(Player->GetController());
	if (!Controller || bInputLocked)
	{
		return;
	}

	LockedController = Controller;

	Controller->SetIgnoreMoveInput(true);
	Controller->SetIgnoreLookInput(true);

	if (IsValid(InputBlocker.Get()))
	{
		Controller->PushInputComponent(InputBlocker.Get());
	}

	bInputLocked = true;
}

void UBossRetryTestComponent::UnlockPlayer()
{
	if (APlayerController* Controller = LockedController.Get())
	{
		if (bInputLocked)
		{
			if (IsValid(InputBlocker.Get()))
			{
				Controller->PopInputComponent(InputBlocker.Get());
			}

			Controller->SetIgnoreMoveInput(false);
			Controller->SetIgnoreLookInput(false);
		}
	}

	if (AWarrior* Player = TestPlayer.Get())
	{
		if (bMovementLocked)
		{
			UCharacterMovementComponent* Movement = Player->GetCharacterMovement();
			const EMovementMode RestoreMode = static_cast<EMovementMode>(SavedMovementMode);

			if (RestoreMode == MOVE_None)
			{
				Movement->SetMovementMode(MOVE_Walking);
			}
			else
			{
				Movement->SetMovementMode(RestoreMode, SavedCustomMovementMode);
			}
		}
	}

	bInputLocked = false;
	bMovementLocked = false;
	LockedController.Reset();
}

void UBossRetryTestComponent::RestorePlayer()
{
#if UE_BUILD_SHIPPING
	return;
#endif

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(RetryTimerHandle);
	}

	if (!bRetryPending)
	{
		return;
	}

	AWarrior* Player = TestPlayer.Get();

	if (!IsValid(Player) || !BossRoom.IsValid())
	{
		CancelRetry();
		return;
	}

	if (!IsValid(RetryPoint.Get()))
	{
		UnlockPlayer();

		UE_LOG(LogTemp, Error, TEXT("BossRetryTest: RetryPoint is invalid. Assign a valid point and call RetryNow."));
		return;
	}

	UCharacterStatComponent* Stat = Player->GetCharacterStatComponent();
	if (!Stat)
	{
		CancelRetry();
		return;
	}

	StopPlayerActions();

	//---------------------------------------------------------------
	// 이동에 성공한 뒤 체력 복구.
	// 위치가 막혀 있으면 방 안에서 부활시키지 않음.
	//
	//---------------------------------------------------------------
	if (!Player->TeleportTo(RetryPoint->GetActorLocation(), RetryPoint->GetActorRotation()))
	{
		UnlockPlayer();

		UE_LOG(LogTemp, Error, TEXT("BossRetryTest: RetryPoint is blocked. Move the point and call RetryNow."));
		return;
	}

	//---------------------------------------------------------------
	// 테스트에 필요한 자원과 누적 자세 수치 복구.
	//
	//---------------------------------------------------------------
	Stat->SetCurrentHP(Stat->GetMaxHP());
	Stat->SetCurrentFP(Stat->GetMaxFP());
	Stat->SetCurrentStamina(Stat->GetMaxStamina());
	Stat->SetCurrentStance(0.0f);

	Player->ResetLockOnTarget();
	Player->ResetCombo();
	Player->EndAction();

	//---------------------------------------------------------------
	// 복귀 지점 방향으로 시점 정렬.
	//
	//---------------------------------------------------------------
	if (APlayerController* Controller = Cast<APlayerController>(Player->GetController()))
	{
		FRotator ViewRotation = RetryPoint->GetActorRotation();
		ViewRotation.Pitch = 0.0f;
		ViewRotation.Roll = 0.0f;

		Controller->SetControlRotation(ViewRotation);
	}

	UnlockPlayer();

	bRetryPending = false;
	bAttemptObserved = false;
	TestPlayer.Reset();

	UE_LOG(LogTemp, Log, TEXT("BossRetryTest: player restored outside the boss room."));
}

void UBossRetryTestComponent::RetryNow()
{
#if !UE_BUILD_SHIPPING
	if (bRetryPending)
	{
		RestorePlayer();
	}
#endif
}

void UBossRetryTestComponent::CancelRetry()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(RetryTimerHandle);
	}

	UnlockPlayer();

	bRetryPending = false;
	bAttemptObserved = false;
	TestPlayer.Reset();
}