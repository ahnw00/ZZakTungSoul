#include <Enemy/Boss/Clown/Data/BossClownAnimationData.h>
#include <Engine/StaticMesh.h>
#include <Materials/MaterialInterface.h>
#include <UObject/ConstructorHelpers.h>

UBossClownAnimationData::UBossClownAnimationData()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Bowl(TEXT("/Game/Assets/Lowpoly_Bowl_3D_Model_for_Architecture_and_Kitchen/bowl/StaticMeshes/bowl.bowl"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Ball(TEXT("/Game/Blueprints/Enemy/Boss/Clown/Projectiles/SM_ClownBeachBallVivid.SM_ClownBeachBallVivid"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> Explosion(TEXT("/Game/Blueprints/Enemy/Boss/Clown/VFX/M_ClownClap.M_ClownClap"));
	Yabawi.BowlMesh=Bowl.Object;
	Yabawi.BallMesh=Ball.Object;
	Yabawi.ExplosionMaterial=Explosion.Object;
	Yabawi.ExplosionDamage.AttackDamage=80.0f;
	Yabawi.ExplosionDamage.bUnblockable=true;
}
