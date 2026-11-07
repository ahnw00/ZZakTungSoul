// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/RestorePropSubsystem.h"
#include <Gimmick/RestorableProp.h>
#include "EngineUtils.h"


bool URestorePropSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	// 게임과 PIE 월드에서만 이 서브시스템을 생성한다.
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

void URestorePropSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	// 레벨에 배치된 복원 대상 프롭 기록.
	Records.Reset();
	for (TActorIterator<ARestorableProp> It(&InWorld); It; ++It)
	{
		FRestorablePropRecord& Record = Records.AddDefaulted_GetRef();
		Record.PropClass = It->GetClass();
		Record.Transform = It->GetActorTransform();
		Record.CurrentProp = *It;
	}

	UE_LOG(LogTemp, Log, TEXT("[RestoreProp] 복원 대상 프롭 %d개 기록."), Records.Num());
}

void URestorePropSubsystem::RestoreProps()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// 겹쳐 있어도 스폰.
	FActorSpawnParameters SpawnParms;
	SpawnParms.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	for (FRestorablePropRecord& Record : Records)
	{
		// 남아 있는 프롭(파편, Delay 대기 포함) 제거.
		if (ARestorableProp* LeftProp = Record.CurrentProp.Get())
		{
			LeftProp->Destroy();
		}

		// 처음 위치에 새로 스폰하고, 다음 복원 위해 기억.
		Record.CurrentProp = World->SpawnActor<ARestorableProp>(Record.PropClass, Record.Transform, SpawnParms);
	}

	UE_LOG(LogTemp, Log, TEXT("[PropRestore] 프롭 %d개 복원."), Records.Num());


}

