#pragma once

#include <CoreMinimal.h>
#include <GameFramework/Actor.h>
#include <Data/DamageInfo.h>
#include "BossClownProjectile.generated.h"

class UStaticMeshComponent;
class UMaterialInterface;

UCLASS()
class ZZAKTUNGSOUL_API ABossClownProjectile : public AActor
{
	GENERATED_BODY()

public:
	ABossClownProjectile();
	virtual void Tick(float DeltaTime) override;

	void Initialize(const FDamageInfo& InDamage, float InSpeed, bool bInShockwave, UMaterialInterface* Material);
	void LaunchAt(const FVector& TargetLocation);
	bool IsLaunched() const { return bLaunched; }

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> VisualMesh;

private:
	FDamageInfo Damage;
	FVector FlightDirection = FVector::ForwardVector;
	float Speed = 700.0f;
	bool bLaunched = false;
	bool bShockwave = false;
};
