// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/WarriorAIController.h"

#include <BehaviorTree/BehaviorTree.h>
#include <BehaviorTree/BlackboardData.h>


const FName AWarriorAIController::TargetKey(TEXT("Target"));
const FName AWarriorAIController::PatrolPosKey(TEXT("PatrolPos"));


AWarriorAIController::AWarriorAIController()
{
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(
		TEXT("/Game/BehaviorTree/BT_WarriorAI.BT_WarriorAI")
	);
	if (BTObject.Succeeded())
	{
		BTAsset = BTObject.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBObject(
		TEXT("/Game/BehaviorTree/BB_WarriorAI.BB_WarriorAI")
	);
	if (BBObject.Succeeded())
	{
		BBAsset = BBObject.Object;
	}
}

void AWarriorAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (!RunBehaviorTree(BTAsset))
	{
		UE_LOG(LogTemp, Log, TEXT("BT doesn't work"));
	}
}
