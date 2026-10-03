// Fill out your copyright notice in the Description page of Project Settings.


#include "GamePlay/ZTSGameMode.h"
#include "Warrior/WarriorController.h"

AZTSGameMode::AZTSGameMode()
{
	// 컨트롤러 세팅
	PlayerControllerClass = AWarriorController::StaticClass();

	// 컨트롤러로 조종하는 Pawn 세팅
	static ConstructorHelpers::FClassFinder<APawn> WarriorClassRef(
		TEXT("/Game/Blueprints/Warrior/BP_Warrior.BP_Warrior_C")
	);

	if (WarriorClassRef.Succeeded())
	{
		DefaultPawnClass = WarriorClassRef.Class;
	}
}
