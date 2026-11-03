#pragma once

#include <CoreMinimal.h>
#include <BehaviorTree/BTService.h>
#include "BTService_BossCombatInfo.generated.h"

UCLASS()
class ZZAKTUNGSOUL_API UBTService_BossCombatInfo : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_BossCombatInfo();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
