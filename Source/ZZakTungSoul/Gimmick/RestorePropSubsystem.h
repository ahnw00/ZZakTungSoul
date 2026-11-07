// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "RestorePropSubsystem.generated.h"

class ARestorableProp;

// 시작 시점의 프롭 정보.
USTRUCT()
struct FRestorablePropRecord
{
	GENERATED_BODY()

	// 다시 스폰할 클래스.
	UPROPERTY()
	TSubclassOf<ARestorableProp> PropClass;

	// 처음 배치된 위치.
	UPROPERTY()
	FTransform Transform;

	// 현재 월드에 있는 프롭.
	UPROPERTY()
	TWeakObjectPtr<ARestorableProp> CurrentProp;
};


// 화톳불 휴식 시 부서진 프롭을 처음 상태로 되돌리는 서브시스템.
UCLASS()
class ZZAKTUNGSOUL_API URestorePropSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	// 기록된 프롭을 전부 처음 상태로 되돌림.
	void RestoreProps();

protected:
	// 이 서브시스템을 만들 월드 타입.
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

private:
	// 레벨 시작 시 기록한 프롭 목록.
	UPROPERTY()
	TArray<FRestorablePropRecord> Records;
};
