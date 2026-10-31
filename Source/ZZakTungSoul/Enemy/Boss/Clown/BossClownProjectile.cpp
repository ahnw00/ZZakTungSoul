#include <Enemy/Boss/Clown/BossClownProjectile.h>
#include <Enemy/Boss/Clown/BossClown.h>
#include <Components/StaticMeshComponent.h>
#include <Engine/StaticMesh.h>
#include <Engine/World.h>
#include <Interface/CombatDamageInterface.h>
#include <UObject/ConstructorHelpers.h>

ABossClownProjectile::ABossClownProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
	SetRootComponent(VisualMesh);
	VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	VisualMesh->SetCastShadow(false);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Mesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (Mesh.Succeeded())
	{
		VisualMesh->SetStaticMesh(Mesh.Object);
	}
	SetLifeSpan(8.0f);
}

void ABossClownProjectile::Initialize(const FDamageInfo& InDamage, float InSpeed, bool bInShockwave, UMaterialInterface* Material)
{
	Damage = InDamage;
	Speed = FMath::Max(InSpeed, 1.0f);
	bShockwave = bInShockwave;
	VisualMesh->SetRelativeScale3D(bShockwave ? FVector(0.5f, 3.6f, 2.2f) : FVector(0.44f));
	if (Material)
	{
		VisualMesh->SetMaterial(0, Material);
	}
}

void ABossClownProjectile::LaunchAt(const FVector& TargetLocation)
{
	FlightDirection = (TargetLocation - GetActorLocation()).GetSafeNormal();
	if (FlightDirection.IsNearlyZero())
	{
		FlightDirection = GetActorForwardVector();
	}
	bLaunched = true;
}

void ABossClownProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 사망, 재도전과 페이즈 전환 후 이전 공격이 남지 않도록 소유자 상태 확인.
	ABossClown* Boss = Cast<ABossClown>(GetOwner());
	if (!IsValid(Boss) || Boss->GetBossPhase() != EBossPhase::Phase1)
	{
		Destroy();
		return;
	}
	if (!bLaunched)
	{
		return;
	}

	const FVector Start = GetActorLocation();
	const FVector End = Start + FlightDirection * Speed * DeltaTime;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(ClownProjectile), false, this);
	Params.AddIgnoredActor(Boss);
	TArray<FHitResult> Hits;
	const FCollisionShape Shape = bShockwave ? FCollisionShape::MakeBox(FVector(25.0f, 180.0f, 110.0f)) : FCollisionShape::MakeSphere(22.0f);
	GetWorld()->SweepMultiByChannel(Hits, Start, End, GetActorQuat(), ECC_Pawn, Shape, Params);
	for (const FHitResult& Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();
		if (!IsValid(HitActor) || HitActor == Boss || HitActor == this || HitActor->IsA<ABossClownProjectile>())
		{
			continue;
		}
		if (ICombatDamageInterface* Receiver = Cast<ICombatDamageInterface>(HitActor))
		{
			Receiver->ReceiveDamage(Damage, Hit);
			UE_LOG(LogTemp, Log, TEXT("Clown %s hit: %s, Damage %.1f"), bShockwave ? TEXT("clap") : TEXT("ball"), *GetNameSafe(HitActor), Damage.AttackDamage);
			Destroy();
			return;
		}
		if (Hit.bBlockingHit)
		{
			Destroy();
			return;
		}
	}
	SetActorLocation(End);
}
