#include <Enemy/Boss/Clown/Gimmick/ClownFracture.h>
#include <Enemy/Boss/Clown/BossClown.h>
#include <Chaos/CacheCollection.h>
#include <Chaos/CacheManagerActor.h>
#include <Components/SkeletalMeshComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Engine/StaticMesh.h>
#include <Engine/World.h>
#include <GeometryCollection/GeometryCollection.h>
#include <GeometryCollection/GeometryCollectionComponent.h>
#include <GeometryCollection/GeometryCollectionObject.h>
#include <TimerManager.h>

AClownFractureCacheManager::AClownFractureCacheManager()
{
	CacheMode = ECacheMode::Play;
	bCanRecord = false;

	Shards = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("CachedShards"));
	Shards->SetupAttachment(GetRootComponent());
	Shards->EnableClustering = false;
	Shards->ObjectType = EObjectStateTypeEnum::Chaos_Object_Kinematic;
	Shards->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	Shards->SetCollisionResponseToAllChannels(ECR_Ignore);
	Shards->SetGenerateOverlapEvents(false);
	Shards->SetCanEverAffectNavigation(false);
}

bool ClownFracture::PlayCached(const ABossClown& Boss, UMeshComponent& Mesh, float LifeSpan)
{
	const UBossClownAnimationData* Data = Boss.GetClownAnimationData();
	UWorld* World = Mesh.GetWorld();
	if (!Data || !Data->FractureCaches || !World || !Mesh.IsVisible())
	{
		return false;
	}

	const UStaticMeshComponent* StaticMesh = Cast<UStaticMeshComponent>(&Mesh);
	const UStaticMesh* StaticAsset = StaticMesh ? StaticMesh->GetStaticMesh() : nullptr;
	const USkeletalMeshComponent* SkeletalMesh = Cast<USkeletalMeshComponent>(&Mesh);
	const USkeletalMesh* SkeletalAsset = SkeletalMesh ? SkeletalMesh->GetSkeletalMeshAsset() : nullptr;
	const FName Shape = StaticAsset ? StaticAsset->GetFName() : SkeletalAsset ? SkeletalAsset->GetFName() : NAME_None;

	const bool bCard = Shape == TEXT("SM_ClownCard");
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

	const TObjectPtr<UGeometryCollection>* RestEntry = Data->Fractures.Find(FractureShape);
	UGeometryCollection* Rest = RestEntry ? RestEntry->Get() : nullptr;
	if (!Rest)
	{
		return false;
	}

	FTransform Transform = Mesh.GetComponentTransform();
	FName CacheName = Rest->GetFName();
	if (CacheName == TEXT("GC_ClownCard") && Transform.GetRotation().GetAxisX().Z > 0.0)
	{
		CacheName = TEXT("GC_ClownCard_Safe");
	}
	else if (CacheName == TEXT("GC_ClownBowl") && Transform.GetRotation().GetAxisZ().Z < 0.0)
	{
		CacheName = TEXT("GC_ClownBowl_Inverted");
	}
	else if (CacheName == TEXT("GC_ClownBall"))
	{
		Transform.SetRotation(FRotator(0.0, Transform.Rotator().Yaw, 0.0).Quaternion());
	}

	const UChaosCache* Cache = Data->FractureCaches->FindCache(CacheName);
	if (!Cache)
	{
		return false;
	}

	if (bCard && StaticAsset)
	{
		// The card collections are authored at their own display sizes.
		FBox ShardBounds(ForceInit);
		for (const FBox& Bounds : Rest->GetGeometryCollection()->BoundingBox)
		{
			ShardBounds += Bounds;
		}
		const FVector BakedSize = ShardBounds.GetSize();
		if (BakedSize.GetMin() > UE_SMALL_NUMBER)
		{
			const FVector SourceSize = StaticAsset->GetBounds().BoxExtent * 2.0;
			Transform.SetScale3D(Transform.GetScale3D() * SourceSize / BakedSize);
		}
	}

	if (bIntroCard)
	{
		Transform.AddToTranslation(FVector(0.0, 0.0, 2.0));
	}

	AClownFractureCacheManager* Manager = World->SpawnActorDeferred<AClownFractureCacheManager>(
		AClownFractureCacheManager::StaticClass(), Transform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (!Manager)
	{
		return false;
	}

	UGeometryCollectionComponent* Collection = Manager->GetShards();
	Collection->SetRestCollection(Rest, false);
	if (UMaterialInterface* Material = Mesh.GetMaterial(0))
	{
		Collection->SetMaterial(0, Material);
	}
	Manager->CacheCollection = Data->FractureCaches;
	Manager->FindOrAddObservedComponent(Collection, CacheName);
	Manager->FinishSpawning(Transform);
	Manager->SetLifeSpan(LifeSpan);

	const float PlaybackTime = Cache->GetDuration() + 0.05f;
	if (PlaybackTime < LifeSpan)
	{
		FTimerHandle StopPlayback;
		World->GetTimerManager().SetTimer(StopPlayback, Manager, &AChaosCacheManager::Stop, PlaybackTime, false);
	}

	Mesh.SetVisibility(false);
	return true;
}
