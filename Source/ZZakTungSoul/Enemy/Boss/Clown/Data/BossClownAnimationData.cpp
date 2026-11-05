#include <Enemy/Boss/Clown/Data/BossClownAnimationData.h>
#include <Engine/StaticMesh.h>
#include <Materials/MaterialInterface.h>
#include <UObject/ConstructorHelpers.h>

UBossClownAnimationData::UBossClownAnimationData()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Bowl(
		TEXT("/Game/Assets/Lowpoly_Bowl_3D_Model_for_Architecture_and_Kitchen/bowl/StaticMeshes/bowl.bowl"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Ball(
		TEXT("/Game/Blueprints/Enemy/Boss/Clown/Projectiles/SM_ClownBeachBallVivid.SM_ClownBeachBallVivid"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> Explosion(
		TEXT("/Game/Blueprints/Enemy/Boss/Clown/VFX/M_ClownClap.M_ClownClap"));
	FClownPhaseOnePattern& Yabawi = PhaseOnePatterns.AddDefaulted_GetRef();
	Yabawi.AttackType = EBossClownAttack::Yabawi;
	Yabawi.bAllowInPhase1 = false;
	Yabawi.bAllowInPhase2 = true;
	Yabawi.BowlMesh = Bowl.Object;
	Yabawi.BallMesh = Ball.Object;
	Yabawi.ExplosionMaterial = Explosion.Object;
	Yabawi.ExplosionDamage.AttackDamage = 80.0f;
	Yabawi.ExplosionDamage.bUnblockable = true;
}

void UBossClownAnimationData::PostLoad()
{
	Super::PostLoad();
	// Existing assets serialize their own pattern array, replacing constructor defaults.
	// Append only the missing Yabawi entry; never replace the user's other attacks.
	if (!PhaseOnePatterns.ContainsByPredicate(
			[](const FClownPhaseOnePattern& Pattern)
			{
				return Pattern.AttackType == EBossClownAttack::Yabawi;
			}))
	{
		const FClownPhaseOnePattern* Defaults = GetDefault<UBossClownAnimationData>()->PhaseOnePatterns.FindByPredicate(
			[](const FClownPhaseOnePattern& Pattern)
			{
				return Pattern.AttackType == EBossClownAttack::Yabawi;
			});

		if (Defaults)
		{
			PhaseOnePatterns.Add(*Defaults);
		}
	}
}
