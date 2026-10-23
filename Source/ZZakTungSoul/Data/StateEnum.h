#pragma once

#include "CoreMinimal.h"
#include "StateEnum.generated.h"


UENUM(BlueprintType)
enum class EMoveState : uint8
{
	Idle,
	Walk,
	Run,
	Block,
	LockOn
};

UENUM(BlueprintType)
enum class EActionState : uint8
{
	None,
	Roll,
	BackStep,
	Attack,
	Hit
};

UENUM(BlueprintType)
enum class EOverlayState : uint8
{
	None,					// 아무런 무기를 안든 상태
	Block,					// 무기를 든 채로 방패로 막기
	BlockWithoutWeapon,		// 무기를 안 든 채로 방패로 막기
	OneHand,				// 방패 든 채로 무기 들고 있는 상태
	OneHandWithoutShield,	// 방패 안 든 채로 무기 들고 있는 상태
	TwoHand					// 두 손으로 무기 들고 있는 상태
};

//---------------------------------------------------------------
// TMap에서 키 값으로 사용하기 위해 해시함수 작성.
// 해시함수 미 작성시 키 값으로 사용 불가.
// 
//---------------------------------------------------------------
FORCEINLINE uint8 GetOverlayStateTypeHash(const EOverlayState A) {
	return static_cast<uint8>(A);
}
