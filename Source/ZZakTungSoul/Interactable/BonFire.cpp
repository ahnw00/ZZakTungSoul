// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable/Bonfire.h"
#include <Components/StaticMeshComponent.h>
#include <Components/SphereComponent.h>
#include <Warrior/Warrior.h>
#include <Warrior/WarriorController.h>
#include <Enemy/EnemyBase.h>
#include <Engine/OverlapResult.h>
#include <Interface/CharacterStatInterface.h>
#include <Character/CharacterStatComponent.h>


// Sets default values
ABonfire::ABonfire()
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
void ABonfire::BeginPlay()
{
	Super::BeginPlay();
	
	// 감지구체 오버랩 콜백 함수 바인딩.
	InteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &ABonfire::OnInteractionSphereBeginOverlap);
	InteractionSphere->OnComponentEndOverlap.AddDynamic(this, &ABonfire::OnInteractionSphereEndOverlap);
}

bool ABonfire::CanInteractBonfire(AWarrior* Interactor) const
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

void ABonfire::OnInteractionSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 들어온 대상이 플레이어면 상호작용 대상에 화톳불 등록.
	if (AWarrior* Warrior = Cast<AWarrior>(OtherActor))
	{
		Warrior->SetCurrentInteractable(this);

		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, TEXT("[화톳불] 화톳불 영역에 진입. 상호작용 대상 등록."));
	}

}

void ABonfire::OnInteractionSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// 나간 대상이 플레이어라면 상호작용 대상 해제.
	if (AWarrior* Warrior = Cast<AWarrior>(OtherActor))
	{
		if (Warrior->GetCurrentInteractable() == this)
		{
			Warrior->SetCurrentInteractable(nullptr);
		}

		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, TEXT("[화톳불] 화톳불 영역에 떠남. 상호작용 대상 해제."));
	}

}

void ABonfire::Interact(AWarrior* Interactor)
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

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("[화톳불] 화톳불 불을 밝힘."));
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

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("[화톳불] 화톳불 휴식 + 스탯 업그레이드 창"));

		// 화톳불 메뉴 요청.
		if (AWarriorController* PC = Interactor->GetController<AWarriorController>())
		{
			PC->OpenBonfireMenu();
		}

		// 휴식 델리게이트 방송.
		OnBonfireRest.Broadcast();
	}
}
