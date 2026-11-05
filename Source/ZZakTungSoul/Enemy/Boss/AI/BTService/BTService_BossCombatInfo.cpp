#include <Enemy/Boss/AI/BTService/BTService_BossCombatInfo.h>
#include <Enemy/Boss/Boss.h>
#include <Enemy/Boss/Data/BossAIData.h>
#include <Enemy/AI/EnemyAIBlackboardKey.h>
#include <AIController.h>
#include <BehaviorTree/BlackboardComponent.h>
#include <Kismet/GameplayStatics.h>

UBTService_BossCombatInfo::UBTService_BossCombatInfo()
{
	NodeName = TEXT("Boss Capsule Surface Distance");
	Interval = 0.1f;
	RandomDeviation = 0.0f;
	bCallTickOnSearchStart = true;
}

void UBTService_BossCombatInfo::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	AAIController* AI = OwnerComp.GetAIOwner();
	ABoss* Boss = AI ? Cast<ABoss>(AI->GetPawn()) : nullptr;
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	const UBossAIData* Data = Boss ? Boss->GetBossAIData() : nullptr;

	if (!Boss || !BB)
	{
		return;
	}

	if (!Data || Boss->IsDead())
	{
		BB->SetValueAsFloat(BBKEY_TARGETDISTANCE, TNumericLimits<float>::Max());
		BB->SetValueAsBool(BBKEY_HASLINEOFSIGHT, false);
		BB->SetValueAsBool(BBKEY_ISINATTACKRANGE, false);
		BB->SetValueAsBool(BBKEY_ISCLOSETOTARGET, false);

		return;
	}

	AActor* Target = Cast<AActor>(BB->GetValueAsObject(BBKEY_TARGETACTOR));

	if (!IsValid(Target))
	{
		Target = UGameplayStatics::GetPlayerPawn(Boss, 0);
	}

	if (!IsValid(Target))
	{
		BB->ClearValue(BBKEY_TARGETACTOR);
		BB->SetValueAsFloat(BBKEY_TARGETDISTANCE, TNumericLimits<float>::Max());
		BB->SetValueAsBool(BBKEY_HASLINEOFSIGHT, false);
		BB->SetValueAsBool(BBKEY_ISINATTACKRANGE, false);
		BB->SetValueAsBool(BBKEY_ISCLOSETOTARGET, false);

		return;
	}

	// 보스전에서는 시야나 가림 여부로 타깃을 잃지 않음.
	const float Distance = Boss->GetTargetSurfaceDistance(Target);
	BB->SetValueAsObject(BBKEY_TARGETACTOR, Target);
	BB->SetValueAsBool(BBKEY_HASLINEOFSIGHT, true);
	BB->SetValueAsFloat(BBKEY_TARGETDISTANCE, Distance);
	BB->SetValueAsBool(BBKEY_ISINATTACKRANGE, Distance <= Data->AttackRange);
	BB->SetValueAsBool(BBKEY_ISCLOSETOTARGET, Distance <= Data->MinMaintainDistance);
}
