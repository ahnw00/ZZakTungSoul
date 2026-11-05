#include <Enemy/Boss/Clown/Components/ClownYabawiComponent.h>
#include <Enemy/Boss/Clown/Gimmick/ClownYabawiBowl.h>
#include <Enemy/Boss/Clown/BossClown.h>
#include <Enemy/Components/EnemyStatComponent.h>
#include <Character/CharacterStatComponent.h>
#include <Interface/CharacterStatInterface.h>
#include <Interface/CombatDamageInterface.h>
#include <Warrior/Warrior.h>
#include <Engine/World.h>
#include <Components/PrimitiveComponent.h>

UClownYabawiComponent::UClownYabawiComponent()
{
	PrimaryComponentTick.bCanEverTick=true;
	PrimaryComponentTick.bStartWithTickEnabled=false;
}

bool UClownYabawiComponent::CanStart() const
{
	const ABossClown* Boss=Cast<ABossClown>(GetOwner());
	const UBossClownAnimationData* Data=Boss ? Boss->GetClownAnimationData() : nullptr;
	return Boss && Boss->GetBossPhase()==EBossPhase::Phase2 && !Boss->IsDead() && Stage==EClownYabawiStage::None
		&& Data && Data->Yabawi.bEnabled && Data->Yabawi.BowlMesh && Data->Yabawi.BallMesh && GetWorld()->GetTimeSeconds()>=ReadyTime;
}

bool UClownYabawiComponent::StartRound(AActor* Player, const TArray<FVector>& Positions)
{
	ABossClown* Boss=Cast<ABossClown>(GetOwner());
	if (!CanStart() || !IsValid(Player) || Positions.Num()<2) { return false; }
	Settings=Boss->GetClownAnimationData()->Yabawi;
	Target=Player;
	RoundID=++RoundSerial;
	CorrectBowl=FMath::RandRange(0,Positions.Num()-1);
	SwapsCompleted=0;
	SwapA=INDEX_NONE;
	SwapB=INDEX_NONE;
	bSuccess=false;
	FActorSpawnParameters Params;
	Params.Owner=Boss;
	Params.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	for (const FVector& Position : Positions)
	{
		AClownYabawiBowl* Bowl=GetWorld()->SpawnActor<AClownYabawiBowl>(AClownYabawiBowl::StaticClass(),Position,Boss->GetActorRotation(),Params);
		if (!Bowl) { CancelRound(); return false; }
		Bowl->Initialize(this,RoundID,Settings);
		Bowls.Add(Bowl);
	}
	Bowls[CorrectBowl]->ShowBall(true);
	SetStage(EClownYabawiStage::Reveal);
	SetComponentTickEnabled(true);
	UE_LOG(LogTemp,Log,TEXT("Clown yabawi %d: %d bowls, reveal"),RoundID,Bowls.Num());
	return true;
}

void UClownYabawiComponent::SetStage(EClownYabawiStage Next)
{
	Stage=Next;
	StageTime=0.0f;
	if (Next==EClownYabawiStage::Selection)
	{
		SelectionDeadline=GetWorld()->GetTimeSeconds()+FMath::Max(Settings.SelectionTime,1.0f);
		UE_LOG(LogTemp,Log,TEXT("Clown yabawi %d: selection %.1f seconds"),RoundID,Settings.SelectionTime);
	}
}

void UClownYabawiComponent::StartSwap()
{
	const int32 PreviousA=SwapA;
	const int32 PreviousB=SwapB;
	SwapA=FMath::RandRange(0,Bowls.Num()-1);
	SwapB=(SwapA+FMath::RandRange(1,Bowls.Num()-1))%Bowls.Num();
	if (Bowls.Num()>2 && ((SwapA==PreviousA && SwapB==PreviousB) || (SwapA==PreviousB && SwapB==PreviousA)))
	{
		for (int32 Index=0; Index<Bowls.Num(); ++Index)
		{
			if (Index!=SwapA && Index!=SwapB) { SwapB=Index; break; }
		}
	}
	SwapStartA=Bowls[SwapA]->GetActorLocation();
	SwapStartB=Bowls[SwapB]->GetActorLocation();
	SwapTime=0.0f;
}

void UClownYabawiComponent::RegisterClosestBowl(AClownYabawiBowl* Bowl)
{
	if (RegisteredBowl.Get()==Bowl) { return; }
	if (RegisteredPlayer.IsValid() && RegisteredBowl.IsValid()) { RegisteredPlayer->RemoveDetectedObject(RegisteredBowl.Get()); }
	RegisteredBowl=Bowl;
	RegisteredPlayer=Cast<AWarrior>(Target.Get());
	if (RegisteredPlayer.IsValid() && Bowl) { RegisteredPlayer->AddDetectedObject(Bowl); }
}

float UClownYabawiComponent::GetSelectionRemaining() const
{
	return Stage==EClownYabawiStage::Selection && GetWorld() ? FMath::Max(static_cast<float>(SelectionDeadline-GetWorld()->GetTimeSeconds()),0.0f) : 0.0f;
}

FVector UClownYabawiComponent::GetPlayerLocation() const
{
	return Target.IsValid() ? Target->GetActorLocation() : FVector::ZeroVector;
}

bool UClownYabawiComponent::SelectBowl(AActor* Player, AClownYabawiBowl* Bowl, int32 InRoundID)
{
	if (Stage!=EClownYabawiStage::Selection || RoundID==INDEX_NONE || InRoundID!=RoundID || !IsValid(Player) || Player!=Target.Get()
		|| !IsValid(Bowl) || !Bowls.Contains(Bowl) || GetSelectionRemaining()<=0.0f
		|| FVector::Dist2D(Player->GetActorLocation(),Bowl->GetActorLocation())>FMath::Max(Settings.SelectionDistance,100.0f)
		|| FMath::Abs(Player->GetActorLocation().Z-Bowl->GetActorLocation().Z)>220.0f)
	{
		return false;
	}
	Resolve(Bowls.IndexOfByKey(Bowl)==CorrectBowl);
	return true;
}

void UClownYabawiComponent::Resolve(bool bCorrect)
{
	if (Stage!=EClownYabawiStage::Selection) { return; }
	// 피해로 사망/초기화 콜백이 발생하기 전에 선택과 중복 보상을 먼저 차단.
	SetStage(EClownYabawiStage::Resolving);
	bSuccess=bCorrect;
	RegisterClosestBowl(nullptr);
	for (AClownYabawiBowl* Bowl : Bowls) { Bowl->SetSelectionHint(false,0.0f); }
	ABossClown* Boss=Cast<ABossClown>(GetOwner());
	AActor* Player=Target.Get();
	if (!Boss || !IsValid(Player)) { CancelRound(); return; }
	UE_LOG(LogTemp,Log,TEXT("Clown yabawi %d: %s"),RoundID,bCorrect ? TEXT("correct") : TEXT("wrong or timeout"));
	if (bCorrect)
	{
		Bowls[CorrectBowl]->ShowBall(true);
		if (ICharacterStatInterface* StatOwner=Cast<ICharacterStatInterface>(Player))
		{
			if (UCharacterStatComponent* Stats=StatOwner->GetCharacterStatComponent())
			{
				Stats->SetCurrentHP(Stats->GetCurrentHP()+Stats->GetMaxHP()*FMath::Clamp(Settings.PlayerHealRatio,0.0f,1.0f));
			}
		}
		if (UEnemyStatComponent* Stats=Boss->FindComponentByClass<UEnemyStatComponent>())
		{
			FDamageInfo RewardDamage;
			RewardDamage.AttackDamage=Stats->GetMaxHp()*FMath::Clamp(Settings.BossDamageRatio,0.0f,1.0f);
			Stats->ApplyDamage(RewardDamage);
		}
		return;
	}
	// 각 그릇은 모두 폭발하되 겹친 폭발로 같은 플레이어에게 피해를 중복 적용하지 않음.
	FVector ExplosionCenter=FVector::ZeroVector;
	bool bInRange=false;
	for (AClownYabawiBowl* Bowl : Bowls)
	{
		Bowl->ShowExplosion(Settings.ExplosionRadius,0.0f);
		if (FVector::Dist(Player->GetActorLocation(),Bowl->GetActorLocation())<=FMath::Max(Settings.ExplosionRadius,100.0f))
		{
			bInRange=true;
			ExplosionCenter=Bowl->GetActorLocation();
		}
	}
	if (bInRange)
	{
		if (ICombatDamageInterface* Victim=Cast<ICombatDamageInterface>(Player))
		{
			FHitResult Hit(Player,Cast<UPrimitiveComponent>(Player->GetRootComponent()),Player->GetActorLocation(),(Player->GetActorLocation()-ExplosionCenter).GetSafeNormal());
			const FDamageInfo Damage=Settings.ExplosionDamage;
			Victim->ReceiveDamage(Damage,Hit);
		}
	}
}

void UClownYabawiComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime,TickType,ThisTickFunction);
	ABossClown* Boss=Cast<ABossClown>(GetOwner());
	if (!Boss || Boss->GetBossPhase()!=EBossPhase::Phase2 || Boss->IsDead() || !Target.IsValid() || Target->IsActorBeingDestroyed()
		|| Bowls.ContainsByPredicate([](const TObjectPtr<AClownYabawiBowl>& Bowl) { return !IsValid(Bowl) || Bowl->IsActorBeingDestroyed(); }))
	{
		if (Boss) { Boss->FinishClownAttack(EBossClownAttackResult::Aborted,true); }
		else { CancelRound(); }
		return;
	}
	if (ICharacterStatInterface* PlayerStats=Cast<ICharacterStatInterface>(Target.Get()))
	{
		if (PlayerStats->GetCharacterStatComponent() && PlayerStats->GetCharacterStatComponent()->GetCurrentHP()<=0.0f)
		{
			Boss->FinishClownAttack(EBossClownAttackResult::Aborted,true);
			return;
		}
	}
	StageTime+=DeltaTime;
	switch (Stage)
	{
	case EClownYabawiStage::Reveal:
		if (StageTime>=FMath::Max(Settings.RevealTime,0.5f)) { SetStage(EClownYabawiStage::Cover); }
		break;
	case EClownYabawiStage::Cover:
	{
		const float Alpha=FMath::Clamp(StageTime/FMath::Max(Settings.CoverTime,0.2f),0.0f,1.0f);
		for (AClownYabawiBowl* Bowl : Bowls) { Bowl->SetCoverAlpha(FMath::SmoothStep(0.0f,1.0f,Alpha)); }
		if (Alpha>=1.0f)
		{
			Bowls[CorrectBowl]->ShowBall(false);
			SetStage(EClownYabawiStage::Shuffle);
			StartSwap();
		}
		break;
	}
	case EClownYabawiStage::Shuffle:
	{
		SwapTime+=DeltaTime;
		const float Alpha=FMath::Clamp(SwapTime/FMath::Clamp(Settings.ShuffleStepTime,0.12f,1.0f),0.0f,1.0f);
		const FVector Side=FVector::CrossProduct((SwapStartB-SwapStartA).GetSafeNormal2D(),FVector::UpVector)*FMath::Max(Settings.BowlDiameter,100.0f)*0.65f*FMath::Sin(PI*Alpha);
		Bowls[SwapA]->SetActorLocation(FMath::Lerp(SwapStartA,SwapStartB,Alpha)+Side);
		Bowls[SwapB]->SetActorLocation(FMath::Lerp(SwapStartB,SwapStartA,Alpha)-Side);
		if (Alpha>=1.0f)
		{
			++SwapsCompleted;
			if (SwapsCompleted>=FMath::Clamp(Settings.ShuffleCount,1,32)) { SetStage(EClownYabawiStage::Selection); }
			else { StartSwap(); }
		}
		break;
	}
	case EClownYabawiStage::Selection:
	{
		const float Remaining=GetSelectionRemaining();
		if (Remaining<=0.0f) { Resolve(false); return; }
		AClownYabawiBowl* Closest=nullptr;
		float BestDistance=FMath::Square(FMath::Max(Settings.SelectionDistance,100.0f));
		for (AClownYabawiBowl* Bowl : Bowls)
		{
			const float Distance=FVector::DistSquared2D(GetPlayerLocation(),Bowl->GetActorLocation());
			if (Distance<BestDistance && FMath::Abs(GetPlayerLocation().Z-Bowl->GetActorLocation().Z)<=220.0f) { Closest=Bowl; BestDistance=Distance; }
		}
		RegisterClosestBowl(Closest);
		for (AClownYabawiBowl* Bowl : Bowls) { Bowl->SetSelectionHint(Bowl==Closest,Remaining); }
		break;
	}
	case EClownYabawiStage::Resolving:
		for (AClownYabawiBowl* Bowl : Bowls)
		{
			if (bSuccess) { Bowl->SetCoverAlpha(1.0f-FMath::Clamp(StageTime/0.6f,0.0f,1.0f)); }
			else { Bowl->ShowExplosion(Settings.ExplosionRadius,StageTime/0.8f); }
		}
		if (StageTime>=1.0f) { Boss->FinishClownAttack(EBossClownAttackResult::Succeeded,true); }
		break;
	default: break;
	}
}

void UClownYabawiComponent::CancelRound(bool bResetCooldown)
{
	const bool bWasActive=Stage!=EClownYabawiStage::None || !Bowls.IsEmpty();
	Stage=EClownYabawiStage::None;
	RoundID=INDEX_NONE;
	RegisterClosestBowl(nullptr);
	RegisteredPlayer.Reset();
	for (AClownYabawiBowl* Bowl : Bowls) { if (IsValid(Bowl)) { Bowl->Destroy(); } }
	Bowls.Reset();
	Target.Reset();
	SetComponentTickEnabled(false);
	if (bResetCooldown) { ReadyTime=0.0; }
	else if (bWasActive && GetWorld()) { ReadyTime=GetWorld()->GetTimeSeconds()+FMath::Max(Settings.Cooldown,0.0f); }
}

void UClownYabawiComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	CancelRound(true);
	Super::EndPlay(EndPlayReason);
}
