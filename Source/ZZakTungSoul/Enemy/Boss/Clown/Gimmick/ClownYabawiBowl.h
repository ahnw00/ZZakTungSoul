#pragma once

#include <CoreMinimal.h>
#include <GameFramework/Actor.h>
#include "ClownYabawiBowl.generated.h"

class UClownYabawiComponent;
class UStaticMeshComponent;
class UBoxComponent;
struct FClownPhaseOnePattern;

UCLASS(Blueprintable)
class ZZAKTUNGSOUL_API AClownYabawiBowl : public AActor
{
	GENERATED_BODY()

public:
	AClownYabawiBowl();
	void Initialize(UClownYabawiComponent* Round, int32 InRoundID, const FClownPhaseOnePattern& Settings);
	void SetAttackSelectionEnabled(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "Clown|Yabawi")
	bool ReceivePlayerAttack(AActor* Player, const FHitResult& Hit);

	UFUNCTION(BlueprintNativeEvent, Category = "Clown|Yabawi")
	void OnBowlStruck(const FHitResult& Hit);
	virtual void OnBowlStruck_Implementation(const FHitResult& Hit);
	void SetCoverAlpha(float Alpha);
	void ShowBall(bool bVisible);
	void ShowExplosion(float Radius, float Alpha);

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> BowlMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> BallMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Explosion;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> AttackHitBox;

	TWeakObjectPtr<UClownYabawiComponent> YabawiRound;
	int32 RoundID = INDEX_NONE;
	FVector MeshCenter = FVector::ZeroVector;
	float HalfHeight = 40.0f;
	float Diameter = 180.0f;
};
