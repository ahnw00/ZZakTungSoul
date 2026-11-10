#include <Enemy/Boss/Clown/Components/ClownProjectileComponent.h>
#include <Enemy/Boss/Clown/BossClown.h>
#include <Enemy/Boss/Clown/Gimmick/ClownFracture.h>
#include <Enemy/Components/AttackTraceComponent.h>
#include <Warrior/Warrior.h>
#include <Components/StaticMeshComponent.h>
#include <Components/BoxComponent.h>
#include <Components/CapsuleComponent.h>
#include <Interface/CombatDamageInterface.h>
#include <Engine/StaticMesh.h>
#include <Engine/Texture2D.h>
#include <Materials/MaterialInstanceDynamic.h>
#include <NiagaraComponent.h>
#include <NiagaraFunctionLibrary.h>
#include <UObject/ConstructorHelpers.h>

UClownProjectileComponent::UClownProjectileComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.TickGroup = TG_PrePhysics;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshRef(TEXT("/Engine/BasicShapes/Plane.Plane"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialRef(
		TEXT("/Engine/EngineMaterials/EmissiveMeshMaterial.EmissiveMeshMaterial"));
	WarningMesh = MeshRef.Object;
	WarningMaterial = MaterialRef.Object;

	// Fab에서 Assets로 이동한 원본 머티리얼과 텍스처를 직접 참조.
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> BallMaterialRef(
		TEXT("/Game/Assets/Beach_Ball_-_Low_Poly/beach_ball_low_poly/Materials/BeachBall.BeachBall"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> CubeMaterialRef(
		TEXT("/Game/Assets/CC0_-_Magic_Cube/cc0_magic_cube/Materials/MagicCube.MagicCube"));
	BallMaterial = BallMaterialRef.Object;
	CubeMaterial = CubeMaterialRef.Object;

	for (int32 Index = 0; Index < 3; ++Index)
	{
		const FString BallPath = FString::Printf( TEXT( "/Game/Assets/Beach_Ball_-_Low_Poly/beach_ball_low_poly/Textures/beach_ball_low_poly_texture_%d.beach_ball_low_poly_texture_%d"), Index, Index);
		const FString CubePath = FString::Printf( TEXT( "/Game/Assets/CC0_-_Magic_Cube/cc0_magic_cube/Textures/cc0_magic_cube_texture_%d.cc0_magic_cube_texture_%d"), Index, Index);
		ConstructorHelpers::FObjectFinder<UTexture2D> BallTextureRef(*BallPath);
		ConstructorHelpers::FObjectFinder<UTexture2D> CubeTextureRef(*CubePath);
		BallTextures.Add(BallTextureRef.Object);
		CubeTextures.Add(CubeTextureRef.Object);
	}
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

	if (Mesh->GetStaticMesh() && Mesh->GetStaticMesh()->GetFName() == TEXT("SM_ClownBeachBallVivid"))
	{
		RestoreProjectileMaterial(Mesh, false);
	}

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

void UClownProjectileComponent::RestoreProjectileMaterial(UStaticMeshComponent* Mesh, bool bCube)
{
	UMaterialInterface* Material = bCube ? CubeMaterial.Get() : BallMaterial.Get();
	const TArray<TObjectPtr<UTexture2D>>& Textures = bCube ? CubeTextures : BallTextures;

	if (!Mesh || !Material || Textures.Num() != 3 || !Textures[0] || !Textures[1] || !Textures[2])
	{
		UE_LOG(LogTemp, Error, TEXT("Clown projectile: relocated %s material or textures are missing."),
			bCube ? TEXT("cube") : TEXT("ball"));

		return;
	}

	UMaterialInstanceDynamic* RestoredMaterial = Mesh->CreateAndSetMaterialInstanceDynamicFromMaterial(0, Material);

	if (!RestoredMaterial)
	{
		return;
	}

	RestoredMaterial->SetTextureParameterValue(TEXT("BaseColorTexture"), Textures[0]);
	RestoredMaterial->SetTextureParameterValue(TEXT("MetallicRoughnessTexture"), Textures[1]);
	RestoredMaterial->SetTextureParameterValue(TEXT("NormalTexture"), Textures[2]);

	if (bCube)
	{
		RestoredMaterial->SetTextureParameterValue(TEXT("OcclusionTexture"), Textures[1]);
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
	bCubeTracing = false;
	bCubeGrounded = false;

	if (CubeCollision)
	{
		CubeCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (CardCollision)
	{
		CardCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

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

void UClownProjectileComponent::ArmCard(const FVector& GroundLocation, float StandTime, float Height, float Width,
	float FallTime, UTexture2D* Face, bool bSafe)
{
	AActor* Owner = GetOwner();
	UStaticMeshComponent* Mesh = VisualMesh.Get();

	if (!Owner || !Mesh || !Mesh->GetStaticMesh() || AttackTraces.IsEmpty() || !Face)
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
	CardWidth = Width;

	const FVector Scale(
		10.0f / FMath::Max(Size.X, 1.0f), Width / FMath::Max(Size.Y, 1.0f), CardTravel / FMath::Max(Size.Z, 1.0f));
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
	CardStandTime = FMath::Max(StandTime, 1.5f);
	CardFallTime = FMath::Max(FallTime, 0.3f);
	CardElapsed = 0.0f;
	bCardHazard = true;
	bSafeCard = bSafe;
	bLaunched = true;
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetCanEverAffectNavigation(false);
	Mesh->SetVisibility(true, true);
	Owner->SetActorLocation(CardOrigin - FVector(0.0f, 0.0f, CardTravel));

	if (!CardCollision)
	{
		CardCollision = NewObject<UBoxComponent>(Owner, TEXT("CardCollision"));
		Owner->AddInstanceComponent(CardCollision);
		CardCollision->SetupAttachment(Owner->GetRootComponent());
		CardCollision->SetCollisionObjectType(ECC_WorldDynamic);
		CardCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
		CardCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
		CardCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CardCollision->SetGenerateOverlapEvents(false);
		CardCollision->SetCanEverAffectNavigation(false);
		CardCollision->CanCharacterStepUpOn = ECB_No;
		CardCollision->RegisterComponent();

		if (ABossClown* Boss = Cast<ABossClown>(Owner->GetOwner()))
		{
			Boss->GetCapsuleComponent()->IgnoreActorWhenMoving(Owner, true);
		}
	}

	CardCollision->SetBoxExtent(FVector(5.0f, Width * 0.5f, CardTravel * 0.5f));
	CardCollision->SetRelativeLocation(FVector(0.0f, 0.0f, CardTravel * 0.5f));
	CardCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	Owner->SetLifeSpan(CardRiseTime + CardStandTime + CardFallTime + LifeTime + 0.3f);
	SetComponentTickEnabled(true);
}

void UClownProjectileComponent::TickCard(float DeltaTime)
{
	AActor* Owner = GetOwner();
	const float PreviousElapsed = CardElapsed;
	CardElapsed += DeltaTime;
	const float FallStart = CardRiseTime + CardStandTime;

	if (CardElapsed < CardRiseTime)
	{
		const float Alpha = FMath::SmoothStep(0.0f, 1.0f, CardElapsed / CardRiseTime);
		Owner->SetActorLocation(CardOrigin - FVector(0.0f, 0.0f, CardTravel * (1.0f - Alpha)));

		return;
	}

	Owner->SetActorLocation(CardOrigin);

	if (PreviousElapsed < CardRiseTime)
	{
		CardCollision->SetAbsolute(true, true, false);
		CardCollision->SetWorldLocationAndRotation(CardOrigin + FVector(0.0f, 0.0f, CardTravel * 0.5f), FRotator(0.0f, CardYaw, 0.0f));
	}

	if (CardElapsed < FallStart)
	{
		// 위치와 방향은 이미 고정. 마지막 순간에는 몸을 조금 기울여 낙하 예고.
		const float Lean = FMath::Clamp((CardElapsed - FallStart + 0.3f) / 0.3f, 0.0f, 1.0f);
		Owner->SetActorRotation(FRotator((bSafeCard ? 4.0f : -4.0f) * Lean, CardYaw, 0.0f));

		return;
	}

	const float Alpha = FMath::Clamp((CardElapsed - FallStart) / CardFallTime, 0.0f, 1.0f);

	if (!bCardActive)
	{
		bCardActive = true;

		CardCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (!bSafeCard)
		{
			SetTracing(true);
		}
	}

	Owner->SetActorRotation(FRotator((bSafeCard ? 1.0f : -1.0f) * FMath::Lerp(4.0f, 90.0f, Alpha * Alpha), CardYaw, 0.0f));

	if (UStaticMeshComponent* Mesh = VisualMesh.Get(); Mesh && Mesh->GetStaticMesh())
	{
		const FVector Extent = Mesh->GetStaticMesh()->GetBounds().BoxExtent * Mesh->GetComponentScale().GetAbs();
		if (AWarrior* Player = FindTouchingPlayer(Mesh->Bounds.Origin, Mesh->GetComponentQuat(), Extent))
		{
			if (bSafeCard)
			{
				Break();
			}
			else
			{
				const FHitResult Contact(Player, Player->GetCapsuleComponent(), Player->GetActorLocation(), FVector::UpVector);
				HandleTraceHit(Contact);
			}
			if (Owner->IsActorBeingDestroyed())
			{
				return;
			}
		}
	}

	// 착지 프레임의 PostPhysics 피해 판정이 먼저 실행되게 한다.
	if (Alpha >= 1.0f && PreviousElapsed >= FallStart + CardFallTime)
	{
		Break();
	}
}

void UClownProjectileComponent::SetVisualSpin(const FRotator& Rate)
{
	VisualSpin = Rate;

	if (VisualMesh.IsValid())
	{
		VisualMesh->SetRelativeRotation(FRotator(FMath::FRandRange(-180.0f, 180.0f), FMath::FRandRange(-180.0f, 180.0f),
			FMath::FRandRange(-180.0f, 180.0f)));
	}

	SetComponentTickEnabled(true);
}

void UClownProjectileComponent::TickBallSpin(float DeltaTime)
{
	UStaticMeshComponent* Mesh = VisualMesh.Get();
	if (!Mesh || VisualSpin.IsNearlyZero())
	{
		return;
	}

	Mesh->AddLocalRotation(VisualSpin * DeltaTime);
	if (!bBallGroundEffects)
	{
		return;
	}

	if (BallSpinEffect)
	{
		if (BallSpinEffect->IsComplete())
		{
			BallSpinEffect->Activate(true);
		}
		return;
	}

	const ABossClown* Boss = Cast<ABossClown>(GetOwner()->GetOwner());
	const UBossClownAnimationData* Data = Boss ? Boss->GetClownAnimationData() : nullptr;
	if (Data && Data->BallSpinEffect)
	{
		BallSpinEffect = UNiagaraFunctionLibrary::SpawnSystemAttached(Data->BallSpinEffect, Mesh, NAME_None,
			FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false, false);
		if (BallSpinEffect)
		{
			BallSpinEffect->SetAbsolute(false, false, true);
			BallSpinEffect->SetWorldScale3D(FVector(0.23f));
			BallSpinEffect->SetEmitterEnable(TEXT("Leaves"), false);
			BallSpinEffect->Activate(true);
		}
	}
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

void UClownProjectileComponent::TickComponent(
	float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	AActor* Owner = GetOwner();
	ABossClown* Boss = Owner ? Cast<ABossClown>(Owner->GetOwner()) : nullptr;

	if (!Owner || !IsValid(Boss) || !Boss->CanKeepClownProjectiles())
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

	TickBallSpin(DeltaTime);
	if (!bLaunched)
	{
		return;
	}

	const FVector Travel = FlightDirection * Speed * DeltaTime;
	if (const UStaticMeshComponent* Mesh = VisualMesh.Get(); bBallGroundEffects && Mesh)
	{
		const FVector Center = Mesh->Bounds.Origin;
		const float Radius = FMath::Max(1.0f, Mesh->Bounds.BoxExtent.GetMin());
		FCollisionQueryParams Query(SCENE_QUERY_STAT(ClownBallImpact), false, Owner);
		Query.AddIgnoredActor(Boss);
		FHitResult Floor;
		if (GetWorld()->SweepSingleByObjectType(Floor, Center, Center + Travel, FQuat::Identity,
			FCollisionObjectQueryParams(ECC_WorldStatic), FCollisionShape::MakeSphere(Radius), Query) &&
			Boss->TryPlayBallGroundEffects(Floor))
		{
			Owner->AddActorWorldOffset(Floor.Location - Center, false);
			Break();
			return;
		}
	}

	Owner->AddActorWorldOffset(Travel, false);
}

void UClownProjectileComponent::HandleTraceHit(const FHitResult& Hit)
{
	AActor* Owner = GetOwner();
	AActor* HitActor = Hit.GetActor();

	if (!bLaunched || (bCubeMarch && !bCubeTracing) || (bCardHazard && (!bCardActive || bSafeCard)) || !Owner || !IsValid(HitActor) ||
		HitActor == Owner->GetOwner() || HitActor->FindComponentByClass<UClownProjectileComponent>())
	{
		return;
	}

	if (HitActors.Contains(HitActor))
	{
		return;
	}

	if (bCardHazard)
	{
		const FVector Local = FRotator(0.0f, -CardYaw, 0.0f).RotateVector(HitActor->GetActorLocation() - CardOrigin);

		if (Local.X < 0.0f || Local.X > CardTravel || FMath::Abs(Local.Y) > CardWidth * 0.5f)
		{
			return;
		}
	}

	// 기존 Enemy와 동일하게 트레이스 결과를 받아 피해 인터페이스로 전달.
	if (ICombatDamageInterface* Target = Cast<ICombatDamageInterface>(HitActor))
	{
		HitActors.Add(HitActor);
		Target->ReceiveDamage(Damage, Hit);

		const AWarrior* Player = Cast<AWarrior>(HitActor);
		const bool bBodyContact = Player && Player->GetCapsuleComponent() == Hit.GetComponent();
		if (Player && (bBallGroundEffects || (bBodyContact && (bCardHazard || bCubeMarch))))
		{
			Break();
		}
		else if (!bCardHazard && !bCubeMarch)
		{
			Owner->Destroy();
		}
	}
	else if (Hit.bBlockingHit && !bCardHazard && !bCubeMarch)
	{
		if (bBallGroundEffects)
		{
			HandleBallGroundHit(Hit);
			if (!Owner->IsActorBeingDestroyed())
			{
				Break();
			}
		}
		else
		{
			Owner->Destroy();
		}
	}
}

void UClownProjectileComponent::EnableBallGroundEffects()
{
	bBallGroundEffects = true;
}

void UClownProjectileComponent::HandleBallGroundHit(const FHitResult& Hit)
{
	if (!bBallGroundEffects || !bLaunched)
	{
		return;
	}

	const AActor* Owner = GetOwner();
	const ABossClown* Boss = Owner ? Cast<ABossClown>(Owner->GetOwner()) : nullptr;
	if (IsValid(Boss) && Boss->TryPlayBallGroundEffects(Hit))
	{
		Break();
	}
}

void UClownProjectileComponent::Break()
{
	AActor* Owner = GetOwner();
	if (!Owner || Owner->IsActorBeingDestroyed() || (bCardHazard && !bCardActive) || (bCubeMarch && !bCubeTracing))
	{
		return;
	}

	const ABossClown* Boss = Cast<ABossClown>(Owner->GetOwner());
	if (Boss && VisualMesh.IsValid())
	{
		ClownFracture::PlayCached(*Boss, *VisualMesh.Get(), ClownFracture::PatternDebrisLifeSpan);
	}
	Owner->Destroy();
}

AWarrior* UClownProjectileComponent::FindTouchingPlayer(const FVector& Location, const FQuat& Rotation, const FVector& HalfExtent) const
{
	const AActor* Owner = GetOwner();
	const ABossClown* Boss = Owner ? Cast<ABossClown>(Owner->GetOwner()) : nullptr;
	AWarrior* Player = Boss ? Cast<AWarrior>(Boss->GetEncounterTarget()) : nullptr;
	if (IsValid(Player) && !Player->IsDead() &&
		Player->GetCapsuleComponent()->OverlapComponent(Location, Rotation, FCollisionShape::MakeBox(HalfExtent)))
	{
		return Player;
	}
	return nullptr;
}

void UClownProjectileComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (IsValid(BallSpinEffect))
	{
		BallSpinEffect->DestroyComponent();
	}
	SetTracing(false);
	AActor* Owner = GetOwner();
	ABossClown* Boss = Owner ? Cast<ABossClown>(Owner->GetOwner()) : nullptr;

	if (IsValid(Boss) && (CubeCollision || CardCollision))
	{
		Boss->GetCapsuleComponent()->IgnoreActorWhenMoving(Owner, false);
	}

	for (const TWeakObjectPtr<UAttackTraceComponent>& Trace : AttackTraces)
	{
		if (!Trace.IsValid())
		{
			continue;
		}

		Trace->OnTraceHit.RemoveAll(this);
		Trace->OnGroundTraceHit.RemoveAll(this);
		Trace->RemoveTickPrerequisiteComponent(this);
	}

	Super::EndPlay(EndPlayReason);
}

void UClownProjectileComponent::ArmCubeMarch(UStaticMesh* Piece, const FVector& Assembly, const FVector& Start,
	const FVector& End, float MarchTime, bool bShowPath)
{
	AActor* Owner = GetOwner();
	UStaticMeshComponent* Mesh = VisualMesh.Get();

	if (!Owner || !Mesh || !Piece)
	{
		return;
	}

	Mesh->SetStaticMesh(Piece);
	Mesh->EmptyOverrideMaterials();
	RestoreProjectileMaterial(Mesh, true);
	const float Size = Piece->GetBounds().BoxExtent.GetMax() * 2.0f;
	Mesh->SetRelativeScale3D(FVector(94.0f / FMath::Max(Size, 0.01f)));
	Mesh->SetRelativeLocation(FVector::ZeroVector);
	Mesh->SetRelativeRotation(FRotator::ZeroRotator);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 조각 메시의 단순 충돌 유무와 관계없이 94cm 큐브를 막는 이동 루트.
	if (!CubeCollision)
	{
		USceneComponent* PreviousRoot = Owner->GetRootComponent();

		CubeCollision = NewObject<UBoxComponent>(Owner, TEXT("CubeCollision"));
		Owner->AddInstanceComponent(CubeCollision);
		CubeCollision->SetBoxExtent(FVector(47.0f));
		CubeCollision->SetCollisionObjectType(ECC_WorldDynamic);
		CubeCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
		CubeCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
		CubeCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CubeCollision->SetGenerateOverlapEvents(false);
		CubeCollision->SetCanEverAffectNavigation(false);
		CubeCollision->RegisterComponent();
		CubeCollision->SetWorldLocationAndRotation(Owner->GetActorLocation(), Owner->GetActorRotation());
		Owner->SetRootComponent(CubeCollision);

		if (PreviousRoot)
		{
			PreviousRoot->AttachToComponent(CubeCollision, FAttachmentTransformRules::KeepWorldTransform);
		}

		if (Owner->GetOwner())
		{
			CubeCollision->IgnoreActorWhenMoving(Owner->GetOwner(), true);

			if (ABossClown* Boss = Cast<ABossClown>(Owner->GetOwner()))
			{
				Boss->GetCapsuleComponent()->IgnoreActorWhenMoving(Owner, true);
			}
		}
	}

	TArray<UPrimitiveComponent*> Primitives;
	Owner->GetComponents<UPrimitiveComponent>(Primitives);

	for (UPrimitiveComponent* Primitive : Primitives)
	{
		if (Primitive != CubeCollision)
		{
			Primitive->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}

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
		CubePathMarker = NewObject<UStaticMeshComponent>(Owner);
		Owner->AddInstanceComponent(CubePathMarker);
		CubePathMarker->SetStaticMesh(WarningMesh);
		CubePathMarker->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CubePathMarker->SetCastShadow(false);
		CubePathMarker->RegisterComponent();
		CubePathMarker->SetWorldLocation((Start + End) * 0.5f - FVector(0.0f, 0.0f, 49.0f));
		CubePathMarker->SetWorldRotation((End - Start).Rotation());
		CubePathMarker->SetWorldScale3D(FVector((FVector::Dist2D(Start, End) + 400.0f) / 100.0f, 1.15f, 1.0f));
		UMaterialInstanceDynamic* Material = UMaterialInstanceDynamic::Create(WarningMaterial, this);
		Material->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.7f, 0.15f, 2.5f));
		Material->SetScalarParameterValue(TEXT("Safe"), 0.0f);
		CubePathMarker->SetMaterial(0, Material);
		CubePathMarker->SetVisibility(false);
	}
}

void UClownProjectileComponent::SetCubeAim(AActor* Target, const FVector& Center)
{
	CubeAimTarget = Target;
	CubeStartOffset = CubeStart - Center;
	CubeEndOffset = CubeEnd - Center;
	bCubeAimLocked = false;
}

void UClownProjectileComponent::TickCubeMarch(float DeltaTime)
{
	CubeElapsed += DeltaTime;
	AActor* Owner = GetOwner();

	// 각 차례의 집결이 시작될 때 다시 조준하고, 예고가 뜬 뒤에는 경로를 고정해 회피를 보장.
	if (!bCubeAimLocked && CubeElapsed >= CubeMarchTime - 1.8f)
	{
		bCubeAimLocked = true;

		if (const AActor* Target = CubeAimTarget.Get())
		{
			const FVector Center = Target->GetActorLocation();
			CubeStart = Center + CubeStartOffset;
			CubeEnd = Center + CubeEndOffset;
		}

		const ABossClown* Boss = Cast<ABossClown>(Owner->GetOwner());
		if (!Boss || !Boss->FitClownCubePath(CubeStart, CubeEnd))
		{
			Owner->Destroy();
			return;
		}

		if (CubePathMarker)
		{
			CubePathMarker->SetWorldLocation((CubeStart + CubeEnd) * 0.5f - FVector(0.0f, 0.0f, 49.0f));
			CubePathMarker->SetWorldRotation((CubeEnd - CubeStart).Rotation());
			CubePathMarker->SetWorldScale3D(FVector(FVector::Dist2D(CubeStart, CubeEnd) / 100.0f, 1.15f, 1.0f));
		}
	}

	const float MoveEnd = CubeMarchTime + 1.2f;
	const bool bMoving = CubeElapsed >= CubeMarchTime && CubeElapsed < MoveEnd;

	// 판정은 고정된 경로를 실제로 이동하는 동안에만 사용.
	if (bMoving != bCubeTracing)
	{
		bCubeTracing = bMoving;
		SetTracing(bMoving);
	}

	if (CubePathMarker)
	{
		CubePathMarker->SetVisibility(CubeElapsed >= CubeMarchTime - 1.0f && CubeElapsed < MoveEnd);
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
		Position = FMath::Lerp( CubeAssembly, CubeStart, FMath::SmoothStep(0.0f, 1.0f, (CubeElapsed - CubeMarchTime + 1.8f) / 0.8f));
	}

	// 집결/공중 배치/회수에는 충돌하지 않는다. 바닥의 대기 및 행진 구간만 플레이어를 막는다.
	const bool bOnGround = CubeElapsed >= CubeMarchTime - 1.0f && CubeElapsed < 8.6f;

	if (!bOnGround && CubeCollision)
	{
		CubeCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	FHitResult BlockingHit;
	Owner->SetActorLocation(Position, bOnGround && bCubeGrounded, &BlockingHit);

	if (CubeCollision)
	{
		CubeCollision->SetCollisionEnabled(bOnGround ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
	}

	bCubeGrounded = bOnGround;

	if (bMoving && BlockingHit.bBlockingHit)
	{
		HandleTraceHit(BlockingHit);
	}

	if (bMoving && CubeCollision && !Owner->IsActorBeingDestroyed())
	{
		const FVector Extent = CubeCollision->GetScaledBoxExtent() + FVector(2.0);
		if (AWarrior* Player = FindTouchingPlayer(Owner->GetActorLocation(), Owner->GetActorQuat(), Extent))
		{
			const FHitResult Contact(Player, Player->GetCapsuleComponent(), Player->GetActorLocation(), FVector::UpVector);
			HandleTraceHit(Contact);
			return;
		}
	}

	if (VisualMesh.IsValid() && CubeElapsed >= 8.6f)
	{
		VisualMesh->SetRelativeRotation(
			FMath::RInterpTo(VisualMesh->GetRelativeRotation(), FRotator::ZeroRotator, DeltaTime, 10.0f));
	}
}
