#pragma once

#include <CoreMinimal.h>
#include <BehaviorTree/BTTaskNode.h>
#include <Enemy/Boss/Clown/Data/BossClownAnimationData.h>
#include "BTTask_ClownAttack.generated.h"

class ABossClown;

//---------------------------------------------------------------
// BT에서 지정한 광대 공격 실행 / 완료 대기 / 중단만 담당.
//
//---------------------------------------------------------------
UCLASS()
class ZZAKTUNGSOUL_API UBTTask_ClownAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_ClownAttack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

	UPROPERTY(EditAnywhere, Category = "Clown|Attack")
	EBossClownAttack AttackType = EBossClownAttack::HammerSweep;

	UPROPERTY(EditAnywhere, Category = "Clown|Attack")
	EClownAttackStyle Style = EClownAttackStyle::Normal;

	UPROPERTY(EditAnywhere, Category = "Clown|Attack", meta = (ClampMin = "0.1", ClampMax = "2"))
	float RecoveryScale = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Clown|Target")
	FName TargetActorKey = TEXT("TargetActor");

private:
	void CleanUp();
	TWeakObjectPtr<ABossClown> AttackingClown;
	int32 ExecutionID = INDEX_NONE;
};
