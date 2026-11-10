#include "Anim/AnimNotify/AnimNotify_FatalImpact.h"

#include <Components/MaterialBillboardComponent.h>
#include <Components/SkeletalMeshComponent.h>
#include <Engine/World.h>
#include <GameFramework/Actor.h>
#include <Materials/MaterialInstanceDynamic.h>

void UAnimNotify_FatalImpact::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	UWorld* World = MeshComp ? MeshComp->GetWorld() : nullptr;
	if (!World || World->GetNetMode() == NM_DedicatedServer || !ImpactMaterial || !MeshComp->DoesSocketExist(BoneName))
	{
		return;
	}

	AActor* Effect = World->SpawnActor<AActor>();
	if (!Effect)
	{
		return;
	}

	UMaterialBillboardComponent* Billboard = NewObject<UMaterialBillboardComponent>(Effect);
	Effect->SetRootComponent(Billboard);
	Billboard->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Billboard->SetWorldLocation(MeshComp->GetSocketTransform(BoneName).TransformPosition(LocationOffset));

	UMaterialInstanceDynamic* Material = UMaterialInstanceDynamic::Create(ImpactMaterial, Billboard);
	Material->SetScalarParameterValue(TEXT("StartTime"), World->GetTimeSeconds());
	Material->SetScalarParameterValue(TEXT("Duration"), Lifetime);
	Billboard->AddElement(Material, nullptr, false, Size, Size, nullptr);
	Billboard->RegisterComponent();
	Effect->SetLifeSpan(Lifetime);
}

FString UAnimNotify_FatalImpact::GetNotifyName_Implementation() const
{
	return TEXT("Fatal Impact");
}
