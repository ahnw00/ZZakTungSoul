#include <Enemy/Boss/Clown/Components/ClownYabawiComponent.h>
#include <Enemy/Boss/Clown/Gimmick/ClownYabawiBowl.h>
#include <Enemy/Boss/Clown/BossClown.h>
#include <Interface/CombatDamageInterface.h>
#include <Engine/World.h>
#include <Components/PrimitiveComponent.h>
#include <Components/WidgetComponent.h>
#include <Enemy/Boss/Clown/UI/ClownYabawiWidget.h>
#include <Warrior/Warrior.h>
#include <TimerManager.h>

namespace
{
	constexpr float AnswerOpenTime = 0.6f;
	constexpr float PanelHeight = 220.0f;
}

UClownYabawiComponent::UClownYabawiComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

bool UClownYabawiComponent::CanStart() const
{
	const ABossClown* Boss = Cast<ABossClown>(GetOwner());
	const UBossClownAnimationData* Data = Boss ? Boss->GetClownAnimationData() : nullptr;

	return Boss && Boss->IsClownCombatPhase() && Stage == EClownYabawiStage::None &&
		GetWorld() && Data && Boss->GetClownAttackData(EBossClownAttack::Yabawi) && Data->Yabawi.BowlMesh &&
		Data->Yabawi.BallMesh && GetWorld()->GetTimeSeconds() >= ReadyTime;
}

bool UClownYabawiComponent::StartRound(AActor* Player, const TArray<FVector>& Positions)
{
	ABossClown* Boss = Cast<ABossClown>(GetOwner());

	if (!CanStart() || !IsValid(Player) || Positions.Num() < 2)
	{
		return false;
	}

	Settings = Boss->GetClownAnimationData()->Yabawi;
	Target = Player;
	RoundID = ++RoundSerial;
	CorrectBowl = FMath::RandRange(0, Positions.Num() - 1);
	SwapsCompleted = 0;
	SwapA = INDEX_NONE;
	SwapB = INDEX_NONE;
	PendingBowl.Reset();

	FActorSpawnParameters Params;
	Params.Owner = Boss;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	UClass* BowlType = Settings.BowlClass ? Settings.BowlClass.Get() : AClownYabawiBowl::StaticClass();
	FVector Center = FVector::ZeroVector;

	for (const FVector& Position : Positions)
	{
		AClownYabawiBowl* Bowl = GetWorld()->SpawnActor<AClownYabawiBowl>(BowlType, Position, Boss->GetActorRotation(), Params);

		if (!Bowl)
		{
			CancelRound();

			return false;
		}

		Bowl->Initialize(this, RoundID, Settings);
		Bowls.Add(Bowl);
		Center += Position;
	}

	Center /= Positions.Num();
	Bowls[CorrectBowl]->ShowBall(true);

	SelectionPanel = NewObject<UWidgetComponent>(Boss);
	Boss->AddInstanceComponent(SelectionPanel);
	SelectionPanel->SetupAttachment(Boss->GetRootComponent());
	SelectionPanel->SetWidgetSpace(EWidgetSpace::Screen);
	SelectionPanel->SetWidgetClass(UClownYabawiWidget::StaticClass());
	SelectionPanel->SetDrawAtDesiredSize(true);
	SelectionPanel->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SelectionPanel->RegisterComponent();
	SelectionPanel->SetAbsolute(true, true, false);
	SelectionPanel->SetWorldLocation(Center + FVector(0.0f, 0.0f, PanelHeight));
	SelectionPanel->InitWidget();

	if (UUserWidget* Widget = SelectionPanel->GetUserWidgetObject())
	{
		Widget->TakeWidget();
	}

	SetStage(EClownYabawiStage::Reveal);
	SetComponentTickEnabled(true);

	return true;
}

void UClownYabawiComponent::SetStage(EClownYabawiStage Next)
{
	Stage = Next;
	StageTime = 0.0f;
	LastPresentedSeconds = INDEX_NONE;

	for (AClownYabawiBowl* Bowl : Bowls)
	{
		if (IsValid(Bowl))
		{
			Bowl->SetAttackSelectionEnabled(Next == EClownYabawiStage::Selection);
		}
	}

	if (Next == EClownYabawiStage::Selection)
	{
		SelectionDeadline = GetWorld()->GetTimeSeconds() + FMath::Max(Settings.SelectionTime, 1.0f);
	}

	UpdatePresentation();
}

void UClownYabawiComponent::StartSwap()
{
	const int32 PreviousA = SwapA;
	const int32 PreviousB = SwapB;
	SwapA = FMath::RandRange(0, Bowls.Num() - 1);
	SwapB = (SwapA + FMath::RandRange(1, Bowls.Num() - 1)) % Bowls.Num();

	if (Bowls.Num() > 2 && ((SwapA == PreviousA && SwapB == PreviousB) || (SwapA == PreviousB && SwapB == PreviousA)))
	{
		for (int32 Index = 0; Index < Bowls.Num(); ++Index)
		{
			if (Index != SwapA && Index != SwapB)
			{
				SwapB = Index;
				break;
			}
		}
	}

	SwapStartA = Bowls[SwapA]->GetActorLocation();
	SwapStartB = Bowls[SwapB]->GetActorLocation();
	SwapTime = 0.0f;
}

void UClownYabawiComponent::UpdatePresentation()
{
	UClownYabawiWidget* Widget = SelectionPanel ? Cast<UClownYabawiWidget>(SelectionPanel->GetUserWidgetObject()) : nullptr;

	if (!Widget)
	{
		return;
	}

	const FLinearColor Gold(0.94f, 0.79f, 0.48f);
	FText Heading;
	FText Description;
	FLinearColor Color = Gold;

	switch (Stage)
	{
	case EClownYabawiStage::Reveal:
		Heading = NSLOCTEXT("ClownYabawi", "Remember", "공의 위치를 기억하세요");
		Description = NSLOCTEXT("ClownYabawi", "Watch", "섞이는 그릇을 끝까지 지켜보세요");
		break;
	case EClownYabawiStage::Cover:
	case EClownYabawiStage::Shuffle:
		Heading = NSLOCTEXT("ClownYabawi", "Shuffling", "어느 그릇에 공이 있을까요?");
		Description = NSLOCTEXT("ClownYabawi", "Wait", "그릇이 멈추면 공격해서 선택하세요");
		break;
	case EClownYabawiStage::Selection:
	{
		const int32 Seconds = FMath::CeilToInt(GetSelectionRemaining());

		if (Seconds == LastPresentedSeconds)
		{
			return;
		}

		LastPresentedSeconds = Seconds;
		Heading = NSLOCTEXT("ClownYabawi", "Choose", "공이 숨겨진 그릇을 공격하세요");
		Description = FText::Format(
			NSLOCTEXT("ClownYabawi", "Remaining", "남은 시간 {0}초 · 처음 맞힌 그릇이 선택됩니다"), FText::AsNumber(Seconds));
		break;
	}

	case EClownYabawiStage::Resolving:
		switch (RoundResult)
		{
		case ERoundResult::Correct:
			Heading = NSLOCTEXT("ClownYabawi", "Correct", "정답입니다");
			break;
		case ERoundResult::Wrong:
			Heading = NSLOCTEXT("ClownYabawi", "Wrong", "아쉽지만 틀렸습니다");
			Color = FLinearColor(0.95f, 0.48f, 0.38f);
			break;
		case ERoundResult::Timeout:
			Heading = NSLOCTEXT("ClownYabawi", "Timeout", "시간이 끝났습니다");
			Color = FLinearColor(0.95f, 0.48f, 0.38f);
			break;
		}

		Description = NSLOCTEXT("ClownYabawi", "Answer", "공이 숨겨져 있던 그릇을 확인하세요");
		break;
	default:
		return;
	}

	Widget->SetPresentation(Heading, Description, Color);
}

float UClownYabawiComponent::GetSelectionRemaining() const
{
	return Stage == EClownYabawiStage::Selection && GetWorld()
		? FMath::Max(static_cast<float>(SelectionDeadline - GetWorld()->GetTimeSeconds()), 0.0f)
		: 0.0f;
}

bool UClownYabawiComponent::SelectBowl(AActor* Player, AClownYabawiBowl* Bowl, int32 InRoundID, const FHitResult& Hit)
{
	if (Stage != EClownYabawiStage::Selection || RoundID == INDEX_NONE || InRoundID != RoundID)
	{
		return false;
	}

	if (!IsValid(Player) || Player != Target.Get() || !IsValid(Bowl) || !Bowls.Contains(Bowl) || GetSelectionRemaining() <= 0.0f)
	{
		return false;
	}

	// 한 프레임의 Sweep 결과를 모아 배열 순서가 아닌 최초 접촉 시간/거리로 선택한다.
	if (PendingBowl.IsValid() && PendingFrame != GFrameCounter)
	{
		return false;
	}

	const float Distance = FVector::DistSquared(Hit.TraceStart, Bowl->GetActorLocation());
	const float PreviousDistance = PendingBowl.IsValid() ? FVector::DistSquared(PendingHit.TraceStart, PendingBowl->GetActorLocation()) : TNumericLimits<float>::Max();

	if (!PendingBowl.IsValid() || Hit.Time < PendingHit.Time || (FMath::IsNearlyEqual(Hit.Time, PendingHit.Time) && Distance < PreviousDistance))
	{
		PendingBowl = Bowl;
		PendingHit = Hit;
		PendingFrame = GFrameCounter;
	}

	if (!GetWorld()->GetTimerManager().IsTimerActive(SelectionCommitTimer))
	{
		SelectionCommitTimer = GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UClownYabawiComponent::CommitSelection);
	}

	return true;
}

void UClownYabawiComponent::CommitSelection()
{
	AClownYabawiBowl* Bowl = PendingBowl.Get();
	PendingBowl.Reset();

	if (Stage != EClownYabawiStage::Selection || !IsValid(Bowl))
	{
		return;
	}

	const bool bCorrect = Bowls.IndexOfByKey(Bowl) == CorrectBowl;
	Bowl->BreakShell();
	Bowl->OnBowlStruck(PendingHit);
	Resolve(bCorrect ? ERoundResult::Correct : ERoundResult::Wrong);
}

void UClownYabawiComponent::Resolve(ERoundResult Result)
{
	if (Stage != EClownYabawiStage::Selection)
	{
		return;
	}

	// 피해로 사망/초기화 콜백이 발생하기 전에 선택과 중복 보상을 먼저 차단.
	RoundResult = Result;
	SetStage(EClownYabawiStage::Resolving);
	ABossClown* Boss = Cast<ABossClown>(GetOwner());
	AActor* Player = Target.Get();

	AClownYabawiBowl* Answer = Bowls[CorrectBowl];

	if (!Boss || !IsValid(Player) || !IsValid(Answer))
	{
		CancelRound();

		return;
	}

	if (SelectionPanel)
	{
		SelectionPanel->SetWorldLocation(Answer->GetActorLocation() + FVector(0.0f, 0.0f, PanelHeight));
	}

	if (Result == ERoundResult::Correct)
	{
		Boss->EnterYabawiGroggy();

		return;
	}

	// 각 그릇의 피해 범위를 합치되, 같은 플레이어에게 피해는 한 번만 적용한다.
	const float Radius = FMath::Max(Settings.ExplosionRadius, 100.0f);
	TArray<FVector> BowlLocations;
	FVector ExplosionCenter = FVector::ZeroVector;
	bool bInRange = false;

	for (AClownYabawiBowl* Bowl : Bowls)
	{
		if (!IsValid(Bowl))
		{
			continue;
		}

		BowlLocations.Add(Bowl->GetActorLocation());

		if (FVector::Dist(Player->GetActorLocation(), Bowl->GetActorLocation()) <= Radius)
		{
			bInRange = true;
			ExplosionCenter = Bowl->GetActorLocation();
		}
	}

	Boss->PlayYabawiFailureEffects(BowlLocations);

	if (bInRange)
	{
		if (ICombatDamageInterface* Victim = Cast<ICombatDamageInterface>(Player))
		{
			FHitResult Hit(Player, Cast<UPrimitiveComponent>(Player->GetRootComponent()), Player->GetActorLocation(),
				(Player->GetActorLocation() - ExplosionCenter).GetSafeNormal());
			const FDamageInfo Damage = Settings.ExplosionDamage;
			Victim->ReceiveDamage(Damage, Hit);
		}
	}
}

void UClownYabawiComponent::TickComponent(
	float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	ABossClown* Boss = Cast<ABossClown>(GetOwner());

	if (!Boss || !Boss->IsClownCombatPhase() || !Target.IsValid() || Target->IsActorBeingDestroyed() ||
		Bowls.ContainsByPredicate(
			[](const TObjectPtr<AClownYabawiBowl>& Bowl)
			{
				return !IsValid(Bowl) || Bowl->IsActorBeingDestroyed();
			}))
	{
		if (Boss)
		{
			Boss->FinishClownAttack(EBossClownAttackResult::Aborted, true);
		}
		else
		{
			CancelRound();
		}

		return;
	}

	if (const AWarrior* Player = Cast<AWarrior>(Target.Get()))
	{
		if (Player->IsDead())
		{
			Boss->FinishClownAttack(EBossClownAttackResult::Aborted, true);
			return;
		}
	}

	StageTime += DeltaTime;

	switch (Stage)
	{
	case EClownYabawiStage::Reveal:
		if (StageTime >= FMath::Max(Settings.RevealTime, 0.5f))
		{
			SetStage(EClownYabawiStage::Cover);
		}

		break;
	case EClownYabawiStage::Cover:
	{
		const float Alpha = FMath::Clamp(StageTime / FMath::Max(Settings.CoverTime, 0.2f), 0.0f, 1.0f);

		for (AClownYabawiBowl* Bowl : Bowls)
		{
			Bowl->SetCoverAlpha(FMath::SmoothStep(0.0f, 1.0f, Alpha));
		}

		if (Alpha >= 1.0f)
		{
			Bowls[CorrectBowl]->ShowBall(false);
			Settings.CoverSound.Play(this, (Bowls[0]->GetActorLocation() + Bowls.Last()->GetActorLocation()) * 0.5f);
			SetStage(EClownYabawiStage::Shuffle);
			StartSwap();
		}

		break;
	}

	case EClownYabawiStage::Shuffle:
	{
		SwapTime += DeltaTime;
		const float Alpha = FMath::Clamp(SwapTime / FMath::Clamp(Settings.ShuffleStepTime, 0.12f, 1.0f), 0.0f, 1.0f);
		const FVector Side = FVector::CrossProduct((SwapStartB - SwapStartA).GetSafeNormal2D(), FVector::UpVector) * FMath::Max(Settings.BowlDiameter, 100.0f) * 0.65f * FMath::Sin(PI * Alpha);
		Bowls[SwapA]->SetActorLocation(FMath::Lerp(SwapStartA, SwapStartB, Alpha) + Side);
		Bowls[SwapB]->SetActorLocation(FMath::Lerp(SwapStartB, SwapStartA, Alpha) - Side);

		if (Alpha >= 1.0f)
		{
			++SwapsCompleted;

			if (SwapsCompleted >= FMath::Clamp(Settings.ShuffleCount, 1, 32))
			{
				SetStage(EClownYabawiStage::Selection);
			}
			else
			{
				StartSwap();
			}
		}

		break;
	}

	case EClownYabawiStage::Selection:
	{
		if (PendingBowl.IsValid())
		{
			CommitSelection();
			return;
		}

		if (GetSelectionRemaining() <= 0.0f)
		{
			Resolve(ERoundResult::Timeout);
		}
		else
		{
			UpdatePresentation();
		}

		break;
	}

	case EClownYabawiStage::Resolving:
	{
		const float OpenAlpha = FMath::Clamp(StageTime / AnswerOpenTime, 0.0f, 1.0f);
		Bowls[CorrectBowl]->UpdateAnswerReveal(OpenAlpha, StageTime);

		const bool bCorrect = RoundResult == ERoundResult::Correct;

		if (StageTime >= FMath::Max(Settings.ResultDisplayTime, 1.0f))
		{
			Boss->CompleteClownYabawi(bCorrect);
		}

		break;
	}
	default:
		break;
	}
}

void UClownYabawiComponent::CancelRound(bool bResetPhase)
{
	const bool bWasActive = Stage != EClownYabawiStage::None || !Bowls.IsEmpty();
	Stage = EClownYabawiStage::None;
	RoundID = INDEX_NONE;

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(SelectionCommitTimer);
	}

	PendingBowl.Reset();

	if (SelectionPanel)
	{
		SelectionPanel->DestroyComponent();
		SelectionPanel = nullptr;
	}

	for (AClownYabawiBowl* Bowl : Bowls)
	{
		if (IsValid(Bowl))
		{
			Bowl->Destroy();
		}
	}

	Bowls.Reset();
	Target.Reset();
	SetComponentTickEnabled(false);

	if (bResetPhase)
	{
		ReadyTime = 0.0;
	}
	else if (bWasActive && GetWorld())
	{
		ReadyTime = GetWorld()->GetTimeSeconds() + FMath::Max(Settings.Cooldown, 0.0f);
	}
}

void UClownYabawiComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	CancelRound(true);
	Super::EndPlay(EndPlayReason);
}
