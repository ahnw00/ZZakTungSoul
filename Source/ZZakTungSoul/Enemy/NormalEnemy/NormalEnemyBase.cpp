// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/NormalEnemy/NormalEnemyBase.h"
#include <Enemy/Data/EnemyAnimationMontageData.h>
#include <Enemy/Animation/NormalEnemyAnimInstance.h>
#include <Enemy/Components/AttackTraceComponent.h>
#include <Enemy/EnemyAIController.h>
#include <Enemy/Data/EnemyAttackData.h>
#include <Enemy/Spawner/EnemySpawner.h>
#include <Item/ItemObject.h>

#include <Engine/World.h>
#include <Components/BoxComponent.h>

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

void ANormalEnemyBase::SetEnemySpawner(AEnemySpawner* InSpawner)
{
    EnemySpawner = InSpawner;
}

void ANormalEnemyBase::DeadStart()
{
    Super::DeadStart();

    EnemyDropItem();

    SetActorEnableCollision(false);

    UNormalEnemyAnimInstance* NormalEnemyAnimInstance = Cast<UNormalEnemyAnimInstance>(GetMesh()->GetAnimInstance());
    if (!NormalEnemyAnimInstance)
    {
        return;
    }

    NormalEnemyAnimInstance->PlayActionMontage();
}

void ANormalEnemyBase::HitStart()
{
    UNormalEnemyAnimInstance* NormalEnemyAnimInstance = Cast<UNormalEnemyAnimInstance>(GetMesh()->GetAnimInstance());
    if (!NormalEnemyAnimInstance)
    {
        SetEnemyActionState(EEnemyActionState::None);
        return;
    }

    AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController());
    if (IsValid(AIController))
    {
        AIController->SetAIWait(true);
        AIController->StopMovement();
    }

    if (IsValid(AttackTraceComponent))
    {
        AttackTraceComponent->EndTrace();
    }

    // Montage 실행
    if (false == NormalEnemyAnimInstance->PlayActionMontage())
    {
        SetEnemyActionState(EEnemyActionState::None);
        AIController->SetAIWait(false);
    }
}

void ANormalEnemyBase::EnemyDropItem()
{
    // 중복 드랍 방지
    if (bIsDrop)
    {
        return;
    }

    bIsDrop = true;

    // 드랍 확률
    if (nullptr == DropItemClass || FMath::FRand() >= (DropPercent / 100))
    {
        return;
    }

    UWorld* World = GetWorld();
    if (nullptr == World)
    {
        return;
    }

    // 바닥 탐색
    const FVector EnemyLocation = GetActorLocation();

    const FVector Start = EnemyLocation + FVector(0.0f, 0.0f, 50.0f);
    const FVector End = EnemyLocation - FVector(0.0f, 0.0f, 1500.0f);

    FHitResult GroundHit;

    FCollisionQueryParams Parms;
    Parms.AddIgnoredActor(this);

    // 바닥 Trace
    if (false == World->LineTraceSingleByChannel(GroundHit, Start, End, ECC_Visibility, Parms))
    {
        return;
    }

    // 박스 크기의 절반 구하기
    float BoxHalfHeight = 0.0f;

    const AItemObject* Item = DropItemClass->GetDefaultObject<AItemObject>();
    if (Item)
    {
        const UBoxComponent* Box = Cast<UBoxComponent>(Item->GetRootComponent());
        if (Box)
        {
            BoxHalfHeight = Box->GetScaledBoxExtent().Z;
        }
    }

    // 아이템 스폰
    const FVector SpawnLocation = GroundHit.ImpactPoint + FVector(0.0f, 0.0f, BoxHalfHeight);

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AItemObject* DroppedItem = World->SpawnActor<AItemObject>(
        DropItemClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams
    );

    if (IsValid(DroppedItem))
    {
        if (AEnemySpawner* Spawner = EnemySpawner.Get())
        {
            UE_LOG(LogTemp, Log, TEXT("아이템 등록"));
            Spawner->RegisterDropItem(DroppedItem);
        }
    }
}
