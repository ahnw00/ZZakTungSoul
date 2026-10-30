// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Boss/AI/BossAIController.h"

ABossAIController::ABossAIController()
{

}

void ABossAIController::UpdateAIFocus(EEnemyState PreviousState, EEnemyState NewState)
{
	switch (NewState)
	{
	case EEnemyState::Idle:
		ClearFocus(EAIFocusPriority::Gameplay);
		break;

	case EEnemyState::Patrol:
		ClearFocus(EAIFocusPriority::Gameplay);
		break;

	case EEnemyState::Chase:
		ClearFocus(EAIFocusPriority::Gameplay);
		break;

	case EEnemyState::Strafe:
		ToggleFocus();
		break;

	case EEnemyState::Attack:
		ClearFocus(EAIFocusPriority::Gameplay);
		break;

	case EEnemyState::Search:
		ClearFocus(EAIFocusPriority::Gameplay);
		break;

	case EEnemyState::Hit:
		ClearFocus(EAIFocusPriority::Gameplay);
		break;

	case EEnemyState::Dead:
		ClearFocus(EAIFocusPriority::Gameplay);
		break;

	case EEnemyState::FastChase:
		ClearFocus(EAIFocusPriority::Gameplay);
		break;

	default:
		break;
	}
}