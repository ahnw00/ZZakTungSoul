#include <Enemy/Boss/Clown/Gimmick/ClownYabawiBowl.h>
#include <Enemy/Boss/Clown/Components/ClownYabawiComponent.h>
#include <Enemy/Boss/Clown/Data/BossClownAnimationData.h>
#include <Enemy/Boss/Clown/BossClown.h>
#include <Enemy/Boss/Clown/Gimmick/ClownFracture.h>
#include <Warrior/Warrior.h>
#include <Components/SceneComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Components/BoxComponent.h>
#include <Components/CapsuleComponent.h>
#include <Engine/StaticMesh.h>

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

	AttackHitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackHitBox"));
	AttackHitBox->SetupAttachment(RootComponent);
	AttackHitBox->SetCollisionObjectType(ECC_WorldDynamic);
	AttackHitBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	AttackHitBox->SetGenerateOverlapEvents(false);
	AttackHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttackHitBox->OnComponentBeginOverlap.AddDynamic(this, &AClownYabawiBowl::OnBodyOverlap);
}

void AClownYabawiBowl::Initialize(UClownYabawiComponent* Round, int32 InRoundID, const FClownPhaseOnePattern& Settings)
{
	YabawiRound = Round;
	RoundID = InRoundID;
	const float Diameter = FMath::Max(Settings.BowlDiameter, 100.0f);

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
	BallRestLocation = FVector(0.0f, 0.0f, BallSize * 0.5f) - BallBounds.Origin * BallScale;
	BallMesh->SetRelativeLocation(BallRestLocation);

	const float HitHalfHeight = FMath::Max(Settings.BowlHitHeight * 0.5f, HalfHeight);
	AttackHitBox->SetBoxExtent(FVector(Diameter * 0.5f, Diameter * 0.5f, HitHalfHeight));
	AttackHitBox->SetRelativeLocation(FVector(0.0f, 0.0f, HitHalfHeight));

	AttackHitBox->SetCollisionResponseToChannel(Settings.AttackTraceChannel, ECR_Overlap);
	AttackHitBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	SetCoverAlpha(0.0f);
	ShowBall(false);
}

void AClownYabawiBowl::SetAttackSelectionEnabled(bool bEnabled)
{
	AttackHitBox->SetGenerateOverlapEvents(bEnabled);
	AttackHitBox->SetCollisionEnabled(bEnabled ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
}

void AClownYabawiBowl::OnBodyOverlap(UPrimitiveComponent* Trigger, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
	int32 BodyIndex, bool bFromSweep, const FHitResult& Sweep)
{
	AWarrior* Player = Cast<AWarrior>(OtherActor);
	UClownYabawiComponent* Round = YabawiRound.Get();
	if (!Round || !Player || Player->GetCapsuleComponent() != OtherComponent)
	{
		return;
	}

	FHitResult Contact = bFromSweep ? Sweep : FHitResult(this, AttackHitBox, AttackHitBox->GetComponentLocation(), FVector::UpVector);
	Contact.TraceStart = Player->GetActorLocation();
	Round->SelectBowl(Player, this, RoundID, Contact);
}

void AClownYabawiBowl::BreakShell(const FHitResult& Hit)
{
	if (const ABossClown* Boss = Cast<ABossClown>(GetOwner()))
	{
		ClownFracture::Spawn(*Boss, *BowlMesh, Hit.ImpactPoint, ClownFracture::PatternDebrisLifeSpan);
	}
}

bool AClownYabawiBowl::ReceivePlayerAttack(AActor* Player, const FHitResult& Hit)
{
	UClownYabawiComponent* Round = YabawiRound.Get();
	return Hit.GetActor() == this && Hit.GetComponent() == AttackHitBox.Get() &&
		Round && Round->SelectBowl(Player, this, RoundID, Hit);
}

void AClownYabawiBowl::OnBowlStruck_Implementation(const FHitResult& Hit)
{
	SetAttackSelectionEnabled(false);
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

void AClownYabawiBowl::UpdateAnswerReveal(float OpenAlpha, float ElapsedTime)
{
	const float LiftAlpha = FMath::SmoothStep(0.0f, 1.0f, OpenAlpha);
	const float FloatHeight = (58.0f + 5.0f * FMath::Sin(ElapsedTime * 5.0f)) * LiftAlpha;

	SetCoverAlpha(1.0f - LiftAlpha);
	BallMesh->SetRelativeLocation(BallRestLocation + FVector(0.0f, 0.0f, FloatHeight));
	ShowBall(OpenAlpha >= 0.4f);
}
