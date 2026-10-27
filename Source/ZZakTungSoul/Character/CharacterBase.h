// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterBase.generated.h"

UCLASS()
class ZZAKTUNGSOUL_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// 데미지 받았을 때 처리 함수
	virtual void ReceiveDamage(const struct FDamageInfo& DamageInfo, const FHitResult& Hit);

protected:
	// 현재 가드 상태 플래그
	UPROPERTY(VisibleAnywhere, Category = "State|Combat")
	uint32 bIsGuard : 1 = false;

	// 현재 무적 상태 플래그
	UPROPERTY(VisibleAnywhere, Category = "State|Combat")
	uint32 bIsInvincibility : 1 = false;
};
