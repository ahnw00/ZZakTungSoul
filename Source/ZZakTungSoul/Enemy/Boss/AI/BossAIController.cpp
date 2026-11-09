#include "Enemy/Boss/AI/BossAIController.h"
#include <Enemy/Boss/AI/BTService/BTService_BossCombatInfo.h>
#include <Enemy/AI/BTService/BTService_UpdateCombatInfo.h>
#include <Enemy/AI/EnemyAIBlackboardKey.h>
#include <BehaviorTree/BehaviorTree.h>
#include <BehaviorTree/BTCompositeNode.h>
#include <BehaviorTree/BTTaskNode.h>
#include <BehaviorTree/Tasks/BTTask_MoveTo.h>
#include <BehaviorTree/Tasks/BTTask_RunBehavior.h>
#include <BehaviorTree/BlackboardComponent.h>
#include <UObject/UnrealType.h>

void ABossAIController::UpdateAIFocus(EEnemyState PreviousState, EEnemyState NewState)
{
	if (NewState == EEnemyState::Strafe)
	{
		ToggleFocus();
	}
	else
	{
		ClearFocus(EAIFocusPriority::Gameplay);
	}
}

bool ABossAIController::RunBehaviorTree(UBehaviorTree* BTAsset)
{
	UBehaviorTree* BossTree = PrepareBossBehaviorTree(BTAsset);

	if (!BossTree)
	{
		return false;
	}

	if (UBlackboardComponent* BB = GetBlackboardComponent())
	{
		// 공유 Blackboard의 일반 적 키는 남기되 보스의 이전 범위 판정은 폐기.
		BB->ClearValue(BBKEY_ISINCOMBATRANGE);
	}

	return Super::RunBehaviorTree(BossTree);
}

UBehaviorTree* ABossAIController::PrepareBossBehaviorTree(UBehaviorTree* SourceTree)
{
	if (!SourceTree || !SourceTree->RootNode)
	{
		return nullptr;
	}

	if (const TObjectPtr<UBehaviorTree>* Cached = BossBehaviorTrees.Find(SourceTree))
	{
		return Cached->Get();
	}

	// 원본 에셋이나 다른 컨트롤러의 BT를 변경하지 않는 보스 전용 복제본.
	UBehaviorTree* BossTree = DuplicateObject<UBehaviorTree>(SourceTree, this);
	BossTree->SetFlags(RF_Transient);
	BossBehaviorTrees.Add(SourceTree, BossTree);
	PrepareBossComposite(BossTree->RootNode);

	return BossTree;
}

void ABossAIController::ReplaceCombatServices(TArray<TObjectPtr<UBTService>>& Services, UObject* Outer)
{
	for (TObjectPtr<UBTService>& Service : Services)
	{
		if (Service && Service->IsA<UBTService_UpdateCombatInfo>())
		{
			Service = NewObject<UBTService_BossCombatInfo>(Outer);
		}
	}
}

void ABossAIController::PrepareBossComposite(UBTCompositeNode* Composite)
{
	if (!Composite)
	{
		return;
	}

	ReplaceCombatServices(Composite->Services, Composite);

	for (FBTCompositeChild& Child : Composite->Children)
	{
		PrepareBossComposite(Child.ChildComposite);

		if (UBTTaskNode* Task = Child.ChildTask.Get())
		{
			ReplaceCombatServices(Task->Services, Task);

			if (UBTTask_MoveTo* MoveTo = Cast<UBTTask_MoveTo>(Task))
			{
				// Actor 목표의 이동 완료 거리도 양쪽 캡슐 반지름을 포함.
				MoveTo->bReachTestIncludesAgentRadius = true;
				MoveTo->bReachTestIncludesGoalRadius = true;
			}

			if (UBTTask_RunBehavior* RunBehavior = Cast<UBTTask_RunBehavior>(Task))
			{
				// 실행 전 복제본에서만 하위 트리 연결을 교체. 엔진에 공개 setter가 없음.
				FObjectPropertyBase* BehaviorProperty = FindFProperty<FObjectPropertyBase>(UBTTask_RunBehavior::StaticClass(), TEXT("BehaviorAsset"));

				if (BehaviorProperty && RunBehavior->GetSubtreeAsset())
				{
					BehaviorProperty->SetObjectPropertyValue_InContainer(
						RunBehavior, PrepareBossBehaviorTree(RunBehavior->GetSubtreeAsset()));
				}
			}
		}
	}
}
