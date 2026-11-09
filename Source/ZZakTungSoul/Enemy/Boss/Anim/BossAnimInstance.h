#pragma once

#include <CoreMinimal.h>
#include <Animation/AnimInstance.h>
#include <Enemy/Boss/Boss.h>
#include "BossAnimInstance.generated.h"

UCLASS()
class ZZAKTUNGSOUL_API UBossAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UBossAnimInstance();

	float GetSpeed() const
	{
		return Speed;
	}

protected:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	// 애니메이션을 사용하는 보스.
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Character", Meta = (AllowPrivateAccess = true))
	TObjectPtr<ABoss> Owner;
	// 보스의 현재 이동 정보.
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Character", Meta = (AllowPrivateAccess = true))
	FVector Velocity = FVector::ZeroVector;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Character", Meta = (AllowPrivateAccess = true))
	float Direction = 0.0f;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Character", Meta = (AllowPrivateAccess = true))
	float Speed = 0.0f;
	// 보스의 현재 전투 페이즈.
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Character", Meta = (AllowPrivateAccess = true))
	EBossPhase BossPhase = EBossPhase::Intro;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	EEnemyState EnemyState = EEnemyState::Idle;
};
