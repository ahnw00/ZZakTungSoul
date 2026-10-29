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
		BlackboardComponent->SetValueAsBool(BBKEY_HASLINEOFSIGHT, true);
	}
	else
	{
		// 마지막 감지 위치 저장
		BlackboardComponent->SetValueAsVector(BBKEY_LASTKNOWNLOCATION, Stimulus.StimulusLocation);
		BlackboardComponent->SetValueAsBool(BBKEY_HASLINEOFSIGHT, false);

		//BlackboardComponent->ClearValue(BBBKEY_TARGETACTOR);
	}


}