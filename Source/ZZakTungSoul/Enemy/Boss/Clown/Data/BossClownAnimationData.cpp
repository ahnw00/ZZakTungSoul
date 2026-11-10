#include <Enemy/Boss/Clown/Data/BossClownAnimationData.h>
#include <Animation/AnimMontage.h>
#include <Engine/StaticMesh.h>
#include <Engine/World.h>
#include <GeometryCollection/GeometryCollectionObject.h>
#include <Kismet/GameplayStatics.h>
#include <NiagaraComponent.h>
#include <NiagaraFunctionLibrary.h>
#include <NiagaraSystem.h>
#include <Sound/SoundBase.h>
#include <TimerManager.h>
#include <UObject/ConstructorHelpers.h>

void FClownPatternEffects::Play(const UObject* WorldContext, const FVector& Location, const FRotator& Rotation) const
{
	PlaySound(WorldContext, Location);
	PlayVisual(WorldContext, Location, Rotation);
}

void FClownPatternEffects::PlaySound(const UObject* WorldContext, const FVector& Location) const
{
	if (Sound)
	{
		UGameplayStatics::PlaySoundAtLocation(WorldContext, Sound, Location, Volume, Pitch);
	}
}

void FClownPatternEffects::PlayVisual(const UObject* WorldContext, const FVector& Location, const FRotator& Rotation) const
{
	if (NiagaraSystem)
	{
		UNiagaraComponent* Effect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			WorldContext, NiagaraSystem, Location, Rotation, FVector(NiagaraScale));
		if (Effect && NiagaraSystem->IsLooping())
		{
			FTimerHandle StopTimer;
			Effect->GetWorld()->GetTimerManager().SetTimer(StopTimer, Effect, &UNiagaraComponent::Deactivate, 0.8f, false);
		}
	}
}

UBossClownAnimationData::UBossClownAnimationData()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Bowl(
		TEXT("/Game/Assets/Lowpoly_Bowl_3D_Model_for_Architecture_and_Kitchen/bowl/StaticMeshes/bowl.bowl"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Ball(
		TEXT("/Game/Blueprints/Enemy/Boss/Clown/Projectiles/SM_ClownBeachBallVivid.SM_ClownBeachBallVivid"));
	FClownPhaseOnePattern& Yabawi = PhaseOnePatterns.AddDefaulted_GetRef();
	Yabawi.AttackType = EBossClownAttack::Yabawi;
	Yabawi.bAllowInPhase1 = false;
	Yabawi.bAllowInPhase2 = true;
	Yabawi.BowlMesh = Bowl.Object;
	Yabawi.BallMesh = Ball.Object;
	Yabawi.ExplosionDamage.AttackDamage = 80.0f;
	Yabawi.ExplosionDamage.bUnblockable = true;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> FrontCritical(
		TEXT("/Game/Blueprints/Enemy/Boss/Clown/Animation/Encounter/AM_Clown_FrontCritical.AM_Clown_FrontCritical"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> BackCritical(
		TEXT("/Game/Blueprints/Enemy/Boss/Clown/Animation/Encounter/AM_Clown_BackCritical.AM_Clown_BackCritical"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> Death(
		TEXT("/Game/Blueprints/Enemy/Boss/Clown/Animation/Encounter/AM_Clown_Death.AM_Clown_Death"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> YabawiGroggy(
		TEXT("/Game/Blueprints/Enemy/Boss/Clown/Animation/Encounter/AM_Clown_YabawiGroggy.AM_Clown_YabawiGroggy"));
	GroggyMontage = YabawiGroggy.Object;
	FrontCriticalReaction = FrontCritical.Object;
	BackCriticalReaction = BackCritical.Object;
	DeathReaction = Death.Object;

	const auto LoadSound = [](const TCHAR* Folder, int32 Variation = 1)
	{
		const FString Name = FString::Printf(TEXT("%s_%02d"), Folder, Variation);
		const FString Path = FString::Printf(TEXT("/Game/FreeGameSoundsVol1/SFX/%s/%s.%s"), Folder, *Name, *Name);
		return ConstructorHelpers::FObjectFinder<USoundBase>(*Path).Object;
	};
	const auto LoadNiagara = [](const TCHAR* Variant)
	{
		const FString Name = FString(TEXT("NS_Free_Magic_")) + Variant;
		const FString Path = FString::Printf(TEXT("/Game/Free_Magic/VFX_Niagara/%s.%s"), *Name, *Name);
		return ConstructorHelpers::FObjectFinder<UNiagaraSystem>(*Path).Object;
	};
	UNiagaraSystem* RevealCircle = LoadNiagara(TEXT("Circle1"));

	const auto LoadClownFoley = [](const TCHAR* Name)
	{
		const FString Path = FString::Printf(TEXT("/Game/Blueprints/Enemy/Boss/Clown/Sound/%s.%s"), Name, Name);
		return ConstructorHelpers::FObjectFinder<USoundBase>(*Path).Object;
	};
	const auto SetPatternEffects = [this](EBossClownAttack Attack, USoundBase* Sound,
		float Volume, UNiagaraSystem* System = nullptr, float Scale = 1.0f, float Pitch = 1.0f)
	{
		FClownPatternEffects& Effects = PatternEffects.FindOrAdd(Attack);
		Effects.Sound = Sound;
		Effects.Volume = Volume;
		Effects.Pitch = Pitch;
		Effects.NiagaraSystem = System;
		Effects.NiagaraScale = Scale;
	};

	USoundBase* Sweep = LoadClownFoley(TEXT("SFX_Clown_WeaponSweep"));
	USoundBase* Slam = LoadClownFoley(TEXT("SFX_Clown_WeaponSlam"));
	SetPatternEffects(EBossClownAttack::HammerSweep, Sweep, 1.0f);
	SetPatternEffects(EBossClownAttack::HammerSlam, Slam, 0.95f);
	SetPatternEffects(EBossClownAttack::FastChaseSlam, Slam, 0.95f, nullptr, 1.0f, 1.08f);
	SetPatternEffects(EBossClownAttack::DashSweep, Sweep, 0.9f, nullptr, 1.0f, 1.12f);
	SetPatternEffects(EBossClownAttack::TripleBalls, LoadClownFoley(TEXT("SFX_Clown_BallRelease")), 0.68f);
	SetPatternEffects(EBossClownAttack::ClapWave, LoadSound(TEXT("Spell_Fireball")), 0.75f, LoadNiagara(TEXT("Attack2")), 0.7f);
	SetPatternEffects(EBossClownAttack::SpawnCards, LoadSound(TEXT("Spell_Ice_Spear")), 0.65f, RevealCircle, 0.65f);
	SetPatternEffects(EBossClownAttack::CubeBarrage, LoadSound(TEXT("Forge_Hammer_On_Anvil_Medium")), 0.7f, LoadNiagara(TEXT("Circle2")), 0.8f);

	const ConstructorHelpers::FObjectFinder<USoundBase> BowlTap(
		TEXT("/Game/Blueprints/Enemy/Boss/Clown/Sound/SFX_PingPong_1.SFX_PingPong_1"));
	FClownPatternEffects& BowlCover = PatternEffects.Add(EBossClownAttack::Yabawi);
	BowlCover.Sound = BowlTap.Object;
	BowlCover.Volume = 0.45f;
	BowlCover.Pitch = 0.85f;

	const ConstructorHelpers::FObjectFinder<UNiagaraSystem> Gravel(
		TEXT("/Game/A_Surface_Footstep/Niagara_FX/ParticleSystems/PSN_Gravel_Surface.PSN_Gravel_Surface"));
	MeleeGroundEffects.Sound = LoadClownFoley(TEXT("SFX_Clown_StoneImpact"));
	MeleeGroundEffects.Volume = 0.95f;
	MeleeGroundEffects.NiagaraSystem = Gravel.Object;
	MeleeGroundEffects.NiagaraScale = 4.5f;
	const ConstructorHelpers::FObjectFinder<UNiagaraSystem> SlamDust(
		TEXT("/Game/A_Surface_Footstep/Niagara_FX/ParticleSystems/PSN_Dirt_Surface.PSN_Dirt_Surface"));
	HeavySlamDust.NiagaraSystem = SlamDust.Object;
	HeavySlamDust.NiagaraScale = 3.5f;
	BallGroundEffects = MeleeGroundEffects;
	BallGroundEffects.Volume = 0.56f;
	BallGroundEffects.Pitch = 1.1f;
	BallGroundEffects.NiagaraScale = 1.45f;
	BallGroundDust.NiagaraSystem = SlamDust.Object;
	BallGroundDust.NiagaraScale = 1.1f;
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> BallWind(
		TEXT("/Game/Assets/BlinkAndDashVFX/VFX_Niagara/NS_Dash_Wind.NS_Dash_Wind"));
	BallSpinEffect = BallWind.Object;

	YabawiFailureEffects.Sound = LoadSound(TEXT("Pickaxe_Strike_Rock"));
	YabawiFailureEffects.Volume = 0.95f;
	YabawiFailureEffects.Pitch = 0.55f;
	YabawiFailureEffects.NiagaraSystem = LoadNiagara(TEXT("Area2"));
	YabawiFailureEffects.NiagaraScale = 2.3f;

	const auto AddFracture = [this](FName Shape, const TCHAR* Asset, const TCHAR* Subfolder = TEXT(""))
	{
		const FString Path = FString::Printf(TEXT("/Game/Blueprints/Enemy/Boss/Clown/Fracture/%s%s.%s"), Subfolder, Asset, Asset);
		Fractures.Add(Shape, ConstructorHelpers::FObjectFinder<UGeometryCollection>(*Path).Object);
	};
	AddFracture(TEXT("bowl"), TEXT("GC_ClownBowl"));
	AddFracture(TEXT("SM_ClownBeachBallVivid"), TEXT("GC_ClownBall"));
	AddFracture(TEXT("SM_ClownCard"), TEXT("GC_ClownCard"));
	AddFracture(TEXT("IntroCard"), TEXT("GC_ClownIntroCard"));
	for (int32 Index = 0; Index < 27; ++Index)
	{
		const FString Suffix = FString::Printf(TEXT("%d%d%d"), Index / 9, Index / 3 % 3, Index % 3);
		AddFracture(FName(*(TEXT("SM_ClownCube_") + Suffix)), *(TEXT("GC_ClownCube_") + Suffix), TEXT("CubePieces/"));
	}
	AddFracture(TEXT("IntroCube"), TEXT("GC_ClownIntroCube"));
	AddFracture(TEXT("SM_Crate"), TEXT("GC_ClownCrate"));
	AddFracture(TEXT("SKM_CanLowGrade"), TEXT("GC_ClownCan"));
}
