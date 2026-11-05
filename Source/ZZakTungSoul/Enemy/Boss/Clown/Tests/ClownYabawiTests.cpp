#if WITH_DEV_AUTOMATION_TESTS

#include <Misc/AutomationTest.h>
#include <Enemy/Boss/Clown/BossClown.h>
#include <Enemy/Boss/Clown/Components/ClownYabawiComponent.h>
#include <Enemy/Boss/Clown/Gimmick/ClownYabawiBowl.h>
#include <Enemy/Components/EnemyStatComponent.h>
#include <Character/CharacterStatComponent.h>
#include <Warrior/Warrior.h>
#include <Components/StaticMeshComponent.h>
#include <Components/CapsuleComponent.h>
#include <Engine/Engine.h>
#include <Engine/World.h>
#include <EngineUtils.h>
#include <UObject/StrongObjectPtr.h>
#include <UObject/UnrealType.h>
#include <HAL/IConsoleManager.h>
#include <Containers/Ticker.h>

// PIE 안에서만 실행. 실제 상호작용/체력/타임아웃을 확인하고 전투를 대기 상태로 복원.
struct FYabawiTestState
{
	FAutomationTestBase* Test=nullptr;
	TWeakObjectPtr<ABossClown> Boss;
	TWeakObjectPtr<AWarrior> Player;
	TStrongObjectPtr<UBossClownAnimationData> OriginalData;
	TStrongObjectPtr<UBossClownAnimationData> TestData;
	TWeakObjectPtr<AClownYabawiBowl> Correct;
	TWeakObjectPtr<AClownYabawiBowl> Other;
	int32 Scenario=0;
	int32 RoundID=INDEX_NONE;
	float PlayerHP=0.0f;
	float BossHP=0.0f;
	double Started=0.0;
	bool bChose=false;
	bool bSawSelection=false;

	void Restore()
	{
		if (Boss.IsValid())
		{
			Boss->YabawiComponent->CancelRound(true);
			FObjectPropertyBase* Property=FindFProperty<FObjectPropertyBase>(ABossClown::StaticClass(),TEXT("ClownAnimationData"));
			Property->SetObjectPropertyValue_InContainer(Boss.Get(),OriginalData.Get());
			Boss->ResetEncounter();
		}
		if (Player.IsValid())
		{
			Player->GetCharacterStatComponent()->SetCurrentHP(Player->GetCharacterStatComponent()->GetMaxHP());
			Player->SetActorEnableCollision(true);
		}
	}

	bool Begin()
	{
		ABossClown* Owner=Boss.Get();
		AWarrior* Pawn=Player.Get();
		Owner->YabawiComponent->CancelRound(true);
		Correct.Reset();
		Other.Reset();
		bChose=false;
		bSawSelection=false;
		TestData->Yabawi.BowlCount=Scenario==1 ? 4 : 3;
		FEnumProperty* Phase=FindFProperty<FEnumProperty>(ABoss::StaticClass(),TEXT("BossPhase"));
		Phase->GetUnderlyingProperty()->SetIntPropertyValue(Phase->ContainerPtrToValuePtr<void>(Owner),static_cast<int64>(EBossPhase::Phase2));
		UCharacterStatComponent* Stats=Pawn->GetCharacterStatComponent();
		Stats->SetCurrentHP(Stats->GetMaxHP()*(Scenario==0 ? 0.3f : 1.0f));
		PlayerHP=Stats->GetCurrentHP();
		UEnemyStatComponent* BossStats=Owner->FindComponentByClass<UEnemyStatComponent>();
		BossStats->SetCurrentHp(BossStats->GetMaxHp());
		BossHP=BossStats->GetCurrentHp();
		Pawn->SetActorLocation(Owner->GetActorLocation()+Owner->GetActorForwardVector()*1000.0f,false,nullptr,ETeleportType::TeleportPhysics);
		int32 ExecutionID=INDEX_NONE;
		const bool bStarted=Owner->StartClownAttack(EBossClownAttack::Yabawi,Pawn,ExecutionID);
		Test->TestTrue(TEXT("Round starts only in phase 2"),bStarted);
		if (!bStarted) { return false; }
		RoundID=Owner->YabawiComponent->GetRoundID();
		int32 Count=0;
		for (TActorIterator<AClownYabawiBowl> It(Owner->GetWorld()); It; ++It)
		{
			if (It->GetOwner()!=Owner || It->IsActorBeingDestroyed()) { continue; }
			++Count;
			TArray<UStaticMeshComponent*> Meshes;
			It->GetComponents(Meshes);
			const bool bBall=Meshes.ContainsByPredicate([](UStaticMeshComponent* Mesh) { return Mesh->GetFName()==TEXT("HiddenBall") && Mesh->IsVisible(); });
			if (bBall) { Correct=*It; }
			else { Other=*It; }
		}
		Test->TestEqual(TEXT("Bowl count comes from data"),Count,TestData->Yabawi.BowlCount);
		Test->TestTrue(TEXT("Exactly one revealed winning bowl"),Correct.IsValid() && Other.IsValid());
		Test->TestFalse(TEXT("Cannot select before shuffling"),Owner->YabawiComponent->SelectBowl(Pawn,Correct.Get(),RoundID));
		return Correct.IsValid() && Other.IsValid();
	}
};

DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FRunClownYabawiScenarios,TSharedPtr<FYabawiTestState>,State);
bool FRunClownYabawiScenarios::Update()
{
	if (!State->Boss.IsValid() || !State->Player.IsValid() || FPlatformTime::Seconds()-State->Started>35.0)
	{
		State->Test->AddError(TEXT("Yabawi test lost PIE actors or timed out"));
		State->Restore();
		return true;
	}
	ABossClown* Boss=State->Boss.Get();
	AWarrior* Player=State->Player.Get();
	UClownYabawiComponent* Round=Boss->YabawiComponent;
	if (Round->GetStage()==EClownYabawiStage::Selection)
	{
		State->bSawSelection=true;
		AClownYabawiBowl* Choice=State->Scenario==1 ? State->Other.Get() : State->Correct.Get();
		Player->SetActorLocation(Choice->GetActorLocation()+FVector(0.0f,0.0f,Player->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()),false,nullptr,ETeleportType::TeleportPhysics);
		if (!State->bChose && State->Scenario<2 && Round->GetSelectionRemaining()<4.7f)
		{
			State->Test->TestFalse(TEXT("Wrong player rejected"),Round->SelectBowl(Boss,Choice,State->RoundID));
			State->Test->TestFalse(TEXT("Stale round rejected"),Round->SelectBowl(Player,Choice,State->RoundID-1));
			// 기존 E키가 호출하는 플레이어 Interact 경로를 그대로 검증.
			Player->Interact();
			State->bChose=true;
			State->Test->TestEqual(TEXT("Existing interaction resolves the choice"),Round->GetStage(),EClownYabawiStage::Resolving);
			State->Test->TestFalse(TEXT("Duplicate choice rejected"),Round->SelectBowl(Player,Choice,State->RoundID));
		}
	}
	if (Round->GetStage()!=EClownYabawiStage::None) { return false; }
	State->Test->TestTrue(TEXT("Shuffling opens the selection window"),State->bSawSelection);
	State->Test->TestFalse(TEXT("Completed round respects cooldown"),Round->CanStart());
	const float PlayerHP=Player->GetCharacterStatComponent()->GetCurrentHP();
	const float BossHP=Boss->FindComponentByClass<UEnemyStatComponent>()->GetCurrentHp();
	if (State->Scenario==0)
	{
		State->Test->TestNearlyEqual(TEXT("Correct answer heals player once"),PlayerHP,State->PlayerHP+Player->GetCharacterStatComponent()->GetMaxHP()*0.2f,0.1f);
		State->Test->TestNearlyEqual(TEXT("Correct answer damages boss once"),BossHP,State->BossHP-Boss->FindComponentByClass<UEnemyStatComponent>()->GetMaxHp()*0.05f,0.1f);
	}
	else
	{
		State->Test->TestNearlyEqual(State->Scenario==1 ? TEXT("Wrong answer explodes once") : TEXT("Five second timeout explodes once"),PlayerHP,State->PlayerHP-25.0f,0.1f);
		State->Test->TestNearlyEqual(TEXT("Failure gives no boss damage"),BossHP,State->BossHP,0.1f);
	}
	if (++State->Scenario<3)
	{
		if (State->Begin()) { return false; }
	}
	if (State->Begin())
	{
		const int32 CancelledID=Round->GetRoundID();
		Boss->ResetEncounter();
		State->Test->TestEqual(TEXT("Reset cancels all stages"),Round->GetStage(),EClownYabawiStage::None);
		State->Test->TestFalse(TEXT("Cancelled callback rejected"),Round->SelectBowl(Player,State->Correct.Get(),CancelledID));
	}
	State->Restore();
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FClownYabawiFlowTest,"ZZakTungSoul.Boss.Clown.Yabawi",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FClownYabawiFlowTest::RunTest(const FString& Parameters)
{
	UWorld* World=nullptr;
	for (const FWorldContext& Context : GEngine->GetWorldContexts()) { if (Context.WorldType==EWorldType::PIE) { World=Context.World(); break; } }
	if (!World) { AddError(TEXT("Run this test in PIE with a boss and player")); return false; }
	TSharedPtr<FYabawiTestState> State=MakeShared<FYabawiTestState>();
	State->Test=this;
	for (TActorIterator<ABossClown> It(World); It; ++It) { State->Boss=*It; break; }
	for (TActorIterator<AWarrior> It(World); It; ++It) { State->Player=*It; break; }
	if (!State->Boss.IsValid() || !State->Player.IsValid()) { AddError(TEXT("PIE boss/player missing")); return false; }
	ABossClown* Boss=State->Boss.Get();
	State->OriginalData.Reset(const_cast<UBossClownAnimationData*>(Boss->GetClownAnimationData()));
	State->TestData.Reset(DuplicateObject<UBossClownAnimationData>(State->OriginalData.Get(),GetTransientPackage()));
	State->TestData->Yabawi.RevealTime=0.5f;
	State->TestData->Yabawi.CoverTime=0.2f;
	State->TestData->Yabawi.ShuffleCount=3;
	State->TestData->Yabawi.ShuffleStepTime=0.12f;
	State->TestData->Yabawi.ExplosionDamage.AttackDamage=25.0f;
	State->TestData->Yabawi.ExplosionDamage.bIgnoreInvincibility=true;
	State->TestData->Yabawi.ExplosionDamage.bUnblockable=true;
	State->TestData->Yabawi.ExplosionRadius=2500.0f;
	State->TestData->Yabawi.PlayerHealRatio=0.2f;
	State->TestData->Yabawi.BossDamageRatio=0.05f;
	Boss->ResetEncounter();
	FObjectPropertyBase* Property=FindFProperty<FObjectPropertyBase>(ABossClown::StaticClass(),TEXT("ClownAnimationData"));
	Property->SetObjectPropertyValue_InContainer(Boss,State->TestData.Get());
	State->Player->SetActorEnableCollision(false);
	TestFalse(TEXT("Phase 1/waiting cannot start yabawi"),Boss->YabawiComponent->CanStart());
	State->Started=FPlatformTime::Seconds();
	if (!State->Begin()) { State->Restore(); return false; }
	ADD_LATENT_AUTOMATION_COMMAND(FRunClownYabawiScenarios(State));
	return true;
}

// AutomationController의 기본 PIE 종료를 거치지 않는 개발용 테스트 진입점.
static FAutoConsoleCommand RunYabawiPIETest(TEXT("Clown.TestYabawi"),TEXT("Run yabawi scenarios in the current PIE world"),FConsoleCommandDelegate::CreateLambda([]()
{
	{
		TGuardValue<bool> EditorWorldScope(GIsPlayInEditorWorld,false);
		FAutomationTestFramework::Get().StartTestByName(TEXT("ZZakTungSoul.Boss.Clown.Yabawi"),0);
	}
	if (!GIsAutomationTesting) { return; }
	FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([](float DeltaTime)
	{
		if (!GIsAutomationTesting) { return false; }
		if (!FAutomationTestFramework::Get().ExecuteLatentCommands()) { return true; }
		FAutomationTestExecutionInfo Info;
		const bool bPassed=FAutomationTestFramework::Get().StopTest(Info);
		UE_LOG(LogTemp,Display,TEXT("Clown Yabawi PIE test: %s"),bPassed ? TEXT("PASSED") : TEXT("FAILED"));
		return false;
	}));
}));

#endif
