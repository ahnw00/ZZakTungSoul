// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CharacterStatComponent.h"

#include <Data/StatDefine.h>


// Sets default values for this component's properties
UCharacterStatComponent::UCharacterStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UDataTable> StatDefineTableAsset(
		TEXT("/Game/Blueprints/Data/DT_StatDefine.DT_StatDefine")
	);
	if (StatDefineTableAsset.Succeeded())
	{
		StatDefineTable = StatDefineTableAsset.Object;
	}
}


// Called when the game starts
void UCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UCharacterStatComponent::LoadCharacterStat()
{
	if (StatDefineTable)
	{
		TArray<FStatDefine*> CharacterStatsRows;
		StatDefineTable->GetAllRows<FStatDefine>(TEXT(""), CharacterStatsRows);
	}
}


// Called every frame
void UCharacterStatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

