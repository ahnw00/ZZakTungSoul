#pragma once

#include <CoreMinimal.h>
#include <BehaviorTree/BTDecorator.h>
#include <Enemy/Boss/Clown/Data/BossClownAnimationData.h>
#include "BTDecorator_ClownCondition.generated.h"
// BT 분기의 실행 조건. 우선순위 / 쿨다운 / 다음 행동은 BT에서 관리.
UCLASS()
class ZZAKTUNGSOUL_API UBTDecorator_ClownCondition : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_ClownCondition();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	UPROPERTY(EditAnywhere, Category = "Clown|Condition")
	bool bCheckAttackData = true;

	UPROPERTY(EditAnywhere, Category = "Clown|Condition")
	EBossClownAttack AttackType = EBossClownAttack::HammerSweep;

	UPROPERTY(EditAnywhere, Category = "Clown|Condition", meta = (ClampMin = "0"))
	float MinDistance = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Clown|Condition", meta = (ClampMin = "0"))
	float MaxDistance = 180.0f;

	UPROPERTY(EditAnywhere, Category = "Clown|Condition", meta = (ClampMin = "0", ClampMax = "1", ToolTip = "이 분기의 거리와 방향 조건을 만족했을 때의 통과 확률. 셀렉터는 위쪽 분기부터 평가합니다."))
	float Chance = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Clown|Condition", meta = (ClampMin = "-1", ClampMax = "1"))
	float MinimumFacingDot = 0.5f;
};
