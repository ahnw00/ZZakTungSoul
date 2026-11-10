#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_FatalImpact.generated.h"

class UMaterialInterface;

UCLASS()
class ZZAKTUNGSOUL_API UAnimNotify_FatalImpact : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	virtual FString GetNotifyName_Implementation() const override;

private:
	UPROPERTY(EditAnywhere, Category = "Fatal Impact")
	TObjectPtr<UMaterialInterface> ImpactMaterial;

	UPROPERTY(EditAnywhere, Category = "Fatal Impact", meta = (AnimNotifyBoneName = "true"))
	FName BoneName;

	UPROPERTY(EditAnywhere, Category = "Fatal Impact")
	FVector LocationOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = "Fatal Impact", meta = (ClampMin = "1.0"))
	float Size = 34.0f;

	UPROPERTY(EditAnywhere, Category = "Fatal Impact", meta = (ClampMin = "0.05"))
	float Lifetime = 0.18f;
};
