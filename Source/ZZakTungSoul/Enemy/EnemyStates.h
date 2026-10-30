#pragma once

#include "CoreMinimal.h"
#include "EnemyStates.generated.h"

// 적의 현재 행동 상태
UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Idle,		// 기초자세
	Patrol,		// 순찰
	Chase,		// 추격
	Strafe,		// 간보기
	Attack,		// 공격
	Search,		// 수색
	Hit,		// 피격
	Dead,		// 사망


	FastChase,	// 빠르게 추격
};