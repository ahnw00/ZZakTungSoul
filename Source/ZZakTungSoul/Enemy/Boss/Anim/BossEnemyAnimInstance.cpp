// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Boss/Anim/BossEnemyAnimInstance.h"
#include <Enemy/Boss/EnemyBoss.h>

#include <GameFramework/CharacterMovementComponent.h>


UBossEnemyAnimInstance::UBossEnemyAnimInstance()
{

}

void UBossEnemyAnimInstance::NativeInitializeAnimation()
{
	// 자신을 소유하고 있는 Enemy 저장
	Owner = Cast<AEnemyBoss>(TryGetPawnOwner());
}

void UBossEnemyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (!Owner)
	{
		return;
	}



}
