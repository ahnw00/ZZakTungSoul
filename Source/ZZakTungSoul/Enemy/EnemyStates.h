#pragma once

#include "CoreMinimal.h"
#include "EnemyStates.generated.h"

// 적의 현재 행동 상태
UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	None		/*이 상태여야 다른 상태로 넘어갈 수 있어*/,
	Idle		/*기초자세*/,
	Patrol		/*순찰*/,
	Chase		/*추격*/,
	Strafe		/*간보기*/,
	Attack		/*공격*/,
	Search		/*수색*/,
	Hit			/*피격*/,
	Groggy		/*그로기*/,
	Avoid		/*회피*/,
	Dead		/*사망*/,
	FastChase	/*빠른추격*/
};