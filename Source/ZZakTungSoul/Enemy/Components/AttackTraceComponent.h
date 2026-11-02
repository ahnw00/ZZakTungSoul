// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttackTraceComponent.generated.h"

// 전방 선언
class UMeshComponent;

// Delegate 선언
// 공격 판정 성공시 호출되는 Delegate
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAttackTraceHit, const FHitResult&);
// 패링 시 호출되는 Delegate
DECLARE_MULTICAST_DELEGATE(FOnGetParried);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZZAKTUNGSOUL_API UAttackTraceComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAttackTraceComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 공격 판정 시작 함수
	void BeginTrace();

	// 공격 판정 종료 함수
	void EndTrace();

	// 매쉬 컴포넌트 설정 함수
	void SetTraceMesh(UMeshComponent* InTraceMesh);

private:
	// Sweep Trace 실행 함수
	void CheckTrace(const FVector& Start, const FVector& End);

	// 충돌 결과 처리 함수
	void TraceHitResult(const FHitResult& Hit);

	// 공격 판정 활성화 여부
	bool IsTracing() const { return bIsTracing; }

public:
	// 충돌 성공 델리게이트 이벤트
	FOnAttackTraceHit OnTraceHit;

	// 패링 당할 시 델리게이트 이벤트
	FOnGetParried OnGetParried;

private:
	// 공격 판정 활성화 여부 플래그
	uint32 bIsTracing : 1 = false;

	// 동일한 공격에서 이미 공격한 Actor
	TSet<TWeakObjectPtr<AActor>> HitActors;

	// 이전 프레임의 공격 위치
	FVector PreviousStart = FVector::ZeroVector;
	FVector PreviousEnd = FVector::ZeroVector;

private:
	// 소켓 이름 키로 지정
	inline static const FName SocketStartKey = TEXT("TraceStart");
	inline static const FName SocketEndKey = TEXT("TraceEnd");

protected:
	// 소켓을 받아올 매쉬
	UPROPERTY(EditDefaultsOnly, Category = "Trace|Socket")
	TWeakObjectPtr<UMeshComponent> TraceMesh;

	// Trace 시작점 소켓 이름
	UPROPERTY(EditDefaultsOnly, Category = "Trace|Socket")
	FName TraceStartSocketName = SocketStartKey;

	// Trace 끝나는 지점 소켓 이름
	UPROPERTY(EditDefaultsOnly, Category = "Trace|Socket")
	FName TraceEndSocketName = SocketEndKey;

	// Trace 크기
	UPROPERTY(EditDefaultsOnly, Category = "Trace|Setting")
	float TraceRadius = 10.0f;

	// 충돌 채널
	UPROPERTY(EditDefaultsOnly, Category = "Trace|Setting")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Pawn;

	// AnimNotifyState에서 찾을 수 있도록 태그이름지정 변수
	UPROPERTY(EditDefaultsOnly, Category = "Trace|Setting")
	FName TraceComponentTagName = TEXT("AttackTrace");
};
