#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyAIController.h"
#include "BossAIController.generated.h"

UCLASS()
class ZZAKTUNGSOUL_API ABossAIController : public AEnemyAIController
{
	GENERATED_BODY()
public:
	virtual bool RunBehaviorTree(UBehaviorTree* BTAsset) override;

	virtual void UpdateAIFocus(EEnemyState PreviousState, EEnemyState NewState) override;

private:
	UBehaviorTree* PrepareBossBehaviorTree(UBehaviorTree* SourceTree);
	void PrepareBossComposite(class UBTCompositeNode* Composite);
	void ReplaceCombatServices(TArray<TObjectPtr<class UBTService>>& Services, UObject* Outer);

	UPROPERTY(Transient)
	TMap<TObjectPtr<UBehaviorTree>, TObjectPtr<UBehaviorTree>> BossBehaviorTrees;
};
