#pragma once

#include <CoreMinimal.h>
#include <BehaviorTree/BTDecorator.h>
#include "BTDecorator_ClownYabawiReady.generated.h"

UCLASS()
class ZZAKTUNGSOUL_API UBTDecorator_ClownYabawiReady : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_ClownYabawiReady();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
