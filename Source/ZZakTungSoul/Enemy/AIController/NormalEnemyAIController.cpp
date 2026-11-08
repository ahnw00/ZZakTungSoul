// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/AIController/NormalEnemyAIController.h"
#include <Enemy/AI/EnemyAIBlackboardKey.h>

#include <BehaviorTree/BlackboardComponent.h>
#include <Perception/AIPerceptionComponent.h>
#include <Perception/AISenseConfig_Sight.h>

#include <Kismet/GameplayStatics.h>

ANormalEnemyAIController::ANormalEnemyAIController()
{
	// Perception 컴포넌트 생성
	EnemyPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));

	// 컴포넌트 설정
	SetPerceptionComponent(*EnemyPerceptionComponent);

	// 시야 객체 생성
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

	// 감지 거리
	SightConfig->SightRadius = 1200.0f;

	// 감지 놓치는 거리
	SightConfig->LoseSightRadius = 1500.0f;

	// 좌우 시야각
	SightConfig->PeripheralVisionAngleDegrees = 70.0f;

	// 마지막으로 타겟을 본 위치를 기억하는 시간
	SightConfig->SetMaxAge(5.0f);

	// 소속 감지별 감지 여부
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	// 컴포넌트에 시야 등록
	EnemyPerceptionComponent->ConfigureSense(*SightConfig);

	// 기본 감지를 Sight로 설정
	EnemyPerceptionComponent->SetDominantSense(UAISense_Sight::StaticClass());

	// 감지 상태 변경 이벤트 연결
	EnemyPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ANormalEnemyAIController::OnTargetPerceptionUpdated);
}

void ANormalEnemyAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (false == IsAIActive())
	{
		return;
	}

	if (!Actor)
	{
		return;
	}

	// 현재 플레이어인지 아닌지 확인
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (Actor != PlayerPawn)
	{
		return;
	}

	UBlackboardComponent* BlackboardComponent = GetBlackboardComponent();
	if (!BlackboardComponent)
	{
		return;
	}

	if (Stimulus.WasSuccessfullySensed())
	{
		// 플레이어 감지 성공
		BlackboardComponent->SetValueAsObject(BBKEY_TARGETACTOR, Actor);		
	}
	
	AActor* TargetActor = Cast<AActor>(BlackboardComponent->GetValueAsObject(BBKEY_TARGETACTOR));
	if (false == IsValid(TargetActor) || TargetActor != Actor)
	{
		return;
	}

	if (!GetWorldTimerManager().IsTimerActive(TargetInfoUpdateTimerHandle))
	{
		GetWorldTimerManager().SetTimer(
			TargetInfoUpdateTimerHandle,
			this,
			&ANormalEnemyAIController::UpdateTargetInfo,
			FMath::Max(0.01f, UpdateInterval),
			true
		);
	}

	UpdateTargetInfo();
}

void ANormalEnemyAIController::UpdateTargetInfo()
{
	if (false == IsAIActive())
	{
		return;
	}

	UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
	if (nullptr == BlackboardComp)
	{
		return;
	}

	// TargetActor 가져오기
	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(BBKEY_TARGETACTOR));
	if (!IsValid(TargetActor))
	{
		GetWorldTimerManager().ClearTimer(TargetInfoUpdateTimerHandle);
		BlackboardComp->SetValueAsBool(BBKEY_HASLINEOFSIGHT, false);

		UpdateAIFocusState();

		return;
	}

	APawn* ControllingPawn = GetPawn();
	if (false == IsValid(ControllingPawn) || false == IsValid(SightConfig))
	{
		return;
	}

	// 타겟과의 거리
	const float Distance = FVector::DistSquared(ControllingPawn->GetActorLocation(), TargetActor->GetActorLocation());

	const bool bIsInRange = Distance <= FMath::Square(SightConfig->LoseSightRadius);

	// 장애물 확인
	const bool bHasLOS = bIsInRange && LineOfSightTo(TargetActor);

	const bool bPreviousLOS = BlackboardComp->GetValueAsBool(BBKEY_HASLINEOFSIGHT);

	if (bPreviousLOS != bHasLOS)
	{
		BlackboardComp->SetValueAsBool(BBKEY_HASLINEOFSIGHT, bHasLOS);
		BlackboardComp->ClearValue(BBKEY_SEARCHLOCATION);

		UpdateAIFocusState();
	}

	if (false == bHasLOS)
	{
		return;
	}

	// Target 정보 갱신
	// 마지막 관측 위치 갱신
	BlackboardComp->SetValueAsVector(BBKEY_LASTKNOWNLOCATION, TargetActor->GetActorLocation());

	// Target의 이동방향 갱신
	FVector TargetVelocity = TargetActor->GetVelocity();
	TargetVelocity.Z = 0.0f;

	if (TargetVelocity.SizeSquared() >= FMath::Square(MinTargetSpeed))
	{
		BlackboardComp->SetValueAsVector(BBKEY_LASTKNOWNDIRECTION, TargetVelocity.GetSafeNormal());
	}
}

void ANormalEnemyAIController::ActiveInitialize(bool bActive)
{
	if (false == IsValid(EnemyPerceptionComponent))
	{
		return;
	}

	if (bActive)
	{
		ClearTargetInfo();

		EnemyPerceptionComponent->SetSenseEnabled(UAISense_Sight::StaticClass(), true);
		EnemyPerceptionComponent->RequestStimuliListenerUpdate();
	}
	else
	{
		EnemyPerceptionComponent->SetSenseEnabled(UAISense_Sight::StaticClass(), false);
		EnemyPerceptionComponent->ForgetAll();

		ClearTargetInfo();
	}
}

void ANormalEnemyAIController::ClearTargetInfo()
{
	GetWorldTimerManager().ClearTimer(TargetInfoUpdateTimerHandle);

	UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
	if (false == IsValid(BlackboardComp))
	{
		return;
	}

	// Blackboard 초기화
	// Target Actor
	BlackboardComp->ClearValue(BBKEY_TARGETACTOR);

	// Has Line Of Sight
	BlackboardComp->SetValueAsBool(BBKEY_HASLINEOFSIGHT, false);

	// Location
	BlackboardComp->ClearValue(BBKEY_LASTKNOWNLOCATION);
	BlackboardComp->ClearValue(BBKEY_LASTKNOWNDIRECTION);
	BlackboardComp->ClearValue(BBKEY_SEARCHLOCATION);
	BlackboardComp->ClearValue(BBKEY_STRAFELOCATION);

	BlackboardComp->SetValueAsBool(BBKEY_ISCLOSETOTARGET, false);
	BlackboardComp->SetValueAsBool(BBKEY_ISINATTACKRANGE, false);
	BlackboardComp->SetValueAsBool(BBKEY_ISINCOMBATRANGE, false);

	BlackboardComp->SetValueAsFloat(BBKEY_TARGETDISTANCE, -1.0f);

	ClearFocus(EAIFocusPriority::Gameplay);
}
