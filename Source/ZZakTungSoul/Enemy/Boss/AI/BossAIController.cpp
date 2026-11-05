// Fill out your copyright notice in the Description page of Project Settings.

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
#include <Enemy/Boss/Clown/BossClown.h>
#include <Enemy/Boss/Data/BossAIData.h>
#include <Enemy/Boss/AI/BTTask/BTTask_ClownAttack.h>
#include <Enemy/Boss/AI/BTDecorator/BTDecorator_ClownYabawiReady.h>
#include <BehaviorTree/Composites/BTComposite_Selector.h>

ABossAIController::ABossAIController()
{
}

void ABossAIController::UpdateAIFocus(EEnemyState PreviousState, EEnemyState NewState)
{
	switch (NewState)
	{
	case EEnemyState::Idle:
		ClearFocus(EAIFocusPriority::Gameplay);
		break;

	case EEnemyState::Patrol:
		ClearFocus(EAIFocusPriority::Gameplay);
		break;

	case EEnemyState::Chase:
		ClearFocus(EAIFocusPriority::Gameplay);
		break;

	case EEnemyState::Strafe:
		ToggleFocus();
		break;

	case EEnemyState::Attack:
		ClearFocus(EAIFocusPriority::Gameplay);
		break;

	case EEnemyState::Search:
		ClearFocus(EAIFocusPriority::Gameplay);
		break;

	case EEnemyState::Hit:
		ClearFocus(EAIFocusPriority::Gameplay);
		break;

	case EEnemyState::Dead:
		ClearFocus(EAIFocusPriority::Gameplay);
		break;

	case EEnemyState::FastChase:
		ClearFocus(EAIFocusPriority::Gameplay);
		break;

	default:
		break;
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
	const ABossClown* Clown = Cast<ABossClown>(GetPawn());
	const UBossAIData* Data = Clown ? Clown->GetBossAIData() : nullptr;

	if (Data && SourceTree == Data->Phase2BT.Get())
	{
		// 기존 2페이즈 트리는 유지하고 런타임 복제본 앞에 야바위 분기를 추가.
		UBTComposite_Selector* Root = NewObject<UBTComposite_Selector>(BossTree);
		UBTTask_ClownAttack* Yabawi = NewObject<UBTTask_ClownAttack>(BossTree);
		Yabawi->SetAttackType(EBossClownAttack::Yabawi);
		FBTCompositeChild Choice;
		Choice.ChildTask = Yabawi;
		Choice.Decorators.Add(NewObject<UBTDecorator_ClownYabawiReady>(BossTree));
		Choice.DecoratorOps.Add(FBTDecoratorLogic(EBTDecoratorLogic::Test, 0));
		Root->Children.Add(Choice);
		FBTCompositeChild Existing;
		Existing.ChildComposite = BossTree->RootNode;
		Root->Children.Add(Existing);
		BossTree->RootNode = Root;
	}

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
				FObjectPropertyBase* BehaviorProperty =
					FindFProperty<FObjectPropertyBase>(UBTTask_RunBehavior::StaticClass(), TEXT("BehaviorAsset"));

				if (BehaviorProperty && RunBehavior->GetSubtreeAsset())
				{
					BehaviorProperty->SetObjectPropertyValue_InContainer(
						RunBehavior, PrepareBossBehaviorTree(RunBehavior->GetSubtreeAsset()));
				}
			}
		}
	}
}
