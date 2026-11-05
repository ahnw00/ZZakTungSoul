// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/Components/AttackTraceComponent.h"

#include <Components/MeshComponent.h>
#include <Engine/World.h>
#include <DrawDebugHelpers.h>

// Sets default values for this component's properties
UAttackTraceComponent::UAttackTraceComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these
	// features off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// 애니메이션 갱신 이후에서 Trace의 Tick 실행
	PrimaryComponentTick.TickGroup = TG_PostPhysics;

	// 필요할때 Tick 활성화
	SetComponentTickEnabled(false);

	// 컴포넌트 태그 지정
	ComponentTags.Add(TraceComponentTagName);
}

// Called when the game starts
void UAttackTraceComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

// Called every frame
void UAttackTraceComponent::TickComponent(
	float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Trace 비활성화에는 건너뛰기
	if (!bIsTracing)
	{
		return;
	}

	UMeshComponent* Mesh = TraceMesh.Get();
	if (!IsValid(Mesh))
	{
		EndTrace();
		return;
	}

	const FVector CurrentStart = Mesh->GetSocketLocation(TraceStartSocketName);
	const FVector CurrentEnd = Mesh->GetSocketLocation(TraceEndSocketName);

	// 이전 프레임과 현재 프레임의 중간 지점
	const FVector PreviousCenter = (PreviousStart + PreviousEnd) * 0.5f;
	const FVector CurrentCenter = (CurrentStart + CurrentEnd) * 0.5f;

	// Trace Capsule Sweep 실행
	CheckTrace(PreviousCenter, CurrentCenter, CurrentStart, CurrentEnd);

	// 다음 프레임을 위해 현재 값 저장
	PreviousStart = CurrentStart;
	PreviousEnd = CurrentEnd;
}

void UAttackTraceComponent::BeginTrace()
{
	UMeshComponent* Mesh = TraceMesh.Get();

	// 매쉬 유효성 검사
	if (!IsValid(Mesh))
	{
		return;
	}

	// 이미 판정중이면 건너뛰기
	if (bIsTracing)
	{
		return;
	}

	// Socket 유효성 검사
	if (!Mesh->DoesSocketExist(TraceStartSocketName) || !Mesh->DoesSocketExist(TraceEndSocketName))
	{
		return;
	}

	// 기존의 판정이 남아있다면 판정 초기화
	EndTrace();

	// 공격 판정 하기 전에 초기화
	HitActors.Reset();

	// 현재 판정하는 소켓 위치 저장
	PreviousStart = Mesh->GetSocketLocation(TraceStartSocketName);
	PreviousEnd = Mesh->GetSocketLocation(TraceEndSocketName);

	bIsTracing = true;

	// 공격 시작 -> Tick 활성화
	SetComponentTickEnabled(true);
}

void UAttackTraceComponent::EndTrace()
{
	// 공격 판정 후 초기화
	bIsTracing = false;

	// Tick 비활성화
	SetComponentTickEnabled(false);

	// 판정 중복 Actor 초기화
	HitActors.Reset();
}

void UAttackTraceComponent::SetTraceMesh(UMeshComponent* InTraceMesh)
{
	// 기존 Mesh와 같으면 건너뛰기
	if (TraceMesh.Get() == InTraceMesh)
	{
		return;
	}

	// 공격 판정 중 Mesh 변경 방지
	if (bIsTracing)
	{
		EndTrace();
	}

	// Null(맨손)일 때 참조 해제
	if (!IsValid(InTraceMesh))
	{
		TraceMesh.Reset();
		return;
	}

	// 소켓 유효성 검사
	if (!InTraceMesh->DoesSocketExist(TraceStartSocketName) || !InTraceMesh->DoesSocketExist(TraceEndSocketName))
	{
		// TraceMesh.Reset();
		return;
	}

	// Mesh 등록
	TraceMesh = InTraceMesh;
}

void UAttackTraceComponent::CheckTrace(
	const FVector& PreviousCenter, const FVector& CurrentCenter, const FVector& Start, const FVector& End)
{
	UWorld* World = GetWorld();
	if (nullptr == World)
	{
		return;
	}

	// 현재 컴포넌트의 오너 가져오기
	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}

	// 현재 Mesh의 Socket 사이 Vector
	const FVector TraceVector = End - Start;
	const float TraceLength = TraceVector.Size();
	if (TraceLength <= KINDA_SMALL_NUMBER || TraceRadius <= 0.0f)
	{
		return;
	}

	// Trace Hit의 저장 값들
	TArray<FHitResult> HitResults;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(AttackTrace), false, Owner);
	Params.bIgnoreTouches = false;

	// 추후 무시할 액터가 늘어나면 QueryParams.AddIgnoreActor(Actor) 활용

	// Trace할 Capsule 크기
	const float CapsuleHalfHeight = TraceLength * 0.5f + TraceRadius;

	// Trace할 모양 선택
	const FCollisionShape TraceShape = FCollisionShape::MakeCapsule(TraceRadius, CapsuleHalfHeight);

	// Capsule은 Local Z축을 길이 방향으로 사용한다...
	const FVector CapsuleDirection = TraceVector.GetSafeNormal();

	const FQuat CapsuleRotation = FRotationMatrix::MakeFromZ(CapsuleDirection).ToQuat();

	// 이전 프레임에서 현재 플레임까지 Sweep
	const bool bResult = World->SweepMultiByChannel(
		HitResults, PreviousCenter, CurrentCenter, CapsuleRotation, TraceChannel.GetValue(), TraceShape, Params);

#ifdef ENABLE_DRAW_DEBUG

	DrawDebugCapsule(World, CurrentCenter, CapsuleHalfHeight, TraceRadius, CapsuleRotation,
		bResult ? FColor::Red : FColor::Green, false, 0.5f);
	DrawDebugLine(World, PreviousCenter, CurrentCenter, FColor::Yellow, false, 1.0f);
#endif // ENABLE_DRAW_DEBUG

	// SweepMulti's bool reports blocking hits only. Non-blocking attack targets
	// (such as yabawi bowls) are still present in HitResults.
	if (HitResults.IsEmpty())
	{
		return;
	}

	// 충돌 대상 처리하기 전 우선 패링을 했는지 판단
	for (const FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		if (!IsValid(HitActor))
		{
			return;
		}

		// 자기자신 제외
		if (HitActor == GetOwner())
		{
			return;
		}

		UPrimitiveComponent* HitComponent = Hit.GetComponent();
		if (HitComponent->ComponentHasTag(TEXT("ParryCollision")))
		{
			UE_LOG(LogTemp, Log, TEXT("Parry Completed!"));
			OnGetParried.Broadcast();
			EndTrace();
			return;
		}
	}

	// 감지된 충돌 대상 처리
	for (const FHitResult& Hit : HitResults)
	{
		TraceHitResult(Hit);
	}
}

void UAttackTraceComponent::TraceHitResult(const FHitResult& Hit)
{
	AActor* HitActor = Hit.GetActor();
	if (!IsValid(HitActor))
	{
		return;
	}

	// 자기자신 제외
	if (HitActor == GetOwner())
	{
		return;
	}

	// 액터 중복 Hit 체크
	const TWeakObjectPtr<AActor> CheckHitActor(HitActor);
	if (HitActors.Contains(CheckHitActor))
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("%s hit"), *HitActor->GetName());

	// 피격 목록에 추가
	HitActors.Add(CheckHitActor);

	// 실제 데미지는 외부에서 처리
	OnTraceHit.Broadcast(Hit);
}
