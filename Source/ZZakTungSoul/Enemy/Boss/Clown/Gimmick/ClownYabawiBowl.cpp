#include <Enemy/Boss/Clown/Gimmick/ClownYabawiBowl.h>
#include <Enemy/Boss/Clown/Components/ClownYabawiComponent.h>
#include <Enemy/Boss/Clown/Data/BossClownAnimationData.h>
#include <Components/SceneComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Components/BoxComponent.h>
#include <Engine/StaticMesh.h>
#include <Materials/MaterialInstanceDynamic.h>
#include <UObject/ConstructorHelpers.h>

AClownYabawiBowl::AClownYabawiBowl()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	BowlMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bowl"));
	BowlMesh->SetupAttachment(RootComponent);
	BowlMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HiddenBall"));
	BallMesh->SetupAttachment(RootComponent);
	BallMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Explosion = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Explosion"));
	Explosion->SetupAttachment(RootComponent);
	Explosion->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Explosion->SetCastShadow(false);
	Explosion->SetVisibility(false);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Plane(TEXT("/Engine/BasicShapes/Plane.Plane"));
	Explosion->SetStaticMesh(Plane.Object);
	AttackHitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackHitBox"));
	AttackHitBox->SetupAttachment(RootComponent);
	AttackHitBox->SetCollisionObjectType(ECC_WorldDynamic);
	AttackHitBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	AttackHitBox->SetGenerateOverlapEvents(false);
	AttackHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AClownYabawiBowl::Initialize(UClownYabawiComponent* Round, int32 InRoundID, const FClownPhaseOnePattern& Settings)
{
	YabawiRound = Round;
	RoundID = InRoundID;
	Diameter = FMath::Max(Settings.BowlDiameter, 100.0f);
	BowlMesh->SetStaticMesh(Settings.BowlMesh);
	const FBoxSphereBounds Bounds = Settings.BowlMesh->GetBounds();
	const float Scale = Diameter / FMath::Max(FMath::Max(Bounds.BoxExtent.X, Bounds.BoxExtent.Y) * 2.0f, 0.01f);
	BowlMesh->SetRelativeScale3D(FVector(Scale));
	MeshCenter = Bounds.Origin * Scale;
	HalfHeight = Bounds.BoxExtent.Z * Scale;
	BallMesh->SetStaticMesh(Settings.BallMesh);
	const float BallSize = Diameter * 0.23f;
	const FBoxSphereBounds BallBounds = Settings.BallMesh->GetBounds();
	const float BallScale = BallSize / FMath::Max(BallBounds.BoxExtent.GetMax() * 2.0f, 0.01f);
	BallMesh->SetRelativeScale3D(FVector(BallScale));
	BallMesh->SetRelativeLocation(FVector(0.0f, 0.0f, BallSize * 0.5f) - BallBounds.Origin * BallScale);
	const float HitHalfHeight = FMath::Max(Settings.BowlHitHeight * 0.5f, HalfHeight);
	AttackHitBox->SetBoxExtent(FVector(Diameter * 0.5f, Diameter * 0.5f, HitHalfHeight));
	AttackHitBox->SetRelativeLocation(FVector(0.0f, 0.0f, HitHalfHeight));
	// Overlap response lets weapons query the bowl without blocking character movement.
	AttackHitBox->SetCollisionResponseToChannel(Settings.AttackTraceChannel, ECR_Overlap);

	if (Settings.ExplosionMaterial)
	{
		UMaterialInstanceDynamic* Material = UMaterialInstanceDynamic::Create(Settings.ExplosionMaterial, this);
		Material->SetVectorParameterValue(TEXT("Color"), FLinearColor(8.0f, 0.4f, 0.02f));
		Explosion->SetMaterial(0, Material);
	}

	SetCoverAlpha(0.0f);
	ShowBall(false);
}

void AClownYabawiBowl::SetAttackSelectionEnabled(bool bEnabled)
{
	AttackHitBox->SetCollisionEnabled(bEnabled ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
}

bool AClownYabawiBowl::ReceivePlayerAttack(AActor* Player, const FHitResult& Hit)
{
	if (Hit.GetActor() != this || Hit.GetComponent() != AttackHitBox.Get() || !YabawiRound.IsValid() ||
		!YabawiRound->SelectBowl(Player, this, RoundID))
	{
		return false;
	}

	if (!IsActorBeingDestroyed())
	{
		OnBowlStruck(Hit);
	}

	return true;
}

void AClownYabawiBowl::OnBowlStruck_Implementation(const FHitResult& Hit)
{
	SetAttackSelectionEnabled(false);
	BowlMesh->SetVisibility(false);
}

void AClownYabawiBowl::SetCoverAlpha(float Alpha)
{
	const FRotator Rotation(180.0f * Alpha, 0.0f, 0.0f);
	BowlMesh->SetRelativeRotation(Rotation);
	BowlMesh->SetRelativeLocation(
		FVector(0.0f, 0.0f, HalfHeight + (1.0f - Alpha) * 150.0f) - Rotation.RotateVector(MeshCenter));
}

void AClownYabawiBowl::ShowBall(bool bVisible)
{
	BallMesh->SetVisibility(bVisible);
}

void AClownYabawiBowl::ShowExplosion(float Radius, float Alpha)
{
	BowlMesh->SetVisibility(false);
	ShowBall(false);
	SetAttackSelectionEnabled(false);
	Explosion->SetVisibility(true);
	Explosion->SetRelativeLocation(FVector(0.0f, 0.0f, 10.0f));
	const float Scale = Radius * 2.0f / 100.0f * FMath::Clamp(Alpha * 3.0f, 0.05f, 1.0f);
	Explosion->SetRelativeScale3D(FVector(Scale, Scale, 1.0f));
}
