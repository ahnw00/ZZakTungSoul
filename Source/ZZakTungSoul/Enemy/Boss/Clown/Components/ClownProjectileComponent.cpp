#include <Enemy/Boss/Clown/Components/ClownProjectileComponent.h>
#include <Enemy/Boss/Clown/BossClown.h>
#include <Enemy/Components/AttackTraceComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Interface/CombatDamageInterface.h>
#include <Engine/StaticMesh.h>
#include <Engine/Texture2D.h>
#include <Materials/MaterialInstanceDynamic.h>
#include <UObject/ConstructorHelpers.h>

UClownProjectileComponent::UClownProjectileComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.TickGroup = TG_PrePhysics;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshRef(TEXT("/Engine/BasicShapes/Plane.Plane"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialRef(TEXT("/Engine/EngineMaterials/EmissiveMeshMaterial.EmissiveMeshMaterial"));
	WarningMesh = MeshRef.Object;
	WarningMaterial = MaterialRef.Object;
}

void UClownProjectileComponent::BeginPlay()
{
	Super::BeginPlay();
	AActor* Owner = GetOwner();
	UStaticMeshComponent* Mesh = Owner ? Owner->FindComponentByClass<UStaticMeshComponent>() : nullptr;
	if (!Owner || !Mesh)
	{
		return;
	}
	VisualMesh = Mesh;
	TArray<UAttackTraceComponent*> Traces;
	Owner->GetComponents<UAttackTraceComponent>(Traces);
	for (UAttackTraceComponent* Trace : Traces)
	{
		Trace->SetTraceMesh(Mesh);
		Trace->OnTraceHit.AddUObject(this, &UClownProjectileComponent::HandleTraceHit);
		Trace->AddTickPrerequisiteComponent(this);
		AttackTraces.Add(Trace);
	}
	if (Traces.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Clown projectile requires AttackTraceComponent and mesh trace sockets."));
	}
}

void UClownProjectileComponent::Initialize(const FDamageInfo& InDamage, float InSpeed, float InLifeTime)
{
	Damage = InDamage;
	Speed = FMath::Max(InSpeed, 1.0f);
	LifeTime = FMath::Max(InLifeTime, 0.1f);
	bLaunched = false;
	bCardHazard = false;
	bCubeMarch = false;
	bCardActive = false;
	SetComponentTickEnabled(false);
	SetTracing(false);
	HitActors.Reset();
	if (GetOwner())
	{
		GetOwner()->SetLifeSpan(0.0f);
	}
}

void UClownProjectileComponent::SetTracing(bool bEnabled)
{
	for (const TWeakObjectPtr<UAttackTraceComponent>& Trace : AttackTraces)
	{
		if (!Trace.IsValid())
		{
			continue;
		}
		if (bEnabled)
		{
			Trace->BeginTrace();
		}
		else
		{
			Trace->EndTrace();
		}
	}
}

void UClownProjectileComponent::ArmCard(const FVector& GroundLocation, float WarningTime, float Height, float Width, float FallTime, UTexture2D* Face, bool bSafe)
{
	AActor* Owner = GetOwner();
	UStaticMeshComponent* Mesh = VisualMesh.Get();
	if (!Owner || !Mesh || !Mesh->GetStaticMesh() || AttackTraces.IsEmpty() || !WarningMesh || !WarningMaterial || !Face)
	{
		if (Owner)
		{
			Owner->Destroy();
		}
		return;
	}

	// 액터 원점을 카드 밑변으로 사용. X 방향으로 넘어지고 Y가 카드 폭.
	const FBoxSphereBounds Bounds = Mesh->GetStaticMesh()->GetBounds();
	const FVector Size = Bounds.BoxExtent * 2.0f;
	CardTravel = FMath::Max(Height, 100.0f);

	const FVector Scale(10.0f / FMath::Max(Size.X, 1.0f), Width / FMath::Max(Size.Y, 1.0f), CardTravel / FMath::Max(Size.Z, 1.0f));
	Mesh->SetRelativeScale3D(Scale);
	Mesh->SetRelativeRotation(FRotator::ZeroRotator);
	Mesh->SetRelativeLocation(FVector(0.0f, 0.0f, CardTravel * 0.5f) - Bounds.Origin * Scale);
	UMaterialInstanceDynamic* FaceMaterial = Mesh->CreateAndSetMaterialInstanceDynamic(0);
	if (FaceMaterial)
	{
		FaceMaterial->SetTextureParameterValue(TEXT("CardFace"), Face);
	}
	CardOrigin = GroundLocation + FVector(0.0f, 0.0f, 6.0f);
	CardYaw = Owner->GetActorRotation().Yaw;
	CardWarningTime = FMath::Max(WarningTime, 1.5f);
	CardFallTime = FMath::Max(FallTime, 0.3f);
	CardElapsed = 0.0f;
	bCardHazard = true;
	bSafeCard = bSafe;
	bLaunched = true;
	Mesh->SetCollisionObjectType(ECC_WorldDynamic);
	Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Mesh->SetCanEverAffectNavigation(false);
	Mesh->SetVisibility(true, true);
	Owner->SetActorLocation(CardOrigin - FVector(0.0f, 0.0f, CardTravel));

	// 넘어지는 전체 길이 / 폭을 표시. 하트 카드는 안전한 녹색 통로.
	WarningMarker = NewObject<UStaticMeshComponent>(Owner);
	Owner->AddInstanceComponent(WarningMarker);
	WarningMarker->SetStaticMesh(WarningMesh);
	WarningMarker->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WarningMarker->SetCastShadow(false);
	WarningMarker->RegisterComponent();
	WarningMarker->SetWorldLocation(GroundLocation + Owner->GetActorForwardVector() * (bSafe ? 40.0f : CardTravel * 0.5f) + FVector(0.0f, 0.0f, 3.0f));
	WarningMarker->SetWorldRotation(FRotator(0.0f, CardYaw, 0.0f));
	WarningMarker->SetWorldScale3D(FVector(bSafe ? 1.0f : (CardTravel + 210.0f) / 100.0f, bSafe ? 1.2f : (Width + 210.0f) / 100.0f, 1.0f));
	UMaterialInstanceDynamic* Material = UMaterialInstanceDynamic::Create(WarningMaterial, this);
	Material->SetVectorParameterValue(TEXT("Color"), bSafe ? FLinearColor(0.08f, 1.8f, 0.8f) : FLinearColor(2.0f, 0.28f, 0.04f));
	Material->SetScalarParameterValue(TEXT("Safe"), bSafe ? 1.0f : 0.0f);
	WarningMarker->SetMaterial(0, Material);
	Owner->SetLifeSpan(CardRiseTime + CardWarningTime + CardFallTime + LifeTime + 0.3f);
	SetComponentTickEnabled(true);
}

void UClownProjectileComponent::TickCard(float DeltaTime)
{
	AActor* Owner = GetOwner();
	CardElapsed += DeltaTime;
	const float FallStart = CardRiseTime + CardWarningTime;
	if (CardElapsed < CardRiseTime)
	{
		const float Alpha = FMath::SmoothStep(0.0f, 1.0f, CardElapsed / CardRiseTime);
		Owner->SetActorLocation(CardOrigin - FVector(0.0f, 0.0f, CardTravel * (1.0f - Alpha)));
		return;
	}
	Owner->SetActorLocation(CardOrigin);
	if (CardElapsed < FallStart)
	{
		// 위치와 방향은 이미 고정. 마지막 순간에는 몸을 조금 기울여 낙하 예고.
		const float Lean = FMath::Clamp((CardElapsed - FallStart + 0.3f) / 0.3f, 0.0f, 1.0f);
		Owner->SetActorRotation(FRotator(-4.0f * Lean, CardYaw, 0.0f));
		return;
	}
	const float Alpha = FMath::Clamp((CardElapsed - FallStart) / CardFallTime, 0.0f, 1.0f);
	if (!bCardActive)
	{
		bCardActive = true;
		if (VisualMesh.IsValid())
		{
			VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		// 하트는 0 데미지 전달도 하지 않음. 피격 / 경직 / 판정 자체 없음.
		if (!bSafeCard)
		{
			SetTracing(true);
		}
	}
	Owner->SetActorRotation(FRotator(-FMath::Lerp(4.0f, 90.0f, Alpha * Alpha), CardYaw, 0.0f));
	if (Alpha >= 1.0f)
	{
		SetTracing(false);
		WarningMarker->SetVisibility(false);
		const float Sink = FMath::Clamp((CardElapsed - FallStart - CardFallTime - LifeTime) / 0.3f, 0.0f, 1.0f);
		Owner->SetActorLocation(CardOrigin - FVector(0.0f, 0.0f, 30.0f * Sink));
		if (Sink >= 1.0f)
		{
			Owner->Destroy();
		}
	}
}

void UClownProjectileComponent::SetVisualSpin(const FRotator& Rate)
{
	VisualSpin = Rate;
	if (VisualMesh.IsValid())
	{
		VisualMesh->SetRelativeRotation(FRotator(FMath::FRandRange(-180.0f, 180.0f), FMath::FRandRange(-180.0f, 180.0f), FMath::FRandRange(-180.0f, 180.0f)));
	}
	SetComponentTickEnabled(true);
}

void UClownProjectileComponent::LaunchAt(const FVector& TargetLocation)
{
	AActor* Owner = GetOwner();
	if (!Owner || AttackTraces.IsEmpty() || bLaunched)
	{
		return;
	}
	FlightDirection = (TargetLocation - Owner->GetActorLocation()).GetSafeNormal();
	if (FlightDirection.IsNearlyZero())
	{
		FlightDirection = Owner->GetActorForwardVector();
	}
	Owner->SetActorRotation(FlightDirection.Rotation());
	bLaunched = true;
	Owner->SetLifeSpan(LifeTime);
	SetTracing(true);
	SetComponentTickEnabled(true);
}

void UClownProjectileComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	AActor* Owner = GetOwner();
	ABossClown* Boss = Owner ? Cast<ABossClown>(Owner->GetOwner()) : nullptr;
	if (!Owner || !IsValid(Boss) || Boss->IsDead() || (Boss->GetBossPhase() != EBossPhase::Phase1 && Boss->GetBossPhase() != EBossPhase::Phase2))
	{
		if (Owner)
		{
			Owner->Destroy();
		}
		return;
	}
	if (bCubeMarch)
	{
		TickCubeMarch(DeltaTime);
		return;
	}
	if (bCardHazard)
	{
		TickCard(DeltaTime);
		return;
	}
	if (VisualMesh.IsValid() && !VisualSpin.IsNearlyZero())
	{
		VisualMesh->AddLocalRotation(VisualSpin * DeltaTime);
	}
	if (bLaunched)
	{
		Owner->AddActorWorldOffset(FlightDirection * Speed * DeltaTime, false);
	}
}

void UClownProjectileComponent::HandleTraceHit(const FHitResult& Hit)
{
	AActor* Owner = GetOwner();
	AActor* HitActor = Hit.GetActor();
	if (!bLaunched || (bCardHazard && (!bCardActive || bSafeCard)) || !Owner || !IsValid(HitActor) || HitActor == Owner->GetOwner() || HitActor->FindComponentByClass<UClownProjectileComponent>())
	{
		return;
	}

	if (HitActors.Contains(HitActor))
	{
		return;
	}

	// 기존 Enemy와 동일하게 트레이스 결과를 받아 피해 인터페이스로 전달.
	if (ICombatDamageInterface* Target = Cast<ICombatDamageInterface>(HitActor))
	{
		HitActors.Add(HitActor);
		Target->ReceiveDamage(Damage, Hit);
		UE_LOG(LogTemp, Log, TEXT("Clown projectile trace hit: %s, Damage %.1f"), *GetNameSafe(HitActor), Damage.AttackDamage);
		if (!bCardHazard && !bCubeMarch)
		{
			Owner->Destroy();
		}
	}
	else if (Hit.bBlockingHit && !bCardHazard && !bCubeMarch)
	{
		Owner->Destroy();
	}
}

void UClownProjectileComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	SetTracing(false);
	for (const TWeakObjectPtr<UAttackTraceComponent>& Trace : AttackTraces)
	{
		if (!Trace.IsValid())
		{
			continue;
		}
		Trace->OnTraceHit.RemoveAll(this);
		Trace->RemoveTickPrerequisiteComponent(this);
	}
	Super::EndPlay(EndPlayReason);
}

void UClownProjectileComponent::ArmCubeMarch(UStaticMesh* Piece, const FVector& Assembly, const FVector& Start, const FVector& End, float MarchTime, bool bShowPath)
{
	AActor* Owner = GetOwner();
	UStaticMeshComponent* Mesh = VisualMesh.Get();
	if (!Owner || !Mesh || !Piece)
	{
		return;
	}
	Mesh->SetStaticMesh(Piece);
	Mesh->EmptyOverrideMaterials();
	const float Size = Piece->GetBounds().BoxExtent.GetMax() * 2.0f;
	Mesh->SetRelativeScale3D(FVector(94.0f / FMath::Max(Size, 0.01f)));
	Mesh->SetRelativeLocation(FVector::ZeroVector);
	Mesh->SetRelativeRotation(FRotator::ZeroRotator);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CubeAssembly = Assembly;
	CubeStart = Start;
	CubeEnd = End;
	CubeMarchTime = MarchTime;
	CubeElapsed = 0.0f;
	bCubeMarch = true;
	bLaunched = true;
	Owner->SetActorLocation(Assembly);
	Owner->SetLifeSpan(CubeMarchDuration + 0.2f);
	SetComponentTickEnabled(true);
	if (bShowPath && WarningMesh && WarningMaterial)
	{
		WarningMarker = NewObject<UStaticMeshComponent>(Owner);
		Owner->AddInstanceComponent(WarningMarker);
		WarningMarker->SetStaticMesh(WarningMesh);
		WarningMarker->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WarningMarker->SetCastShadow(false);
		WarningMarker->RegisterComponent();
		WarningMarker->SetWorldLocation((Start + End) * 0.5f - FVector(0.0f, 0.0f, 49.0f));
		WarningMarker->SetWorldRotation((End - Start).Rotation());
		WarningMarker->SetWorldScale3D(FVector((FVector::Dist2D(Start, End) + 400.0f) / 100.0f, 1.15f, 1.0f));
		UMaterialInstanceDynamic* Material = UMaterialInstanceDynamic::Create(WarningMaterial, this);
		Material->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.7f, 0.15f, 2.5f));
		Material->SetScalarParameterValue(TEXT("Safe"), 0.0f);
		WarningMarker->SetMaterial(0, Material);
		WarningMarker->SetVisibility(false);
	}
}

void UClownProjectileComponent::TickCubeMarch(float DeltaTime)
{
	CubeElapsed += DeltaTime;
	AActor* Owner = GetOwner();
	const float MoveEnd = CubeMarchTime + 1.2f;
	const bool bMoving = CubeElapsed >= CubeMarchTime && CubeElapsed < MoveEnd;
	// 판정은 고정된 경로를 실제로 이동하는 동안에만 사용.
	if (bMoving != bCubeTracing)
	{
		bCubeTracing = bMoving;
		SetTracing(bMoving);
	}
	if (WarningMarker)
	{
		WarningMarker->SetVisibility(CubeElapsed >= CubeMarchTime - 1.0f && CubeElapsed < MoveEnd);
	}
	FVector Position = CubeAssembly;
	if (CubeElapsed >= 8.6f)
	{
		Position = FMath::Lerp(CubeEnd, CubeAssembly, FMath::SmoothStep(0.0f, 1.0f, (CubeElapsed - 8.6f) / 0.8f));
	}
	else if (CubeElapsed >= MoveEnd)
	{
		Position = CubeEnd;
	}
	else if (CubeElapsed >= CubeMarchTime)
	{
		Position = FMath::Lerp(CubeStart, CubeEnd, FMath::SmoothStep(0.0f, 1.0f, (CubeElapsed - CubeMarchTime) / 1.2f));
		if (VisualMesh.IsValid())
		{
			VisualMesh->AddLocalRotation(FRotator(0.0f, 80.0f * DeltaTime, 0.0f));
		}
	}
	else if (CubeElapsed >= CubeMarchTime - 1.0f)
	{
		Position = CubeStart;
	}
	else if (CubeElapsed >= CubeMarchTime - 1.8f)
	{
		Position = FMath::Lerp(CubeAssembly, CubeStart, FMath::SmoothStep(0.0f, 1.0f, (CubeElapsed - CubeMarchTime + 1.8f) / 0.8f));
	}
	Owner->SetActorLocation(Position);
	if (VisualMesh.IsValid() && CubeElapsed >= 8.6f)
	{
		VisualMesh->SetRelativeRotation(FMath::RInterpTo(VisualMesh->GetRelativeRotation(), FRotator::ZeroRotator, DeltaTime, 10.0f));
	}
}
