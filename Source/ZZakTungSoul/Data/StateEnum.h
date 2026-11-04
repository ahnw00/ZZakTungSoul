#pragma once

#include "CoreMinimal.h"
#include "StateEnum.generated.h"


UENUM(BlueprintType)
enum class EMoveState : uint8
{
	Idle,
	Walk,
	Run,
	//Block,
	//LockOn
};

UENUM(BlueprintType)
enum class EActionState : uint8
{
	None,
	Roll,
	BackStep,
	Attack,
	ChargeAttack,
	FrontFatalAttack,
	BackFatalAttack,
	Block,
	Parry,
	Hit,
	Stun,
	PickUp,
	Drink
};

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	None,				// 아무런 무기를 안든 상태
	//OneHand,			// 한손 무기
	TwoHand,			// 두손 무기	
	OneHandWithShield,	// 방패 든 채로 무기 들고 있는 상태
	Katana,
};


//---------------------------------------------------------------
// TMap에서 키 값으로 사용하기 위해 해시함수 작성.
// 해시함수 미 작성시 키 값으로 사용 불가.
// 
//---------------------------------------------------------------
FORCEINLINE uint8 GetOverlayStateTypeHash(const EWeaponState A) {
	return static_cast<uint8>(A);
}
