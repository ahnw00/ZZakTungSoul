#pragma once

#include "CoreMinimal.h"
#include "EnemyStates.generated.h"

// 적의 현재 상태
UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Idle		/*기초자세*/,
	Patrol		/*순찰*/,
	Chase		/*추격*/,
	Strafe		/*간보기*/,
	Attack		/*공격*/,
	Search		/*수색*/,
	Hit			/*피격*/,
	Avoid		/*회피*/,
	Dead		/*사망*/,
	FastChase	/*빠른추격*/
};

// 적의 이동 상태 - 이동 속도 설정
UENUM(BlueprintType)
enum class EEnemyMoveState : uint8
{
	None		/*기본*/,
	Idle		/*기초자세*/,
	Patrol		/*순찰*/,
	Chase		/*추격*/,
	FastChase	/*빠른추격*/,
	Strafe		/*간보기*/,
	Search		/*수색*/,
	Slow		/*가드 및 느려지는 상황*/
};

// 적의 행동 상태 - Montage 설정
UENUM(BlueprintType)
enum class EEnemyActionState : uint8
{
	None		/*기본*/,
	Attack		/*공격*/,
	Hit			/*피격*/,
	Avoid		/*회피*/,
	Block		/*가드*/,
	Stance		/*그로기*/,
	Dead		/*사망*/
};