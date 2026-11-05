// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/LanternComponent.h"
#include <Components/PointLightComponent.h>


// Sets default values for this component's properties
ULanternComponent::ULanternComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	LanternMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LanternMesh"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> LanternMeshRef(
		TEXT("/Game/Assets/Ornate_lantern_3d_model_1_/LanternStaticMesh.LanternStaticMesh")
	);
	if (LanternMeshRef.Succeeded())
	{
		LanternMesh->SetStaticMesh(LanternMeshRef.Object);
		LanternMesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
		//LanternMesh->SetRelativeLocation(FVector(-30.f, 0.f, -40.f));
	}
	//LanternMesh->SetupAttachment(this);


	LanternLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("LanternLight"));
	LanternLight->SetupAttachment(LanternMesh);
	LanternLight->SetRelativeLocation(FVector(0.f, -30.f, -30.f));
	LanternLight->SetLightColor(FLinearColor::Yellow);
}


// Called when the game starts
void ULanternComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	LanternLight->Activate(false);
	//LanternLight->SetupAttachment(GetOwner()->getmesh)
}


// Called every frame
void ULanternComponent::TickComponent(
	float DeltaTime, 
	ELevelTick TickType, 
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsLanternOn)
		return;

	// 랜턴 켜져있으면 밝기 왔다갔다 하도록
}

void ULanternComponent::ToggleLantern()
{
	if (bIsLanternOn)
	{
		// 빛 꺼주기
		LanternLight->Activate(false);
		bIsLanternOn = false;
	}
	else
	{
		// 빛 켜주기
		LanternLight->Activate(true);
		bIsLanternOn = true;
	}

	// 토글 해주기
	//bIsLanternOn = 1 - bIsLanternOn;
	//LanternLight->Activate(bIsLanternOn);
}

