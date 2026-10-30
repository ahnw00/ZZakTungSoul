// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Components/AttackTraceComponent.h"

#include <Components/MeshComponent.h>
#include <Engine/World.h>
#include <DrawDebugHelpers.h>

// Sets default values for this component's properties
UAttackTraceComponent::UAttackTraceComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
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
void UAttackTraceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
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
	const FVector PreviousMiddle = (PreviousStart + PreviousEnd) / 2.0f;
	const FVector CurrentMiddle = (CurrentStart + CurrentEnd) / 2.0f;

	// Trace의 3지점 검사
	CheckTrace(PreviousStart, CurrentStart);
	CheckTrace(PreviousMiddle, CurrentMiddle);
	CheckTrace(PreviousEnd, CurrentEnd);

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
		//TraceMesh.Reset();
		return;
	}

	// Mesh 등록
	TraceMesh = InTraceMesh;
}

void UAttackTraceComponent::CheckTrace(const FVector& Start, const FVector& End)
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

	// Trace Hit의 저장 값들
	TArray<FHitResult> HitResults;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(AttackTrace), false, Owner);

	// 추후 무시할 액터가 늘어나면 QueryParams.AddIgnoreActor(Actor) 활용

	// Trace할 모양 선택
	const FCollisionShape TraceShape = FCollisionShape::MakeSphere(TraceRadius);

	// 이전 프레임에서 현재 플레임까지 Sweep
	const bool bResult = World->SweepMultiByChannel(
		HitResults,
		Start,
		End,
		FQuat::Identity,
		TraceChannel.GetValue(),
		TraceShape,
		Params
	);

#ifdef ENABLE_DRAW_DEBUG

	DrawDebugLine(World, Start, End, FColor::Green, false, 1.0f, 0, 1.0f);
	DrawDebugSphere(World, End, TraceRadius, 12, bResult ? FColor::Red : FColor::Green, false, 1.0f);
#endif // ENABLE_DRAW_DEBUG


	if (!bResult)
	{
		return;
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

	// 피격 목록에 추가
	HitActors.Add(CheckHitActor);

	// 실제 데미지는 외부에서 처리
	OnTraceHit.Broadcast(Hit);
}