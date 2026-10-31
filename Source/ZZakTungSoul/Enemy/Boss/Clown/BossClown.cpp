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
#include <Engine/World.h>
#include <UObject/ConstructorHelpers.h>
#include <Components/CapsuleComponent.h>
#include <Navigation/PathFollowingComponent.h>
#include <Enemy/Boss/Clown/BossClownProjectile.h>

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

	const UBossClownAnimationData* Data = GetClownAnimationData();
	if (!Data)
	{
		return nullptr;
	}

	switch (AttackType)
	{
	case EBossClownAttack::HammerSweep:
		return &Data->HammerSweep;
	case EBossClownAttack::HammerSlam:
		return &Data->HammerSlam;
	default:
		return nullptr;
	}
}

bool ABossClown::IsClownCombatPhase() const
{
	return !IsDead() && (BossPhase == EBossPhase::Phase1 || BossPhase == EBossPhase::Phase2);
}

bool ABossClown::StartClownAttack(EBossClownAttack AttackType, AActor* Target, int32& OutExecutionID)
{
	OutExecutionID = INDEX_NONE;

	if (!IsClownCombatPhase() || ActiveAttackID != INDEX_NONE || !IsValid(Target) || !AttackTraceComponent)
	{
		return false;
	}

	const FEnemyAttackData* AttackData = GetClownAttackData(AttackType);
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

	if (!Direction.IsNearlyZero())
	{
		SetActorRotation(Direction.Rotation());
	}

	CurrentClownAttack = *AttackData;
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
		ClownAttackStage = EClownAttackStage::Approach;
		NextChasePathTime = 0.0;
		SetEnemyState(EEnemyState::FastChase);
		GetCharacterMovement()->MaxWalkSpeed = GetClownAnimationData()->FastChaseSpeed;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		// 이동은 CharacterMovement가 담당하고 달리기만 반복 재생.
		if (UAnimMontage* Run = GetClownAnimationData()->FastChaseMontage.Get())
		{
			if (Anim->Montage_Play(Run, GetClownAnimationData()->FastChaseAnimationRate) > 0.0f)
			{
				ChaseMontage = Run;
				const FName Section = Run->GetSectionName(0);
				Anim->Montage_SetNextSection(Section, Section, Run);
			}
		}
		GetWorld()->GetTimerManager().SetTimer(AttackTimeoutHandle, this, &ABossClown::OnClownAttackTimeout, FMath::Max(AttackTimeout, 0.1f), false);
		return true;
	}

	if (!PlayCurrentClownMontage())
	{
		return false;
	}

	return true;
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

	if (CurrentAttackType == EBossClownAttack::TripleBalls || CurrentAttackType == EBossClownAttack::ClapWave)
	{
		if (WeaponMeshComponent)
		{
			bWeaponWasVisible = WeaponMeshComponent->IsVisible();
			bHidWeapon = true;
			WeaponMeshComponent->SetVisibility(false);
		}
		if (CurrentAttackType == EBossClownAttack::TripleBalls)
		{
			PrepareClownBalls();
		}
	}

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &ABossClown::OnClownAttackEnded, ActiveAttackID);
	Anim->Montage_SetEndDelegate(EndDelegate, Montage);

	GetWorld()->GetTimerManager().SetTimer(AttackTimeoutHandle, this, &ABossClown::OnClownAttackTimeout, FMath::Max(AttackTimeout, 0.1f), false);

	return true;
}

const FClownPhaseOnePattern* ABossClown::GetPhaseOnePattern(EBossClownAttack AttackType) const
{
	const UBossClownAnimationData* Data = GetClownAnimationData();
	return Data ? Data->PhaseOnePatterns.FindByPredicate([AttackType](const FClownPhaseOnePattern& Pattern) { return Pattern.AttackType == AttackType; }) : nullptr;
}

bool ABossClown::StartBossCombatBehavior(AActor* Target)
{
	bNativePhaseOneActive = false;
	ClownCombatTarget.Reset();
	const UBossClownAnimationData* Data = GetClownAnimationData();
	if (BossPhase != EBossPhase::Phase1 || !Data || !Data->bUseNativePhaseOne || !IsValid(Target))
	{
		return false;
	}

	// 미완성 데이터에서는 기존 BT로 복귀. 빈 패턴으로 전투가 멈추지 않도록 검사.
	if (!Data->PhaseOnePatterns.ContainsByPredicate([](const FClownPhaseOnePattern& Pattern)
		{ return Pattern.bEnabled && Pattern.Attack.Montage && Pattern.Attack.DamageInfo.IsValidIndex(0) && Pattern.Weight > 0.0f; }))
	{
		return false;
	}

	ClownCombatTarget = Target;
	bNativePhaseOneActive = true;
	NextPatternDecisionTime = GetWorld()->GetTimeSeconds() + 0.6;
	NextChasePathTime = 0.0;
	return true;
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

	if (ActiveAttackID != INDEX_NONE)
	{
		TickClownAttack(DeltaTime);
	}
	if (bNativePhaseOneActive)
	{
		if (BossPhase != EBossPhase::Phase1 || IsDead())
		{
			bNativePhaseOneActive = false;
			ClownCombatTarget.Reset();
		}
		else if (ActiveAttackID == INDEX_NONE)
		{
			TickPhaseOneSelection(DeltaTime);
		}
	}
}

void ABossClown::TickPhaseOneSelection(float DeltaTime)
{
	AActor* Target = ClownCombatTarget.Get();
	AAIController* AI = Cast<AAIController>(GetController());
	const UBossClownAnimationData* Data = GetClownAnimationData();
	if (!IsValid(Target) || Target->IsActorBeingDestroyed() || !AI || !Data)
	{
		ResetEncounter();
		return;
	}

	const double Now = GetWorld()->GetTimeSeconds();
	const float Distance = GetTargetSurfaceDistance(Target);
	FaceClownTarget(Target, DeltaTime);
	if (Now < NextPatternDecisionTime)
	{
		return;
	}
	NextPatternDecisionTime = Now + 0.2;

	FVector Direction = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();
	TArray<const FClownPhaseOnePattern*> Candidates;
	float TotalWeight = 0.0f;
	if (FVector::DotProduct(GetActorForwardVector(), Direction) > 0.5f && AI->LineOfSightTo(Target))
	{
		for (const FClownPhaseOnePattern& Pattern : Data->PhaseOnePatterns)
		{
			const bool bMovement = Pattern.AttackType == EBossClownAttack::FastChaseSlam || Pattern.AttackType == EBossClownAttack::DashSweep;
			if (!Pattern.bEnabled || !Pattern.Attack.Montage || !Pattern.Attack.DamageInfo.IsValidIndex(0) || Pattern.Weight <= 0.0f ||
				Distance < Pattern.MinDistance || Distance > Pattern.MaxDistance ||
				(bHasPreviousAttack && Pattern.AttackType == PreviousAttackType) ||
				Now < PatternReadyTimes.FindRef(Pattern.AttackType) || (bMovement && Now < MovementPatternReadyTime))
			{
				continue;
			}
			Candidates.Add(&Pattern);
			TotalWeight += Pattern.Weight;
		}
	}

	if (Candidates.Num() > 0)
	{
		float Pick = FMath::FRandRange(0.0f, TotalWeight);
		const FClownPhaseOnePattern* Selected = Candidates.Last();
		for (const FClownPhaseOnePattern* Candidate : Candidates)
		{
			Pick -= Candidate->Weight;
			if (Pick <= 0.0f)
			{
				Selected = Candidate;
				break;
			}
		}
		int32 StartedID = INDEX_NONE;
		if (StartClownAttack(Selected->AttackType, Target, StartedID))
		{
			PreviousAttackType = Selected->AttackType;
			bHasPreviousAttack = true;
			PatternReadyTimes.Add(Selected->AttackType, Now + Selected->Cooldown);
			if (Selected->AttackType == EBossClownAttack::FastChaseSlam || Selected->AttackType == EBossClownAttack::DashSweep)
			{
				MovementPatternReadyTime = Now + Data->MovementAttackCooldown;
			}
			UE_LOG(LogTemp, Log, TEXT("Clown Phase1: pattern %d, surface distance %.1f"), static_cast<int32>(Selected->AttackType), Distance);
			return;
		}
	}

	// 공격이 준비되지 않았을 때는 일반 접근. 근거리에서 이동 공격을 억지로 선택하지 않음.
	if (Distance > 100.0f && Now >= NextChasePathTime)
	{
		SetEnemyState(EEnemyState::Chase);
		AI->MoveToActor(Target, 90.0f, true);
		NextChasePathTime = Now + 0.4;
	}
	else if (Distance <= 100.0f)
	{
		AI->StopMovement();
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
		if (AttackStageTime >= Data->FastChaseMaxTime)
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
			if (Now >= NextChasePathTime)
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

	if (ClownAttackStage == EClownAttackStage::Recovery)
	{
		if (!Pattern || AttackStageTime >= Pattern->RecoveryTime)
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
	NextPatternDecisionTime = GetWorld()->GetTimeSeconds() + 0.15;
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
	ClearClownProjectiles(false);
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
	ClearClownProjectiles(true);
	FinishClownAttack(EBossClownAttackResult::Aborted, true, false);
	bNativePhaseOneActive = false;
	ClownCombatTarget.Reset();
	PatternReadyTimes.Reset();
	bHasPreviousAttack = false;
	NextPatternDecisionTime = 0.0;
	MovementPatternReadyTime = 0.0;
	NextChasePathTime = 0.0;
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
	if (!IsClownCombatPhase() || ActiveAttackID == INDEX_NONE ||
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
	for (const TWeakObjectPtr<ABossClownProjectile>& Item : ClownProjectiles)
	{
		if (Item.IsValid() && (bIncludeLaunched || !Item->IsLaunched()))
		{
			Item->Destroy();
		}
	}
	ClownProjectiles.RemoveAll([](const TWeakObjectPtr<ABossClownProjectile>& Item) { return !Item.IsValid(); });
}

void ABossClown::PrepareClownBalls()
{
	ClearClownProjectiles(false);
	const UBossClownAnimationData* Data = GetClownAnimationData();
	for (int32 Index = 0; Index < 3; ++Index)
	{
		const FVector Position = GetActorLocation() + GetActorForwardVector() * 80.0f +
			GetActorRightVector() * (Index - 1) * 95.0f + FVector(0.0f, 0.0f, 110.0f);
		FActorSpawnParameters Params;
		Params.Owner = this;
		Params.Instigator = this;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		if (ABossClownProjectile* Ball = GetWorld()->SpawnActor<ABossClownProjectile>(Position, GetActorRotation(), Params))
		{
			Ball->Initialize(CurrentClownAttack.DamageInfo[0], Data->BallSpeed, false, Data->BallMaterial.Get());
			ClownProjectiles.Add(Ball);
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
	if (CurrentAttackType == EBossClownAttack::TripleBalls)
	{
		// 매 발사 순간의 위치만 조준. 발사 후에는 유도하지 않아 횡이동 / 회피 가능.
		while (BallsLaunched < 3 && MontageTime >= Data->BallFirstLaunchTime + BallsLaunched * Data->BallLaunchInterval)
		{
			for (const TWeakObjectPtr<ABossClownProjectile>& Item : ClownProjectiles)
			{
				if (Item.IsValid() && !Item->IsLaunched())
				{
					Item->LaunchAt(ClownAttackTarget->GetActorLocation());
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
		FActorSpawnParameters Params;
		Params.Owner = this;
		Params.Instigator = this;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		const FVector Position = GetActorLocation() + GetActorForwardVector() * 100.0f;
		if (ABossClownProjectile* Wave = GetWorld()->SpawnActor<ABossClownProjectile>(Position, GetActorRotation(), Params))
		{
			Wave->Initialize(CurrentClownAttack.DamageInfo[0], Data->ClapWaveSpeed, true, Data->ClapWaveMaterial.Get());
			Wave->SetLifeSpan(Data->ClapWaveRange / FMath::Max(Data->ClapWaveSpeed, 1.0f));
			Wave->LaunchAt(Position + GetActorForwardVector() * Data->ClapWaveRange);
			ClownProjectiles.Add(Wave);
			UE_LOG(LogTemp, Log, TEXT("Clown clap released"));
		}
	}
}
