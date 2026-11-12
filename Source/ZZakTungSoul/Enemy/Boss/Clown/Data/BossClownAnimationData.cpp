#include <Enemy/Boss/Clown/Data/BossClownAnimationData.h>
#include <Animation/AnimMontage.h>
#include <Engine/StaticMesh.h>
#include <Engine/World.h>
#include <GeometryCollection/GeometryCollectionObject.h>
#include <Chaos/CacheCollection.h>
#include <Kismet/GameplayStatics.h>
#include <Materials/MaterialInterface.h>
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
	FClownSoundEffect{Sound, Volume, Pitch}.Play(WorldContext, Location);
}

void FClownSoundEffect::Play(const UObject* WorldContext, const FVector& Location) const
{
	if (Sound)
	{
		UGameplayStatics::PlaySoundAtLocation(WorldContext, Sound, Location, Volume, Pitch);
	}
}

void FClownPatternEffects::PlayVisual(const UObject* WorldContext, const FVector& Location, const FRotator& Rotation) const
{
	FClownVisualEffect{NiagaraSystem, NiagaraScale}.Play(WorldContext, Location, Rotation);
}

void FClownVisualEffect::Play(const UObject* WorldContext, const FVector& Location, const FRotator& Rotation) const
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

	FClownPhaseOnePattern& YabawiPattern = PhaseOnePatterns.AddDefaulted_GetRef();
	YabawiPattern.AttackType = EBossClownAttack::Yabawi;
	YabawiPattern.bAllowInPhase1 = false;
	YabawiPattern.bAllowInPhase2 = true;
	Yabawi.BowlMesh = Bowl.Object;
	Yabawi.BallMesh = Ball.Object;
	Yabawi.ExplosionDamage.AttackDamage = 80.0f;
	Yabawi.ExplosionDamage.bUnblockable = true;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> FrontCritical(
		TEXT("/Game/Blueprints/Enemy/Boss/Clown/Animation/Montages/Encounter/AM_Clown_FrontCritical.AM_Clown_FrontCritical"));

	static ConstructorHelpers::FObjectFinder<UAnimMontage> BackCritical(
		TEXT("/Game/Blueprints/Enemy/Boss/Clown/Animation/Montages/Encounter/AM_Clown_BackCritical.AM_Clown_BackCritical"));

	static ConstructorHelpers::FObjectFinder<UAnimMontage> Death(
		TEXT("/Game/Blueprints/Enemy/Boss/Clown/Animation/Montages/Encounter/AM_Clown_Death.AM_Clown_Death"));

	static ConstructorHelpers::FObjectFinder<UAnimMontage> YabawiGroggy(
		TEXT("/Game/Blueprints/Enemy/Boss/Clown/Animation/Montages/Encounter/AM_Clown_YabawiGroggy.AM_Clown_YabawiGroggy"));

	GroggyMontage = YabawiGroggy.Object;
	FrontCriticalReaction = FrontCritical.Object;
	BackCriticalReaction = BackCritical.Object;
	DeathReaction = Death.Object;

	static ConstructorHelpers::FObjectFinder<USoundBase> Sweep(
		TEXT("/Game/Assets/Sound/Clown/SFX_Clown_WeaponSweep.SFX_Clown_WeaponSweep"));
	static ConstructorHelpers::FObjectFinder<USoundBase> Slam(
		TEXT("/Game/Assets/Sound/Clown/SFX_Clown_WeaponSlam.SFX_Clown_WeaponSlam"));
	Melee.SwingSounds.Add(EBossClownAttack::HammerSweep, {Sweep.Object});
	Melee.SwingSounds.Add(EBossClownAttack::HammerSlam, {Slam.Object, 0.95f});
	Melee.SwingSounds.Add(EBossClownAttack::FastChaseSlam, {Slam.Object, 0.95f, 1.08f});
	Melee.SwingSounds.Add(EBossClownAttack::DashSweep, {Sweep.Object, 0.9f, 1.12f});

	static ConstructorHelpers::FObjectFinder<USoundBase> GroundImpact(
		TEXT("/Game/Assets/Sound/Attack/RockSmash01.RockSmash01"));
	static ConstructorHelpers::FObjectFinder<USoundBase> HeavyGroundImpact(
		TEXT("/Game/Assets/Sound/Attack/RockSmash03.RockSmash03"));
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> Gravel(
		TEXT("/Game/Assets/A_Surface_Footstep/Niagara_FX/ParticleSystems/PSN_Gravel_Surface.PSN_Gravel_Surface"));
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> CeilingDust(
		TEXT("/Game/Assets/A_Surface_Footstep/Niagara_FX/ParticleSystems/PSN_Sand_Surface.PSN_Sand_Surface"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> GroundCrack(
		TEXT("/Game/Blueprints/Enemy/Boss/Clown/VFX/M_ClownGroundCrack.M_ClownGroundCrack"));
	Melee.GroundImpact = {GroundImpact.Object, 0.75f, 1.0f, Gravel.Object, 2.6f};
	Melee.HeavyGroundImpact = {HeavyGroundImpact.Object, 0.95f, 0.9f, Gravel.Object, 4.0f};
	Melee.CeilingDust = {CeilingDust.Object, 2.8f};
	Melee.GroundCrackMaterial = GroundCrack.Object;

	static ConstructorHelpers::FObjectFinder<USoundBase> BallRelease(
		TEXT("/Game/Assets/Sound/Clown/SFX_Clown_BallRelease.SFX_Clown_BallRelease"));
	static ConstructorHelpers::FObjectFinder<USoundBase> BallGroundImpact(
		TEXT("/Game/Assets/Sound/Clown/SFX_Clown_StoneImpact.SFX_Clown_StoneImpact"));
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> BallWind(
		TEXT("/Game/Assets/BlinkAndDashVFX/VFX_Niagara/NS_Dash_Wind.NS_Dash_Wind"));
	Balls.LaunchEffects = {BallRelease.Object, 0.68f};
	Balls.GroundImpact = {BallGroundImpact.Object, 0.56f, 1.1f, Gravel.Object, 1.45f};
	Balls.SpinEffect = BallWind.Object;

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> ClapWaveVisual(
		TEXT("/Game/Assets/Free_Magic/VFX_Niagara/NS_Free_Magic_Attack2.NS_Free_Magic_Attack2"));
	Clap.WaveEffects = {HeavyGroundImpact.Object, 0.75f, 0.82f, ClapWaveVisual.Object, 0.7f};

	static ConstructorHelpers::FObjectFinder<USoundBase> CardSpawn(
		TEXT("/Game/Assets/FreeGameSoundsVol1/SFX/Sword_Swing_Fast_Light/Sword_Swing_Fast_Light_01.Sword_Swing_Fast_Light_01"));
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> CardSpawnVisual(
		TEXT("/Game/Assets/Free_Magic/VFX_Niagara/NS_Free_Magic_Circle1.NS_Free_Magic_Circle1"));
	Cards.SpawnEffects = {CardSpawn.Object, 0.65f, 1.0f, CardSpawnVisual.Object, 0.65f};

	static ConstructorHelpers::FObjectFinder<USoundBase> CubeBarrage(
		TEXT("/Game/Assets/Sound/Attack/RockSmash04.RockSmash04"));
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> CubeBarrageVisual(
		TEXT("/Game/Assets/A_Surface_Footstep/Niagara_FX/ParticleSystems/PSN_Sparks_Surface.PSN_Sparks_Surface"));
	Cubes.AssemblyEffects = {CubeBarrage.Object, 0.88f, 0.78f, CubeBarrageVisual.Object, 1.3f};

	static ConstructorHelpers::FObjectFinder<USoundBase> BowlTap(
		TEXT("/Game/Assets/Sound/Clown/SFX_PingPong_1.SFX_PingPong_1"));
	Yabawi.CoverSound = {BowlTap.Object, 0.45f, 0.85f};

	static ConstructorHelpers::FObjectFinder<USoundBase> YabawiFailureSound(
		TEXT("/Game/Assets/FreeGameSoundsVol1/SFX/Pickaxe_Strike_Rock/Pickaxe_Strike_Rock_01.Pickaxe_Strike_Rock_01"));

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> YabawiFailureVisual(
		TEXT("/Game/Assets/Free_Magic/VFX_Niagara/NS_Free_Magic_Area2.NS_Free_Magic_Area2"));

	Yabawi.FailureEffects.Sound = YabawiFailureSound.Object;
	Yabawi.FailureEffects.Volume = 0.95f;
	Yabawi.FailureEffects.Pitch = 0.55f;
	Yabawi.FailureEffects.NiagaraSystem = YabawiFailureVisual.Object;
	Yabawi.FailureEffects.NiagaraScale = 2.3f;

	struct FFractureAssetPath
	{
		const TCHAR* Shape;
		const TCHAR* Path;
	};

	static const FFractureAssetPath FractureAssetPaths[] =
	{
		{TEXT("bowl"), TEXT("/Game/Blueprints/Enemy/Boss/Clown/Fracture/GC_ClownBowl.GC_ClownBowl")},
		{TEXT("SM_ClownBeachBallVivid"), TEXT("/Game/Blueprints/Enemy/Boss/Clown/Fracture/GC_ClownBall.GC_ClownBall")},
		{TEXT("SM_ClownCard"), TEXT("/Game/Blueprints/Enemy/Boss/Clown/Fracture/GC_ClownCard.GC_ClownCard")},
		{TEXT("IntroCard"), TEXT("/Game/Blueprints/Enemy/Boss/Clown/Fracture/GC_ClownIntroCard.GC_ClownIntroCard")},
		{TEXT("IntroCube"), TEXT("/Game/Blueprints/Enemy/Boss/Clown/Fracture/GC_ClownIntroCube.GC_ClownIntroCube")},
		{TEXT("SM_Crate"), TEXT("/Game/Blueprints/Enemy/Boss/Clown/Fracture/GC_ClownCrate.GC_ClownCrate")},
		{TEXT("SKM_CanLowGrade"), TEXT("/Game/Blueprints/Enemy/Boss/Clown/Fracture/GC_ClownCan.GC_ClownCan")},
	};

	for (const FFractureAssetPath& Asset : FractureAssetPaths)
	{
		const ConstructorHelpers::FObjectFinder<UGeometryCollection> Collection(Asset.Path);
		Fractures.Add(FName(Asset.Shape), Collection.Object);
	}

	for (int32 Index = 0; Index < 27; ++Index)
	{
		const FString Suffix = FString::Printf(TEXT("%d%d%d"), Index / 9, Index / 3 % 3, Index % 3);
		const FString Path = FString::Printf(
			TEXT("/Game/Blueprints/Enemy/Boss/Clown/Fracture/CubePieces/GC_ClownCube_%s.GC_ClownCube_%s"), *Suffix, *Suffix);
		const ConstructorHelpers::FObjectFinder<UGeometryCollection> Collection(*Path);
		Fractures.Add(FName(*(TEXT("SM_ClownCube_") + Suffix)), Collection.Object);
	}

	static ConstructorHelpers::FObjectFinder<UChaosCacheCollection> CacheCollection(TEXT(
		"/Game/Blueprints/Enemy/Boss/Clown/Fracture/CC_ClownFractures.CC_ClownFractures"));
	FractureCaches = CacheCollection.Object;
}
