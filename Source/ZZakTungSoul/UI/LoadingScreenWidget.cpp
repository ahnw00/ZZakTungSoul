// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LoadingScreenWidget.h"

#include <Kismet/GameplayStatics.h>

#include <Components/ProgressBar.h>
#include <Components/TextBlock.h>
#include <TimerManager.h>

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

	// 위젯이 화면에 생성되면 비동기 로딩 시작.
	StartAsyncLoading(TargetMapName);
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

		Super::NativeDestruct();
	}
}

void ULoadingScreenWidget::StartAsyncLoading(FName InTargetMapName)
{
	TargetMapName = InTargetMapName;
	ElapsedTime = 0.f;
	bIsAssetLoadFinished = false;

	UE_LOG(LogTemp, Log, TEXT("[비동기 로딩] 목표 맵: %s | 로딩 대상 에셋 수: %d"), *TargetMapName.ToString(), TargetAssets.Num());

	// 진행률 갱신 타이머 시작.
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(ProgressTimerHandle, this, &ULoadingScreenWidget::UpdateLoadingProgress, 0.05f, true);
	}

	// 에셋 목록이 비어있지 않으면 비동기 로딩 요청.
	if (TargetAssets.Num() > 0)
	{
		LoadingHandle = StreamableManager.RequestAsyncLoad(TargetAssets, FStreamableDelegate::CreateUObject(this, &ULoadingScreenWidget::OnAsyncLoadingFinished));
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

	// 7. 목표 인게임 맵으로 전환.
}

void ULoadingScreenWidget::OnAsyncLoadingFinished()
{
	bIsAssetLoadFinished = true;
	UE_LOG(LogTemp, Log, TEXT("[비동기 로딩] 모든 등록 에셋 메모리 스트리밍 완료."));
}
