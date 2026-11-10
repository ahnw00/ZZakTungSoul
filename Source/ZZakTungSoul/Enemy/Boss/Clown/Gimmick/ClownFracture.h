#pragma once

#include <CoreMinimal.h>

class ABossClown;
class UMeshComponent;

namespace ClownFracture
{
	inline constexpr float PatternDebrisLifeSpan = 7.5f;
	bool Spawn(const ABossClown& Boss, UMeshComponent& Mesh, const FVector& ImpactPoint, float LifeSpan = 2.6f);
}
