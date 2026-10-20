// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable/BonFire.h"
#include <Components/StaticMeshComponent.h>
#include <Components/SphereComponent.h>
#include <Warrior/Warrior.h>
#include <Enemy/EnemyBase.h>
#include <Engine/OverlapResult.h>
#include <Interface/CharacterStatInterface.h>
#include <Character/CharacterStatComponent.h>


// Sets default values
ABonFire::ABonFire()
{
	PrimaryActorTick.bCanEverTick = false;
	
	// 외형 메시 컴포넌트 지정.
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;
	// 상호작용 감지용 구체 콜리전.
	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphere->SetupAttachment(RootComponent);
	InteractionSphere->SetSphereRadius(200.f);
	// 플레이어만 감지하도록 설정.
	InteractionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

}
// Called when the game starts or when spawned
void ABonFire::BeginPlay()
{
	Super::BeginPlay();
	
}

bool ABonFire::CanInteractBonfire(AWarrior* Interactor) const
{
	if (!Interactor)
	{
		return false;
	}

	// 1차 검사: 플레이어가 락온 중(전투 중).
	if (Interactor->IsLockOn())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Bonfire] 전투 중에는 화톳불 사용 불가."));
		return false;
	}

	// 2차 검사: 일정 범위 내 폰 탐색.
	TArray<FOverlapResult> OverlapResults;
	FCollisionShape SphereShape = FCollisionShape::MakeSphere(EnemyCheckRadius);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(Interactor);

	bool bHasOverlap = GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		GetActorLocation(),
		FQuat::Identity,
		ECC_Pawn,
		SphereShape,
		QueryParams
	);

	if (bHasOverlap)
	{
		for (const FOverlapResult& Overlap : OverlapResults)
		{
			if (AEnemyBase* Enemy = Cast<AEnemyBase>(Overlap.GetActor()))
			{
				if (!Enemy->IsDead())
				{
					UE_LOG(LogTemp, Warning, TEXT("[Bonfire] 주변에 적이 있습니다."));
					return false;
				}
			}
		}
	}
	return true;
}

void ABonFire::Interact(AWarrior* Interactor)
{
	// 검사.
	if (!CanInteractBonfire(Interactor))
	{
		return;
	}
	// 최초 상호작용.
	if (BonfireState == EBonfireState::Unlit)
	{
		BonfireState = EBonfireState::Lit;
		return;
	}
	// 이후 상호작용.
	if (BonfireState == EBonfireState::Lit)
	{
		if (ICharacterStatInterface* StatOwner = Cast<ICharacterStatInterface>(Interactor))
		{
			if (UCharacterStatComponent* StatComp = StatOwner->GetCharacterStatComponent())
			{
				StatComp->SetCurrentHP(StatComp->GetMaxHP());
				StatComp->SetCurrentFP(StatComp->GetMaxFP());
				StatComp->SetCurrentStamina(StatComp->GetMaxStamina());
			}
		}

		// 휴식 방송.
		OnBonfireRest.Broadcast();
	}
}
