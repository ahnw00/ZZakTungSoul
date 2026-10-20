// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/InteractionInterface.h"
#include "BonFire.generated.h"


// 화톳불 상태.
UENUM(BlueprintType)
enum class EBonfireState : uint8
{
	Unlit UMETA(DisplayName = "Unlit"),
	Lit UMETA(DisplayName = "Lit")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBonfireRestDelegate);

UCLASS()
class ZZAKTUNGSOUL_API ABonFire
	: public AActor,
	public IInteractionInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABonFire();

	// 상호작용 인터페이스.
	virtual void Interact(class AWarrior* Interactor) override;
	// 휴식 이벤트 델리게이트.
	UPROPERTY(BlueprintAssignable, Category = "Bonfire|Events")
	FOnBonfireRestDelegate OnBonfireRest;
	// Getter.
	FORCEINLINE EBonfireState GetBonfireState() const { return BonfireState; }
	FORCEINLINE FName GetBonfireID() const { return BonfireID; }
	FORCEINLINE FText GetBonfireDisplayName() const { return BonFireDisplayName; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 작동 가능 검사.
	bool CanInteractBonfire(class AWarrior* Interactor) const;

protected:
	// 외형 메시.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bonfire|Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;
	// 플레이어 감지용 상호작용 구체.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bonfire|Components")
	TObjectPtr<class USphereComponent> InteractionSphere;
	// 화톳불 사용 방해 적 감지 범위.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bonfire|Components")
	float EnemyCheckRadius = 1500.f;
	// 레벨 내 겹치지 않는 ID(전송용 식별자).
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bonfire|Components")
	FName BonfireID = TEXT("Bonfire_Default");
	// 전송 메뉴에 쓸 화톳불 이름.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bonfire|Components")
	FText BonFireDisplayName;
	// 현재 화톳불 상태.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bonfire|State")
	EBonfireState BonfireState = EBonfireState::Unlit;
};
