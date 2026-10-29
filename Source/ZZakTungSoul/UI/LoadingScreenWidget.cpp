// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LoadingScreenWidget.h"

#include <Kismet/GameplayStatics.h>

#include <Components/ProgressBar.h>
#include <Components/TextBlock.h>
#include <TimerManager.h>

#include <AssetRegistry/AssetRegistryModule.h>
#include <Misc/PackageName.h>
#include <Misc/Paths.h>

void ULoadingScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// UI 초기 상태 세팅.
	if (ProgressBar_Loading)
	{
		ProgressBar_Loading->SetPercent(0.f);
	}
	if (TextBlock_Progress)
	{
		TextBlock_Progress->SetText(FText::FromString(TEXT("0%")));
	}
	if (TextBlock_LoadingStatus)
	{
		TextBlock_LoadingStatus->SetText(FText::FromString(TEXT("게임 데이터 로딩 중...")));
	}
}

void ULoadingScreenWidget::NativeDestruct()
{
	// 위젯 제거 시 진행 타이머 해제.
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ProgressTimerHandle);
	}

	// 로딩 핸들 정리.
	if (LoadingHandle.IsValid())
	{
		LoadingHandle->CancelHandle();
		LoadingHandle.Reset();

	}
		Super::NativeDestruct();
}

void ULoadingScreenWidget::StartAsyncLoading(FName InTargetMapName, const FString& InMapLoadOption)
{
	TargetMapName = InTargetMapName;
	MapLoadOption = InMapLoadOption;
	ElapsedTime = 0.f;
	bIsAssetLoadFinished = false;

	// 개별 지정 에셋 목록 복사.
	TArray<FSoftObjectPath> FinalAssetsToLoad = TargetAssets;
	// 지정 폴더들에서 모든 에셋 자동 수집.
	CollectAssetsFromFolders(FinalAssetsToLoad);

	UE_LOG(LogTemp, Log, 
		TEXT("[비동기 로딩] 목표 맵: %s | 로드옵션: %s | 총 로딩 에셋: %d"), 
		*TargetMapName.ToString(), *MapLoadOption, FinalAssetsToLoad.Num());

	// 진행률 갱신 타이머 시작.
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(ProgressTimerHandle, this, &ULoadingScreenWidget::UpdateLoadingProgress, 0.05f, true);
	}

	// 에셋 목록이 비어있지 않으면 비동기 로딩 요청.
	if (FinalAssetsToLoad.Num() > 0)
	{
		LoadingHandle = StreamableManager.RequestAsyncLoad(FinalAssetsToLoad, FStreamableDelegate::CreateUObject(this, &ULoadingScreenWidget::OnAsyncLoadingFinished));
	}
	else
	{
		// 등록 에셋이 없으면 로딩은 즉시 완료 처리(연출 시간 동안 진행).
		bIsAssetLoadFinished = true;
	}
}


void ULoadingScreenWidget::UpdateLoadingProgress()
{
	ElapsedTime += 0.05f;

	// 1. 에셋 로딩 실제 진행률(0.f~1.f).
	float AssetProgress = 1.f;
	if (LoadingHandle.IsValid())
	{
		AssetProgress = LoadingHandle->GetProgress();
	}

	// 2. 시간 연출 진행률(번쩍 화면 피하기 위해서).
	const float TimeProgress = FMath::Clamp(ElapsedTime / MinLoadingDuration, 0.f, 1.f);

	// 3. 최종 진행률 계산.
	// 에셋 로딩 중에는 90%까지, 다른 마무리 작업 때문에 100%까지 가게 만들지 않는다.
	float CurrentPercent = 0.f;
	if (bIsAssetLoadFinished)
	{
		CurrentPercent = TimeProgress;
	}
	else
	{
		CurrentPercent = FMath::Min(AssetProgress, 0.9f);
	}

	// 4. UI 갱신.
	if (ProgressBar_Loading)
	{
		ProgressBar_Loading->SetPercent(CurrentPercent);
	}
	const int32 PercentInt = FMath::Clamp(FMath::RoundToInt(CurrentPercent * 100.f), 0, 100);
	if (TextBlock_Progress)
	{
		TextBlock_Progress->SetText(FText::FromString(FString::Printf(TEXT("%d%%"), PercentInt)));
	}

	// 5. Output Log에 진행률 출력(0.25초마다 주기적 확인용 로그).
	static float LastLogTime = 0.f;
	if (ElapsedTime - LastLogTime >= 0.25f)
	{
		LastLogTime = ElapsedTime;
		UE_LOG(LogTemp, Log, TEXT("[비동기 로딩 중...] 진행률: %d%% (에셋: %.9f%%"), PercentInt, AssetProgress * 100.f);
	}

	// 6. 100% 완료시 레벨 전환.
	if (CurrentPercent >= 1.f)
	{
		// 타이머 해제.
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(ProgressTimerHandle);
		}

		UE_LOG(LogTemp, Log, TEXT("[비동기 로딩 완료] %s 레벨로 전환합니다. 옵션: %s"), *TargetMapName.ToString(), *MapLoadOption);
		

		// 7. 목표 인게임 맵으로 전환(옵션 전달).
		UGameplayStatics::OpenLevel(this, TargetMapName, true, MapLoadOption);
	}

}

void ULoadingScreenWidget::OnAsyncLoadingFinished()
{
	bIsAssetLoadFinished = true;
	UE_LOG(LogTemp, Log, TEXT("[비동기 로딩] 모든 등록 에셋 메모리 스트리밍 완료."));
}

void ULoadingScreenWidget::CollectAssetsFromFolders(TArray<FSoftObjectPath>& OutAssets)
{
	if (TargetFolders.Num() == 0)
	{
		return;
	}

	IAssetRegistry* AssetRegistry = IAssetRegistry::Get();
	if (!AssetRegistry)
	{
		UE_LOG(LogTemp, Warning, TEXT("[비동기 로딩] AssetRegistry 인스턴스를 가져오지 못함."));
		return;
	}
	for (const FDirectoryPath& Folder : TargetFolders)
	{
		if (Folder.Path.IsEmpty())
		{
			continue;
		}

		// 폴더 경로를 /Game/... 패키지 경로로 정규화.
		const FString PackagePath = ConverToPackagePath(Folder.Path);
		TArray<FAssetData> AssetDataList;

		// 하위 폴더(bRecursive = true)까지 모든 에셋 검색.
		AssetRegistry->GetAssetsByPath(FName(*PackagePath), AssetDataList, true);
		int32 AddedCount = 0;
		for (const FAssetData& AssetData : AssetDataList)
		{
			const FSoftObjectPath SoftPath = AssetData.GetSoftObjectPath();
			if (SoftPath.IsValid())
			{
				OutAssets.AddUnique(SoftPath);
				AddedCount++;
			}
		}
		UE_LOG(LogTemp, Log, TEXT("[비동기 로딩] 폴더 스캔 완료: %s -> %d개 에셋 발견"), *PackagePath, AddedCount);
	}
}

FString ULoadingScreenWidget::ConverToPackagePath(const FString& InPath) const
{
	if (InPath.IsEmpty())
	{
		return FString();
	}

	// 이미 /Game으로 시작하는 패키지 경로인 경우.
	if (InPath.StartsWith(TEXT("/Game")))
	{
		return InPath;
	}
	// 디스크 풀 경로인 경우 패키지명 변환 시도.
	FString LongPackageName;
	if (FPackageName::TryConvertFilenameToLongPackageName(InPath, LongPackageName))
	{
		return LongPackageName;
	}
	// Content 상대 경로인 경우(예: Characters/Warrior).
	const FString FullContentPath = FPaths::ProjectContentDir() / InPath;
	if (FPackageName::TryConvertFilenameToLongPackageName(FullContentPath, LongPackageName))
	{
		return LongPackageName;
	}
	// 슬래시 정리 후 기본 /Game/ 접두사 결합.
	FString CleanPath = InPath;
	CleanPath.RemoveFromStart(TEXT("/"));
	return FString(TEXT("/Game/") + CleanPath);
}

