// Fill out your copyright notice in the Description page of Project Settings.


#include <Enemy/Boss/Clown/BossClown.h>
#include <Interface/CombatDamageInterface.h>
#include <Enemy/Boss/Clown/Anim/BossClownAnimationInstance.h>
#include <Enemy/Components/AttackTraceComponent.h>
#include <Animation/AnimInstance.h>
#include <Animation/AnimMontage.h>
#include <Components/SkeletalMeshComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Engine/SkeletalMesh.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <AIController.h>
#include <EnhancedInputComponent.h>
#include <InputAction.h>
#include <Kismet/GameplayStatics.h>
#include <GameFramework/PlayerController.h>
#include <Engine/World.h>
#include <UObject/ConstructorHelpers.h>
#include <Components/CapsuleComponent.h>
#include <Navigation/PathFollowingComponent.h>
#include <Enemy/Boss/Clown/Components/ClownProjectileComponent.h>

#include <Anim/AnimNotifyState/AnimNotifyState_AttackTrace.h>

ABossClown::ABossClown()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	//---------------------------------------------------------------
	// 광대 SkeletalMesh 설정.
	//
	//---------------------------------------------------------------
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletonMeshRef(
		TEXT("/Game/Assets/Clown_monster/Meshes/base_mesh/SKM_base_mesh.SKM_base_mesh")
	);

	if (SkeletonMeshRef.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SkeletonMeshRef.Object);

		GetMesh()->SetRelativeLocation(FVector(0, 0, -90));
		GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));
		GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

		static ConstructorHelpers::FClassFinder<UBossClownAnimationInstance> AnimClassRef(
			TEXT("/Game/Blueprints/Enemy/Boss/Clown/Animation/ABP_BossClown.ABP_BossClown_C")
		);

		if (AnimClassRef.Succeeded())
		{
			GetMesh()->SetAnimInstanceClass(AnimClassRef.Class);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("BossClown: failed to load animation blueprint class."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BossClown: failed to load skeleton mesh."));
	}


}
const UBossClownAnimationData* ABossClown::GetClownAnimationData() const
{
	return ClownAnimationData.Get();
}

const FEnemyAttackData* ABossClown::GetOpeningAttackData() const
{
	const UBossClownAnimationData* Data = GetClownAnimationData();
	return Data ? &Data->OpeningAttack : nullptr;
}

const FEnemyAttackData* ABossClown::GetClownAttackData(EBossClownAttack AttackType) const
{
	if (const FClownPhaseOnePattern* Pattern = GetPhaseOnePattern(AttackType))
	{
		if (Pattern->bEnabled && Pattern->Attack.Montage)
		{
			return &Pattern->Attack;
		}
	}

	return nullptr;
}

bool ABossClown::IsClownCombatPhase() const
{
	return !IsDead() && (BossPhase == EBossPhase::Phase1 || BossPhase == EBossPhase::Phase2);
}

bool ABossClown::StartClownAttack(EBossClownAttack AttackType, AActor* Target, int32& OutExecutionID, EClownAttackStyle Style, float RecoveryScale)
{
	OutExecutionID = INDEX_NONE;

	if (!IsClownCombatPhase() || ActiveAttackID != INDEX_NONE || !IsValid(Target) || !AttackTraceComponent)
	{
		return false;
	}

	const FEnemyAttackData* AttackData = GetClownAttackData(AttackType);
	const FClownPhaseOnePattern* Pattern = GetPhaseOnePattern(AttackType);
	TArray<const FEnemyAttackData*> Choices;
	if (AttackData && AttackData->Montage != LastPlayedAttackMontage.Get())
	{
		Choices.Add(AttackData);
	}
	if (Pattern)
	{
		for (const FEnemyAttackData& Variant : Pattern->AttackVariants)
		{
			if (Variant.Montage && Variant.Montage != LastPlayedAttackMontage.Get() && Variant.DamageInfo.IsValidIndex(0))
			{
				Choices.Add(&Variant);
			}
		}
	}
	if (!Choices.IsEmpty())
	{
		AttackData = Choices[FMath::RandRange(0, Choices.Num() - 1)];
	}
	UAnimInstance* Anim = GetMesh()->GetAnimInstance();

	if (!AttackData || !AttackData->Montage || !AttackData->DamageInfo.IsValidIndex(0) || !Anim)
	{
		UE_LOG(LogTemp, Error, TEXT("BossClown: attack montage, DamageInfo[0] and AnimInstance are required."));
		return false;
	}

	const bool bMovementAttack = AttackType == EBossClownAttack::FastChaseSlam || AttackType == EBossClownAttack::DashSweep;
	if (bMovementAttack && (!GetPhaseOnePattern(AttackType) || !GetCharacterMovement()->IsMovingOnGround()))
	{
		return false;
	}

	if ((AttackType == EBossClownAttack::SpawnCards || AttackType == EBossClownAttack::CubeBarrage) && GetTargetSurfaceDistance(Target) < 850.0f)
	{
		return false;
	}

	SetStrafeMode(false);

	SavedWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	bSavedOrientToMovement = GetCharacterMovement()->bOrientRotationToMovement;
	bSavedControllerDesiredRotation = GetCharacterMovement()->bUseControllerDesiredRotation;
	bSavedControllerYaw = bUseControllerRotationYaw;
	bHasSavedMovement = true;

	//---------------------------------------------------------------
	// 공격 시작 시 이동 정지와 타깃 방향 정렬.
	//
	//---------------------------------------------------------------
	if (AAIController* AI = Cast<AAIController>(GetController()))
	{
		AI->StopMovement();
		AI->ClearFocus(EAIFocusPriority::Gameplay);
	}

	GetCharacterMovement()->StopMovementImmediately();
	AttackTraceComponent->EndTrace();

	FVector Direction = Target->GetActorLocation() - GetActorLocation();
	Direction.Z = 0.0f;

	// BT 전투에서는 예비 동작 중 회전하여 공격 시작 시 순간 회전 방지.
	// 공격 예비 동작의 추적 회전만 사용. 시작 시 순간 회전하지 않음.

	CurrentClownAttack = *AttackData;
	LastPlayedAttackMontage = AttackData->Montage;
	CurrentStyle = Style;
	CurrentRecoveryScale = FMath::Clamp(RecoveryScale, 0.1f, 2.0f);
	bPursuitUsed = false;
	CurrentAttackType = AttackType;
	ClownAttackTarget = Target;
	AttackStageTime = 0.0f;
	bDashStarted = false;
	bDashEnded = false;
	BallsLaunched = 0;
	bClapReleased = false;
	DashDirection = Direction.GetSafeNormal();

	//---------------------------------------------------------------
	// 단타 공격은 첫 번째 데미지를 기본으로 사용.
	// 다단 공격은 Notify에서 인덱스 변경.
	//
	//---------------------------------------------------------------
	CurrentDamageIndex = 0;
	AttackAnimInstance = Anim;
	ActiveAttackID = ++AttackSerial;
	OutExecutionID = ActiveAttackID;

	SetEnemyState(EEnemyState::Attack);
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	bUseControllerRotationYaw = false;

	if (AttackType == EBossClownAttack::FastChaseSlam && GetTargetSurfaceDistance(Target) > GetClownAnimationData()->FastChaseStopDistance)
	{
		BeginClownApproach();
		return true;
	}

	if (!PlayCurrentClownMontage())
	{
		return false;
	}

	return true;
}

void ABossClown::PreferClownRangedAttack()
{
	RangedPreferenceUntil = GetWorld()->GetTimeSeconds() + 5.0;
}

bool ABossClown::PrefersClownRangedAttack() const
{
	return GetWorld() && GetWorld()->GetTimeSeconds() < RangedPreferenceUntil;
}

void ABossClown::BeginClownApproach()
{
	ClownAttackStage = EClownAttackStage::Approach;
	AttackStageTime = 0.0f;
	NextChasePathTime = 0.0;
	SetEnemyState(EEnemyState::FastChase);
	GetCharacterMovement()->MaxWalkSpeed = GetClownAnimationData()->FastChaseSpeed * (bPursuitUsed ? 0.75f : 1.0f);
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	// 이동은 CharacterMovement가 담당하고 달리기만 반복 재생.
	UAnimInstance* Anim = AttackAnimInstance.Get();
	if (UAnimMontage* Run = GetClownAnimationData()->FastChaseMontage.Get())
	{
		if (Anim && Anim->Montage_Play(Run, GetClownAnimationData()->FastChaseAnimationRate) > 0.0f)
		{
			ChaseMontage = Run;
			const FName Section = Run->GetSectionName(0);
			Anim->Montage_SetNextSection(Section, Section, Run);
		}
	}
	GetWorld()->GetTimerManager().SetTimer(AttackTimeoutHandle, this, &ABossClown::OnClownAttackTimeout, FMath::Max(AttackTimeout, 0.1f), false);
}

bool ABossClown::HasRecentClownAttack(EBossClownAttack Type, float Seconds) const
{
	const double* Time = CompletedAttackTimes.Find(Type);
	return Time && GetWorld() && GetWorld()->GetTimeSeconds() - *Time < Seconds;
}

bool ABossClown::PlayCurrentClownMontage()
{
	UAnimInstance* Anim = AttackAnimInstance.Get();
	if (!Anim || ActiveAttackID == INDEX_NONE)
	{
		FinishClownAttack(EBossClownAttackResult::Failed, false);
		return false;
	}

	StopFastChaseAnimation();
	if (AAIController* AI = Cast<AAIController>(GetController()))
	{
		AI->StopMovement();
	}
	GetCharacterMovement()->StopMovementImmediately();
	SetEnemyState(EEnemyState::Attack);
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	bUseControllerRotationYaw = false;
	ClownAttackStage = EClownAttackStage::Montage;
	AttackStageTime = 0.0f;

	UAnimMontage* Montage = CurrentClownAttack.Montage.Get();

	if (Anim->Montage_Play(Montage) <= 0.0f)
	{
		FinishClownAttack(EBossClownAttackResult::Failed, false);
		return false;
	}

	// 타격 직전까지 자세를 먼저 보여 주고 본 동작을 재개.
	const FClownPhaseOnePattern* Pattern = GetPhaseOnePattern(CurrentAttackType);
	if (Pattern && CurrentStyle == EClownAttackStyle::DelayedPursuit && !bPursuitUsed && Pattern->AnticipationTime > 0.0f)
	{

		AttackTraceComponent->EndTrace();
		ClownAttackStage = EClownAttackStage::Anticipation;
	}

	if (CurrentAttackType == EBossClownAttack::TripleBalls || CurrentAttackType == EBossClownAttack::CubeBarrage || CurrentAttackType == EBossClownAttack::ClapWave
		|| CurrentAttackType == EBossClownAttack::SpawnCards)
	{
		if (WeaponMeshComponent)
		{
			bWeaponWasVisible = WeaponMeshComponent->IsVisible();
			bHidWeapon = true;
			WeaponMeshComponent->SetVisibility(false);
		}

		if (CurrentAttackType == EBossClownAttack::CubeBarrage)
		{
			if (!PrepareClownCubeMarch())
			{
				FinishClownAttack(EBossClownAttackResult::Failed, true);
				return false;
			}
			// 지휘 한 마디를 끊지 않고 세 차례 행진까지 반복.
			const FName Section = Montage->GetSectionName(0);
			Anim->Montage_SetPlayRate(Montage, Montage->GetPlayLength() / UClownProjectileComponent::CubeMarchCycle);
			Anim->Montage_SetNextSection(Section, Section, Montage);
		}
		else if (CurrentAttackType == EBossClownAttack::TripleBalls)
		{
			PrepareClownBalls();
		}

		if (CurrentAttackType == EBossClownAttack::SpawnCards)
		{
			PrepareClownCards();
		}

	}

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &ABossClown::OnClownAttackEnded, ActiveAttackID);
	Anim->Montage_SetEndDelegate(EndDelegate, Montage);

	GetWorld()->GetTimerManager().SetTimer(AttackTimeoutHandle, this, &ABossClown::OnClownAttackTimeout, FMath::Max(AttackTimeout, 0.1f), false);

	return true;
}

float ABossClown::GetAnticipationPoseTime(const UAnimMontage* Montage)
{
	if (!Montage)
	{
		return 0.0f;
	}
	float PoseTime = FMath::Min(0.3f, Montage->GetPlayLength() * 0.2f);
	for (const FAnimNotifyEvent& Notify : Montage->Notifies)
	{
		if (Notify.NotifyStateClass && Notify.NotifyStateClass->IsA<UAnimNotifyState_AttackTrace>())
		{
			PoseTime = FMath::Min(PoseTime, FMath::Max(0.0f, Notify.GetTriggerTime() - 0.08f));
		}
	}
	return PoseTime;
}

const FClownPhaseOnePattern* ABossClown::GetPhaseOnePattern(EBossClownAttack AttackType) const
{
	const UBossClownAnimationData* Data = GetClownAnimationData();
	return Data ? Data->PhaseOnePatterns.FindByPredicate([AttackType](const FClownPhaseOnePattern& Pattern) { return Pattern.AttackType == AttackType; }) : nullptr;
}

float ABossClown::GetTargetSurfaceDistance(const AActor* Target) const
{
	if (!IsValid(Target))
	{
		return TNumericLimits<float>::Max();
	}

	float TargetRadius = 0.0f;
	if (const ACharacter* Character = Cast<ACharacter>(Target))
	{
		TargetRadius = Character->GetCapsuleComponent()->GetScaledCapsuleRadius();
	}
	return FMath::Max(0.0f, FVector::Dist2D(GetActorLocation(), Target->GetActorLocation()) - GetCapsuleComponent()->GetScaledCapsuleRadius() - TargetRadius);
}

void ABossClown::FaceClownTarget(AActor* Target, float DeltaTime)
{
	if (!IsValid(Target) || !GetClownAnimationData())
	{
		return;
	}
	FVector Direction = Target->GetActorLocation() - GetActorLocation();
	Direction.Z = 0.0f;
	if (!Direction.IsNearlyZero())
	{
		SetActorRotation(FMath::RInterpConstantTo(GetActorRotation(), Direction.Rotation(), DeltaTime, GetClownAnimationData()->AttackTurnSpeed));
	}
}

void ABossClown::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateClownTraceDebug();

	if (ActiveAttackID != INDEX_NONE)
	{
		TickClownAttack(DeltaTime);
	}
}

void ABossClown::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (InputComponent && bTraceDebugInputReady)
	{
		if (APlayerController* Player = UGameplayStatics::GetPlayerController(this, 0))
		{
			Player->PopInputComponent(InputComponent);
		}
	}
	Super::EndPlay(EndPlayReason);
}

void ABossClown::ToggleClownTraceDebug()
{
	bDrawClownTraces = !bDrawClownTraces;
}

void ABossClown::UpdateClownTraceDebug()
{
	if (!bTraceDebugInputReady)
	{
		APlayerController* Player = UGameplayStatics::GetPlayerController(this, 0);
		UInputAction* Action = LoadObject<UInputAction>(nullptr, TEXT("/Game/Input/Debug/IA_Debug_Num1.IA_Debug_Num1"));
		if (Player && Action)
		{
			UEnhancedInputComponent* DebugInput = NewObject<UEnhancedInputComponent>(this, TEXT("ClownTraceDebugInput"));
			DebugInput->RegisterComponent();
			DebugInput->Priority = 20;
			DebugInput->BindAction(Action, ETriggerEvent::Started, this, &ABossClown::ToggleClownTraceDebug);
			InputComponent = DebugInput;
			Player->PushInputComponent(DebugInput);
			bTraceDebugInputReady = true;
		}
	}
	TArray<UAttackTraceComponent*> Traces;
	GetComponents<UAttackTraceComponent>(Traces);
	for (UAttackTraceComponent* Trace : Traces)
	{
		Trace->bDrawDebug = bDrawClownTraces;
	}
	for (const TWeakObjectPtr<AActor>& Item : ClownProjectiles)
	{
		if (!Item.IsValid())
		{
			continue;
		}
		Traces.Reset();
		Item->GetComponents<UAttackTraceComponent>(Traces);
		for (UAttackTraceComponent* Trace : Traces)
		{
			Trace->bDrawDebug = bDrawClownTraces;
		}
	}
}

void ABossClown::TickClownAttack(float DeltaTime)
{
	AActor* Target = ClownAttackTarget.Get();
	const UBossClownAnimationData* Data = GetClownAnimationData();
	if (!IsClownCombatPhase() || !IsValid(Target) || Target->IsActorBeingDestroyed() || !Data)
	{
		FinishClownAttack(EBossClownAttackResult::Aborted, true, false);
		return;
	}

	AttackStageTime += DeltaTime;
	const FClownPhaseOnePattern* Pattern = GetPhaseOnePattern(CurrentAttackType);
	if (ClownAttackStage == EClownAttackStage::Approach)
	{
		if (AttackStageTime >= (bPursuitUsed ? 1.2f : Data->FastChaseMaxTime))
		{
			FinishClownAttack(EBossClownAttackResult::Failed, false);
			return;
		}
		FaceClownTarget(Target, DeltaTime);
		if (GetTargetSurfaceDistance(Target) <= Data->FastChaseStopDistance)
		{
			PlayCurrentClownMontage();
			return;
		}
		if (AAIController* AI = Cast<AAIController>(GetController()))
		{
			const double Now = GetWorld()->GetTimeSeconds();
			if (Now >= NextChasePathTime && AI->GetMoveStatus() != EPathFollowingStatus::Moving)
			{
				if (AI->MoveToActor(Target, Data->FastChaseStopDistance, true) == EPathFollowingRequestResult::Failed)
				{
					FinishClownAttack(EBossClownAttackResult::Failed, false);
					return;
				}
				NextChasePathTime = Now + 0.2;
			}
		}
		return;
	}

	if (ClownAttackStage == EClownAttackStage::Anticipation)
	{
		// 예고 중에만 천천히 조준하고, 돌진 방향은 준비가 끝날 때 고정.
		FaceClownTarget(Target, DeltaTime);
		if (UAnimInstance* Anim = AttackAnimInstance.Get())
		{
			if (Anim->Montage_GetPosition(CurrentClownAttack.Montage.Get()) >= GetAnticipationPoseTime(CurrentClownAttack.Montage.Get()))
			{
				Anim->Montage_Pause(CurrentClownAttack.Montage.Get());
			}
		}
		AttackTraceComponent->EndTrace();
		if (!Pattern || AttackStageTime >= GetAnticipationPoseTime(CurrentClownAttack.Montage.Get()) + Pattern->AnticipationTime + (CurrentStyle == EClownAttackStyle::DelayedPursuit ? 0.35f : 0.0f))
		{
			if (CurrentStyle == EClownAttackStyle::DelayedPursuit && !bPursuitUsed && GetTargetSurfaceDistance(Target) > Data->FastChaseStopDistance + 20.0f)
			{
				bPursuitUsed = true;
				if (UAnimInstance* Anim = AttackAnimInstance.Get())
				{
					FOnMontageEnded EmptyDelegate;
					Anim->Montage_SetEndDelegate(EmptyDelegate, CurrentClownAttack.Montage.Get());
					Anim->Montage_Stop(0.15f, CurrentClownAttack.Montage.Get());
				}
				BeginClownApproach();
				return;
			}
			if (UAnimInstance* Anim = AttackAnimInstance.Get())
			{
				Anim->Montage_Resume(CurrentClownAttack.Montage.Get());
			}
			DashDirection = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();
			ClownAttackStage = EClownAttackStage::Montage;
			AttackStageTime = 0.0f;
		}
		return;
	}

	if (ClownAttackStage == EClownAttackStage::Recovery)
	{
		FaceClownTarget(Target, DeltaTime);
		if (!Pattern || AttackStageTime >= Pattern->RecoveryTime * CurrentRecoveryScale)
		{
			FinishClownAttack(EBossClownAttackResult::Succeeded, false);
		}
		return;
	}

	if (ClownAttackStage != EClownAttackStage::Montage || !Pattern)
	{
		return;
	}
	UAnimInstance* Anim = AttackAnimInstance.Get();
	const float MontageTime = Anim ? Anim->Montage_GetPosition(CurrentClownAttack.Montage.Get()) : 0.0f;
	TickClownSpecial(MontageTime);
	if (CurrentAttackType != EBossClownAttack::DashSweep && MontageTime < Pattern->TrackingTime)
	{
		FaceClownTarget(Target, DeltaTime);
	}
	if (CurrentAttackType == EBossClownAttack::DashSweep && !bDashEnded && MontageTime >= Data->DashStartTime)
	{
		if (!bDashStarted)
		{
			bDashStarted = true;
			DashStartLocation = GetActorLocation();
			GetCharacterMovement()->MaxWalkSpeed = Data->DashSpeed;
		}
		if (MontageTime >= Data->DashStartTime + Data->DashDuration ||
			FVector::Dist2D(DashStartLocation, GetActorLocation()) >= Data->DashMaxDistance || !GetCharacterMovement()->IsMovingOnGround())
		{
			bDashEnded = true;
			GetCharacterMovement()->StopMovementImmediately();
			ConsumeMovementInputVector();
		}
		else
		{
			// CharacterMovement의 충돌 처리를 사용하며 돌진 중 목표 방향은 변경하지 않음.
			GetCharacterMovement()->Velocity = DashDirection * Data->DashSpeed;
			AddMovementInput(DashDirection, 1.0f);
		}
	}
}

void ABossClown::BeginClownRecovery()
{
	if (AttackTraceComponent)
	{
		AttackTraceComponent->EndTrace();
	}
	GetCharacterMovement()->StopMovementImmediately();
	ConsumeMovementInputVector();
	ClownAttackStage = EClownAttackStage::Recovery;
	AttackStageTime = 0.0f;
	const FClownPhaseOnePattern* Pattern = GetPhaseOnePattern(CurrentAttackType);
	if (!Pattern || Pattern->RecoveryTime <= 0.0f)
	{
		FinishClownAttack(EBossClownAttackResult::Succeeded, false);
	}
}

void ABossClown::RestoreClownMovement()
{
	if (!bHasSavedMovement)
	{
		return;
	}
	if (AAIController* AI = Cast<AAIController>(GetController()))
	{
		AI->StopMovement();
	}
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->MaxWalkSpeed = SavedWalkSpeed;
	ConsumeMovementInputVector();
	GetCharacterMovement()->bOrientRotationToMovement = bSavedOrientToMovement;
	GetCharacterMovement()->bUseControllerDesiredRotation = bSavedControllerDesiredRotation;
	bUseControllerRotationYaw = bSavedControllerYaw;
	bHasSavedMovement = false;
	NextChasePathTime = 0.0;
}

void ABossClown::SetClownAttackDamage(int32 DamageIndex, UAnimSequenceBase* SourceAnimation)
{
	if (ActiveAttackID == INDEX_NONE || !IsClownCombatPhase() || SourceAnimation != CurrentClownAttack.Montage.Get())
	{
		return;
	}

	//---------------------------------------------------------------
	// 현재 광대 공격에서 사용할 타격 데미지 인덱스 변경.
	//
	//---------------------------------------------------------------
	if (!CurrentClownAttack.DamageInfo.IsValidIndex(DamageIndex))
	{
		CurrentDamageIndex = INDEX_NONE;
		UE_LOG(LogTemp, Error, TEXT("BossClown: invalid attack DamageIndex %d."), DamageIndex);
		return;
	}

	CurrentDamageIndex = DamageIndex;
}

EBossClownAttackResult ABossClown::GetClownAttackResult(int32 ExecutionID) const
{
	if (ExecutionID != INDEX_NONE && ExecutionID == ActiveAttackID)
	{
		return EBossClownAttackResult::Running;
	}

	return ExecutionID != INDEX_NONE && ExecutionID == LastAttackID ? LastAttackResult : EBossClownAttackResult::Aborted;
}

void ABossClown::CancelClownAttack(int32 ExecutionID)
{
	if (ActiveAttackID != INDEX_NONE && ExecutionID == ActiveAttackID)
	{
		FinishClownAttack(EBossClownAttackResult::Aborted, true);
	}
}

void ABossClown::OnClownAttackEnded(UAnimMontage* Montage, bool bInterrupted, int32 ExecutionID)
{
	if (ActiveAttackID != INDEX_NONE && ExecutionID == ActiveAttackID && Montage == CurrentClownAttack.Montage.Get())
	{
		if (bInterrupted || !IsClownCombatPhase())
		{
			FinishClownAttack(EBossClownAttackResult::Aborted, false);
		}
		else
		{
			// 공 개수가 늘어 지휘 한 주기보다 길어지면 동일 동작을 이어서 재생.
			const UBossClownAnimationData* Data = GetClownAnimationData();
			if (CurrentAttackType == EBossClownAttack::TripleBalls && Data && BallsLaunched < FMath::Clamp(Data->BallCount, 1, 16))
			{
				if (UAnimInstance* Anim = AttackAnimInstance.Get())
				{
					if (Anim->Montage_Play(Montage) > 0.0f)
					{
						FOnMontageEnded EndDelegate;
						EndDelegate.BindUObject(this, &ABossClown::OnClownAttackEnded, ExecutionID);
						Anim->Montage_SetEndDelegate(EndDelegate, Montage);
						return;
					}
				}
			}
			BeginClownRecovery();
		}
	}
}

void ABossClown::OnClownAttackTimeout()
{
	UE_LOG(LogTemp, Error, TEXT("BossClown: attack timed out."));
	FinishClownAttack(EBossClownAttackResult::Failed, true);
}

void ABossClown::FinishClownAttack(EBossClownAttackResult Result, bool bStopMontage, bool bRestoreState)
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(AttackTimeoutHandle);
	}

	if (ActiveAttackID == INDEX_NONE)
	{
		return;
	}

	StopFastChaseAnimation();
	ClearClownProjectiles(Result != EBossClownAttackResult::Succeeded);
	if (bHidWeapon && WeaponMeshComponent)
	{
		WeaponMeshComponent->SetVisibility(bWeaponWasVisible);
	}
	bHidWeapon = false;

	UAnimInstance* Anim = AttackAnimInstance.Get();
	UAnimMontage* Montage = CurrentClownAttack.Montage.Get();

	//---------------------------------------------------------------
	// 종료 콜백보다 먼저 실행 번호 무효화.
	//
	//---------------------------------------------------------------
	if (Result == EBossClownAttackResult::Succeeded)
	{
		LastCompletedAttack = CurrentAttackType;
		LastCompletedAttackTime = GetWorld()->GetTimeSeconds();
		CompletedAttackTimes.Add(CurrentAttackType, LastCompletedAttackTime);
	}
	LastAttackID = ActiveAttackID;
	LastAttackResult = Result;
	ActiveAttackID = INDEX_NONE;
	CurrentDamageIndex = INDEX_NONE;

	if (Anim && Montage)
	{
		FOnMontageEnded EmptyDelegate;
		Anim->Montage_SetEndDelegate(EmptyDelegate, Montage);

		if (bStopMontage)
		{
			Anim->Montage_Stop(0.0f, Montage);
		}
	}

	if (AttackTraceComponent)
	{
		AttackTraceComponent->EndTrace();
	}

	CurrentClownAttack = FEnemyAttackData();
	AttackAnimInstance.Reset();
	ClownAttackTarget.Reset();
	ClownAttackStage = EClownAttackStage::None;
	bDashStarted = false;
	bDashEnded = false;

	if (bRestoreState && !IsActorBeingDestroyed() && IsClownCombatPhase() &&
		(GetEnemyState() == EEnemyState::Attack || GetEnemyState() == EEnemyState::FastChase))
	{
		SetEnemyState(EEnemyState::Chase);
	}
	RestoreClownMovement();
}

void ABossClown::ResetBossGimmicks()
{
	LastCompletedAttackTime = -1000.0;
	CompletedAttackTimes.Reset();
	LastPlayedAttackMontage.Reset();
	RangedPreferenceUntil = 0.0;
	ClearClownProjectiles(true);
	FinishClownAttack(EBossClownAttackResult::Aborted, true, false);
	NextChasePathTime = 0.0;
	SetStrafeMode(false);
	Super::ResetBossGimmicks();
}


void ABossClown::HandleAttackTraceHit(const FHitResult& Hit)
{
	//---------------------------------------------------------------
	// 첫 공격만 공통 보스에서 처리.
	// 일반 적의 피해 전달 함수는 사용하지 않음.
	//
	//---------------------------------------------------------------
	if (BossPhase == EBossPhase::OpeningAttack)
	{
		Super::HandleAttackTraceHit(Hit);
		return;
	}

	//---------------------------------------------------------------
	// 광대 전투 공격의 현재 타격 데이터에서 직접 피해 전달.
	//
	//---------------------------------------------------------------
	if (!IsClownCombatPhase() || ActiveAttackID == INDEX_NONE || ClownAttackStage != EClownAttackStage::Montage ||
		!CurrentClownAttack.DamageInfo.IsValidIndex(CurrentDamageIndex))
	{
		return;
	}

	if (ICombatDamageInterface* Target = Cast<ICombatDamageInterface>(Hit.GetActor()))
	{
		const FDamageInfo& Damage = CurrentClownAttack.DamageInfo[CurrentDamageIndex];

		UE_LOG(LogTemp, Log, TEXT("BossClown: Hit %s, DamageIndex %d, Damage %.1f"), *GetNameSafe(Hit.GetActor()), CurrentDamageIndex, Damage.AttackDamage);

		Target->ReceiveDamage(Damage, Hit);
	}
}

void ABossClown::StopFastChaseAnimation()
{
	if (UAnimInstance* Anim = AttackAnimInstance.Get())
	{
		if (ChaseMontage.IsValid())
		{
			Anim->Montage_Stop(0.12f, ChaseMontage.Get());
		}
	}
	ChaseMontage.Reset();
}

void ABossClown::ClearClownProjectiles(bool bIncludeLaunched)
{
	for (const TWeakObjectPtr<AActor>& Item : ClownProjectiles)
	{
		AActor* Actor = Item.Get();
		UClownProjectileComponent* Projectile = Actor ? Actor->FindComponentByClass<UClownProjectileComponent>() : nullptr;
		if (Actor && (bIncludeLaunched || !Projectile || !Projectile->IsLaunched()))
		{
			Actor->Destroy();
		}
	}
	ClownProjectiles.RemoveAll([](const TWeakObjectPtr<AActor>& Item) { return !Item.IsValid(); });
}

UClownProjectileComponent* ABossClown::CreateClownProjectile(TSubclassOf<AActor> Class, const FVector& Location, const FRotator& Rotation, float Speed, float LifeTime)
{
	if (!Class || !CurrentClownAttack.DamageInfo.IsValidIndex(0))
	{
		return nullptr;
	}
	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.Instigator = this;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActor* Actor = GetWorld()->SpawnActor<AActor>(Class, Location, Rotation, Params);
	UClownProjectileComponent* Projectile = Actor ? Actor->FindComponentByClass<UClownProjectileComponent>() : nullptr;
	if (!Projectile)
	{
		if (Actor)
		{
			Actor->Destroy();
		}
		return nullptr;
	}
	Projectile->Initialize(CurrentClownAttack.DamageInfo[0], Speed, LifeTime);
	ClownProjectiles.Add(Actor);
	return Projectile;
}

void ABossClown::PrepareClownBalls()
{
	ClearClownProjectiles(false);
	const UBossClownAnimationData* Data = GetClownAnimationData();
	AActor* Target = ClownAttackTarget.Get();

	const int32 Count = FMath::Clamp(Data->BallCount, 1, 16);
	// 바닥에 정확히 수직인 원. 플레이어 높이에 따라 소환 평면을 기울이지 않음.
	const FVector Center = GetActorLocation() + (Target ? Target->GetActorLocation() - GetActorLocation() : GetActorForwardVector()).GetSafeNormal2D() * 180.0f + FVector(0.0f, 0.0f, 260.0f);
	const FVector Forward = (Target ? Target->GetActorLocation() - Center : GetActorForwardVector()).GetSafeNormal2D();
	const FQuat Facing = FRotationMatrix::MakeFromX(Forward).ToQuat();
	const FVector Right = Facing.GetRightVector();
	const FVector Up = FVector::UpVector;
	const float MarkerRadius = 210.0f;

	for (int32 Index = 0; Index < Count; ++Index)
	{
		const double StartAngle = 2.0 * PI * Index / Count;
		const FVector Position = Center + (Right * FMath::Cos(StartAngle) + Up * FMath::Sin(StartAngle)) * MarkerRadius;

		if (UClownProjectileComponent* Ball = CreateClownProjectile(Data->BallClass, Position, Facing.Rotator(), Data->BallSpeed, 6.0f))
		{
			const FVector Axis = FMath::VRand();
			const float Rate = FMath::FRandRange(140.0f, 320.0f);
			Ball->SetVisualSpin(FRotator(Axis.X * Rate, Axis.Y * Rate, Axis.Z * Rate));
		}
	}
}

void ABossClown::PrepareClownCards()
{
	ClearClownProjectiles(false);
	const UBossClownAnimationData* Data = GetClownAnimationData();
	AActor* Target = ClownAttackTarget.Get();
	if (!Data || !Data->CardClass || !IsValid(Target) || Data->CardFaces.IsEmpty() || Data->HeartFaces.IsEmpty())
	{
		return;
	}

	// 모든 낙하 방향은 소환 순간의 플레이어 위치로 고정. 이후 추적하지 않음.
	const FVector Center = Target->GetActorLocation();
	const int32 Count = FMath::Clamp(Data->CardCount, 2, 10);
	const float Width = FMath::Clamp(Data->CardWidth, 350.0f, 550.0f);
	// 하트 밑변의 안전 표시(100 x 120)에 인접 카드와 플레이어 캡슐이 겹치지 않도록 간격 확보.
	const float AngleStep = 2.0f * PI / Count;
	const float SafeRadius = Count > 2 ? (Width * 0.5f + 157.0f + 60.0f * FMath::Abs(FMath::Cos(AngleStep))) / FMath::Sin(AngleStep) + 90.0f : 600.0f;
	const float Radius = FMath::Max(FMath::Clamp(Data->CardRadius, 600.0f, 900.0f), SafeRadius);
	const float Height = FMath::Clamp(Data->CardHeight, Radius - 100.0f, Radius + 50.0f);
	const float Facing = FMath::FRandRange(0.0f, 2.0f * PI);
	TArray<FVector> Positions;
	FCollisionQueryParams Query(SCENE_QUERY_STAT(ClownCardGround), false, this);
	Query.AddIgnoredActor(Target);
	for (int32 Index = 0; Index < Count; ++Index)
	{
		const float Angle = Facing + 2.0f * PI * Index / Count;
		const FVector Position = Center + FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0.0f) * Radius;
		FHitResult Ground;
		if (GetWorld()->LineTraceSingleByChannel(Ground, Position + FVector(0.0f, 0.0f, 250.0f), Position - FVector(0.0f, 0.0f, 600.0f), ECC_Visibility, Query) && Ground.ImpactNormal.Z >= 0.9f)
		{
			Positions.Add(Ground.ImpactPoint);
		}
	}
	if (Positions.IsEmpty())
	{
		return;
	}
	// 실제 생성 가능한 위치 중 하트를 먼저 생성. 실패하면 나머지 위험 카드도 생성하지 않음.
	Positions.Swap(0, FMath::RandRange(0, Positions.Num() - 1));
	for (int32 Index = 0; Index < Positions.Num(); ++Index)
	{
		const bool bSafe = Index == 0 || FMath::FRand() < 0.12f;
		const auto& Faces = bSafe ? Data->HeartFaces : Data->CardFaces;
		UTexture2D* Face = Faces[FMath::RandRange(0, Faces.Num() - 1)].Get();
		FVector Direction = Center - Positions[Index];
		Direction.Z = 0.0f;
		UClownProjectileComponent* Projectile = Face ? CreateClownProjectile(Data->CardClass, Positions[Index], Direction.Rotation(), 0.0f, Data->CardActiveTime) : nullptr;
		if (!Projectile)
		{
			if (Index == 0)
			{
				return;
			}
			continue;
		}
		Projectile->ArmCard(Positions[Index], Data->CardFirstLaunchTime, Height, Width, Data->CardFallTime, Face, bSafe);
		if (Projectile->GetOwner()->IsActorBeingDestroyed() && Index == 0)
		{
			return;
		}
	}
}

void ABossClown::TickClownSpecial(float MontageTime)
{
	const UBossClownAnimationData* Data = GetClownAnimationData();
	if (!Data || !ClownAttackTarget.IsValid())
	{
		return;
	}
	if (CurrentAttackType == EBossClownAttack::CubeBarrage)
	{
		if (AttackStageTime >= UClownProjectileComponent::CubeMarchCycle * 3.0f)
		{
			if (UAnimInstance* Anim = AttackAnimInstance.Get())
			{
				Anim->Montage_SetNextSection(CurrentClownAttack.Montage->GetSectionName(0), NAME_None, CurrentClownAttack.Montage.Get());
			}
		}
	}
	else if (CurrentAttackType == EBossClownAttack::TripleBalls)
	{
		// 매 발사 순간의 위치만 조준. 발사 후에는 유도하지 않아 횡이동 / 회피 가능.
		while (BallsLaunched < FMath::Clamp(Data->BallCount, 1, 16) && AttackStageTime >= Data->BallFirstLaunchTime + BallsLaunched * Data->BallLaunchInterval)
		{
			for (const TWeakObjectPtr<AActor>& Item : ClownProjectiles)
			{
				if (Item.IsValid() && Item->FindComponentByClass<UClownProjectileComponent>() && !Item->FindComponentByClass<UClownProjectileComponent>()->IsLaunched())
				{
					const FVector Aim = ClownAttackTarget->GetActorLocation();
					Item->FindComponentByClass<UClownProjectileComponent>()->LaunchAt(Aim);
					UE_LOG(LogTemp, Log, TEXT("Clown ball fired: %d"), BallsLaunched + 1);
					break;
				}
			}
			++BallsLaunched;
		}
	}
	else if (CurrentAttackType == EBossClownAttack::ClapWave && !bClapReleased && MontageTime >= Data->ClapImpactTime)
	{
		bClapReleased = true;
		const FVector Position = GetActorLocation() + GetActorForwardVector() * 100.0f;
		const int32 NoteCount = FMath::RandRange(16, 22);
		for (int32 Index = 0; Index < NoteCount; ++Index)
		{
			const float Angle = FMath::Lerp(-28.0f, 28.0f, (Index + FMath::FRand()) / NoteCount);
			const FVector Direction = GetActorForwardVector().RotateAngleAxis(Angle, FVector::UpVector);
			const FVector Spawn = Position + GetActorRightVector() * FMath::FRandRange(-150.0f, 150.0f) + FVector(0.0f, 0.0f, FMath::FRandRange(-35.0f, 65.0f));
			if (UClownProjectileComponent* Projectile = CreateClownProjectile(Data->ClapWaveClass, Spawn, Direction.Rotation(), Data->ClapWaveSpeed, Data->ClapWaveRange / FMath::Max(Data->ClapWaveSpeed, 1.0f)))
			{
				Projectile->GetOwner()->SetActorScale3D(FVector(FMath::FRandRange(0.75f, 1.25f)));
				Projectile->LaunchAt(Spawn + Direction * Data->ClapWaveRange);
			}
		}
	}
}


bool ABossClown::PrepareClownCubeMarch()
{
	ClearClownProjectiles(false);
	const UBossClownAnimationData* Data = GetClownAnimationData();
	AActor* Target = ClownAttackTarget.Get();
	if (!Data || !Data->CubeClass || Data->CubePieces.Num() != 27 || !IsValid(Target))
	{
		return false;
	}
	const FVector Center = Target->GetActorLocation();
	const FVector Forward = (Center - GetActorLocation()).GetSafeNormal2D();
	const FVector Assembly = GetActorLocation() + Forward * 180.0f + FVector(0.0f, 0.0f, 480.0f);
	TArray<FVector> Starts;
	TArray<FVector> Ends;
	FCollisionQueryParams Query(SCENE_QUERY_STAT(ClownCubeGround), false, this);
	Query.AddIgnoredActor(Target);
	for (int32 Wave = 0; Wave < 3; ++Wave)
	{
		const FVector Direction = Forward.RotateAngleAxis(Wave == 0 ? 0.0f : (Wave == 1 ? 90.0f : -45.0f), FVector::UpVector);
		const FVector Right = FVector::CrossProduct(FVector::UpVector, Direction);
		const int32 SafeLane = FMath::RandRange(0, 3);
		for (int32 Lane = 0; Lane < 3; ++Lane)
		{
			const int32 Slot = Lane >= SafeLane ? Lane + 1 : Lane;
			for (int32 Row = 0; Row < 3; ++Row)
			{
				const FVector Offset = Right * ((Slot - 1.5f) * 240.0f) - Direction * (Row * 145.0f);
				FVector Start = Center - Direction * 650.0f + Offset;
				FVector End = Center + Direction * 650.0f + Offset;
				FHitResult Ground;
				if (!GetWorld()->LineTraceSingleByChannel(Ground, Start + FVector(0.0f, 0.0f, 400.0f), Start - FVector(0.0f, 0.0f, 700.0f), ECC_Visibility, Query) || Ground.ImpactNormal.Z < 0.9f)
				{
					return false;
				}
				Start.Z = Ground.ImpactPoint.Z + 52.0f;
				if (!GetWorld()->LineTraceSingleByChannel(Ground, End + FVector(0.0f, 0.0f, 400.0f), End - FVector(0.0f, 0.0f, 700.0f), ECC_Visibility, Query) || Ground.ImpactNormal.Z < 0.9f || FMath::Abs(Ground.ImpactPoint.Z + 52.0f - Start.Z) > 25.0f)
				{
					return false;
				}
				End.Z = Start.Z;
				Starts.Add(Start);
				Ends.Add(End);
			}
		}
	}
	for (int32 Index = 0; Index < 27; ++Index)
	{
		const FVector Offset((Index % 3 - 1) * 98.0f, (Index / 3 % 3 - 1) * 98.0f, (Index / 9 - 1) * 98.0f);
		UClownProjectileComponent* Piece = CreateClownProjectile(Data->CubeClass, Assembly + Offset, FRotator::ZeroRotator, 1.0f, UClownProjectileComponent::CubeMarchDuration);
		if (!Piece || !Data->CubePieces[Index])
		{
			ClearClownProjectiles(true);
			return false;
		}
		Piece->ArmCubeMarch(Data->CubePieces[Index], Assembly + Offset, Starts[Index], Ends[Index], UClownProjectileComponent::CubeMarchCycle * (Index / 9 + 1), Index % 3 == 0);
	}
	return true;
}
