#include <Enemy/Boss/Clown/BossClown.h>
#include <Interface/CombatDamageInterface.h>
#include <Enemy/Boss/Clown/Anim/BossClownAnimationInstance.h>
#include <Enemy/Components/AttackTraceComponent.h>
#include <Animation/AnimInstance.h>
#include <Animation/AnimMontage.h>
#include <Animation/AnimSequence.h>
#include <Components/PointLightComponent.h>
#include <Engine/PointLight.h>
#include <Chaos/CacheManagerActor.h>
#include <GeometryCollection/GeometryCollectionComponent.h>
#include <GeometryCollection/GeometryCollectionObject.h>
#include <NiagaraComponent.h>
#include <NiagaraFunctionLibrary.h>
#include <NiagaraSystem.h>
#include <Components/SkeletalMeshComponent.h>
#include <Engine/SkeletalMesh.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <GameFramework/ProjectileMovementComponent.h>
#include <AIController.h>
#include <BrainComponent.h>
#include <Engine/World.h>
#include <UObject/ConstructorHelpers.h>
#include <Components/CapsuleComponent.h>
#include <Navigation/PathFollowingComponent.h>
#include <Algo/AllOf.h>
#include <Engine/OverlapResult.h>
#include <Enemy/Boss/Clown/Components/ClownProjectileComponent.h>
#include <Enemy/EnemyAIController.h>
#include <Enemy/Components/EnemyStatComponent.h>
#include <Enemy/Boss/Clown/Components/ClownYabawiComponent.h>
#include <Enemy/Boss/Clown/Gimmick/ClownFracture.h>
#include <Enemy/Boss/Encounter/BossRoom.h>
#include <Warrior/Warrior.h>
#include <EngineUtils.h>
#include <Components/BoxComponent.h>
#include <Engine/StaticMesh.h>
#include <Engine/StaticMeshActor.h>
#include <Components/StaticMeshComponent.h>
#include <Engine/Texture2D.h>
#include <Materials/MaterialInterface.h>
#include <Materials/MaterialInterface.h>

namespace
{
constexpr float Phase2DebrisFadeStartTime = 20.0f;
}

ABossClown::ABossClown()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	YabawiComponent = CreateDefaultSubobject<UClownYabawiComponent>(TEXT("YabawiComponent"));

	Phase2RevealLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("Phase2RevealLight"));
	Phase2RevealLight->SetupAttachment(GetRootComponent());
	Phase2RevealLight->SetRelativeLocation(FVector(56.0f, 0.0f, -60.0f));
	Phase2RevealLight->SetLightColor(FLinearColor(1.0f, 0.18f, 0.08f));
	Phase2RevealLight->SetAttenuationRadius(650.0f);
	Phase2RevealLight->SetCastShadows(false);
	Phase2RevealLight->SetVisibility(false);

	Phase2WallFog = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Phase2WallFog"));
	Phase2WallFog->SetupAttachment(GetRootComponent());
	Phase2WallFog->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Phase2WallFog->SetGenerateOverlapEvents(false);
	Phase2WallFog->SetCanEverAffectNavigation(false);
	Phase2WallFog->SetCastShadow(false);
	Phase2WallFog->SetVisibility(false);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> FogPlane(TEXT("/Engine/BasicShapes/Plane.Plane"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> RoomFog(
		TEXT("/Game/Blueprints/Enemy/Boss/Common/Encounter/M_Fog.M_Fog"));
	if (FogPlane.Succeeded() && RoomFog.Succeeded())
	{
		Phase2WallFog->SetStaticMesh(FogPlane.Object);
		Phase2WallFog->SetMaterial(0, RoomFog.Object);
	}

	// 광대 SkeletalMesh 설정.
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletonMeshRef(
		TEXT("/Game/Assets/Clown_monster/Meshes/base_mesh/SKM_base_mesh.SKM_base_mesh"));

	if (SkeletonMeshRef.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SkeletonMeshRef.Object);

		GetMesh()->SetRelativeLocation(FVector(0, 0, -90));
		GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));
		GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

		static ConstructorHelpers::FClassFinder<UBossClownAnimationInstance> AnimClassRef(
			TEXT("/Game/Blueprints/Enemy/Boss/Clown/Animation/ABP_BossClown.ABP_BossClown_C"));

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

void ABossClown::BeginPlay()
{
	Super::BeginPlay();
	FindEncounterFloor();
	RegisterIntroBreakables();
	if (AttackTraceComponent)
	{
		AttackTraceComponent->OnGroundTraceHit.AddUObject(this, &ABossClown::HandleMeleeGroundContact);
	}

	OnPhase2StageRequested.AddDynamic(this, &ABossClown::PlayPhase2Stage);
	OnPhase2TransitionStopped.AddDynamic(this, &ABossClown::StopPhase2Presentation);
}

void ABossClown::RegisterIntroBreakables()
{
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* Prop = *It;
		if (!Prop->ActorHasTag(TEXT("ClownIntroBreakable")))
		{
			continue;
		}

		if (UMeshComponent* PropMesh = Prop->FindComponentByClass<UMeshComponent>())
		{
			// Contact is queried separately so stage props cannot push either character.
			PropMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
			IntroBreakables.Add(PropMesh);
		}
	}
}

void ABossClown::UpdateIntroBreakables()
{
	if (!IsClownCombatPhase() && GetBossPhase() != EBossPhase::OpeningAttack)
	{
		bIntroBreakablesActive = false;
		return;
	}

	const AWarrior* Player = Cast<AWarrior>(GetEncounterTarget());
	const FVector BossPosition = GetActorLocation();
	const FVector PlayerPosition = Player ? Player->GetActorLocation() : FVector::ZeroVector;
	const bool bBossMoved = bIntroBreakablesActive && !BossPosition.Equals(LastIntroBossPosition, 0.1);
	const bool bPlayerMoved = bIntroBreakablesActive && Player && !Player->IsDead() && !PlayerPosition.Equals(LastIntroPlayerPosition, 0.1);
	LastIntroBossPosition = BossPosition;
	LastIntroPlayerPosition = PlayerPosition;
	bIntroBreakablesActive = true;
	if (!bBossMoved && !bPlayerMoved)
	{
		return;
	}

	for (const TWeakObjectPtr<UMeshComponent>& Prop : IntroBreakables)
	{
		UMeshComponent* PropMesh = Prop.Get();
		if (!PropMesh || !PropMesh->IsVisible() || PropMesh->GetOwner()->IsHidden() || BrokenIntroProps.Contains(Prop))
		{
			continue;
		}

		const FTransform Transform = PropMesh->GetComponentTransform();
		const FBoxSphereBounds Bounds = PropMesh->CalcBounds(FTransform::Identity);
		const FVector Center = Transform.TransformPosition(Bounds.Origin);
		const FVector Extent = Bounds.BoxExtent * Transform.GetScale3D().GetAbs() + FVector(2.0);
		const FCollisionShape Shape = FCollisionShape::MakeBox(Extent);
		const FQuat Rotation = Transform.GetRotation();
		const bool bBossContact = bBossMoved && GetCapsuleComponent()->OverlapComponent(Center, Rotation, Shape);
		const bool bPlayerContact = bPlayerMoved && Player->GetCapsuleComponent()->OverlapComponent(Center, Rotation, Shape);
		if ((bBossContact || bPlayerContact) && ClownFracture::PlayCached(*this, *PropMesh))
		{
			BrokenIntroProps.Add(Prop, PropMesh->GetCollisionEnabled());
			PropMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

bool ABossClown::CanBeFatalAttacked(AActor* Attacker) const
{
	if (!IsValid(Attacker) || !IsClownCombatPhase() || bReceiveFatalAttack || !Super::CanBeFatalAttacked(Attacker))
	{
		return false;
	}

	const UBossClownAnimationData* Data = GetClownAnimationData();
	const FVector Offset = (Attacker->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();
	const double Side = FVector::DotProduct(Offset, GetActorForwardVector());
	const bool bFront = FVector::DotProduct(Attacker->GetActorForwardVector(), GetActorForwardVector()) < 0.0;
	return Data && (bFront ? (Side > 0.35 && Data->FrontCriticalReaction) : (Side < -0.35 && Data->BackCriticalReaction));
}

void ABossClown::GetCriticalAttackPlacement(const ACharacter& Attacker, bool bFront, FVector& Location, FRotator& Rotation) const
{
	const double Clearance = GetCapsuleComponent()->GetScaledCapsuleRadius() + Attacker.GetCapsuleComponent()->GetScaledCapsuleRadius();
	const double ForwardOffset = bFront ? Clearance + 52.0 : -Clearance - 9.0;
	Location = GetActorLocation() + GetActorForwardVector() * ForwardOffset + GetActorRightVector() * (bFront ? 32.0 : -18.0);
	Location.Z = Attacker.GetActorLocation().Z;
	Rotation = FRotator(0.0, GetActorRotation().Yaw + (bFront ? 180.0 : 0.0), 0.0);
}

void ABossClown::ReceiveFatalAttack(const FDamageInfo DamageInfo, bool bIsFront)
{
	const UBossClownAnimationData* Data = GetClownAnimationData();
	UAnimInstance* Anim = GetMesh()->GetAnimInstance();
	UAnimMontage* Reaction = Data ? (bIsFront ? Data->FrontCriticalReaction.Get() : Data->BackCriticalReaction.Get()) : nullptr;
	if (!IsClownCombatPhase() || bReceiveFatalAttack || !Anim || !Reaction)
	{
		return;
	}

	FinishClownAttack(EBossClownAttackResult::Aborted, true, false);
	SetEnemyState(EEnemyState::Idle);
	GetCharacterMovement()->StopMovementImmediately();
	if (AttackTraceComponent)
	{
		AttackTraceComponent->EndTrace();
	}
	if (Anim->Montage_Play(Reaction) <= 0.0f)
	{
		return;
	}

	bReceiveFatalAttack = true;
	CurrentFatalMontage = Reaction;
	SetEnemyActionState(bIsFront ? EEnemyActionState::GetFrontFatalAttack : EEnemyActionState::GetBackFatalAttack);
	if (AEnemyAIController* AI = Cast<AEnemyAIController>(GetController()))
	{
		AI->SetAIWait(true);
		if (UBrainComponent* Brain = AI->GetBrainComponent(); Brain && Brain->IsRunning() && !Brain->IsPaused())
		{
			Brain->PauseLogic(TEXT("Clown critical reaction"));
			CriticalReactionBrain = Brain;
		}
	}

	FOnMontageEnded OnEnded;
	OnEnded.BindUObject(this, &ABossClown::OnCriticalReactionEnded);
	Anim->Montage_SetEndDelegate(OnEnded, Reaction);
	ReceiveDamage(DamageInfo, FHitResult());
}

void ABossClown::UpdateRotationModeState()
{
	if (!bReceiveFatalAttack)
	{
		Super::UpdateRotationModeState();
		return;
	}

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
}

void ABossClown::OnCriticalReactionEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != CurrentFatalMontage)
	{
		return;
	}

	const bool bShouldDie = bDeathAfterCritical;
	bDeathAfterCritical = false;
	const TWeakObjectPtr<UBrainComponent> PausedBrain = CriticalReactionBrain;
	CriticalReactionBrain.Reset();
	OnFatalAttackFinished();
	if (!IsClownCombatPhase() || IsActorBeingDestroyed())
	{
		return;
	}

	if (bShouldDie)
	{
		SetDead();
	}
	else if (EnemyStatComponet)
	{
		TryStartPhase2Transition(EnemyStatComponet->GetCurrentHp(), EnemyStatComponet->GetMaxHp());
	}

	if (IsClownCombatPhase() && PausedBrain.IsValid())
	{
		PausedBrain->ResumeLogic(TEXT("Clown critical reaction ended"));
	}
}

void ABossClown::TryStartPhase2Transition(float CurrentHp, float MaxHp)
{
	if (!bReceiveFatalAttack)
	{
		Super::TryStartPhase2Transition(CurrentHp, MaxHp);
	}
}

void ABossClown::SetDead()
{
	if (bReceiveFatalAttack)
	{
		bDeathAfterCritical = true;
		return;
	}

	Super::SetDead();
}

void ABossClown::DeadStart()
{
	const UBossClownAnimationData* Data = GetClownAnimationData();
	if (UAnimInstance* Anim = GetMesh()->GetAnimInstance(); Anim && Data && Data->DeathReaction)
	{
		Anim->Montage_Play(Data->DeathReaction, 0.76f);
	}
}

void ABossClown::PlayPhase2Stage(ABoss* Boss, EBossPhase2TransitionStage Stage, int32 TransitionID)
{
	if (GetPhase2TransitionID() != TransitionID || GetPhase2TransitionStage() != Stage)
	{
		return;
	}

	StopPhase2Presentation(Boss, TransitionID, false);

	if (Stage == EBossPhase2TransitionStage::Searching)
	{
		if (IsValid(Phase2SearchLight))
		{
			Phase2SearchLight->GetLightComponent()->SetVisibility(true);
		}
		return;
	}

	const bool bEmerging = Stage == EBossPhase2TransitionStage::Emerging;
	UAnimSequence* Sequence = bEmerging ? Phase2EmergenceAnimation.Get() : Phase2DisappearAnimation.Get();
	const float Duration = bEmerging ? Phase2EmergencePlaceholderTime : Phase2DisappearPlaceholderTime;

	if (UAnimInstance* Anim = GetMesh()->GetAnimInstance(); Anim && Sequence)
	{
		const float Rate = Sequence->GetPlayLength() / FMath::Max(Duration, 0.01f);
		Phase2StageMontage = Anim->PlaySlotAnimationAsDynamicMontage(Sequence, TEXT("DefaultSlot"), 0.08f, 0.12f, Rate);
	}

	if (bEmerging && Phase2StageMontage.IsValid() && WeaponMeshComponent)
	{
		Phase2WeaponVisibility = WeaponMeshComponent->IsVisible();
		WeaponMeshComponent->SetVisibility(false);
	}

	if (bEmerging && Sequence)
	{
		if (IsValid(Phase2CombatPoint))
		{
			Phase2AdvanceOrigin = GetActorLocation();
		}

		Phase2RevealElapsed = 0.0f;
		Phase2RevealLight->SetIntensity(4200.0f);
		Phase2RevealLight->SetVisibility(true);
	}
	else if (!bEmerging && Phase2DisappearEffect)
	{
		const float Offset = GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 0.45f;
		const FVector SpawnLocation = GetActorLocation() - FVector::UpVector * Offset;
		const FVector Scale(Phase2DisappearEffectScale);
		Phase2Smoke = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this, Phase2DisappearEffect, SpawnLocation, GetActorRotation(), Scale, true, true, ENCPoolMethod::None, false);
	}
}

void ABossClown::StopPhase2Presentation(ABoss* Boss, int32 TransitionID, bool bCancelled)
{
	if (UAnimInstance* Anim = GetMesh()->GetAnimInstance(); Anim && Phase2StageMontage.IsValid())
	{
		Anim->Montage_Stop(0.12f, Phase2StageMontage.Get());
	}
	Phase2StageMontage.Reset();

	if (Phase2WeaponVisibility.IsSet() && WeaponMeshComponent)
	{
		WeaponMeshComponent->SetVisibility(Phase2WeaponVisibility.GetValue());
	}

	Phase2WeaponVisibility.Reset();

	if (!bCancelled && Phase2AdvanceOrigin.IsSet() && IsValid(Phase2CombatPoint))
	{
		SetActorLocation(Phase2CombatPoint->GetActorLocation(), false, nullptr, ETeleportType::TeleportPhysics);
	}

	if (!bCancelled && bPhase2WallBreakShown)
	{
		ShowPhase2WallFog(Phase2AdvanceOrigin.Get(GetActorLocation()));
	}

	Phase2AdvanceOrigin.Reset();
	Phase2AdvanceStartedAt.Reset();
	RestorePhase2Wall();
	Phase2WallComponent.Reset();
	bPhase2WallBreakShown = false;
	NextPhase2WallChip = 0;

	if (bCancelled)
	{
		ClearPhase2WallChips();
	}

	Phase2RevealLight->SetVisibility(false);

	if (IsValid(Phase2SearchLight))
	{
		Phase2SearchLight->GetLightComponent()->SetVisibility(false);
	}

	if (UNiagaraComponent* Smoke = Phase2Smoke.Get())
	{
		if (bCancelled)
		{
			Smoke->DestroyComponent();
			Phase2Smoke.Reset();
		}
		else
		{
			Smoke->Deactivate();
		}
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

bool ABossClown::IsClownCombatPhase() const
{
	return !IsDead() && (BossPhase == EBossPhase::Phase1 || BossPhase == EBossPhase::Phase2);
}

bool ABossClown::CanKeepClownProjectiles() const
{
	return IsClownCombatPhase();
}

void ABossClown::CollectClownAttackChoices(EBossClownAttack Type, TArray<const FEnemyAttackData*>& Choices) const
{
	Choices.Reset();
	const FClownPhaseOnePattern* Pattern = GetAttackPattern(Type);

	if (!IsClownCombatPhase() || !Pattern || !Pattern->bEnabled)
	{
		return;
	}

	const EBossPhase Phase = GetBossPhase();

	if ((Phase == EBossPhase::Phase1 && !Pattern->bAllowInPhase1) || (Phase == EBossPhase::Phase2 && !Pattern->bAllowInPhase2))
	{
		return;
	}

	const auto AddChoice = [&Choices, Type](const FEnemyAttackData& Attack)
	{
		if (Attack.Montage && (Type == EBossClownAttack::Yabawi || !Attack.DamageInfo.IsEmpty()))
		{
			Choices.Add(&Attack);
		}
	};
	AddChoice(Pattern->Attack);
	for (const FEnemyAttackData& Variant : Pattern->AttackVariants) AddChoice(Variant);

	if (Type == EBossClownAttack::Yabawi && Choices.IsEmpty())
	{
		Choices.Add(&Pattern->Attack);
	}
}

const FEnemyAttackData* ABossClown::GetClownAttackData(EBossClownAttack Type) const
{
	TArray<const FEnemyAttackData*> Choices;
	CollectClownAttackChoices(Type, Choices);

	return Choices.IsEmpty() ? nullptr : Choices[0];
}

bool ABossClown::CanStartClownAttack(EBossClownAttack Type, AActor* Target) const
{
	const UBossClownAnimationData* Data = GetClownAnimationData();
	const FClownPhaseOnePattern* Pattern = GetAttackPattern(Type);

	if (!Data || !Pattern || !IsValid(Target) || Target->IsActorBeingDestroyed())
	{
		return false;
	}

	if (ActiveAttackID != INDEX_NONE || bReceiveFatalAttack || GetEnemyState() == EEnemyState::Groggy)
	{
		return false;
	}

	if (!GetClownAttackData(Type) || GetClownPatternSelectionScale(Type) <= 0.0f)
	{
		return false;
	}

	const float Distance = GetTargetSurfaceDistance(Target);

	if (Distance < Pattern->MinimumRange || Distance > Pattern->MaximumRange)
	{
		return false;
	}

	if (!GetCharacterMovement()->IsMovingOnGround())
	{
		return false;
	}

	switch (Type)
	{
	case EBossClownAttack::TripleBalls: return Data->BallClass != nullptr;
	case EBossClownAttack::ClapWave: return Data->ClapWaveClass != nullptr;
	case EBossClownAttack::SpawnCards: return Data->CardClass && !Data->CardFaces.IsEmpty() && !Data->HeartFaces.IsEmpty();
	case EBossClownAttack::CubeBarrage: return Data->CubeClass && Data->CubePieces.Num() == 27 && EncounterFloor.IsValid();
	case EBossClownAttack::Yabawi: return YabawiComponent && YabawiComponent->CanStart();
	default: return true;
	}
}

bool ABossClown::EvaluateClownCondition(AActor* Target, bool bAttack, EBossClownAttack Type, float MinDistance, float MaxDistance, float Chance, float FacingDot) const
{
	const UBossClownAnimationData* Data = GetClownAnimationData();

	if (!Data || !IsValid(Target) || Chance <= 0.0f || bReceiveFatalAttack || GetEnemyState() == EEnemyState::Groggy)
	{
		return false;
	}

	if (bAttack ? !CanStartClownAttack(Type, Target) : !(Data->bEnableTacticalActions && IsClownCombatPhase() && GetCharacterMovement()->IsMovingOnGround()))
	{
		return false;
	}

	const float Distance = GetTargetSurfaceDistance(Target);
	const FVector Direction = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();

	if (Distance < MinDistance || Distance > MaxDistance || FVector::DotProduct(GetActorForwardVector(), Direction) < FacingDot)
	{
		return false;
	}

	if (!bAttack)
	{
		return FMath::FRand() < FMath::Clamp(Chance, 0.0f, 1.0f);
	}

	// 선호는 이 패턴의 확률만 높인다. BT 쿨다운으로 막힌 패턴이 다른 공격까지 막지 않는다.
	const bool bCubePriority = Type == EBossClownAttack::CubeBarrage && ShouldPrioritizeClownCube();
	const FClownPhaseOnePattern* Pattern = GetAttackPattern(Type);
	const bool bRangedPreference = PrefersClownRangedAttack() && Distance > 450.0f;
	const float EffectiveChance = bCubePriority || (bRangedPreference && Pattern->bRanged) ? 1.0f : FMath::Clamp(Chance, 0.0f, 1.0f);

	return FMath::FRand() < EffectiveChance * (bCubePriority ? 1.0f : GetClownPatternSelectionScale(Type));
}

float ABossClown::GetClownPatternSelectionScale(EBossClownAttack Type) const
{
	const FClownPhaseOnePattern* Pattern = GetAttackPattern(Type);

	if (!Pattern || !GetWorld())
	{
		return 0.0f;
	}

	const double Now = GetWorld()->GetTimeSeconds();
	const double* LastFinished = LastFinishedByPattern.Find(Type);

	if (LastFinished && Now - *LastFinished < Pattern->ReuseDelay)
	{
		return 0.0f;
	}

	const int32 RecentIndex = RecentPatterns.Find(Type);

	if (RecentIndex == 0)
	{
		return Now - LastPatternTime < 6.0 ? 0.0f : 0.25f * Pattern->FrequencyScale;
	}

	return (RecentIndex == 1 ? 0.2f : (RecentIndex == 2 ? 0.5f : 1.0f)) * Pattern->FrequencyScale;
}

bool ABossClown::ShouldPrioritizeClownCube() const
{
	if (RecentPatterns.Num() < 2)
	{
		return false;
	}

	if (RecentPatterns[0] == EBossClownAttack::CubeBarrage || RecentPatterns[1] == EBossClownAttack::CubeBarrage)
	{
		return false;
	}

	return GetWorld()->GetTimeSeconds() - LastCubeTime >= 8.0;
}

void ABossClown::RecordClownAttackStarted(EBossClownAttack Type)
{
	RecentPatterns.Insert(Type, 0);
	RecentPatterns.SetNum(FMath::Min(RecentPatterns.Num(), 3));
	LastPatternTime = GetWorld()->GetTimeSeconds();

	if (Type == EBossClownAttack::CubeBarrage)
	{
		LastCubeTime = LastPatternTime;
	}
}

void ABossClown::RecordClownAttackFinished(EBossClownAttack Type)
{
	if (!RecentPatterns.IsEmpty() && RecentPatterns[0] == Type)
	{
		LastPatternTime = GetWorld()->GetTimeSeconds();
	}

	LastFinishedByPattern.Add(Type, GetWorld()->GetTimeSeconds());
}

void ABossClown::PreferClownRangedAttack()
{
	RangedPreferenceUntil = GetWorld()->GetTimeSeconds() + 5.0;
}

bool ABossClown::PrefersClownRangedAttack() const
{
	return GetWorld() && GetWorld()->GetTimeSeconds() < RangedPreferenceUntil;
}

void ABossClown::ResetClownAttackSelection()
{
	RecentPatterns.Reset();
	LastFinishedByPattern.Reset();
	LastPatternTime = LastCubeTime = -1000.0;
	RangedPreferenceUntil = 0.0;
}

bool ABossClown::CanStartClownYabawi() const
{
	return CanStartClownAttack(EBossClownAttack::Yabawi, GetEncounterTarget());
}

bool ABossClown::StartClownAttack(EBossClownAttack AttackType, AActor* Target, int32& OutExecutionID, float RecoveryScale)
{
	OutExecutionID = INDEX_NONE;

	if (!AttackTraceComponent || !CanStartClownAttack(AttackType, Target))
	{
		return false;
	}

	TArray<const FEnemyAttackData*> Choices;
	CollectClownAttackChoices(AttackType, Choices);

	if (Choices.IsEmpty())
	{
		return false;
	}

	const FEnemyAttackData* AttackData = Choices[0];
	Choices.RemoveAll(
		[this](const FEnemyAttackData* Choice)
		{
			return Choice->Montage == LastPlayedAttackMontage.Get();
		});

	if (!Choices.IsEmpty())
	{
		AttackData = Choices[FMath::RandRange(0, Choices.Num() - 1)];
	}

	UAnimInstance* Anim = GetMesh()->GetAnimInstance();

	if (!Anim)
	{
		UE_LOG(LogTemp, Error, TEXT("BossClown: AnimInstance is required to start an attack."));

		return false;
	}

	SetStrafeMode(false);

	bSavedOrientToMovement = GetCharacterMovement()->bOrientRotationToMovement;
	bSavedControllerDesiredRotation = GetCharacterMovement()->bUseControllerDesiredRotation;
	bSavedControllerYaw = bUseControllerRotationYaw;
	bHasSavedMovement = true;

	// 공격 시작 시 이동 정지와 타깃 방향 정렬.
	if (AAIController* AI = Cast<AAIController>(GetController()))
	{
		AI->StopMovement();
		AI->ClearFocus(EAIFocusPriority::Gameplay);
	}

	ConsumeMovementInputVector();
	AttackTraceComponent->EndTrace();

	FVector Direction = Target->GetActorLocation() - GetActorLocation();
	Direction.Z = 0.0f;

	// BT 전투에서는 예비 동작 중 회전하여 공격 시작 시 순간 회전 방지.
	// 공격 예비 동작의 추적 회전만 사용. 시작 시 순간 회전하지 않음.

	CurrentClownAttack = *AttackData;
	LastPlayedAttackMontage = AttackData->Montage;
	CurrentRecoveryScale = FMath::Clamp(RecoveryScale, 0.1f, 2.0f);
	CurrentAttackType = AttackType;

	ClownAttackTarget = Target;
	AttackStageTime = 0.0f;
	bDashStarted = false;
	bDashEnded = false;
	DashDirection = Direction.GetSafeNormal();

	// 단타 공격은 첫 번째 데미지를 기본으로 사용.
	// 다단 공격은 Notify에서 인덱스 변경.
	CurrentDamageIndex = 0;
	AttackAnimInstance = Anim;
	ActiveAttackID = ++AttackSerial;
	OutExecutionID = ActiveAttackID;

	SetEnemyState(EEnemyState::Attack);
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	bUseControllerRotationYaw = false;

	if (AttackType == EBossClownAttack::FastChaseSlam &&
		GetTargetSurfaceDistance(Target) > GetClownAnimationData()->FastChaseStopDistance)
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

void ABossClown::BeginClownApproach()
{
	ClownAttackStage = EClownAttackStage::Approach;
	AttackStageTime = 0.0f;
	NextChasePathTime = 0.0;
	SetEnemyState(EEnemyState::FastChase);
	GetCharacterMovement()->MaxWalkSpeed = GetClownAnimationData()->FastChaseSpeed;
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

	GetWorld()->GetTimerManager().SetTimer(
		AttackTimeoutHandle, this, &ABossClown::OnClownAttackTimeout, FMath::Max(AttackTimeout, 0.1f), false);
}

bool ABossClown::PlayCurrentClownMontage()
{
	UAnimInstance* Anim = AttackAnimInstance.Get();

	if (!Anim || ActiveAttackID == INDEX_NONE)
	{
		FinishClownAttack(EBossClownAttackResult::Failed, false);

		return false;
	}

	const bool bSpawningCards = CurrentAttackType == EBossClownAttack::SpawnCards;
	FClownCardLayout CardLayout;
	if (bSpawningCards && !FindClownCardLayout(CardLayout))
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

	if ((!Montage && CurrentAttackType != EBossClownAttack::Yabawi) || (Montage && Anim->Montage_Play(Montage) <= 0.0f))
	{
		FinishClownAttack(EBossClownAttackResult::Failed, false);

		return false;
	}

	const bool bPrepared = bSpawningCards ? SpawnClownCards(CardLayout) : PrepareClownPattern();
	if (!bPrepared)
	{
		FinishClownAttack(EBossClownAttackResult::Failed, true);
		return false;
	}

	if (UsesSpecialPresentation(CurrentAttackType) && WeaponMeshComponent)
	{
		bWeaponWasVisible = WeaponMeshComponent->IsVisible();
		bHidWeapon = true;
		WeaponMeshComponent->SetVisibility(false);
	}

	if (CurrentAttackType == EBossClownAttack::Yabawi)
	{
		ClownAttackStage = EClownAttackStage::Yabawi;
	}

	if (Montage && UsesLoopingMontage(CurrentAttackType) && Montage->GetNumSections() > 0)
	{
		const FName Section = Montage->GetSectionName(0);

		if (CurrentAttackType == EBossClownAttack::CubeBarrage)
		{
			Anim->Montage_SetPlayRate(Montage, Montage->GetPlayLength() / UClownProjectileComponent::CubeMarchCycle);
		}
		Anim->Montage_SetNextSection(Section, Section, Montage);
	}

	RecordClownAttackStarted(CurrentAttackType);

	if (Montage)
	{
		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &ABossClown::OnClownAttackEnded, ActiveAttackID);
		Anim->Montage_SetEndDelegate(EndDelegate, Montage);
	}

	float Timeout = FMath::Max(AttackTimeout, 0.1f);

	if (CurrentAttackType == EBossClownAttack::Yabawi)
	{
		const FClownPhaseOnePattern& Rules = *GetAttackPattern(EBossClownAttack::Yabawi);
		Timeout = FMath::Max(Timeout, FMath::Max(Rules.RevealTime, 0.5f) + FMath::Max(Rules.CoverTime, 0.2f) + FMath::Clamp(Rules.ShuffleCount, 1, 32) * FMath::Clamp(Rules.ShuffleStepTime, 0.12f, 1.0f) + FMath::Max(Rules.SelectionTime, 1.0f) + 10.0f);
	}

	GetWorld()->GetTimerManager().SetTimer(
		AttackTimeoutHandle, this, &ABossClown::OnClownAttackTimeout, Timeout, false);

	return true;
}

const FClownPhaseOnePattern* ABossClown::GetAttackPattern(EBossClownAttack AttackType) const
{
	const UBossClownAnimationData* Data = GetClownAnimationData();

	return Data ? Data->PhaseOnePatterns.FindByPredicate(
					  [AttackType](const FClownPhaseOnePattern& Pattern)
					  {
						  return Pattern.AttackType == AttackType;
					  })
				: nullptr;
}

void ABossClown::RefreshClownMovement()
{
	UpdateMovementSpeed();
}

void ABossClown::FaceClownTarget(AActor* Target, float DeltaTime)
{
	if (bReceiveFatalAttack || GetEnemyState() == EEnemyState::Groggy || !IsValid(Target) || !GetClownAnimationData())
	{
		return;
	}

	FVector Direction = Target->GetActorLocation() - GetActorLocation();
	Direction.Z = 0.0f;

	if (!Direction.IsNearlyZero())
	{
		SetActorRotation(FMath::RInterpConstantTo(
			GetActorRotation(), Direction.Rotation(), DeltaTime, GetClownAnimationData()->AttackTurnSpeed));
	}
}

void ABossClown::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateIntroBreakables();

	if (GetPhase2TransitionStage() == EBossPhase2TransitionStage::Emerging)
	{
		TickPhase2Emergence();
	}

	if (!FallingPhase2WallBricks.IsEmpty() || !RollingPhase2WallBricks.IsEmpty())
	{
		SettlePhase2WallBricks(DeltaTime);
	}

	if (Phase2DebrisFadeElapsed.IsSet())
	{
		TickPhase2WallDebrisFade(DeltaTime);
	}

	if (Phase2RevealLight->IsVisible())
	{
		Phase2RevealElapsed += DeltaTime;
		const float Alpha = FMath::Clamp(Phase2RevealElapsed / FMath::Max(Phase2EmergencePlaceholderTime, 0.01f), 0.0f, 1.0f);
		Phase2RevealLight->SetIntensity(4200.0f * FMath::Square(1.0f - Alpha));
	}

	if (ActiveAttackID != INDEX_NONE)
	{
		TickClownAttack(DeltaTime);
	}
}

void ABossClown::TickPhase2Emergence()
{
	UAnimInstance* Anim = GetMesh()->GetAnimInstance();
	UAnimMontage* Montage = Phase2StageMontage.Get();
	const bool bPlaying = Anim && Montage && Anim->Montage_IsActive(Montage);
	const float Position = bPlaying ? Anim->Montage_GetPosition(Montage) : 0.0f;

	while (bPlaying && NextPhase2WallChip < 2 &&
		Position >= Phase2WallChipTimes[NextPhase2WallChip])
	{
		if (Phase2WallChipMesh)
		{
			SpawnPhase2WallChips(NextPhase2WallChip);
		}

		ShowPhase2WallOpening(NextPhase2WallChip == 0 ? Phase2WallCrack.Get() : Phase2WallOpening.Get());
		++NextPhase2WallChip;
	}

	if (!bPhase2WallPlayed && IsValid(Phase2WallCache) && (!bPlaying || Position >= Phase2WallPlaybackTime))
	{
		Phase2WallComponent = FindPhase2Wall();
		if (UGeometryCollectionComponent* Wall = Phase2WallComponent.Get())
		{
			Phase2IntactWallTransforms = Wall->GetComponentSpaceTransforms3f();
		}
		bPhase2WallMoved = Phase2IntactWallTransforms.IsEmpty();

		Phase2WallCache->TriggerAll();
		bPhase2WallPlayed = true;
	}
	else if (bPhase2WallPlayed && !bPhase2WallMoved)
	{
		CheckPhase2WallBreak();
	}

	const bool bWallCleared = !IsValid(Phase2WallCache) || (bPhase2WallPlayed && bPhase2WallMoved);
	const bool bFinalImpact = bPlaying && Phase2WallImpactTime > 0.0f ? Position >= Phase2WallImpactTime : bWallCleared;
	if (bPhase2WallPlayed && !bPhase2WallBreakShown && bFinalImpact)
	{
		ShowPhase2WallBreak();
	}

	if (bWallCleared && bPhase2WallBreakShown && !Phase2WallCollision.IsSet() && IsValid(Phase2WallExitBlocker))
	{
		if (UGeometryCollectionComponent* Wall = Phase2WallComponent.Get())
		{
			Phase2WallCollision = Wall->GetCollisionEnabled();
			Phase2WallExitBlocker->SetActorEnableCollision(true);
			Wall->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}

	if (bPlaying && bWallCleared && Phase2AdvanceOrigin.IsSet() && IsValid(Phase2CombatPoint) && Position >= Phase2AdvanceStartTime)
	{
		if (!Phase2AdvanceStartedAt.IsSet())
		{
			Phase2AdvanceStartedAt = Position;
		}

		const float Start = Phase2AdvanceStartedAt.GetValue();
		const float Duration = FMath::Clamp(Montage->GetPlayLength() - Start - 0.12f, 0.01f, 0.78f);
		const float Alpha = FMath::Clamp((Position - Start) / Duration, 0.0f, 1.0f);
		const float Drive = 1.0f - FMath::Cube(1.0f - Alpha);
		const float Travel = FMath::SmoothStep(0.0f, 1.0f, Drive);
		FVector Location = FMath::Lerp(Phase2AdvanceOrigin.GetValue(), Phase2CombatPoint->GetActorLocation(), Travel);
		Location.Z += 9.0f * 4.0f * Travel * (1.0f - Travel);
		SetActorLocation(Location, false, nullptr, ETeleportType::TeleportPhysics);
	}
}

UGeometryCollectionComponent* ABossClown::FindPhase2Wall() const
{
	if (IsValid(Phase2WallCache))
	{
		for (const FObservedComponent& Observed : Phase2WallCache->GetObservedComponents())
		{
			if (UGeometryCollectionComponent* Wall = Cast<UGeometryCollectionComponent>(Observed.GetComponent(Phase2WallCache.Get())))
			{
				return Wall;
			}
		}
	}

	return nullptr;
}

void ABossClown::ShowPhase2WallOpening(AActor* Opening)
{
	if (IsValid(Opening))
	{
		RestorePhase2Wall();
		Phase2WallComponent = FindPhase2Wall();
		if (UGeometryCollectionComponent* Wall = Phase2WallComponent.Get())
		{
			Wall->SetVisibility(false);
			Opening->SetActorHiddenInGame(false);
		}
	}
}

void ABossClown::RestorePhase2Wall()
{
	const bool bCracked = IsValid(Phase2WallCrack) && !Phase2WallCrack->IsHidden();
	const bool bOpened = IsValid(Phase2WallOpening) && !Phase2WallOpening->IsHidden();
	if (!bCracked && !bOpened)
	{
		return;
	}

	if (bCracked)
	{
		Phase2WallCrack->SetActorHiddenInGame(true);
	}
	if (bOpened)
	{
		Phase2WallOpening->SetActorHiddenInGame(true);
	}

	if (UGeometryCollectionComponent* Wall = Phase2WallComponent.Get())
	{
		Wall->SetVisibility(true);
	}
}

void ABossClown::CheckPhase2WallBreak()
{
	if (UGeometryCollectionComponent* Wall = Phase2WallComponent.Get())
	{
		const TArray<FTransform3f>& Transforms = Wall->GetComponentSpaceTransforms3f();
		for (int32 Index = 0; Index < FMath::Min(Transforms.Num(), Phase2IntactWallTransforms.Num()); ++Index)
		{
			if (FVector3f::DistSquared(Transforms[Index].GetLocation(), Phase2IntactWallTransforms[Index].GetLocation()) > FMath::Square(15.0f))
			{
				bPhase2WallMoved = true;
				return;
			}
		}
	}
	else
	{
		bPhase2WallMoved = true;
	}
}

void ABossClown::ShowPhase2WallBreak()
{
	bPhase2WallBreakShown = true;
	RestorePhase2Wall();

	if (UGeometryCollectionComponent* Wall = Phase2WallComponent.Get(); Wall && IsValid(Phase2BrokenWall))
	{
		Wall->SetVisibility(bShowPhase2ChaosFracture);
		Phase2BrokenWall->SetActorHiddenInGame(bShowPhase2ChaosFracture);
	}

	if (Phase2WallChipMesh)
	{
		SpawnPhase2WallChips(2);
	}
	GetWorldTimerManager().SetTimer(Phase2WallDebrisTimer, this, &ABossClown::BeginPhase2WallDebrisFade, Phase2DebrisFadeStartTime, false);
}

void ABossClown::BeginPhase2WallDebrisFade()
{
	for (int32 Index = 0; Index < Phase2LargeWallBricks.Num(); ++Index)
	{
		FPhase2WallBrickFade& Piece = Phase2LargeWallBricks[Index];
		if (AStaticMeshActor* Brick = Piece.Brick.Get())
		{
			if (UProjectileMovementComponent* Movement = Brick->FindComponentByClass<UProjectileMovementComponent>(); Movement && Movement->IsActive())
			{
				Movement->StopMovementImmediately();
				Movement->Deactivate();
			}

			FVector Center, Extent;
			Brick->GetActorBounds(false, Center, Extent);
			Piece.RestPose = Brick->GetActorTransform();
			Piece.FloorPivot = Center - FVector::UpVector * (Extent.Z + 8.0);
			Piece.Delay = ((Index * 7) % 17) * 0.1f;
			Piece.Duration = FMath::GetMappedRangeValueClamped(FVector2D(10.0, 110.0), FVector2D(0.8, 1.45), Extent.Z);
		}
	}

	if (bShowPhase2ChaosFracture)
	{
		PreparePhase2ChaosDebrisFade();
	}
	Phase2DebrisFadeElapsed = 0.0f;
}

void ABossClown::PreparePhase2ChaosDebrisFade()
{
	UGeometryCollectionComponent* Wall = FindPhase2Wall();
	const UBoxComponent* Boundary = IsValid(Phase2WallExitBlocker) ? Phase2WallExitBlocker->FindComponentByClass<UBoxComponent>() : nullptr;
	const UGeometryCollection* RestCollection = Wall ? Wall->GetRestCollection() : nullptr;
	const TSharedPtr<FGeometryCollection, ESPMode::ThreadSafe> Pieces = RestCollection ? RestCollection->GetGeometryCollection() : nullptr;
	if (!Pieces || !Boundary || Phase2IntactWallTransforms.IsEmpty())
	{
		return;
	}

	const TArray<FTransform3f> WallPose = Wall->GetComponentSpaceTransforms3f();
	if (WallPose.Num() != Phase2IntactWallTransforms.Num())
	{
		return;
	}

	const FTransform WallToWorld = Wall->GetComponentTransform();
	const FVector Extent = Boundary->GetScaledBoxExtent();
	const FVector WallNormal = Extent.X >= Extent.Y ? Boundary->GetRightVector() : Boundary->GetForwardVector();
	const double WallHalfDepth = FMath::Min(Extent.X, Extent.Y);
	const FVector WallCenter = Boundary->GetComponentLocation();
	const double FloorZ = EncounterFloor.IsValid() ? EncounterFloorZ : Boundary->Bounds.GetBox().Min.Z;
	for (int32 Index = 0; Index < Pieces->TransformIndex.Num(); ++Index)
	{
		const int32 Bone = Pieces->TransformIndex[Index];
		if (!WallPose.IsValidIndex(Bone))
		{
			continue;
		}

		const FBox& PieceBounds = Pieces->BoundingBox[Index];
		const FVector3f PieceCenter(PieceBounds.GetCenter());
		const FVector Start = WallToWorld.TransformPosition(FVector(Phase2IntactWallTransforms[Bone].TransformPosition(PieceCenter)));
		const FVector End = WallToWorld.TransformPosition(FVector(WallPose[Bone].TransformPosition(PieceCenter)));
		const bool bLeftWall = FMath::Abs(FVector::DotProduct(End - WallCenter, WallNormal)) > WallHalfDepth + 25.0;
		const bool bFellToFloor = End.Z < FloorZ + 125.0 && Start.Z - End.Z > 50.0;
		if ((bLeftWall || bFellToFloor) && FVector::DistSquared(Start, End) > FMath::Square(15.0))
		{
			const FBox WorldBounds = PieceBounds.TransformBy(FTransform(WallPose[Bone]) * WallToWorld);
			const FVector GroundPivot(End.X, End.Y, WorldBounds.Min.Z - 8.0);
			const int32 Parent = Wall->GetParent(Bone);
			const FTransform ParentToWorld = WallPose.IsValidIndex(Parent) ? FTransform(WallPose[Parent]) * WallToWorld : WallToWorld;
			const float Delay = (Bone % 17) * 0.1f;
			const float Duration = FMath::GetMappedRangeValueClamped(FVector2D(10.0, 110.0), FVector2D(0.8, 1.45), WorldBounds.GetExtent().Z);
			Phase2FadingDebris.Add({ Bone, ParentToWorld.InverseTransformPosition(GroundPivot), Delay, Duration });
		}
	}
	Phase2IntactWallTransforms.Reset();
	if (Phase2FadingDebris.IsEmpty())
	{
		return;
	}

	// Keep the cache's final wall pose as the base for each fade frame.
	Phase2WallFadePose.Reset(WallPose.Num());
	for (int32 Index = 0; Index < WallPose.Num(); ++Index)
	{
		const int32 Parent = Wall->GetParent(Index);
		Phase2WallFadePose.Emplace(WallPose.IsValidIndex(Parent) ? WallPose[Index].GetRelativeTransform(WallPose[Parent]) : WallPose[Index]);
	}

	Phase2WallCache->Stop();
	Wall->SetSimulatePhysics(false);
	Wall->DestroyPhysicsState();
	Phase2WallComponent = Wall;
}

void ABossClown::TickPhase2WallDebrisFade(float DeltaTime)
{
	const float Elapsed = Phase2DebrisFadeElapsed.GetValue() + DeltaTime;
	Phase2DebrisFadeElapsed = Elapsed;
	const auto FadeAt = [Elapsed](float Delay, float Duration)
	{
		return FMath::SmoothStep(0.0f, 1.0f, (Elapsed - Delay) / Duration);
	};
	bool bFinished = true;

	if (UGeometryCollectionComponent* Wall = Phase2WallComponent.Get(); Wall && !Phase2FadingDebris.IsEmpty())
	{
		TArray<FTransform> Pose = Phase2WallFadePose;
		for (const FPhase2DebrisFade& Piece : Phase2FadingDebris)
		{
			const double Fade = FadeAt(Piece.Delay, Piece.Duration);
			FTransform& Transform = Pose[Piece.Bone];
			Transform.SetTranslation(FMath::Lerp(Transform.GetLocation(), Piece.FloorPivot, Fade));
			Transform.SetScale3D(Transform.GetScale3D() * (1.0 - Fade));
			bFinished &= Fade >= 1.0;
		}
		Wall->SetRestState(MoveTemp(Pose));
	}

	for (const FPhase2WallBrickFade& Piece : Phase2LargeWallBricks)
	{
		if (AStaticMeshActor* Brick = Piece.Brick.Get())
		{
			const double Fade = FadeAt(Piece.Delay, Piece.Duration);
			if (Fade >= 1.0)
			{
				Brick->Destroy();
				continue;
			}

			FTransform Pose = Piece.RestPose;
			Pose.SetTranslation(FMath::Lerp(Pose.GetLocation(), Piece.FloorPivot, Fade));
			Pose.SetScale3D(Pose.GetScale3D() * (1.0 - Fade));
			Brick->SetActorTransform(Pose);
			bFinished = false;
		}
	}

	if (bFinished)
	{
		Phase2FadingDebris.Reset();
		Phase2WallFadePose.Reset();
		Phase2LargeWallBricks.Reset();
		Phase2DebrisFadeElapsed.Reset();
	}
}

void ABossClown::ShowPhase2WallFog(const FVector& BreachLocation)
{
	const UStaticMesh* Plane = Phase2WallFog->GetStaticMesh();
	UBoxComponent* Boundary = IsValid(Phase2WallExitBlocker) ? Phase2WallExitBlocker->FindComponentByClass<UBoxComponent>() : nullptr;
	if (!Plane || !Boundary)
	{
		return;
	}

	const FVector Extent = Boundary->GetScaledBoxExtent();
	const bool bWideAlongX = Extent.X >= Extent.Y;
	const FVector WidthAxis = bWideAlongX ? Boundary->GetForwardVector() : Boundary->GetRightVector();
	const FVector Center = Boundary->GetComponentLocation();
	const double WallHalfWidth = bWideAlongX ? Extent.X : Extent.Y;
	const double FogHalfWidth = FMath::Min(625.0, WallHalfWidth);
	const double Offset = FMath::Clamp(FVector::DotProduct(BreachLocation - Center, WidthAxis), FogHalfWidth - WallHalfWidth, WallHalfWidth - FogHalfWidth);
	const FVector Location = Center + WidthAxis * Offset;
	const FRotator Rotation = FRotationMatrix::MakeFromXY(WidthAxis, FVector::UpVector).Rotator();
	const FVector PlaneExtent = Plane->GetBounds().BoxExtent;

	Phase2WallFog->AttachToComponent(Boundary, FAttachmentTransformRules::KeepWorldTransform);
	Phase2WallFog->SetWorldTransform(FTransform(Rotation, Location, FVector(FogHalfWidth / PlaneExtent.X, Extent.Z / PlaneExtent.Y, 1.0)));
	Phase2WallFog->SetVisibility(true);
}

void ABossClown::SpawnPhase2WallChips(int32 HitIndex)
{
	static const FName RightHand(TEXT("hand_r"));
	static const FName LeftHand(TEXT("hand_l"));
	const FVector Forward = GetActorForwardVector();
	const FVector Right = GetActorRightVector();
	const FVector RightFist = GetMesh()->GetSocketLocation(RightHand);
	const FVector LeftFist = GetMesh()->GetSocketLocation(LeftHand);
	const FVector Contact = FVector::DotProduct(RightFist - LeftFist, Forward) > 0.0f ? RightFist : LeftFist;
	const FVector Impact = Contact + Forward * 120.0f;
	constexpr int32 ChipCounts[] = { 10, 26, 156 };
	constexpr float Spreads[] = { 34.0f, 64.0f, 185.0f };
	constexpr float MinSpeeds[] = { 125.0f, 160.0f, 225.0f };
	constexpr float MaxSpeeds[] = { 270.0f, 370.0f, 635.0f };
	constexpr float MinScales[] = { 0.25f, 0.31f, 0.45f };
	constexpr float MaxScales[] = { 0.46f, 0.68f, 1.08f };
	const int32 ChipCount = ChipCounts[HitIndex];
	const float Spread = Spreads[HitIndex];
	const bool bWallCollapse = HitIndex == 2;
	const FVector Breach = Impact + FVector::UpVector * 55.0f;
	const float LeftSpread = bWallCollapse ? Spread * 0.42f : Spread;
	const float RightSpread = bWallCollapse ? Spread * 2.25f : Spread;
	const float TopSpread = bWallCollapse ? Spread * 1.5f : Spread;

	Phase2WallChips.RemoveAll([](const TWeakObjectPtr<AActor>& Chip) { return !Chip.IsValid(); });

	for (int32 Index = 0; Index < ChipCount; ++Index)
	{
		const bool bLargeBlock = bWallCollapse && Index % 20 == 0;
		const bool bLargePiece = bWallCollapse && Index % 4 == 0;
		const bool bLargeChip = !bWallCollapse && Index % 4 == 0;
		const float Lateral = FMath::FRandRange(-LeftSpread, RightSpread) * (bLargeBlock ? 0.75f : 1.0f);
		const float Height = FMath::FRandRange(-Spread * (bLargeBlock ? 0.4f : 0.82f), TopSpread);
		const FVector Offset = Right * Lateral + FVector::UpVector * Height;
		FVector Origin = (bWallCollapse ? Breach : Impact) + Offset;
		if (bLargeChip && IsValid(Phase2WallOpening))
		{
			const double ToFront = FVector::DotProduct(Phase2WallOpening->GetActorLocation() - Origin, Forward) + 72.0;
			Origin += Forward * FMath::Max(ToFront, 0.0);
		}

		AStaticMeshActor* Chip = GetWorld()->SpawnActor<AStaticMeshActor>(Origin, FMath::VRand().Rotation());

		if (!Chip)
		{
			continue;
		}

		UStaticMeshComponent* ChipMesh = Chip->GetStaticMeshComponent();
		ChipMesh->SetMobility(EComponentMobility::Movable);
		ChipMesh->SetStaticMesh(Phase2WallChipMesh);
		ChipMesh->SetCanEverAffectNavigation(false);
		float Scale = FMath::FRandRange(MinScales[HitIndex], MaxScales[HitIndex]);
		if (bLargePiece)
		{
			Scale = bLargeBlock ? FMath::FRandRange(4.65f, 6.15f) : FMath::FRandRange(2.1f, 3.8f);
		}
		else if (bLargeChip)
		{
			const float ChipScale = 1.3f + HitIndex * 0.32f;
			Scale = FMath::FRandRange(ChipScale, ChipScale + 0.42f);
		}

		Chip->SetActorScale3D(FVector(Scale, Scale * FMath::FRandRange(0.65f, 1.2f), Scale));
		ChipMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Chip->SetActorEnableCollision(false);

		UProjectileMovementComponent* Movement = NewObject<UProjectileMovementComponent>(Chip);
		Movement->SetUpdatedComponent(ChipMesh);
		Movement->bInitialVelocityInLocalSpace = false;
		Movement->Velocity = Forward * FMath::FRandRange(MinSpeeds[HitIndex], MaxSpeeds[HitIndex])
			+ Right * (Lateral * 0.92f + FMath::FRandRange(-90.0f, 90.0f)) + FVector::UpVector * FMath::FRandRange(55.0f, 205.0f);
		if (bLargePiece)
		{
			Movement->Velocity *= bLargeBlock ? 0.58f : 0.7f;
		}
		else if (bLargeChip)
		{
			Movement->Velocity *= 0.8f;
		}

		Movement->ProjectileGravityScale = bLargePiece ? 1.0f : 0.85f;
		Movement->RegisterComponent();
		if (!bWallCollapse || bLargePiece)
		{
			FallingPhase2WallBricks.Add(Movement);
		}

		if (bLargePiece)
		{
			Phase2LargeWallBricks.AddDefaulted_GetRef().Brick = Chip;
		}
		else
		{
			const float Life = bWallCollapse ? 1.45f : FMath::FRandRange(bLargeChip ? 4.4f : 2.35f, bLargeChip ? 5.3f : 2.85f);
			Chip->SetLifeSpan(Life);
		}

		Phase2WallChips.Add(Chip);
	}
}

void ABossClown::SettlePhase2WallBricks(float DeltaTime)
{
	RollingPhase2WallBricks.RemoveAll([DeltaTime](FPhase2BrickRoll& Roll)
	{
		AStaticMeshActor* Brick = Roll.Brick.Get();
		if (!IsValid(Brick))
		{
			return true;
		}

		Roll.Elapsed += DeltaTime;
		const float Alpha = FMath::Clamp(Roll.Elapsed / 0.42f, 0.0f, 1.0f);
		const float Travel = 1.0f - FMath::Square(1.0f - Alpha);
		const FQuat Rotation = FQuat::Slerp(Roll.StartRotation, Roll.RestRotation, Travel);
		const FVector Scale = Brick->GetActorScale3D();
		FVector Location = Roll.Start + Roll.Step * Travel;
		const UStaticMesh* Mesh = Brick->GetStaticMeshComponent()->GetStaticMesh();
		const double Bottom = Mesh->GetBounds().TransformBy(FTransform(Rotation, Location, Scale)).GetBox().Min.Z;
		Location.Z += Roll.FloorZ - Bottom + 0.5;
		Brick->SetActorLocationAndRotation(Location, Rotation);
		return Alpha >= 1.0f;
	});

	const double FloorZ = EncounterFloor.IsValid() ? EncounterFloorZ : GetActorLocation().Z - GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	FCollisionObjectQueryParams Objects;
	Objects.AddObjectTypesToQuery(ECC_WorldStatic);
	Objects.AddObjectTypesToQuery(ECC_WorldDynamic);
	FCollisionQueryParams Query(SCENE_QUERY_STAT(ClownWallBrickFloor), false, this);
	if (const AActor* Arena = GetEncounterArena())
	{
		Query.AddIgnoredComponent(Arena->FindComponentByClass<UBoxComponent>());
	}

	TArray<FHitResult> Hits;
	const auto FindBrickFloor = [&](const FVector& Location) -> TOptional<double>
	{
		Hits.Reset();
		GetWorld()->LineTraceMultiByObjectType(Hits, FVector(Location.X, Location.Y, FloorZ + 24.0),
			FVector(Location.X, Location.Y, FloorZ - 240.0), Objects, Query);
		const FHitResult* FloorHit = Hits.FindByPredicate([this](const FHitResult& Hit) { return IsEncounterFloorHit(Hit); });
		if (!FloorHit)
		{
			FloorHit = Hits.FindByPredicate([](const FHitResult& Hit)
			{
				const UPrimitiveComponent* Surface = Hit.GetComponent();
				return Surface && Surface->GetCollisionResponseToChannel(ECC_Pawn) == ECR_Block &&
					!Hit.bStartPenetrating && Hit.ImpactNormal.Z >= 0.9f;
			});
		}
		return FloorHit ? TOptional<double>(FloorHit->ImpactPoint.Z) : TOptional<double>();
	};

	FallingPhase2WallBricks.RemoveAll([&](const TWeakObjectPtr<UProjectileMovementComponent>& Entry)
	{
		UProjectileMovementComponent* Movement = Entry.Get();
		AStaticMeshActor* Brick = Movement ? Cast<AStaticMeshActor>(Movement->GetOwner()) : nullptr;
		if (!IsValid(Brick))
		{
			return true;
		}

		const double Bottom = Brick->GetStaticMeshComponent()->Bounds.GetBox().Min.Z;
		if (Movement->Velocity.Z > 0.0 || Bottom > FloorZ + 24.0)
		{
			return false;
		}

		const FVector Location = Brick->GetActorLocation();
		const TOptional<double> LandingZ = FindBrickFloor(Location);
		if (LandingZ.IsSet() && Bottom <= LandingZ.GetValue())
		{
			const FVector Direction = Movement->Velocity.GetSafeNormal2D();
			const double Distance = FMath::Clamp(Movement->Velocity.Size2D() * 0.16, 12.0, 48.0);
			FVector Step = Direction * Distance;
			const TOptional<double> EndFloor = FindBrickFloor(Location + Step);
			if (!EndFloor.IsSet() || !FMath::IsNearlyEqual(EndFloor.GetValue(), LandingZ.GetValue(), 3.0))
			{
				Step = FVector::ZeroVector;
			}

			const FQuat StartRotation = Brick->GetActorQuat();
			const FVector RollAxis = FVector::CrossProduct(FVector::UpVector, Direction);
			const double Radius = FMath::Max(Brick->GetStaticMeshComponent()->Bounds.BoxExtent.Z, 10.0);
			const FQuat Rolled = FQuat(RollAxis, FMath::Clamp(Distance / Radius, 0.45, 1.4)) * StartRotation;
			const FVector LocalUp = Rolled.UnrotateVector(FVector::UpVector);
			FVector Face = FVector::UpVector;
			double Best = 0.0;
			for (const FVector& Axis : {FVector::ForwardVector, FVector::RightVector, FVector::UpVector})
			{
				const double Alignment = FVector::DotProduct(LocalUp, Axis);
				if (FMath::Abs(Alignment) > Best)
				{
					Best = FMath::Abs(Alignment);
					Face = Axis * FMath::Sign(Alignment);
				}
			}
			const FQuat RestRotation = FQuat::FindBetweenNormals(Rolled.RotateVector(Face), FVector::UpVector) * Rolled;
			const FVector Grounded = Location + FVector(0.0, 0.0, LandingZ.GetValue() - Bottom + 0.5);
			RollingPhase2WallBricks.Add({Brick, Grounded, Step, StartRotation, RestRotation, LandingZ.GetValue()});
			Movement->StopMovementImmediately();
			Movement->Deactivate();
			Brick->SetActorLocation(Grounded);
			return true;
		}

		if (Bottom < FloorZ - 240.0)
		{
			Brick->Destroy();
			return true;
		}

		return false;
	});
}

void ABossClown::ClearPhase2WallChips()
{
	for (const TWeakObjectPtr<AActor>& Chip : Phase2WallChips)
	{
		if (AActor* Actor = Chip.Get())
		{
			Actor->Destroy();
		}
	}
	Phase2WallChips.Reset();
	FallingPhase2WallBricks.Reset();
	RollingPhase2WallBricks.Reset();
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

	if (ClownAttackStage == EClownAttackStage::Yabawi)
	{
		FaceClownTarget(Target, DeltaTime);

		return;
	}

	if (ClownAttackStage == EClownAttackStage::Groggy)
	{
		AttackStageTime += DeltaTime;

		if (AttackStageTime >= YabawiGroggyDuration)
		{
			FinishClownAttack(EBossClownAttackResult::Succeeded, true);
		}

		return;
	}

	AttackStageTime += DeltaTime;
	const FClownPhaseOnePattern* Pattern = GetAttackPattern(CurrentAttackType);

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

			if (Now >= NextChasePathTime && AI->GetMoveStatus() != EPathFollowingStatus::Moving)
			{
				if (AI->MoveToActor(Target, FMath::Max(Data->FastChaseStopDistance - 10.0f, 0.0f), true) == EPathFollowingRequestResult::Failed)
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

	if (TickClownSpecial(DeltaTime, MontageTime) && Anim && CurrentClownAttack.Montage)
	{
		Anim->Montage_SetNextSection(CurrentClownAttack.Montage->GetSectionName(0), NAME_None, CurrentClownAttack.Montage);
	}

	if (ActiveAttackID == INDEX_NONE)
	{
		return;
	}

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
			FVector::Dist2D(DashStartLocation, GetActorLocation()) >= Data->DashMaxDistance ||
			!GetCharacterMovement()->IsMovingOnGround())
		{
			bDashEnded = true;
			GetCharacterMovement()->StopMovementImmediately();
			ConsumeMovementInputVector();
		}
		else
		{
			// CharacterMovement의 충돌 처리를 사용하며 돌진 중 목표 방향은 변경하지 않음.
			const float DashTime = MontageTime - Data->DashStartTime;
			const float BlendTime = FMath::Min(0.15f, Data->DashDuration * 0.25f);
			const float BlendIn = BlendTime > 0.0f ? FMath::Clamp(DashTime / BlendTime, 0.0f, 1.0f) : 1.0f;
			const float BlendOut = BlendTime > 0.0f ? FMath::Clamp((Data->DashDuration - DashTime) / BlendTime, 0.0f, 1.0f) : 1.0f;
			const float SpeedScale = FMath::Min(BlendIn, BlendOut);
			GetCharacterMovement()->Velocity = DashDirection * Data->DashSpeed * SpeedScale;
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
	const FClownPhaseOnePattern* Pattern = GetAttackPattern(CurrentAttackType);

	if (!Pattern || Pattern->RecoveryTime <= 0.0f)
	{
		FinishClownAttack(EBossClownAttackResult::Succeeded, false);
	}
}

void ABossClown::EnterYabawiGroggy()
{
	if (ActiveAttackID == INDEX_NONE || ClownAttackStage != EClownAttackStage::Yabawi || !IsClownCombatPhase())
	{
		return;
	}

	const UBossClownAnimationData* Data = GetClownAnimationData();
	const FClownPhaseOnePattern* Rules = GetAttackPattern(EBossClownAttack::Yabawi);
	YabawiGroggyDuration = FMath::Max(Data ? Data->GroggyDuration : 4.5f, Rules ? Rules->ResultDisplayTime + 0.25f : 4.5f);
	ClownAttackStage = EClownAttackStage::Groggy;
	AttackStageTime = 0.0f;
	GetWorldTimerManager().ClearTimer(AttackTimeoutHandle);

	if (AttackTraceComponent)
	{
		AttackTraceComponent->EndTrace();
	}

	if (AAIController* AI = Cast<AAIController>(GetController()))
	{
		AI->StopMovement();
		AI->ClearFocus(EAIFocusPriority::Gameplay);
	}

	GetCharacterMovement()->StopMovementImmediately();
	ConsumeMovementInputVector();
	SetEnemyState(EEnemyState::Groggy);
	SetEnemyActionState(EEnemyActionState::Groggy);

	if (UAnimInstance* Anim = AttackAnimInstance.Get())
	{
		if (UAnimMontage* AttackMontage = CurrentClownAttack.Montage.Get())
		{
			FOnMontageEnded EmptyDelegate;
			Anim->Montage_SetEndDelegate(EmptyDelegate, AttackMontage);
			Anim->Montage_Stop(0.15f, AttackMontage);
		}

		if (Data && Data->GroggyMontage && Anim->Montage_Play(Data->GroggyMontage) > 0.0f)
		{
			ActiveGroggyMontage = Data->GroggyMontage;
		}
	}
}

void ABossClown::CompleteClownYabawi(bool bCorrect)
{
	if (bCorrect && ClownAttackStage == EClownAttackStage::Groggy)
	{
		if (YabawiComponent)
		{
			YabawiComponent->CancelRound();
		}

		return;
	}

	if (ActiveAttackID == INDEX_NONE || ClownAttackStage != EClownAttackStage::Yabawi)
	{
		return;
	}

	if (YabawiComponent)
	{
		YabawiComponent->CancelRound();
	}

	if (UAnimInstance* Anim = AttackAnimInstance.Get())
	{
		if (UAnimMontage* Montage = CurrentClownAttack.Montage.Get())
		{
			FOnMontageEnded EmptyDelegate;
			Anim->Montage_SetEndDelegate(EmptyDelegate, Montage);
			Anim->Montage_Stop(0.15f, Montage);
		}
	}

	GetWorldTimerManager().ClearTimer(AttackTimeoutHandle);
	BeginClownRecovery();
}

void ABossClown::RestoreClownMovement()
{
	if (!bHasSavedMovement)
	{
		return;
	}

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

	// 현재 광대 공격에서 사용할 타격 데미지 인덱스 변경.
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

	return ExecutionID != INDEX_NONE && ExecutionID == LastAttackID ? LastAttackResult
																	: EBossClownAttackResult::Aborted;
}

void ABossClown::CancelClownAttack(int32 ExecutionID)
{
	if (ActiveAttackID != INDEX_NONE && ExecutionID == ActiveAttackID && ClownAttackStage != EClownAttackStage::Groggy)
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

			if (ClownAttackStage == EClownAttackStage::Yabawi)
			{
				return;
			}

			// 공 개수가 늘어 지휘 한 주기보다 길어지면 동일 동작을 이어서 재생.
			const UBossClownAnimationData* Data = GetClownAnimationData();

			if (CurrentAttackType == EBossClownAttack::TripleBalls && Data && !HasFinishedLaunchingBalls())
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
	if (ActiveAttackID == INDEX_NONE)
	{
		return;
	}

	LastAttackID = ActiveAttackID;
	LastAttackResult = Result;
	ActiveAttackID = INDEX_NONE;
	CurrentDamageIndex = INDEX_NONE;
	const bool bWasYabawiGroggy = ClownAttackStage == EClownAttackStage::Groggy;

	if (YabawiComponent && YabawiComponent->GetStage() != EClownYabawiStage::None)
	{
		YabawiComponent->CancelRound();
	}

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(AttackTimeoutHandle);
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

	if (Anim && bStopMontage && ActiveGroggyMontage.IsValid())
	{
		Anim->Montage_Stop(IsClownCombatPhase() ? 0.15f : 0.0f, ActiveGroggyMontage.Get());
	}

	ActiveGroggyMontage.Reset();
	YabawiGroggyDuration = 0.0f;
	RecordClownAttackFinished(CurrentAttackType);

	if (Anim && Montage)
	{
		FOnMontageEnded EmptyDelegate;
		Anim->Montage_SetEndDelegate(EmptyDelegate, Montage);

		if (bStopMontage)
		{
			Anim->Montage_Stop(IsClownCombatPhase() ? 0.15f : 0.0f, Montage);
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

	RestoreClownMovement();
	if (bWasYabawiGroggy && GetEnemyActionState() == EEnemyActionState::Groggy)
	{
		SetEnemyActionState(EEnemyActionState::None);
	}

	if (bRestoreState && !IsActorBeingDestroyed() && IsClownCombatPhase() &&
		(GetEnemyState() == EEnemyState::Attack || GetEnemyState() == EEnemyState::FastChase ||
			(bWasYabawiGroggy && GetEnemyState() == EEnemyState::Groggy)))
	{
		SetEnemyState(EEnemyState::Chase);
	}

	RefreshClownMovement();
}

void ABossClown::ResetBossGimmicks()
{
	CriticalReactionBrain.Reset();
	if (BossPhase == EBossPhase::Waiting)
	{
		for (const auto& Entry : BrokenIntroProps)
		{
			if (UMeshComponent* Prop = Entry.Key.Get())
			{
				Prop->SetVisibility(true);
				Prop->SetCollisionEnabled(Entry.Value);
			}
		}
		BrokenIntroProps.Reset();
	}
	UpdateIntroBreakables();

	if (BossPhase == EBossPhase::Waiting || IsActorBeingDestroyed())
	{
		GetWorldTimerManager().ClearTimer(Phase2WallDebrisTimer);
		Phase2IntactWallTransforms.Reset();
		Phase2FadingDebris.Reset();
		Phase2WallFadePose.Reset();
		Phase2LargeWallBricks.Reset();
		Phase2DebrisFadeElapsed.Reset();
		ClearPhase2WallChips();
		Phase2WallFog->SetVisibility(false);
	}

	if (BossPhase == EBossPhase::Waiting && bPhase2WallPlayed && IsValid(Phase2WallCache))
	{
		Phase2WallCache->Stop();
		if (IsValid(Phase2WallExitBlocker))
		{
			Phase2WallExitBlocker->SetActorEnableCollision(false);
		}
		if (IsValid(Phase2BrokenWall))
		{
			Phase2BrokenWall->SetActorHiddenInGame(true);
		}
		for (const FObservedComponent& Observed : Phase2WallCache->GetObservedComponents())
		{
			if (UGeometryCollectionComponent* Wall = Cast<UGeometryCollectionComponent>(Observed.GetComponent(Phase2WallCache.Get())))
			{
				if (Phase2WallCollision.IsSet())
				{
					Wall->SetCollisionEnabled(Phase2WallCollision.GetValue());
				}
				Wall->SetVisibility(true);
				Wall->SetSimulatePhysics(false);
				Wall->ResetRestTransforms();
				Wall->ResetState();
			}
		}
		Phase2WallCache->Start(0.0f);
		Phase2WallCollision.Reset();
		bPhase2WallPlayed = false;
	}

	if (YabawiComponent)
	{
		YabawiComponent->CancelRound(true);
	}

	if (ActiveAttackID == INDEX_NONE)
	{
		ClearClownProjectiles(true);
	}
	else
	{
		FinishClownAttack(EBossClownAttackResult::Aborted, true, false);
	}

	ResetClownAttackSelection();
	LastPlayedAttackMontage.Reset();
	NextChasePathTime = 0.0;
	SetStrafeMode(false);
	Super::ResetBossGimmicks();
}

void ABossClown::HandleAttackTraceHit(const FHitResult& Hit)
{
	// 첫 공격만 공통 보스에서 처리.
	// 일반 적의 피해 전달 함수는 사용하지 않음.
	if (BossPhase == EBossPhase::OpeningAttack)
	{
		Super::HandleAttackTraceHit(Hit);

		return;
	}

	// 광대 전투 공격의 현재 타격 데이터에서 직접 피해 전달.
	if (!IsClownCombatPhase() || ActiveAttackID == INDEX_NONE || ClownAttackStage != EClownAttackStage::Montage ||
		!CurrentClownAttack.DamageInfo.IsValidIndex(CurrentDamageIndex))
	{
		return;
	}

	if (ICombatDamageInterface* Target = Cast<ICombatDamageInterface>(Hit.GetActor()))
	{
		const FDamageInfo& Damage = CurrentClownAttack.DamageInfo[CurrentDamageIndex];

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

bool ABossClown::UsesSpecialPresentation(EBossClownAttack Type)
{
	return Type == EBossClownAttack::TripleBalls || Type == EBossClownAttack::ClapWave || Type == EBossClownAttack::SpawnCards || Type == EBossClownAttack::CubeBarrage || Type == EBossClownAttack::Yabawi;
}

bool ABossClown::UsesLoopingMontage(EBossClownAttack Type)
{
	return Type == EBossClownAttack::TripleBalls || Type == EBossClownAttack::CubeBarrage || Type == EBossClownAttack::Yabawi;
}

bool ABossClown::IsMeleeSwingActive() const
{
	return IsClownCombatPhase() && ActiveAttackID != INDEX_NONE && ClownAttackStage == EClownAttackStage::Montage &&
		!UsesSpecialPresentation(CurrentAttackType);
}

void ABossClown::PlayMeleeSwingSound()
{
	if (!IsMeleeSwingActive())
	{
		return;
	}

	bPlayedMeleeGroundContact = false;
	if (const UBossClownAnimationData* Data = GetClownAnimationData())
	{
		if (const FClownPatternEffects* Effects = Data->PatternEffects.Find(CurrentAttackType))
		{
			Effects->PlaySound(this, GetActorLocation());
		}
	}
}

void ABossClown::HandleMeleeGroundContact(const FHitResult& Hit)
{
	const UBossClownAnimationData* Data = GetClownAnimationData();
	if (!Data || bPlayedMeleeGroundContact || !IsMeleeSwingActive() || !IsEncounterFloorHit(Hit))
	{
		return;
	}

	bPlayedMeleeGroundContact = true;
	const bool bHeavy = CurrentAttackType == EBossClownAttack::HammerSlam || CurrentAttackType == EBossClownAttack::FastChaseSlam;
	FClownPatternEffects Impact = Data->MeleeGroundEffects;
	Impact.Volume *= bHeavy ? 1.0f : 0.65f;
	const FRotator SurfaceRotation = FRotationMatrix::MakeFromZ(Hit.ImpactNormal).Rotator();
	Impact.Play(this, Hit.ImpactPoint + Hit.ImpactNormal * 2.0, SurfaceRotation);
	if (bHeavy && Data->HeavySlamDust.NiagaraSystem)
	{
		const FCollisionQueryParams FloorQuery(SCENE_QUERY_STAT(HeavySlamDust), false, this);
		for (const double Offset : { -37.0, 37.0 })
		{
			FVector Ground;
			if (FindClownPatternFloor(Hit.ImpactPoint + GetActorRightVector() * Offset, FloorQuery, Ground))
			{
				Data->HeavySlamDust.PlayVisual(this, Ground + FVector(0.0, 0.0, 6.0), SurfaceRotation);
			}
		}
	}
}

bool ABossClown::TryPlayBallGroundEffects(const FHitResult& Hit) const
{
	const UPrimitiveComponent* Surface = Hit.GetComponent();
	if (!Surface || Hit.bStartPenetrating || Hit.ImpactNormal.Z < 0.65f ||
		Surface->GetCollisionResponseToChannel(ECC_Pawn) != ECR_Block)
	{
		return false;
	}

	if (const UBossClownAnimationData* Data = GetClownAnimationData())
	{
		const FVector Position = Hit.ImpactPoint + Hit.ImpactNormal * 2.0;
		const FRotator Rotation = FRotationMatrix::MakeFromZ(Hit.ImpactNormal).Rotator();
		Data->BallGroundEffects.Play(this, Position, Rotation);
		Data->BallGroundDust.PlayVisual(this, Position, Rotation);
	}
	return true;
}

void ABossClown::PlayPatternEffects(EBossClownAttack Attack, const FVector& Location, const FRotator& Rotation) const
{
	if (const UBossClownAnimationData* Data = GetClownAnimationData())
	{
		if (const FClownPatternEffects* Effects = Data->PatternEffects.Find(Attack))
		{
			Effects->Play(this, Location, Rotation);
		}
	}
}

void ABossClown::PlayYabawiFailureEffects(const TArray<FVector>& BowlLocations)
{
	const UBossClownAnimationData* Data = GetClownAnimationData();
	if (!Data || BowlLocations.IsEmpty())
	{
		return;
	}

	const FBox BowlBounds(BowlLocations);
	const FClownPatternEffects& Effects = Data->YabawiFailureEffects;
	const FVector Center = BowlBounds.GetCenter();
	Effects.PlaySound(this, Center);

	FVector Ground;
	const FCollisionQueryParams FloorQuery(SCENE_QUERY_STAT(YabawiFailureFloor), false, this);
	if (!Effects.NiagaraSystem || !FindClownPatternFloor(Center, FloorQuery, Ground))
	{
		return;
	}

	UNiagaraComponent* Wave = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		this, Effects.NiagaraSystem, Ground + FVector(0.0, 0.0, 4.0), FRotator::ZeroRotator,
		FVector(Effects.NiagaraScale, Effects.NiagaraScale, 1.0), true, false);
	if (Wave)
	{
		// Keep the purple floor wave and suppress its high, bright emitters.
		Wave->SetEmitterEnable(TEXT("Sparks1"), false);
		Wave->SetEmitterEnable(TEXT("Sparks2"), false);
		Wave->SetEmitterEnable(TEXT("Ray"), false);
		Wave->SetEmitterEnable(TEXT("Spiral1"), false);
		const FNiagaraUserRedirectionParameterStore& Parameters = Effects.NiagaraSystem->GetExposedParameters();
		for (const FName ColorName : { FName(TEXT("User.Color_Smoke")), FName(TEXT("User.Color_Wave")) })
		{
			const FNiagaraVariableBase Parameter(FNiagaraTypeDefinition::GetColorDef(), ColorName);
			if (const TOptional<FLinearColor> Color = Parameters.GetParameterOptionalValue<FLinearColor>(Parameter))
			{
				Wave->SetVariableLinearColor(ColorName, Color.GetValue() * 0.64f);
			}
		}
		Wave->Activate(true);
		if (Effects.NiagaraSystem->IsLooping())
		{
			FTimerHandle StopTimer;
			GetWorldTimerManager().SetTimer(StopTimer, Wave, &UNiagaraComponent::Deactivate, 0.8f, false);
		}
	}
}

bool ABossClown::PrepareClownPattern()
{
	BallsLaunched = 0;
	bClapReleased = false;

	switch (CurrentAttackType)
	{
	case EBossClownAttack::TripleBalls: return PrepareClownBalls();
	case EBossClownAttack::CubeBarrage: return PrepareClownCubeMarch();
	case EBossClownAttack::Yabawi: return PrepareClownYabawi();
	default: return true;
	}
}

bool ABossClown::TickClownSpecial(float DeltaTime, float MontageTime)
{
	const UBossClownAnimationData* Data = GetClownAnimationData();
	AActor* Target = ClownAttackTarget.Get();

	if (!Data || !ClownAttackTarget.IsValid())
	{
		return false;
	}

	if (CurrentAttackType == EBossClownAttack::CubeBarrage)
	{
		return AttackStageTime >= UClownProjectileComponent::CubeMarchCycle * 3.0f;
	}

	if (CurrentAttackType == EBossClownAttack::TripleBalls)
	{
		for (const TWeakObjectPtr<AActor>& Item : Projectiles)
		{
			UClownProjectileComponent* Ball = Item.IsValid() ? Item->FindComponentByClass<UClownProjectileComponent>() : nullptr;
			if (Ball && !Ball->IsLaunched())
			{
				Item->SetActorLocation(BallOrbitCenter + (Item->GetActorLocation() - BallOrbitCenter).RotateAngleAxis(45.0f * DeltaTime, BallOrbitAxis));
			}
		}

		while (BallsLaunched < BallCount && AttackStageTime >= Data->BallFirstLaunchTime + BallsLaunched * Data->BallLaunchInterval)
		{
			for (const TWeakObjectPtr<AActor>& Item : Projectiles)
			{
				UClownProjectileComponent* Ball = Item.IsValid() ? Item->FindComponentByClass<UClownProjectileComponent>() : nullptr;
				if (Ball && !Ball->IsLaunched())
				{
					Ball->LaunchAt(Target->GetActorLocation());
					if (Ball->IsLaunched())
					{
						PlayPatternEffects(CurrentAttackType, Item->GetActorLocation(), Item->GetActorRotation());
					}
					break;
				}
			}
			++BallsLaunched;
		}

		return HasFinishedLaunchingBalls();
	}

	if (CurrentAttackType == EBossClownAttack::ClapWave && !bClapReleased && MontageTime >= Data->ClapImpactTime)
	{
		const FDamageInfo Damage = CurrentClownAttack.DamageInfo.IsEmpty() ? FDamageInfo() : CurrentClownAttack.DamageInfo[0];
		LaunchClapWave(*Data, Damage, Data->ClapWaveRange, Data->ClapWaveSpeed);
	}

	return false;
}

void ABossClown::LaunchClapWave(const UBossClownAnimationData& Data, const FDamageInfo& Damage, float Range, float Speed)
{
	bClapReleased = true;
	const FVector Position = GetActorLocation() + GetActorForwardVector() * 100.0f;
	const int32 NoteCount = FMath::RandRange(16, 22);
	const float LifeTime = Range / FMath::Max(Speed, 1.0f);
	PlayPatternEffects(EBossClownAttack::ClapWave, Position, GetActorRotation());

	for (int32 Index = 0; Index < NoteCount; ++Index)
	{
		const float Angle = FMath::Lerp(-28.0f, 28.0f, (Index + FMath::FRand()) / NoteCount);
		const FVector Direction = GetActorForwardVector().RotateAngleAxis(Angle, FVector::UpVector);
		const FVector Spawn = Position + GetActorRightVector() * FMath::FRandRange(-150.0f, 150.0f) + FVector(0.0f, 0.0f, FMath::FRandRange(-35.0f, 65.0f));
		UClownProjectileComponent* Projectile = CreateClownProjectile(Data.ClapWaveClass, Spawn, Direction.Rotation(), Speed, LifeTime, &Damage);

		if (Projectile)
		{
			Projectile->GetOwner()->SetActorScale3D(FVector(FMath::FRandRange(0.75f, 1.25f)));
			Projectile->LaunchAt(Spawn + Direction * Range);
		}
	}
}

void ABossClown::FindEncounterFloor()
{
	// 입장 당시 보스 주변의 실제 바닥 컴포넌트를 기억. 전투 중 소품 위에 올라가도 기준을 바꾸지 않음.
	FCollisionObjectQueryParams Objects;
	Objects.AddObjectTypesToQuery(ECC_WorldStatic);
	Objects.AddObjectTypesToQuery(ECC_WorldDynamic);
	FCollisionQueryParams Query(SCENE_QUERY_STAT(ClownEncounterFloor), false, this);

	if (const UBoxComponent* Arena = GetEncounterArena() ? GetEncounterArena()->FindComponentByClass<UBoxComponent>() : nullptr)
	{
		Query.AddIgnoredComponent(Arena);
	}

	const float FootZ = GetActorLocation().Z - GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const FVector Offsets[] = {FVector::ZeroVector, FVector(180.0f, 0.0f, 0.0f), FVector(-180.0f, 0.0f, 0.0f),
		FVector(0.0f, 180.0f, 0.0f), FVector(0.0f, -180.0f, 0.0f)};
	double ClosestFloorDistance = TNumericLimits<double>::Max();

	for (const FVector& Offset : Offsets)
	{
		const FVector Probe = GetActorLocation() + Offset;
		TArray<FHitResult> Hits;
		GetWorld()->LineTraceMultiByObjectType(
			Hits, FVector(Probe.X, Probe.Y, FootZ + 30.0f), FVector(Probe.X, Probe.Y, FootZ - 250.0f), Objects, Query);

		for (const FHitResult& Hit : Hits)
		{
			UPrimitiveComponent* Surface = Hit.GetComponent();
			const double DistanceToFoot = FMath::Abs(Hit.ImpactPoint.Z - FootZ);
			if (Surface && Surface->GetCollisionResponseToChannel(ECC_Pawn) == ECR_Block &&
				Hit.ImpactNormal.Z >= 0.9f && !Hit.bStartPenetrating && DistanceToFoot < ClosestFloorDistance)
			{
				ClosestFloorDistance = DistanceToFoot;
				EncounterFloor = Surface;
				EncounterFloorZ = Hit.ImpactPoint.Z;
			}
		}
	}

	if (const UPrimitiveComponent* Floor = EncounterFloor.Get())
	{
		EncounterFloorBounds = Floor->Bounds.GetBox();
		TInlineComponentArray<UPrimitiveComponent*> Surfaces(Floor->GetOwner());
		for (const UPrimitiveComponent* Surface : Surfaces)
		{
			const FBox Bounds = Surface->Bounds.GetBox();
			if (Surface->IsQueryCollisionEnabled() && Surface->GetCollisionResponseToChannel(ECC_Pawn) == ECR_Block &&
				FMath::IsNearlyEqual(Bounds.Max.Z, EncounterFloorZ, 2.0))
			{
				EncounterFloorBounds += Bounds;
			}
		}
	}
}

bool ABossClown::IsEncounterFloorHit(const FHitResult& Hit) const
{
	const UPrimitiveComponent* Floor = EncounterFloor.Get();
	const UPrimitiveComponent* Surface = Hit.GetComponent();
	return Floor && Surface && Surface->GetOwner() == Floor->GetOwner() &&
		Surface->GetCollisionResponseToChannel(ECC_Pawn) == ECR_Block &&
		!Hit.bStartPenetrating && Hit.ImpactNormal.Z >= 0.9f && FMath::Abs(Hit.ImpactPoint.Z - EncounterFloorZ) <= 3.0;
}

bool ABossClown::FitClownCubePath(FVector& Start, FVector& End) const
{
	const UPrimitiveComponent* Floor = EncounterFloor.Get();
	if (!Floor || !EncounterFloorBounds.IsValid)
	{
		return false;
	}

	// 회전 중에도 큐브 모서리가 바닥 안쪽에 남도록 반대각선만큼 여유를 둔다.
	constexpr double Margin = 67.0;
	const FVector Inset(Margin + 1.0, Margin + 1.0, 0.0);
	const FVector Min = EncounterFloorBounds.Min + Inset;
	const FVector Max = EncounterFloorBounds.Max - Inset;
	if (Min.X >= Max.X || Min.Y >= Max.Y)
	{
		return false;
	}

	const AActor* ArenaActor = GetEncounterArena();
	const UBoxComponent* Arena = ArenaActor ? ArenaActor->FindComponentByClass<UBoxComponent>() : nullptr;
	for (FVector* Point : {&Start, &End})
	{
		if (Arena)
		{
			const FVector Extent = Arena->GetUnscaledBoxExtent();
			const FVector Scale = Arena->GetComponentTransform().GetScale3D().GetAbs();
			const double MaxX = FMath::Max(Extent.X - Margin / FMath::Max(Scale.X, static_cast<double>(UE_KINDA_SMALL_NUMBER)), 0.0);
			const double MaxY = FMath::Max(Extent.Y - Margin / FMath::Max(Scale.Y, static_cast<double>(UE_KINDA_SMALL_NUMBER)), 0.0);
			FVector Local = Arena->GetComponentTransform().InverseTransformPosition(*Point);
			Local.X = FMath::Clamp(Local.X, -MaxX, MaxX);
			Local.Y = FMath::Clamp(Local.Y, -MaxY, MaxY);
			*Point = Arena->GetComponentTransform().TransformPosition(Local);
		}

		Point->X = FMath::Clamp(Point->X, Min.X, Max.X);
		Point->Y = FMath::Clamp(Point->Y, Min.Y, Max.Y);
		Point->Z = EncounterFloorZ + 52.0f;
	}

	const double Length = FVector::Dist2D(Start, End);
	if (Length < 94.0)
	{
		return false;
	}

	FCollisionObjectQueryParams FloorObjects;
	FloorObjects.AddObjectTypesToQuery(ECC_WorldStatic);
	FloorObjects.AddObjectTypesToQuery(ECC_WorldDynamic);
	const FCollisionQueryParams Query(SCENE_QUERY_STAT(ClownCubeFloor), false, this);
	const FVector Footprint[] = {FVector::ZeroVector, FVector(Margin, Margin, 0.0), FVector(Margin, -Margin, 0.0),
		FVector(-Margin, Margin, 0.0), FVector(-Margin, -Margin, 0.0)};
	const int32 Steps = FMath::CeilToInt(Length / 47.0);
	int32 SegmentStart = 0;
	int32 BestStart = 0;
	int32 BestEnd = 0;
	TArray<FHitResult> Hits;

	for (int32 Step = 0; Step <= Steps; ++Step)
	{
		const FVector Point = FMath::Lerp(Start, End, static_cast<double>(Step) / Steps);
		if (!Algo::AllOf(Footprint, [&](const FVector& Offset)
		{
			const FVector Probe(Point.X + Offset.X, Point.Y + Offset.Y, EncounterFloorZ);
			if (Arena)
			{
				const FVector Local = Arena->GetComponentTransform().InverseTransformPosition(Probe);
				const FVector Extent = Arena->GetUnscaledBoxExtent();
				if (FMath::Abs(Local.X) > Extent.X || FMath::Abs(Local.Y) > Extent.Y)
				{
					return false;
				}
			}

			GetWorld()->LineTraceMultiByObjectType(Hits, Probe + FVector(0.0, 0.0, 3.0), Probe - FVector(0.0, 0.0, 3.0), FloorObjects, Query);
			return Hits.ContainsByPredicate([this](const FHitResult& Hit) { return IsEncounterFloorHit(Hit); });
		}))
		{
			SegmentStart = Step + 1;
		}
		else if (Step - SegmentStart > BestEnd - BestStart)
		{
			BestStart = SegmentStart;
			BestEnd = Step;
		}
	}

	if (Length * (BestEnd - BestStart) / Steps < 94.0)
	{
		return false;
	}

	const FVector FittedStart = FMath::Lerp(Start, End, static_cast<double>(BestStart) / Steps);
	End = FMath::Lerp(Start, End, static_cast<double>(BestEnd) / Steps);
	Start = FittedStart;
	return true;
}

UClownProjectileComponent* ABossClown::CreateClownProjectile(
	TSubclassOf<AActor> Class, const FVector& Location, const FRotator& Rotation, float Speed, float LifeTime, const FDamageInfo* DamageOverride)
{
	if (!Class)
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

	const FDamageInfo Damage = CurrentClownAttack.DamageInfo.IsEmpty() ? FDamageInfo() : CurrentClownAttack.DamageInfo[0];
	Projectile->Initialize(DamageOverride ? *DamageOverride : Damage, Speed, LifeTime);
	Projectiles.Add(Actor);
	return Projectile;
}

void ABossClown::ClearClownProjectiles(bool bIncludeLaunched)
{
	for (const TWeakObjectPtr<AActor>& Item : Projectiles)
	{
		AActor* Actor = Item.Get();
		UClownProjectileComponent* Projectile = Actor ? Actor->FindComponentByClass<UClownProjectileComponent>() : nullptr;

		if (Actor && (bIncludeLaunched || !Projectile || !Projectile->IsLaunched()))
		{
			Actor->Destroy();
		}
	}

	Projectiles.RemoveAll(
		[](const TWeakObjectPtr<AActor>& Item)
		{
			return !Item.IsValid();
		});
}

bool ABossClown::PrepareClownBalls()
{
	ClearClownProjectiles(false);
	const UBossClownAnimationData* Data = GetClownAnimationData();
	AActor* Target = ClownAttackTarget.Get();

	const int32 Count = BallCount;

	// 바닥에 정확히 수직인 원. 플레이어 높이에 따라 소환 평면을 기울이지 않음.
	const FVector Center = GetActorLocation() + (Target ? Target->GetActorLocation() - GetActorLocation() : GetActorForwardVector()).GetSafeNormal2D() * 180.0f + FVector(0.0f, 0.0f, 260.0f);
	const FVector Forward = (Target ? Target->GetActorLocation() - Center : GetActorForwardVector()).GetSafeNormal2D();
	const FQuat Facing = FRotationMatrix::MakeFromX(Forward).ToQuat();
	const FVector Right = Facing.GetRightVector();
	const FVector Up = FVector::UpVector;
	BallOrbitCenter = Center;
	BallOrbitAxis = Forward;
	const float MarkerRadius = 300.0f;

	int32 SpawnedBalls = 0;

	for (int32 Index = 0; Index < Count; ++Index)
	{
		const double StartAngle = 2.0 * PI * Index / Count;
		const FVector Position = Center + (Right * FMath::Cos(StartAngle) + Up * FMath::Sin(StartAngle)) * MarkerRadius;

		if (UClownProjectileComponent* Ball = CreateClownProjectile(Data->BallClass, Position, Facing.Rotator(), Data->BallSpeed, 6.0f))
		{
			++SpawnedBalls;
			Ball->EnableBallGroundEffects();
			const FVector Axis = FMath::VRand();
			const float Rate = FMath::FRandRange(140.0f, 320.0f);
			Ball->SetVisualSpin(FRotator(Axis.X * Rate, Axis.Y * Rate, Axis.Z * Rate));
		}
	}

	return SpawnedBalls == Count;
}

bool ABossClown::FindClownPatternFloor(const FVector& Position, const FCollisionQueryParams& Query, FVector& OutGround) const
{
	if (!EncounterFloor.IsValid())
	{
		return false;
	}

	const AActor* ArenaActor = GetEncounterArena();
	const UBoxComponent* Arena = ArenaActor ? ArenaActor->FindComponentByClass<UBoxComponent>() : nullptr;
	const FVector Probe(Position.X, Position.Y, EncounterFloorZ);

	if (Arena)
	{
		const FVector Local = Arena->GetComponentTransform().InverseTransformPosition(Probe);
		const FVector Extent = Arena->GetUnscaledBoxExtent();

		if (FMath::Abs(Local.X) > Extent.X || FMath::Abs(Local.Y) > Extent.Y)
		{
			return false;
		}
	}

	FCollisionObjectQueryParams Objects;
	Objects.AddObjectTypesToQuery(ECC_WorldStatic);
	Objects.AddObjectTypesToQuery(ECC_WorldDynamic);
	TArray<FHitResult> Hits;
	GetWorld()->LineTraceMultiByObjectType(Hits, Probe + FVector(0.0, 0.0, 10.0), Probe - FVector(0.0, 0.0, 10.0), Objects, Query);
	const FHitResult* FloorHit = Hits.FindByPredicate([this](const FHitResult& Hit) { return IsEncounterFloorHit(Hit); });
	if (!FloorHit)
	{
		return false;
	}

	OutGround = FloorHit->ImpactPoint;
	return true;
}

bool ABossClown::FindClownPatternGround(
	const FVector& Position, const FVector& Center, float Width, float Height, FVector& OutGround) const
{
	FCollisionQueryParams Query(SCENE_QUERY_STAT(ClownYabawiGround), false, this);
	Query.AddIgnoredActor(ClownAttackTarget.Get());
	if (const AActor* Arena = GetEncounterArena())
	{
		Query.AddIgnoredComponent(Arena->FindComponentByClass<UBoxComponent>());
	}

	FVector Ground;
	if (!FindClownPatternFloor(Position, Query, Ground))
	{
		return false;
	}

	const FVector Direction = (Center - Ground).GetSafeNormal2D();
	const FVector FootprintCenter = Ground + Direction * (Height * 0.5f) + FVector(0.0f, 0.0f, Height * 0.5f + 16.0f);

	// 그릇 주변의 장애물과 플레이어가 접근할 공간을 검사한다.
	const FCollisionShape Footprint = FCollisionShape::MakeBox(FVector(Height * 0.5f + 5.0f, Width * 0.5f + 5.0f, Height * 0.5f + 5.0f));

	if (GetWorld()->OverlapBlockingTestByChannel(
			FootprintCenter, Direction.Rotation().Quaternion(), ECC_Visibility, Footprint, Query) ||
		GetWorld()->OverlapBlockingTestByChannel(
			FootprintCenter, Direction.Rotation().Quaternion(), ECC_Pawn, Footprint, Query))
	{
		return false;
	}

	OutGround = Ground;

	return true;
}

bool ABossClown::PrepareClownYabawi()
{
	const FClownPhaseOnePattern* Rules = GetAttackPattern(EBossClownAttack::Yabawi);
	AActor* Target = ClownAttackTarget.Get();

	if (!Rules || !IsValid(Target) || !YabawiComponent)
	{
		return false;
	}

	const int32 Count = FMath::Clamp(Rules->BowlCount, 2, 8);
	const float Diameter = FMath::Max(Rules->BowlDiameter, 100.0f);
	const float Spacing = FMath::Max(Rules->BowlSpacing, Diameter + 60.0f);
	FVector Forward = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();

	if (Forward.IsNearlyZero())
	{
		Forward = GetActorForwardVector();
	}

	const FVector Right = FVector::CrossProduct(FVector::UpVector, Forward);
	const float Distance = FVector::Dist2D(Target->GetActorLocation(), GetActorLocation());
	const FVector BaseCenter = GetActorLocation() + Forward * FMath::Clamp(Distance - 300.0f, 500.0f, 900.0f);

	// 기존 카드와 같은 실제 바닥 검사로 소품 윗면과 막힌 영역을 제외.
	for (int32 Attempt = 0; Attempt < 17; ++Attempt)
	{
		const float Angle = 2.0f * PI * (Attempt - 1) / 8.0f;
		const float Radius = Attempt == 0 ? 0.0f : (Attempt <= 8 ? 300.0f : 600.0f);
		const FVector Center = BaseCenter + (Forward * FMath::Cos(Angle) + Right * FMath::Sin(Angle)) * Radius;
		TArray<FVector> Positions;

		for (int32 Index = 0; Index < Count; ++Index)
		{
			const FVector Candidate = Center + Right * (Index - (Count - 1) * 0.5f) * Spacing;
			FVector Ground;

			if (!FindClownPatternGround(Candidate, Center, Diameter, Diameter, Ground))
			{
				break;
			}

			Positions.Add(Ground + FVector(0.0f, 0.0f, 3.0f));
		}

		if (Positions.Num() == Count)
		{
			return YabawiComponent->StartRound(Target, Positions);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Clown yabawi: no clear floor for %d bowls"), Count);

	return false;
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

	if (!EncounterFloor.IsValid())
	{
		return false;
	}

	const FVector Center = Target->GetActorLocation();
	const FVector Forward = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();
	const FVector Assembly = GetActorLocation() + Forward * 180.0f + FVector(0.0f, 0.0f, 480.0f);
	TArray<FVector> Starts;
	TArray<FVector> Ends;

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
				Start.Z = EncounterFloorZ + 52.0f;
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

		Piece->ArmCubeMarch(Data->CubePieces[Index], Assembly + Offset, Starts[Index], Ends[Index],
			UClownProjectileComponent::CubeMarchCycle * (Index / 9 + 1), Index % 3 == 0);
		Piece->SetCubeAim(Target, Center);
	}

	PlayPatternEffects(EBossClownAttack::CubeBarrage, Assembly, Forward.Rotation());
	return true;
}

bool ABossClown::FindClownCardLayout(FClownCardLayout& Layout) const
{
	const UBossClownAnimationData* Data = GetClownAnimationData();
	const AActor* Target = ClownAttackTarget.Get();

	if (!Data || !Data->CardClass || !IsValid(Target) || !EncounterFloor.IsValid() ||
		!Data->HeartFaces.ContainsByPredicate([](const TObjectPtr<UTexture2D>& Face) { return IsValid(Face.Get()); }) ||
		!Data->CardFaces.ContainsByPredicate([Data](const TObjectPtr<UTexture2D>& Face) { return IsValid(Face.Get()) && !Data->HeartFaces.Contains(Face); }))
	{
		return false;
	}

	const FVector TargetCenter = Target->GetActorLocation();
	const int32 Count = FMath::Clamp(Data->CardCount, 3, 10);
	const float Width = FMath::Clamp(Data->CardWidth, 350.0f, 550.0f);
	const float HalfAngle = PI / Count;
	const float AngleStep = HalfAngle * 2.0f;
	const float OriginalRadius = FMath::Max(FMath::Clamp(Data->CardRadius, 600.0f, 900.0f),
		(Width * 0.5f + 157.0f + 60.0f * FMath::Abs(FMath::Cos(AngleStep))) / FMath::Sin(AngleStep) + 90.0f);
	const float Height = FMath::Clamp(Data->CardHeight, OriginalRadius - 100.0f, OriginalRadius + 50.0f);
	const UCapsuleComponent* Capsule = Target->FindComponentByClass<UCapsuleComponent>();
	const float ExitRadius = Capsule ? Capsule->GetScaledCapsuleRadius() : 34.0f;
	const float ExitHalfHeight = Capsule ? Capsule->GetScaledCapsuleHalfHeight() : 88.0f;
	const FVector ExitHeight(0.0, 0.0, ExitHalfHeight + 2.0f);
	const FCollisionShape ExitCapsule = FCollisionShape::MakeCapsule(ExitRadius, ExitHalfHeight);
	FCollisionQueryParams Query(SCENE_QUERY_STAT(ClownCardPlacement), false, this);
	Query.AddIgnoredActor(Target);
	const AActor* ArenaActor = GetEncounterArena();
	const UBoxComponent* Arena = ArenaActor ? ArenaActor->FindComponentByClass<UBoxComponent>() : nullptr;
	if (Arena)
	{
		Query.AddIgnoredComponent(Arena);
	}

	// 폭과 반경을 같은 비율로 줄여도 인접 카드 사이의 틈은 닫힌 상태로 유지된다.
	const float ClosedRadius = Width / (2.0f * FMath::Tan(HalfAngle));
	const float Radius = FMath::Clamp(OriginalRadius, ClosedRadius, ClosedRadius + ExitRadius / (2.0f * FMath::Sin(HalfAngle)));
	const float InitialFacing = FMath::FRandRange(0.0f, AngleStep);
	const AActor* FloorOwner = EncounterFloor->GetOwner();
	const FVector Interior = Arena ? Arena->GetComponentLocation() : EncounterFloorBounds.GetCenter();
	const FVector Inward = (Interior - TargetCenter).GetSafeNormal2D();
	TArray<int32> HeartCandidates;
	TArray<FOverlapResult> Overlaps;
	constexpr int32 LayoutAttempts = 12;

	const auto TryFacing = [&](const FVector& Center, float Scale, float Facing)
	{
		Layout.Positions.Reset(Count);
		HeartCandidates.Reset(Count);
		const float RingWidth = Width * Scale;
		const float RingHeight = Height * Scale;
		const FCollisionShape StandingCard = FCollisionShape::MakeBox(FVector(5.0, RingWidth * 0.5f, RingHeight * 0.5f));

		for (int32 Index = 0; Index < Count; ++Index)
		{
			const float Angle = Facing + AngleStep * Index;
			const FVector Outward(FMath::Cos(Angle), FMath::Sin(Angle), 0.0f);
			const FVector Position = Center + Outward * (Radius * Scale);
			FVector Ground;
			if (!FindClownPatternFloor(Position, Query, Ground))
			{
				return false;
			}

			const FVector Side = FVector::CrossProduct(FVector::UpVector, Outward) * (RingWidth * 0.5f);
			FVector EdgeGround;
			if (!FindClownPatternFloor(Position + Side, Query, EdgeGround) || !FindClownPatternFloor(Position - Side, Query, EdgeGround))
			{
				return false;
			}

			const FVector CardCenter = Ground + FVector(0.0, 0.0, RingHeight * 0.5f + 6.0f);
			Overlaps.Reset();
			GetWorld()->OverlapMultiByChannel(Overlaps, CardCenter, Outward.Rotation().Quaternion(), ECC_Pawn, StandingCard, Query);
			if (Overlaps.ContainsByPredicate([this, FloorOwner](const FOverlapResult& Hit)
			{
				const AActor* Obstacle = Hit.GetActor();
				return Hit.bBlockingHit && Obstacle &&
					(Obstacle == FloorOwner || Obstacle == Phase2WallExitBlocker.Get() || Obstacle->IsA<ABossRoom>());
			}))
			{
				return false;
			}

			Layout.Positions.Add(Ground);
			const FVector Exit = Ground + Outward * (ExitRadius * 2.0f + 30.0f);
			FVector ExitGround;
			if (FindClownPatternFloor(Exit, Query, ExitGround) &&
				!GetWorld()->SweepTestByChannel(Ground - Outward * ExitRadius + ExitHeight, ExitGround + ExitHeight,
					FQuat::Identity, ECC_Pawn, ExitCapsule, Query))
			{
				HeartCandidates.Add(Index);
			}
		}

		if (HeartCandidates.IsEmpty())
		{
			return false;
		}

		Layout.Positions.Swap(0, HeartCandidates[FMath::RandRange(0, HeartCandidates.Num() - 1)]);
		Layout.Center = Center;
		Layout.Width = RingWidth;
		Layout.Height = RingHeight;
		return true;
	};

	for (const float Scale : {1.0f, 0.86f, 0.72f, 0.58f})
	{
		// 타깃 캡슐 전체가 원 안에 남을 수 있는 거리만큼만 중심을 옮긴다.
		const double AvailableShift = Radius * Scale - ExitRadius - 10.0;
		if (AvailableShift < 0.0)
		{
			continue;
		}

		const double Shift = FMath::Min(FVector::Dist2D(TargetCenter, Interior), AvailableShift);
		const FVector Centers[] = { TargetCenter, TargetCenter + Inward * (Shift * 0.45),
			TargetCenter + Inward * (Shift * 0.75), TargetCenter + Inward * Shift,
			TargetCenter + Inward.RotateAngleAxis(30.0, FVector::UpVector) * Shift,
			TargetCenter + Inward.RotateAngleAxis(-30.0, FVector::UpVector) * Shift };
		const int32 CenterCount = Shift > 1.0 ? UE_ARRAY_COUNT(Centers) : 1;

		for (int32 CenterIndex = 0; CenterIndex < CenterCount; ++CenterIndex)
		{
			for (int32 Attempt = 0; Attempt < LayoutAttempts; ++Attempt)
			{
				const float Facing = InitialFacing + AngleStep * Attempt / LayoutAttempts;
				if (TryFacing(Centers[CenterIndex], Scale, Facing))
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool ABossClown::SpawnClownCards(const FClownCardLayout& Layout)
{
	ClearClownProjectiles(false);
	const UBossClownAnimationData* Data = GetClownAnimationData();
	if (!Data)
	{
		return false;
	}

	TArray<UTexture2D*> HeartFaces;
	TArray<UTexture2D*> DangerousFaces;
	for (const TObjectPtr<UTexture2D>& Face : Data->HeartFaces)
	{
		if (IsValid(Face.Get()))
		{
			HeartFaces.Add(Face.Get());
		}
	}
	for (const TObjectPtr<UTexture2D>& Face : Data->CardFaces)
	{
		if (IsValid(Face.Get()) && !Data->HeartFaces.Contains(Face))
		{
			DangerousFaces.Add(Face.Get());
		}
	}

	if (HeartFaces.IsEmpty() || DangerousFaces.IsEmpty())
	{
		return false;
	}

	constexpr float HeartLeadTime = 0.55f;
	const float StandTime = FMath::Max(Data->CardFirstLaunchTime, 1.5f);
	TArray<AActor*> SpawnedCards;
	for (int32 Index = 0; Index < Layout.Positions.Num(); ++Index)
	{
		const bool bHeart = Index == 0;
		const TArray<UTexture2D*>& Faces = bHeart ? HeartFaces : DangerousFaces;
		UTexture2D* Face = Faces[FMath::RandRange(0, Faces.Num() - 1)];
		const FVector& Position = Layout.Positions[Index];
		const FVector Direction = (Layout.Center - Position).GetSafeNormal2D();
		UClownProjectileComponent* Projectile = CreateClownProjectile(Data->CardClass, Position, Direction.Rotation(), 0.0f, Data->CardActiveTime);

		if (Projectile)
		{
			const float CardStandTime = bHeart ? StandTime : StandTime + HeartLeadTime;
			Projectile->ArmCard(Position, CardStandTime, Layout.Height, Layout.Width, Data->CardFallTime, Face, bHeart);
			if (!Projectile->GetOwner()->IsActorBeingDestroyed())
			{
				SpawnedCards.Add(Projectile->GetOwner());
				continue;
			}
		}

		for (AActor* Card : SpawnedCards) Card->Destroy();

		return false;
	}

	PlayPatternEffects(EBossClownAttack::SpawnCards, Layout.Center, GetActorRotation());
	return true;
}
