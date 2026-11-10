// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/NormalEnemy/Ambush/AmbushTrigger.h"
#include <Enemy/NormalEnemy/AmbushEnemy.h>
#include <Enemy/Spawner/EnemySpawner.h>
#include <Warrior/Warrior.h>

#include <Components/BoxComponent.h>
#include <Components/CapsuleComponent.h>

// Sets default values
AAmbushTrigger::AAmbushTrigger()
{
    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));

    RootComponent = TriggerBox;

    TriggerBox->SetBoxExtent(FVector(150.0f, 150.0f, 100.0f));

    TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TriggerBox->SetCollisionResponseToChannels(ECR_Ignore);
    TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    TriggerBox->SetGenerateOverlapEvents(true);

    TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AAmbushTrigger::OnTriggerBeginOverlap);
}

void AAmbushTrigger::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    AWarrior* Player = Cast<AWarrior>(OtherActor);
    if (false == IsValid(Player) || Player->IsDead())
    {
        return;
    }

    if (OtherComp != Player->GetCapsuleComponent())
    {
        return;
    }

    if (false == IsValid(EnemySpawner))
    {
        return;
    }

    AAmbushEnemy* AmbushEnemy = Cast<AAmbushEnemy>(EnemySpawner->GetSpawnEnemy(SpawnIndex));
    if (false == IsValid(AmbushEnemy))
    {
        return;
    }

    // 매복 상태 검사
    if (AmbushEnemy->GetAmbushState() != EAmbushState::Waiting)
    {
        return;
    }

    // 기습 발동
    if (AmbushEnemy->ActiveAmbush(Player))
    {

    }
}