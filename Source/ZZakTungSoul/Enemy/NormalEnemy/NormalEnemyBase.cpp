// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/NormalEnemy/NormalEnemyBase.h"
#include <Enemy/Data/EnemyAnimationMontageData.h>
#include <Enemy/Animation/NormalEnemyAnimInstance.h>
#include <Enemy/Data/EnemyAttackData.h>

bool ANormalEnemyBase::GetActionDamageInfo(int32 Index, FDamageInfo& DamageInfo) const
{
    if (GetEnemyActionState() != EEnemyActionState::Attack)
    {
        return false;
    }

    if (false == IsValid(ActionMontageData))
    {
        return false;
    }

    const FEnemyAttackData* AttackData = nullptr;

    switch (CurrentSelectAttack)
    {
    case EEnemySelectAttack::None:
        return false;

    case EEnemySelectAttack::Attack01:
        AttackData = &ActionMontageData->ComboAttack01;
        break;

    case EEnemySelectAttack::Attack02:
        AttackData = &ActionMontageData->ComboAttack02;
        break;

    default:
        return false;
    }

    if (false == AttackData->DamageInfo.IsValidIndex(Index))
    {
        return false;
    }

    DamageInfo = AttackData->DamageInfo[Index];

    return true;
}

void ANormalEnemyBase::DeadStart()
{
    Super::DeadStart();

    SetActorEnableCollision(false);

    UNormalEnemyAnimInstance* AnimInstance = Cast<UNormalEnemyAnimInstance>(GetMesh()->GetAnimInstance());
    if (!AnimInstance)
    {
        return;
    }

    AnimInstance->PlayActionMontage();
}
