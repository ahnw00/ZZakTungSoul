#include <Enemy/Boss/Clown/Gimmick/ClownFracture.h>
#include <Enemy/Boss/Clown/BossClown.h>
#include <Components/SkeletalMeshComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Engine/StaticMesh.h>
#include <Engine/World.h>
#include <GeometryCollection/GeometryCollection.h>
#include <GeometryCollection/GeometryCollectionActor.h>
#include <GeometryCollection/GeometryCollectionComponent.h>
#include <GeometryCollection/GeometryCollectionObject.h>
#include <TimerManager.h>

namespace
{
	void ScatterPieces(UGeometryCollectionComponent& Collection, const FVector& Center, const FVector& HitPoint, float Speed, float SpinScale, float LiftScale)
	{
		const FGeometryCollection& Rest = *Collection.GetRestCollection()->GetGeometryCollection();
		const TArray<FTransform3f>& Transforms = Collection.GetComponentSpaceTransforms3f();
		const FTransform ToWorld = Collection.GetComponentTransform();
		const FVector ImpactDirection = (Center - HitPoint).GetSafeNormal2D();
		FRandomStream Random(static_cast<int32>(Collection.GetUniqueID()));
		const int32 PieceCount = FMath::Min(Transforms.Num(), Rest.TransformToGeometryIndex.Num());
		for (int32 Index = 0; Index < PieceCount; ++Index)
		{
			const int32 GeometryIndex = Rest.TransformToGeometryIndex[Index];
			if (GeometryIndex == INDEX_NONE || !Rest.Children[Index].IsEmpty())
			{
				continue;
			}

			const FVector3f LocalCenter(Rest.BoundingBox[GeometryIndex].GetCenter());
			const FVector Piece = ToWorld.TransformPosition(FVector(Transforms[Index].TransformPosition(LocalCenter)));
			FVector Outward = (Piece - Center).GetSafeNormal2D();
			if (Outward.IsNearlyZero())
			{
				Outward = Random.VRand().GetSafeNormal2D();
			}

			const FVector Velocity = (Outward + ImpactDirection * 0.28) * Speed * Random.FRandRange(0.7f, 1.15f) +
				FVector::UpVector * (Random.FRandRange(90.f, 175.f) * LiftScale);
			Collection.ApplyLinearVelocity(Index, Velocity);
			Collection.ApplyAngularVelocity(Index, Random.VRand() * Random.FRandRange(4.f, 9.f) * SpinScale);
		}
	}
}

bool ClownFracture::Spawn(const ABossClown& Boss, UMeshComponent& Mesh, const FVector& ImpactPoint, float LifeSpan)
{
	const UBossClownAnimationData* Data = Boss.GetClownAnimationData();
	if (!Data || !Mesh.IsVisible())
	{
		return false;
	}

	FName Shape = NAME_None;
	if (const UStaticMeshComponent* StaticMesh = Cast<UStaticMeshComponent>(&Mesh))
	{
		Shape = StaticMesh->GetStaticMesh() ? StaticMesh->GetStaticMesh()->GetFName() : NAME_None;
	}
	else if (const USkeletalMeshComponent* SkeletalMesh = Cast<USkeletalMeshComponent>(&Mesh))
	{
		Shape = SkeletalMesh->GetSkeletalMeshAsset() ? SkeletalMesh->GetSkeletalMeshAsset()->GetFName() : NAME_None;
	}

	const bool bCard = Shape == TEXT("SM_ClownCard");
	const bool bCubePiece = Shape.ToString().StartsWith(TEXT("SM_ClownCube_"));
	const bool bIntroProp = Mesh.GetOwner() && Mesh.GetOwner()->ActorHasTag(TEXT("ClownIntroBreakable"));
	const bool bIntroCard = bCard && bIntroProp;
	FName FractureShape = Shape;
	if (bIntroCard)
	{
		FractureShape = TEXT("IntroCard");
	}
	else if (bIntroProp && Shape == TEXT("SM_ClownMagicCube"))
	{
		FractureShape = TEXT("IntroCube");
	}

	const TObjectPtr<UGeometryCollection>* Rest = Data->Fractures.Find(FractureShape);
	if (!Rest || !Rest->Get())
	{
		return false;
	}

	FTransform Transform = Mesh.GetComponentTransform();
	if (const UStaticMeshComponent* Card = Cast<UStaticMeshComponent>(&Mesh); bCard && Card && Card->GetStaticMesh())
	{
		// Each card collection is baked at its own display size.
		FBox ShardBounds(ForceInit);
		const FGeometryCollection& Geometry = *Rest->Get()->GetGeometryCollection();
		for (int32 Index = 0; Index < Geometry.BoundingBox.Num(); ++Index)
		{
			ShardBounds += Geometry.BoundingBox[Index];
		}
		const FVector BakedSize = ShardBounds.GetSize();
		if (BakedSize.GetMin() > UE_SMALL_NUMBER)
		{
			const FVector SourceSize = Card->GetStaticMesh()->GetBounds().BoxExtent * 2.0;
			Transform.SetScale3D(Transform.GetScale3D() * SourceSize / BakedSize);
		}
	}

	if (bIntroCard)
	{
		Transform.AddToTranslation(FVector(0.0, 0.0, 2.0));
	}

	UWorld* World = Mesh.GetWorld();
	AGeometryCollectionActor* Debris = World->SpawnActorDeferred<AGeometryCollectionActor>(
		AGeometryCollectionActor::StaticClass(), Transform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (!Debris)
	{
		return false;
	}

	UGeometryCollectionComponent* Collection = Debris->GetGeometryCollectionComponent();
	const bool bStartAsShards = bCard || bCubePiece;
	Collection->EnableClustering = !bStartAsShards;
	Collection->ObjectType = EObjectStateTypeEnum::Chaos_Object_Dynamic;
	Collection->SetCollisionObjectType(ECC_PhysicsBody);
	Collection->SetCollisionResponseToAllChannels(ECR_Ignore);
	Collection->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	Collection->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	Collection->SetGenerateOverlapEvents(false);
	Collection->BodyInstance.SetMaxDepenetrationVelocity(110.f);
	if (bIntroCard)
	{
		Collection->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
		Collection->BodyInstance.bUseCCD = true;
	}
	Collection->SetSimulatePhysics(true);
	// Assigning the collection creates the physics proxy from these settings.
	Collection->SetRestCollection(Rest->Get(), false);
	if (UMaterialInterface* Material = Mesh.GetMaterial(0))
	{
		Collection->SetMaterial(0, Material);
	}

	Debris->FinishSpawning(Transform);
	Debris->SetLifeSpan(LifeSpan);
	const FVector Center = Mesh.Bounds.Origin;
	const float Speed = bIntroCard ? 58.0f : FMath::Clamp(Mesh.Bounds.SphereRadius * 1.6f, 95.f, 210.f);
	const float SpinScale = bIntroCard ? 0.42f :
		FMath::GetMappedRangeValueClamped(FVector2D(24.f, 140.f), FVector2D(1.f, 0.24f), Mesh.Bounds.SphereRadius);
	const float LiftScale = bIntroCard ? 0.32f : 1.0f;
	FTimerDelegate Scatter = FTimerDelegate::CreateWeakLambda(Collection, [Collection, Center, ImpactPoint, Speed, SpinScale, LiftScale]
	{
		ScatterPieces(*Collection, Center, ImpactPoint, Speed, SpinScale, LiftScale);
	});
	if (!bStartAsShards)
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(Collection, [Collection, Scatter]
		{
			Collection->CrumbleActiveClusters();
			// Let Chaos release the child bodies before applying their individual velocities.
			Collection->GetWorld()->GetTimerManager().SetTimerForNextTick(Scatter);
		}));
	}
	else
	{
		World->GetTimerManager().SetTimerForNextTick(Scatter);
	}
	Mesh.SetVisibility(false);
	return true;
}
