// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable/Bonfire.h"
#include <Engine/OverlapResult.h>

#include <GamePlay/ZTSGameMode.h>

#include <Components/StaticMeshComponent.h>
#include <Components/SphereComponent.h>
#include <Components/ArrowComponent.h>

#include <Character/CharacterStatComponent.h>

#include <Warrior/Warrior.h>
#include <Warrior/WarriorController.h>
#include <Warrior/WarriorPlayerState.h>

#include <Interface/CharacterStatInterface.h>

#include <Enemy/EnemyBase.h>
#include <Enemy/Spawner/EnemySpawnSubsystem.h>




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
	// 전송 착지 지점.(현재: 화톳불 앞 120cm, 화톳불 바라보는 방향).
	TeleportPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("TeleportPoint"));
	TeleportPoint->SetupAttachment(RootComponent);
	TeleportPoint->SetRelativeLocation(FVector(120.f, 0.f, 0.f));
	TeleportPoint->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));

}
FTransform ABonfire::GetTeleportTransform() const
{
	if (TeleportPoint)
	{
		return TeleportPoint->GetComponentTransform();
	}
	return GetActorTransform(); // 착지점 미설정시 화톳불 자체 지점 반환.
}
// Called when the game starts or when spawned
void ABonfire::BeginPlay()
{
	Super::BeginPlay();
	
	// 감지구체 오버랩 콜백 함수 바인딩.
	InteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &ABonfire::OnInteractionSphereBeginOverlap);
	InteractionSphere->OnComponentEndOverlap.AddDynamic(this, &ABonfire::OnInteractionSphereEndOverlap);

	// Bonfire ID 검사.
	if (BonfireID.IsNone() || BonfireID == TEXT("Bonfire_Default"))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Bonfire] %s: BonfireID가 기본값입니다. 고유 ID 지정 필요."), *GetName());
	}
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

void ABonfire::SyncLitState(const AWarriorPlayerState* PS)
{
	if (BonfireState == EBonfireState::Unlit && PS && PS->IsBonfireLit(BonfireID))
	{
		BonfireState = EBonfireState::Lit;

		// TODO: 활성화 효과 등은 여기서 활성화 예정.
	}
}

void ABonfire::OnInteractionSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 들어온 대상이 플레이어면 상호작용 대상에 화톳불 등록.
	if (AWarrior* Warrior = Cast<AWarrior>(OtherActor))
	{
		Warrior->AddDetectedObject(this);

		// 접근 시 세이브 활성화 상태 동기화.
		SyncLitState(Warrior->GetPlayerState<AWarriorPlayerState>());
		UE_LOG(LogTemp, Log, TEXT("[화톳불] 화톳불 영역에 진입. 상호작용 대상 등록."));
	}

}

void ABonfire::OnInteractionSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// 나간 대상이 플레이어라면 상호작용 대상 해제.
	if (AWarrior* Warrior = Cast<AWarrior>(OtherActor))
	{
		Warrior->RemoveDetectedObject(this);

		UE_LOG(LogTemp, Log, TEXT("[화톳불] 화톳불 영역에 떠남. 상호작용 대상 해제."));
	}

}

void ABonfire::Interact(AActor* detectedActor)
{
	// 검사.
	AWarrior* Interactor = Cast<AWarrior>(detectedActor);
	if (!Interactor || !CanInteractBonfire(Interactor))
	{
		return;
	}

	// 상호작용 직전 세이브 기반 활성화 상태 재확인.
	SyncLitState(Interactor->GetPlayerState<AWarriorPlayerState>());

	// 최초 상호작용.
	if (BonfireState == EBonfireState::Unlit)
	{
		BonfireState = EBonfireState::Lit;

		// PlayerState에 활성화한 화톳불로 등록 및 마지막 화톳불 갱신.
		if (AWarriorPlayerState* PS = Interactor->GetPlayerState<AWarriorPlayerState>())
		{
			PS->RegisterLitBonfire(BonfireID);
			PS->SetLastBonfireID(BonfireID);
		}

		// [자동 저장]
		if (AZTSGameMode* GM = Cast<AZTSGameMode>(GetWorld()->GetAuthGameMode()))
		{
			GM->SaveGame();
		}

		// 디버깅용 메세지.
		UE_LOG(LogTemp, Log, TEXT("[화톳불] 화톳불 불을 밝힘 + 자동저장."));
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

		// 마지막 상호작용 화톳불 갱신.
		if (AWarriorPlayerState* PS = Interactor->GetPlayerState<AWarriorPlayerState>())
		{
			PS->SetLastBonfireID(BonfireID);
		}

		// [자동 저장]
		if (AZTSGameMode* GM = Cast<AZTSGameMode>(GetWorld()->GetAuthGameMode()))
		{
			GM->SaveGame();
		}

		// 디버깅용 메세지.
		UE_LOG(LogTemp, Log, TEXT("[화톳불] 현재 활성화 상태 화톳불 + 자동저장."));

		// 화톳불 메뉴 요청.
		if (AWarriorController* PC = Interactor->GetController<AWarriorController>())
		{
			PC->OpenBonfireMenu();
		}

		// 휴식 델리게이트 방송.
		OnBonfireRest.Broadcast();

		// 적 초기화
		if (UEnemySpawnSubsystem* SpawnSubsystem = GetWorld()->GetSubsystem<UEnemySpawnSubsystem>())
		{
			SpawnSubsystem->ResetAllEnemies();
		}
	}
}
