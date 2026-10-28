// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Engine/StreamableManager.h>
#include "LoadingScreenWidget.generated.h"

// 전방 선언.
class UProgressBar;
class UTextBlock;


/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API ULoadingScreenWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 비동기 로딩 시작 함수(목표 맵 이름 전달).
	UFUNCTION(BlueprintCallable, Category = "Loading")
	void StartAsyncLoading(FName InTargetMapName);


protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// 비동기 진행률 갱신 함수(타이머 호출).
	void UpdateLoadingProgress();

	// 비동기 로딩 완료 콜백.
	void OnAsyncLoadingFinished();

protected:
	// 프로그레스 바.
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> ProgressBar_Loading;
	// 퍼센트 텍스트.
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr <UTextBlock> TextBlock_Progress;
	// 로딩 상태 안내 텍스트.
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TextBlock_LoadingStatus;

	// 로딩할 에셋 경로 목록(에디터 디테일에서 추가 가능).
	UPROPERTY(EditDefaultsOnly, Category = "Loading|Assets")
	TArray<FSoftObjectPath> TargetAssets;
	// 전환할 목표 맵 이름.
	UPROPERTY(EditDefaultsOnly, Category = "Loading|Map")
	FName TargetMapName = TEXT("BaeTestMap");
	
private:
	// 비동기 스트리밍 매니저 및 핸들.
	FStreamableManager StreamableManager;
	TSharedPtr<FStreamableHandle> LoadingHandle;

	// 진행률 체크용 타이머 핸들.
	FTimerHandle ProgressTimerHandle;

	// 최소 로딩 연출 시간 보장용(너무 빨리 켜지는 것을 방지).
	float ElapsedTime = 0.f;
	const float MinLoadingDuration = 1.5f;
	bool bIsAssetLoadFinished = false;
};
