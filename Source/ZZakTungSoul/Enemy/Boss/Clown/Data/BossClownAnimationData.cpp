#include <Enemy/Boss/Clown/Data/BossClownAnimationData.h>
#include <Animation/AnimMontage.h>
#include <Engine/StaticMesh.h>
#include <Engine/World.h>
#include <GeometryCollection/GeometryCollectionObject.h>
#include <Chaos/CacheCollection.h>
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

	static ConstructorHelpers::FObjectFinder<USoundBase> Sweep(
		TEXT("/Game/Assets/Sound/Clown/SFX_Clown_WeaponSweep.SFX_Clown_WeaponSweep"));

	static ConstructorHelpers::FObjectFinder<USoundBase> Slam(
		TEXT("/Game/Assets/Sound/Clown/SFX_Clown_WeaponSlam.SFX_Clown_WeaponSlam"));

	static ConstructorHelpers::FObjectFinder<USoundBase> BallRelease(
		TEXT("/Game/Assets/Sound/Clown/SFX_Clown_BallRelease.SFX_Clown_BallRelease"));

	static ConstructorHelpers::FObjectFinder<USoundBase> ClapWave(
		TEXT("/Game/Assets/FreeGameSoundsVol1/SFX/Spell_Fireball/Spell_Fireball_01.Spell_Fireball_01"));

	static ConstructorHelpers::FObjectFinder<USoundBase> CardSpawn(
		TEXT("/Game/Assets/FreeGameSoundsVol1/SFX/Spell_Ice_Spear/Spell_Ice_Spear_01.Spell_Ice_Spear_01"));

	static ConstructorHelpers::FObjectFinder<USoundBase> CubeBarrage(
		TEXT("/Game/Assets/FreeGameSoundsVol1/SFX/Forge_Hammer_On_Anvil_Medium/Forge_Hammer_On_Anvil_Medium_01.Forge_Hammer_On_Anvil_Medium_01"));

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> ClapWaveVisual(
		TEXT("/Game/Assets/Free_Magic/VFX_Niagara/NS_Free_Magic_Attack2.NS_Free_Magic_Attack2"));

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> CardSpawnVisual(
		TEXT("/Game/Assets/Free_Magic/VFX_Niagara/NS_Free_Magic_Circle1.NS_Free_Magic_Circle1"));

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> CubeBarrageVisual(
		TEXT("/Game/Assets/Free_Magic/VFX_Niagara/NS_Free_Magic_Circle2.NS_Free_Magic_Circle2"));

	PatternEffects.Add(EBossClownAttack::HammerSweep, {Sweep.Object});
	PatternEffects.Add(EBossClownAttack::HammerSlam, {Slam.Object, 0.95f});
	PatternEffects.Add(EBossClownAttack::FastChaseSlam, {Slam.Object, 0.95f, 1.08f});
	PatternEffects.Add(EBossClownAttack::DashSweep, {Sweep.Object, 0.9f, 1.12f});
	PatternEffects.Add(EBossClownAttack::TripleBalls, {BallRelease.Object, 0.68f});
	PatternEffects.Add(EBossClownAttack::ClapWave, {ClapWave.Object, 0.75f, 1.0f, ClapWaveVisual.Object, 0.7f});
	PatternEffects.Add(EBossClownAttack::SpawnCards, {CardSpawn.Object, 0.65f, 1.0f, CardSpawnVisual.Object, 0.65f});
	PatternEffects.Add(EBossClownAttack::CubeBarrage, {CubeBarrage.Object, 0.7f, 1.0f, CubeBarrageVisual.Object, 0.8f});

	const ConstructorHelpers::FObjectFinder<USoundBase> BowlTap(
		TEXT("/Game/Assets/Sound/Clown/SFX_PingPong_1.SFX_PingPong_1"));

	FClownPatternEffects& BowlCover = PatternEffects.Add(EBossClownAttack::Yabawi);

	BowlCover.Sound = BowlTap.Object;
	BowlCover.Volume = 0.45f;
	BowlCover.Pitch = 0.85f;

	const ConstructorHelpers::FObjectFinder<UNiagaraSystem> Gravel(
		TEXT("/Game/Assets/A_Surface_Footstep/Niagara_FX/ParticleSystems/PSN_Gravel_Surface.PSN_Gravel_Surface"));

	static ConstructorHelpers::FObjectFinder<USoundBase> StoneImpact(
		TEXT("/Game/Assets/Sound/Clown/SFX_Clown_StoneImpact.SFX_Clown_StoneImpact"));

	MeleeGroundEffects.Sound = StoneImpact.Object;
	MeleeGroundEffects.Volume = 0.95f;
	MeleeGroundEffects.NiagaraSystem = Gravel.Object;
	MeleeGroundEffects.NiagaraScale = 4.5f;

	const ConstructorHelpers::FObjectFinder<UNiagaraSystem> SlamDust(
		TEXT("/Game/Assets/A_Surface_Footstep/Niagara_FX/ParticleSystems/PSN_Dirt_Surface.PSN_Dirt_Surface"));

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

	static ConstructorHelpers::FObjectFinder<USoundBase> YabawiFailureSound(
		TEXT("/Game/Assets/FreeGameSoundsVol1/SFX/Pickaxe_Strike_Rock/Pickaxe_Strike_Rock_01.Pickaxe_Strike_Rock_01"));

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> YabawiFailureVisual(
		TEXT("/Game/Assets/Free_Magic/VFX_Niagara/NS_Free_Magic_Area2.NS_Free_Magic_Area2"));

	YabawiFailureEffects.Sound = YabawiFailureSound.Object;
	YabawiFailureEffects.Volume = 0.95f;
	YabawiFailureEffects.Pitch = 0.55f;
	YabawiFailureEffects.NiagaraSystem = YabawiFailureVisual.Object;
	YabawiFailureEffects.NiagaraScale = 2.3f;

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
