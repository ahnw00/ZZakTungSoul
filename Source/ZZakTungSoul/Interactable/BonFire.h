// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/InteractableInterface.h"
#include "Bonfire.generated.h"

// 전방선언.
class AWarrior;
class AWarriorPlayerState;
class USphereComponent;
class UArrowComponent;
class UNiagaraComponent;
class UAudioComponent;
class USoundBase;

// 화톳불 상태.
UENUM(BlueprintType)
enum class EBonfireState : uint8
{
	Unlit UMETA(DisplayName = "Unlit"),
	Lit UMETA(DisplayName = "Lit")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBonfireRestDelegate);

UCLASS()
class ZZAKTUNGSOUL_API ABonfire
	: public AActor,
	public IInteractableInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABonfire();

	// 상호작용 인터페이스.
	virtual void Interact(AActor* detectedActor) override;
	// 휴식 이벤트 델리게이트.
	UPROPERTY(BlueprintAssignable, Category = "Bonfire|Events")
	FOnBonfireRestDelegate OnBonfireRest;
	// Getter.
	FORCEINLINE EBonfireState GetBonfireState() const { return BonfireState; }
	FORCEINLINE FName GetBonfireID() const { return BonfireID; }
	FORCEINLINE FText GetBonfireDisplayName() const { return BonfireDisplayName; }
	FORCEINLINE UArrowComponent* GetTeleportPoint() const { return TeleportPoint; }
	FTransform GetTeleportTransform() const;

	// 세이브 데이터화 화톳불 활성화 상태 동기화.
	void SyncLitState(const AWarriorPlayerState* PS);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 작동 가능 검사.
	bool CanInteractBonfire(AWarrior* Interactor) const;
	
	

	UFUNCTION()
	void OnInteractionSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnInteractionSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex);

protected:
	// 루트 컴포넌트.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bonfire|Components")
	TObjectPtr<USceneComponent> SceneRoot;

	// 외형 메시.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bonfire|Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	// 플레이어 감지용 상호작용 구체.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bonfire|Components")
	TObjectPtr<USphereComponent> InteractionSphere;
	
	// 화톳불 사용 방해 적 감지 범위.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bonfire|Components")
	float EnemyCheckRadius = 1500.f;
	
	// 레벨 내 겹치지 않는 ID(전송용 식별자).
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bonfire|Components")
	FName BonfireID = TEXT("Bonfire_Default");
	
	// 전송 메뉴에 쓸 화톳불 이름.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bonfire|Components")
	FText BonfireDisplayName;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bonfire|Components")
	TObjectPtr<UArrowComponent> TeleportPoint;

	// Lit 상태에서 불 이펙트.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bonfire|Components")
	TObjectPtr<UNiagaraComponent> FireEffect;

	// 불 붙일 때 나는 소리.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bonfire|Audio")
	TObjectPtr<USoundBase> IgniteSound;

	// Lit 상태 불 지속 소리.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bonfire|Components")
	TObjectPtr<UAudioComponent> FireLoopAudio;

	// 현재 화톳불 상태.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bonfire|State")
	EBonfireState BonfireState = EBonfireState::Unlit;


};
