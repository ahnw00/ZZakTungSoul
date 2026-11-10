#pragma once

#include <CoreMinimal.h>
#include <Chaos/CacheManagerActor.h>
#include "ClownFracture.generated.h"

class ABossClown;
class UMeshComponent;
class UGeometryCollectionComponent;

UCLASS(NotBlueprintable, NotPlaceable)
class AClownFractureCacheManager final : public AChaosCacheManager
{
	GENERATED_BODY()

public:
	AClownFractureCacheManager();

	UGeometryCollectionComponent* GetShards() const { return Shards; }

private:
	UPROPERTY()
	TObjectPtr<UGeometryCollectionComponent> Shards;
};

namespace ClownFracture
{
	inline constexpr float PatternDebrisLifeSpan = 7.5f;
	bool PlayCached(const ABossClown& Boss, UMeshComponent& Mesh, float LifeSpan = 2.6f);
}
