#pragma once

#include <CoreMinimal.h>
#include <GameFramework/Actor.h>
#include <Interface/InteractableInterface.h>
#include "ClownYabawiBowl.generated.h"

class UClownYabawiComponent;
class UStaticMeshComponent;
class UTextRenderComponent;
struct FClownYabawiSettings;

UCLASS()
class ZZAKTUNGSOUL_API AClownYabawiBowl : public AActor, public IInteractableInterface
{
	GENERATED_BODY()

public:
	AClownYabawiBowl();
	void Initialize(UClownYabawiComponent* Round, int32 InRoundID, const FClownYabawiSettings& Settings);
	virtual void Interact(AActor* Player) override;
	void SetCoverAlpha(float Alpha);
	void ShowBall(bool bVisible);
	void SetSelectionHint(bool bClosest, float Remaining);
	void ShowExplosion(float Radius, float Alpha);

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> BowlMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> BallMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Explosion;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UTextRenderComponent> Hint;

	TWeakObjectPtr<UClownYabawiComponent> YabawiRound;
	int32 RoundID=INDEX_NONE;
	FVector MeshCenter=FVector::ZeroVector;
	float HalfHeight=40.0f;
	float Diameter=180.0f;
};
