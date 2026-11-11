// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Physics/Experimental/ChaosEventType.h>
#include "RestorableProp.generated.h"

class USoundBase;

UCLASS()
class ZZAKTUNGSOUL_API ARestorableProp : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARestorableProp();

protected:
	// 컴포넌트 태그 -> 부서질 때 소리. Class Defaults에서 채움.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Break")
	TMap<FName, TObjectPtr<USoundBase>> BreakSounds;

	// 소리 냈는 지확인.
	TSet<FName> PlayedGroups;

	virtual void BeginPlay() override;

	// 각 GC 컴포넌트의 OnChaosBreakEvent가 호출하는 함수.
	UFUNCTION()
	void OnPieceBroken(const FChaosBreakEvent& BreakEvent);



};
