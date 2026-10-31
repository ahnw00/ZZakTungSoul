#include <Enemy/Boss/Clown/Data/BossClownAnimationData.h>

#if WITH_EDITOR
#include <Anim/AnimNotifyState/AnimNotifyState_AttackTrace.h>
#include <Animation/AnimMontage.h>
#include <Animation/AnimSequence.h>
#include <AnimationBlueprintLibrary.h>
#include <AssetToolsModule.h>
#include <Factories/AnimMontageFactory.h>
#include <HAL/IConsoleManager.h>
#include <Misc/PackageName.h>
#include <UObject/Package.h>
#include <UObject/SavePackage.h>
#endif

UBossClownAnimationData::UBossClownAnimationData()
{

}

void UBossClownAnimationData::BuildPhaseOneMontages()
{
#if WITH_EDITOR
	//---------------------------------------------------------------
	// 기존 공격 데이터와 몽타주를 보존하고 신규 폴더에 생성.
	// 판정 시간은 원본 애니메이션의 초기 조정값이며 에디터에서 수정 가능.
	//
	//---------------------------------------------------------------
	struct FPatternSource
	{
		EBossClownAttack Type;
		const TCHAR* Name;
		const TCHAR* Source;
		float TraceStart;
		float TraceEnd;
		float MinDistance;
		float MaxDistance;
		float Cooldown;
		float Recovery;
		float Damage;
	};

	const FPatternSource Sources[] = {
		{ EBossClownAttack::HammerSweep, TEXT("AM_ClownP1_Sweep"), TEXT("anim_attack_2"), 0.70f, 1.50f, 0.0f, 180.0f, 2.5f, 0.75f, 30.0f },
		{ EBossClownAttack::HammerSlam, TEXT("AM_ClownP1_Slam"), TEXT("anim_attack_4"), 0.60f, 1.05f, 0.0f, 210.0f, 4.0f, 1.25f, 45.0f },
		{ EBossClownAttack::FastChaseSlam, TEXT("AM_ClownP1_ChaseSlam"), TEXT("anim_attack_4"), 0.60f, 1.05f, 350.0f, 1800.0f, 10.0f, 1.5f, 45.0f },
		{ EBossClownAttack::DashSweep, TEXT("AM_ClownP1_DashSweep_Low"), TEXT("anim_attack_2"), 0.75f, 1.35f, 180.0f, 600.0f, 7.0f, 1.25f, 35.0f }
	};

	TArray<FClownPhaseOnePattern> BuiltPatterns;
	TArray<UObject*> NewMontages;
	for (const FPatternSource& Source : Sources)
	{
		const FString Folder = TEXT("/Game/Blueprints/Enemy/Boss/Clown/Animation/Montages/Phase1");
		const FString SourcePath = FString(TEXT("/Game/Assets/clown_monster/demo/animation/")) + Source.Source;
		UAnimSequence* Sequence = LoadObject<UAnimSequence>(nullptr, *SourcePath);
		if (!Sequence)
		{
			UE_LOG(LogTemp, Error, TEXT("ClownSetup: missing source %s. Pattern data was not changed."), *SourcePath);
			return;
		}

		UAnimMontage* Montage = LoadObject<UAnimMontage>(nullptr, *(Folder / Source.Name));
		if (!Montage)
		{
			UAnimMontageFactory* Factory = NewObject<UAnimMontageFactory>();
			Factory->TargetSkeleton = Sequence->GetSkeleton();
			Factory->SourceAnimation = Sequence;
			Montage = Cast<UAnimMontage>(FAssetToolsModule::GetModule().Get().CreateAsset(Source.Name, Folder, UAnimMontage::StaticClass(), Factory));
			if (!Montage)
			{
				UE_LOG(LogTemp, Error, TEXT("ClownSetup: montage creation failed. Pattern data was not changed."));
				return;
			}
			if (UAnimMontage* Template = LoadObject<UAnimMontage>(nullptr, TEXT("/Game/Blueprints/Enemy/Boss/Clown/Animation/Montages/AM_BossClown_NormalAttack")))
			{
				if (Template->SlotAnimTracks.Num() > 0 && Montage->SlotAnimTracks.Num() > 0)
				{
					Montage->SlotAnimTracks[0].SlotName = Template->SlotAnimTracks[0].SlotName;
				}
			}

			//---------------------------------------------------------------
			// 기존 일반 적 Notify는 수정하지 않고 공용 판정 State만 재사용.
			// 광대의 현재 공격 데이터는 StartClownAttack에서 지정.
			//
			//---------------------------------------------------------------
			UAnimationBlueprintLibrary::AddAnimationNotifyTrack(Montage, TEXT("ClownDamage"));
			UAnimationBlueprintLibrary::AddAnimationNotifyStateEvent(Montage, TEXT("ClownDamage"), Source.TraceStart, Source.TraceEnd - Source.TraceStart, UAnimNotifyState_AttackTrace::StaticClass());
			Montage->RefreshCacheData();
			Montage->PostEditChange();
			Montage->MarkPackageDirty();
			NewMontages.Add(Montage);
		}

		FClownPhaseOnePattern Pattern;
		Pattern.AttackType = Source.Type;
		Pattern.Attack.Montage = Montage;
		FDamageInfo Damage;
		Damage.AttackDamage = Source.Damage;
		Damage.PoiseDamage = Source.Damage;
		Damage.StanceDamage = Source.Damage * 0.5f;
		Pattern.Attack.DamageInfo.Add(Damage);
		Pattern.MinDistance = Source.MinDistance;
		Pattern.MaxDistance = Source.MaxDistance;
		Pattern.Cooldown = Source.Cooldown;
		Pattern.RecoveryTime = Source.Recovery;
		Pattern.TrackingTime = Source.Type == EBossClownAttack::DashSweep ? 0.0f : 0.25f;

		// 재실행 시 이미 조정한 거리, 데미지와 후딜 유지.
		if (const FClownPhaseOnePattern* Existing = PhaseOnePatterns.FindByPredicate([&Source](const FClownPhaseOnePattern& Item) { return Item.AttackType == Source.Type; }))
		{
			Pattern = *Existing;
			Pattern.Attack.Montage = Montage;
		}
		BuiltPatterns.Add(Pattern);
	}

	// 먼저 신규 몽타주를 저장한 뒤 데이터 연결을 확정.
	for (UObject* Asset : NewMontages)
	{
		FSavePackageArgs Args;
		Args.TopLevelFlags = RF_Public | RF_Standalone;
		const FString Filename = FPackageName::LongPackageNameToFilename(Asset->GetOutermost()->GetName(), FPackageName::GetAssetPackageExtension());
		if (!UPackage::SavePackage(Asset->GetOutermost(), Asset, *Filename, Args))
		{
			UE_LOG(LogTemp, Error, TEXT("ClownSetup: could not save %s. Pattern data was not changed."), *Filename);
			return;
		}
	}

	for (const FClownPhaseOnePattern& Existing : PhaseOnePatterns)
	{
		if (Existing.AttackType == EBossClownAttack::TripleBalls || Existing.AttackType == EBossClownAttack::ClapWave)
		{
			BuiltPatterns.Add(Existing);
		}
	}
	Modify();
	PhaseOnePatterns = MoveTemp(BuiltPatterns);
	bUseNativePhaseOne = true;
	MarkPackageDirty();
	UE_LOG(LogTemp, Display, TEXT("ClownSetup: four Phase1 patterns connected. Save this data asset to persist the settings."));
#endif
}

#if WITH_EDITOR
static FAutoConsoleCommand ClownSetupPhaseOneCommand(
	TEXT("Clown.SetupPhaseOne"),
	TEXT("Create and connect the Clown Phase1 montages. Existing montage assets are preserved."),
	FConsoleCommandDelegate::CreateLambda([]()
	{
		UBossClownAnimationData* Data = LoadObject<UBossClownAnimationData>(nullptr, TEXT("/Game/Blueprints/Enemy/Boss/Clown/Data/DA_ClownCombat"));
		if (Data)
		{
			Data->BuildPhaseOneMontages();
		}
	})
);
#endif

void UBossClownAnimationData::BuildAdditionalPatterns()
{
#if WITH_EDITOR
	FastChaseMontage = LoadObject<UAnimMontage>(nullptr, TEXT("/Game/Blueprints/Enemy/Boss/Clown/Animation/Montages/Phase1/AM_ClownP1_FastRun"));
	struct FSpecialSource
	{
		EBossClownAttack Type;
		const TCHAR* Name;
		const TCHAR* Source;
		float Start;
		float End;
		float Min;
		float Max;
		float Damage;
	};
	const FSpecialSource Sources[] = {
		{ EBossClownAttack::DashSweep, TEXT("AM_ClownP1_DashSweep_Low"), TEXT("/Game/Assets/clown_monster/demo/animation/anim_attack_2"), 0.0f, 2.7333333f, 180.0f, 600.0f, 35.0f },
		{ EBossClownAttack::TripleBalls, TEXT("AM_ClownP1_TripleBalls"), TEXT("/Game/Blueprints/Enemy/Boss/Clown/Animation/Retarget/Clown_AS_OrchestraConductor_50BPM_3x4_1Bar_Crescendo"), 0.0f, 3.6f, 350.0f, 1600.0f, 20.0f },
		{ EBossClownAttack::ClapWave, TEXT("AM_ClownP1_Clap"), TEXT("/Game/Blueprints/Enemy/Boss/Clown/Animation/Retarget/Clown_AS_OrchestraConductor_EndConcert_01"), 22.0f, 23.4f, 120.0f, 900.0f, 35.0f }
	};
	for (const FSpecialSource& Source : Sources)
	{
		UAnimSequence* Sequence = LoadObject<UAnimSequence>(nullptr, Source.Source);
		if (!Sequence || Sequence->GetPlayLength() + KINDA_SMALL_NUMBER < Source.End)
		{
			UE_LOG(LogTemp, Error, TEXT("ClownSetup: special source missing or too short: %s"), Source.Source);
			return;
		}
		const FString Folder = TEXT("/Game/Blueprints/Enemy/Boss/Clown/Animation/Montages/Phase1");
		UAnimMontage* Montage = LoadObject<UAnimMontage>(nullptr, *(Folder / Source.Name));
		if (!Montage)
		{
			UAnimMontageFactory* Factory = NewObject<UAnimMontageFactory>();
			Factory->TargetSkeleton = Sequence->GetSkeleton();
			Factory->SourceAnimation = Sequence;
			Montage = Cast<UAnimMontage>(FAssetToolsModule::GetModule().Get().CreateAsset(Source.Name, Folder, UAnimMontage::StaticClass(), Factory));
			if (!Montage || Montage->SlotAnimTracks.IsEmpty() || Montage->SlotAnimTracks[0].AnimTrack.AnimSegments.IsEmpty())
			{
				return;
			}
			FAnimSegment& Segment = Montage->SlotAnimTracks[0].AnimTrack.AnimSegments[0];
			Segment.AnimStartTime = Source.Start;
			Segment.AnimEndTime = Source.End;
			Montage->SetCompositeLength(Source.End - Source.Start);
			if (Source.Type == EBossClownAttack::DashSweep)
			{
				UAnimationBlueprintLibrary::AddAnimationNotifyTrack(Montage, TEXT("ClownDamage"));
				UAnimationBlueprintLibrary::AddAnimationNotifyStateEvent(Montage, TEXT("ClownDamage"), 0.75f, 0.60f, UAnimNotifyState_AttackTrace::StaticClass());
			}
			Montage->RefreshCacheData();
			Montage->PostEditChange();
			FSavePackageArgs Args;
			Args.TopLevelFlags = RF_Public | RF_Standalone;
			const FString Filename = FPackageName::LongPackageNameToFilename(Montage->GetOutermost()->GetName(), FPackageName::GetAssetPackageExtension());
			if (!UPackage::SavePackage(Montage->GetOutermost(), Montage, *Filename, Args))
			{
				return;
			}
		}
		if (Source.Type == EBossClownAttack::DashSweep)
		{
			if (FClownPhaseOnePattern* Existing = PhaseOnePatterns.FindByPredicate([&Source](const FClownPhaseOnePattern& Item) { return Item.AttackType == Source.Type; }))
			{
				Existing->Attack.Montage = Montage;
			}
		}
		if (!PhaseOnePatterns.ContainsByPredicate([&Source](const FClownPhaseOnePattern& Item) { return Item.AttackType == Source.Type; }))
		{
			FClownPhaseOnePattern Pattern;
			Pattern.AttackType = Source.Type;
			Pattern.Attack.Montage = Montage;
			FDamageInfo Hit;
			Hit.AttackDamage = Source.Damage;
			Hit.PoiseDamage = Source.Damage;
			Hit.StanceDamage = Source.Damage * 0.5f;
			Pattern.Attack.DamageInfo.Add(Hit);
			Pattern.MinDistance = Source.Min;
			Pattern.MaxDistance = Source.Max;
			Pattern.Cooldown = Source.Type == EBossClownAttack::TripleBalls ? 12.0f : 9.0f;
			Pattern.RecoveryTime = 1.2f;
			Pattern.TrackingTime = Source.Type == EBossClownAttack::TripleBalls ? 0.8f : 0.35f;
			PhaseOnePatterns.Add(Pattern);
		}
	}
	MarkPackageDirty();
	UE_LOG(LogTemp, Display, TEXT("ClownSetup: run, triple balls and clap connected. Save the data asset."));
#endif
}

#if WITH_EDITOR
static FAutoConsoleCommand ClownSetupAdditionalCommand(
	TEXT("Clown.SetupAdditional"), TEXT("Connect run and additional Clown Phase1 patterns."),
	FConsoleCommandDelegate::CreateLambda([]()
	{
		if (UBossClownAnimationData* Data = LoadObject<UBossClownAnimationData>(nullptr, TEXT("/Game/Blueprints/Enemy/Boss/Clown/Data/DA_ClownCombat")))
		{
			Data->BuildAdditionalPatterns();
		}
	})
);
#endif
