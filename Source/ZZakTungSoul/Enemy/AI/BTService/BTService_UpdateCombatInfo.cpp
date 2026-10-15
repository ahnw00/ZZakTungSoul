// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/AI/BTService/BTService_UpdateCombatInfo.h"
#include <Enemy/AI/EnemyAIBlackboardKey.h>

#include <AIController.h>
#include <BehaviorTree/BlackboardComponent.h>

UBTService_UpdateCombatInfo::UBTService_UpdateCombatInfo()
{
	NodeName = TEXT("Update Combat Info");

	Interval = 0.2f;
	RandomDeviation = 0.0f;
}

void UBTService_UpdateCombatInfo::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// Get Blackboard
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (nullptr == BlackboardComp)
	{
		return;
	}

	// Get EnemyPawn
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();

	// Blackboard에 저장된 TargetActor 가져오기
	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(BBKEY_TARGETACTOR));

	// 조종하는 Pawn이나 TargetActor가 존재하지 않거나 파괴 중이면 값 초기화후 건너뛰기
	if (!IsValid(ControllingPawn) || !IsValid(TargetActor))
	{
		// 이전에 저장된 거리 및 범위정보 초기화
		BlackboardComp->SetValueAsFloat(BBKEY_TARGETDISTANCE, -1.0f);
		BlackboardComp->SetValueAsBool(BBKEY_ISINCOMBATRANGE, false);
		BlackboardComp->SetValueAsBool(BBKEY_ISINATTACKRANGE, false);
		BlackboardComp->SetValueAsBool(BBKEY_ISCLOSETOTARGET, false);

		return;
	}

	// ControllingPawn과 TargetActor 사이의 거리
	const float Distance = FVector::Dist(ControllingPawn->GetActorLocation(), TargetActor->GetActorLocation());

	// 전투 상태 범위 진입 판정
	const bool bInCombatRange = Distance <= CombatRange;

	// 공격 범위 진입 판정
	const bool bInAttackRange = Distance <= Attackrange;

	// 플레이어와 최소 유지 거리 판정
	const bool bTooClose = Distance <= MinMaintainDistance;

	// Blackboard 거리값 갱신
	BlackboardComp->SetValueAsFloat(BBKEY_TARGETDISTANCE, Distance);

	// Blackboard 전투상태 플래그 갱신
	BlackboardComp->SetValueAsBool(BBKEY_ISINCOMBATRANGE, bInCombatRange);

	// Blackboard 공격상태 플래그 갱신
	BlackboardComp->SetValueAsBool(BBKEY_ISINATTACKRANGE, bInAttackRange);

	// Blackboard 최소유지거리 플래그 갱신
	BlackboardComp->SetValueAsBool(BBKEY_ISCLOSETOTARGET, bTooClose);
}
