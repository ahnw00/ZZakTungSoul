// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/AnimNotify/AnimNotify_EnemyComboCheck.h"
#include <Enemy/AI/EnemyAIBlackboardKey.h>

#include <AIController.h>
#include <BehaviorTree/BlackboardComponent.h>

#include <DrawDebugHelpers.h>

void UAnimNotify_EnemyComboCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	// 유효성 검사
	if (nullptr == MeshComp)
	{
		return;
	}

	// Owner 가져오기
	APawn* Pawn = Cast<APawn>(MeshComp->GetOwner());
	if (nullptr == Pawn)
	{
		return;
	}

	// AICotroller 가져오기
	AAIController* AIController = Cast<AAIController>(Pawn->GetController());
	if (false == IsValid(AIController))
	{
		return;
	}

	// Blackboard 가져오기
	UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
	if (false == IsValid(BlackboardComp))
	{
		return;
	}

	// TargetActor 가져오기
	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(BBKEY_TARGETACTOR));
	if (false == IsValid(TargetActor))
	{
		return;
	}

	// Target 까지의 거리 계산
	const float TargetDistance = FVector::DistSquared2D(Pawn->GetActorLocation(), TargetActor->GetActorLocation());

	const float TargetRange = FMath::Square(DetectRange);

#if ENABLE_DRAW_DEBUG

	// Debug Sphere
	DrawDebugSphere(Pawn->GetWorld(),
		Pawn->GetActorLocation(),
		DetectRange,
		32,
		TargetDistance <= FMath::Square(DetectRange) ? FColor::Green : FColor::Red,
		false,
		1.0f,
		0,
		1.0f
	);

#endif

	// 범위 바깥이면 콤보 끝
	if (TargetDistance > TargetRange)
	{
		return;
	}

	// AnimInstance 가져오기
	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
	if (false == IsValid(AnimInstance))
	{
		return;
	}

	// 실행 중인 Montage 확인
	UAnimMontage* Montage = Cast<UAnimMontage>(Animation);
	if (false == IsValid(Montage) || false == AnimInstance->Montage_IsPlaying(Montage))
	{
		return;
	}

	// Montage Section 검사
	if (Montage->GetSectionIndex(NextSection) == INDEX_NONE)
	{
		return;
	}

	// 다음 Section 실행
	AnimInstance->Montage_JumpToSection(NextSection, Montage);
}
