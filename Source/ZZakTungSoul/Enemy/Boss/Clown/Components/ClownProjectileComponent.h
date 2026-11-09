#pragma once

#include <CoreMinimal.h>
#include <Components/ActorComponent.h>
#include <Data/DamageInfo.h>
#include "ClownProjectileComponent.generated.h"

class UAttackTraceComponent;
class UStaticMeshComponent;
class UStaticMesh;
class UMaterialInterface;
class UTexture2D;
class UBoxComponent;
// 일반 Actor BP에 추가하는 발사체 이동 / 피해 전달 컴포넌트.
// 판정은 BP에 추가한 기존 AttackTraceComponent가 담당.
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ZZAKTUNGSOUL_API UClownProjectileComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UClownProjectileComponent();
	virtual void TickComponent(
		float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void Initialize(const FDamageInfo& InDamage, float InSpeed, float InLifeTime);

	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void LaunchAt(const FVector& TargetLocation);

	void SetVisualSpin(const FRotator& Rate);
	void ArmCubeMarch(UStaticMesh* Piece, const FVector& Assembly, const FVector& Start, const FVector& End,
		float MarchTime, bool bShowPath);
	void SetCubeAim(AActor* Target, const FVector& Center);
	static constexpr float CubeMarchCycle = 2.4f;
	static constexpr float CubeMarchDuration = 9.6f;

	// 고정 위치에 카드 소환 / 예고 / 낙하. 피해는 기존 Trace 사용.
	void ArmCard(const FVector& GroundLocation, float StandTime, float Height, float Width, float FallTime,
		UTexture2D* Face, bool bSafe);
	bool IsLaunched() const
	{
		return bLaunched;
	}

	UPROPERTY(EditAnywhere, Category = "Projectile|Cube|Path")
	TObjectPtr<UStaticMesh> WarningMesh;

	UPROPERTY(EditAnywhere, Category = "Projectile|Cube|Path")
	TObjectPtr<UMaterialInterface> WarningMaterial;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void HandleTraceHit(const FHitResult& Hit);
	void SetTracing(bool bEnabled);
	void TickCard(float DeltaTime);
	void TickCubeMarch(float DeltaTime);
	void RestoreProjectileMaterial(UStaticMeshComponent* Mesh, bool bCube);

	UPROPERTY()
	TObjectPtr<UMaterialInterface> BallMaterial;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> CubeMaterial;

	UPROPERTY()
	TArray<TObjectPtr<UTexture2D>> BallTextures;

	UPROPERTY()
	TArray<TObjectPtr<UTexture2D>> CubeTextures;

	UPROPERTY(Transient)
	FDamageInfo Damage;

	UPROPERTY(Transient)
	TObjectPtr<UStaticMeshComponent> CubePathMarker;

	UPROPERTY(Transient)
	TObjectPtr<UBoxComponent> CubeCollision;

	UPROPERTY(Transient)
	TObjectPtr<UBoxComponent> CardCollision;

	TWeakObjectPtr<UStaticMeshComponent> VisualMesh;

	FVector CardOrigin = FVector::ZeroVector;
	float CardElapsed = 0.0f;
	float CardStandTime = 1.2f;
	float CardRiseTime = 0.45f;
	float CardTravel = 650.0f;
	float CardWidth = 550.0f;
	float CardFallTime = 0.65f;
	float CardYaw = 0.0f;
	bool bSafeCard = false;
	bool bCardHazard = false;
	bool bCardActive = false;

	TArray<TWeakObjectPtr<UAttackTraceComponent>> AttackTraces;
	TSet<TWeakObjectPtr<AActor>> HitActors;

	FVector FlightDirection = FVector::ForwardVector;
	FRotator VisualSpin = FRotator::ZeroRotator;
	float Speed = 700.0f;
	float LifeTime = 6.0f;
	bool bLaunched = false;

	bool bCubeMarch = false;
	bool bCubeTracing = false;
	bool bCubeGrounded = false;
	float CubeElapsed = 0.0f;
	float CubeMarchTime = 0.0f;

	TWeakObjectPtr<AActor> CubeAimTarget;
	FVector CubeStartOffset = FVector::ZeroVector;
	FVector CubeEndOffset = FVector::ZeroVector;
	bool bCubeAimLocked = false;
	FVector CubeAssembly;
	FVector CubeStart;
	FVector CubeEnd;
};
