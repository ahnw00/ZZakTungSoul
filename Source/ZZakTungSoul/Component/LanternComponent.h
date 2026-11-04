// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LanternComponent.generated.h"

// 전방 선언
class UStaticMeshComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZZAKTUNGSOUL_API ULanternComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULanternComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(
		float DeltaTime, 
		ELevelTick TickType, 
		FActorComponentTickFunction* ThisTickFunction) override;

	void ToggleLantern();

	//void SetAttachment(class AWarrior* User);

	FORCEINLINE UStaticMeshComponent* GetLanternMesh() const
	{
		return LanternMesh;
	}

private:
	bool bIsLanternOn = false;

	UPROPERTY(VisibleAnywhere, Category = "Lantern", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UPointLightComponent> LanternLight;

	UPROPERTY(VisibleAnywhere, Category = "Lantern", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> LanternMesh;
};
