#pragma once

#include <CoreMinimal.h>
#include <BehaviorTree/BTTaskNode.h>
#include <Enemy/Boss/Clown/Data/BossClownAnimationData.h>
#include "BTTask_DashAttack.generated.h"

class ABossClown;

//---------------------------------------------------------------
// 기존 보스 Task 파일을 광대 공격 실행용으로 재사용.
// 클래스 이름은 기존 BT 참조 유지를 위해 유지.
//
//---------------------------------------------------------------
UCLASS()
class ZZAKTUNGSOUL_API UBTTask_DashAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_DashAttack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

	UPROPERTY(EditAnywhere, Category = "Clown|Attack")
	EBossClownAttack AttackType = EBossClownAttack::HammerSweep;

	UPROPERTY(EditAnywhere, Category = "Clown|Target")
	FName TargetActorKey = TEXT("TargetActor");

private:
	TWeakObjectPtr<ABossClown> AttackingClown;
	int32 ExecutionID = INDEX_NONE;
};
