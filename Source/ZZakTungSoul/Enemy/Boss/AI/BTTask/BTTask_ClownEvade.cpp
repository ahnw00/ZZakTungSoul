#include <Enemy/Boss/AI/BTTask/BTTask_ClownEvade.h>
#include <Enemy/Boss/Clown/BossClown.h>
#include <AIController.h>
#include <BehaviorTree/BehaviorTreeComponent.h>
#include <BehaviorTree/BlackboardComponent.h>
#include <Animation/AnimInstance.h>
#include <Animation/AnimMontage.h>
#include <Components/CapsuleComponent.h>
#include <Components/SkeletalMeshComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <NavigationSystem.h>
#include <Engine/World.h>
#include <Enemy/Components/AttackTraceComponent.h>

UBTTask_ClownEvade::UBTTask_ClownEvade()
{
	NodeName = TEXT("Clown Evade / Orbit");
	bCreateNodeInstance = true;
	bNotifyTick = true;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_ClownEvade::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	ABossClown* Clown = AIController ? Cast<ABossClown>(AIController->GetPawn()) : nullptr;
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	AActor* Target = BlackboardComp ? Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey)) : nullptr;

	if (!IsValid(Clown) || !IsValid(Target) || !Clown->IsClownCombatPhase())
	{
		return EBTNodeResult::Failed;
	}

	return BeginTactical(OwnerComp, Clown, Target) ? EBTNodeResult::InProgress : EBTNodeResult::Failed;
}

void UBTTask_ClownEvade::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	ABossClown* Clown = AttackingClown.Get();
	AActor* Target = TacticalTarget.Get();

	if (!Clown || !IsValid(Target) || Clown->IsDead() || !Clown->IsClownCombatPhase())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

		return;
	}

	Elapsed += DeltaSeconds;
	Clown->FaceClownTarget(Target, DeltaSeconds);
	UCharacterMovementComponent* Move = Clown->GetCharacterMovement();

	if (Action == EClownEvadeAction::Orbit)
	{
		const float Gap = Clown->GetTargetSurfaceDistance(Target);

		if (Elapsed > 0.25f && Gap <= 180.0f)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

			return;
		}

		const FVector Forward = (Target->GetActorLocation() - Clown->GetActorLocation()).GetSafeNormal2D();
		const FVector Tangent = FVector::CrossProduct(FVector::UpVector, Forward) * OrbitSign;
		FVector Direction = (Tangent + Forward * FMath::Clamp((Gap - 340.0f) / 150.0f, -1.5f, 1.5f)).GetSafeNormal();

		if (UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent())
		{
			const FVector Home = BB->GetValueAsVector(TEXT("HomeLocation"));

			if (FVector::Dist2D(Clown->GetActorLocation(), Home) > Clown->GetClownAnimationData()->TacticalLeashRadius)
			{
				Direction = (Home - Clown->GetActorLocation()).GetSafeNormal2D();
			}
		}

		if (Elapsed >= NextOrbitProbeTime)
		{
			NextOrbitProbeTime = Elapsed + 0.2f;
			FVector SafeGoal;

			if (!FindSafeGoal(OwnerComp, Clown, Target, Clown->GetActorLocation() + Direction * 120.0f, SafeGoal))
			{
				OrbitSign = -OrbitSign;
				Direction = (-Tangent + Forward * FMath::Clamp((Gap - 340.0f) / 150.0f, -1.5f, 1.5f)).GetSafeNormal();

				if (!FindSafeGoal(OwnerComp, Clown, Target, Clown->GetActorLocation() + Direction * 120.0f, SafeGoal))
				{
					FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

					return;
				}
			}
		}

		Move->MaxWalkSpeed = FMath::FInterpTo(Move->MaxWalkSpeed, 260.0f, DeltaSeconds, 5.0f);
		Clown->AddMovementInput(Direction);
	}
	else if (Action != EClownEvadeAction::Backjump && Elapsed < MoveDuration)
	{
		const float Alpha = FMath::Clamp(Elapsed / MoveDuration, 0.0f, 1.0f);
		Move->MaxWalkSpeed = FMath::Max(40.0f, FVector::Dist2D(MoveGoal, TacticalStart) / MoveDuration * 1.65f * FMath::Sin(PI * Alpha));
		const FVector ToGoal = MoveGoal - Clown->GetActorLocation();

		if (ToGoal.Size2D() > 25.0f)
		{
			Clown->AddMovementInput(ToGoal.GetSafeNormal2D());
		}
	}

	const bool bLanded = Action != EClownEvadeAction::Backjump || (Elapsed > 0.2f && Move->IsMovingOnGround());

	if (Elapsed >= Duration && bLanded)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
	else if (Elapsed > Duration + 2.0f)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
}

EBTNodeResult::Type UBTTask_ClownEvade::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FinishTactical(true);

	return EBTNodeResult::Aborted;
}

void UBTTask_ClownEvade::OnTaskFinished(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
	FinishTactical(TaskResult != EBTNodeResult::Succeeded);
	AttackingClown.Reset();
}

bool UBTTask_ClownEvade::FindSafeGoal(UBehaviorTreeComponent& OwnerComp, ABossClown* Clown, AActor* Target,
	const FVector& Desired, FVector& OutGoal) const
{
	const UBossClownAnimationData* Data = Clown->GetClownAnimationData();
	UNavigationSystemV1* Nav = UNavigationSystemV1::GetNavigationSystem(Clown->GetWorld());

	if (!Data)
	{
		return false;
	}

	const float Half = Clown->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	FNavLocation NavGoal;

	if (Action == EClownEvadeAction::Backjump || !Nav ||
		!Nav->ProjectPointToNavigation(Desired - FVector(0.0f, 0.0f, Half), NavGoal, FVector(100.0f, 100.0f, 200.0f)))
	{
		FCollisionQueryParams GroundParams(SCENE_QUERY_STAT(ClownEvadeGround), false, Clown);
		GroundParams.AddIgnoredActor(Target);
		FHitResult Ground;

		if (!Clown->GetWorld()->LineTraceSingleByChannel(Ground, Desired + FVector(0.0f, 0.0f, 100.0f),
				Desired - FVector(0.0f, 0.0f, Half + 150.0f), ECC_Visibility, GroundParams) ||
			Ground.ImpactNormal.Z < 0.9f)
		{
			return false;
		}

		NavGoal.Location = Ground.ImpactPoint;
	}

	OutGoal = NavGoal.Location + FVector(0.0f, 0.0f, Half + 3.0f);

	if (FVector::Dist2D(OutGoal, Desired) > 100.0f || FMath::Abs(OutGoal.Z - Clown->GetActorLocation().Z) > 60.0f)
	{

		return false;
	}

	if (UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent())
	{
		if (FVector::Dist2D(OutGoal, BB->GetValueAsVector(TEXT("HomeLocation"))) > Data->TacticalLeashRadius)
		{
			return false;
		}
	}

	FVector HitLocation;
	const FVector StartFoot = Clown->GetActorLocation() - FVector(0.0f, 0.0f, Half - 10.0f);

	if (Action != EClownEvadeAction::Backjump && Nav && Nav->GetDefaultNavDataInstance() &&
		UNavigationSystemV1::NavigationRaycast(
			Clown, StartFoot, NavGoal.Location, HitLocation, nullptr, OwnerComp.GetAIOwner()))
	{

		return false;
	}

	FCollisionQueryParams Params(SCENE_QUERY_STAT(ClownTacticalPath), false, Clown);
	Params.AddIgnoredActor(Target);
	FHitResult Hit;
	const FCollisionShape Shape = FCollisionShape::MakeCapsule(Clown->GetCapsuleComponent()->GetScaledCapsuleRadius() + 3.0f, Half);

	if (Action == EClownEvadeAction::Backjump)
	{
		// 점프는 지상 경로가 아니라 착지 공간과 BuildJumpVelocity의 포물선 검사 사용.
		return !Clown->GetWorld()->OverlapBlockingTestByChannel(
			OutGoal + FVector(0.0f, 0.0f, 5.0f), FQuat::Identity, ECC_Pawn, Shape, Params);
	}

	if (Clown->GetWorld()->SweepSingleByChannel(Hit, Clown->GetActorLocation() + FVector(0.0f, 0.0f, 5.0f),
			OutGoal + FVector(0.0f, 0.0f, 5.0f), FQuat::Identity, ECC_Pawn, Shape, Params))
	{

		return false;
	}

	return true;
}

bool UBTTask_ClownEvade::BuildJumpVelocity(ABossClown* Clown, AActor* Target, FVector& OutVelocity)
{
	const float Gravity = FMath::Max(-Clown->GetCharacterMovement()->GetGravityZ(), 1.0f);
	const FVector Displacement = MoveGoal - Clown->GetActorLocation();
	const float Height = FMath::Max(Clown->GetClownAnimationData()->BackjumpHeight, FMath::Max(Displacement.Z, 0.0f) + 10.0f);
	const float UpSpeed = FMath::Sqrt(2.0f * Gravity * Height);
	const float FlightTime = (UpSpeed + FMath::Sqrt(FMath::Max(UpSpeed * UpSpeed - 2.0f * Gravity * Displacement.Z, 0.0f))) / Gravity;
	OutVelocity = Displacement.GetSafeNormal2D() * (Displacement.Size2D() / FlightTime);
	OutVelocity.Z = UpSpeed;
	Duration = FlightTime + 0.25f;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(ClownJumpArc), false, Clown);
	Params.AddIgnoredActor(Target);
	const FCollisionShape Shape = FCollisionShape::MakeCapsule(Clown->GetCapsuleComponent()->GetScaledCapsuleRadius(), Clown->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	const FVector Start = Clown->GetActorLocation() + FVector(0.0f, 0.0f, 5.0f);
	FVector Previous = Start;

	for (int32 Index = 1; Index <= 12; ++Index)
	{
		const float Time = FlightTime * Index / 12.0f;
		const FVector Next = Start + OutVelocity * Time - FVector(0.0f, 0.0f, 0.5f * Gravity * Time * Time);
		FHitResult Hit;

		if (Clown->GetWorld()->SweepSingleByChannel(Hit, Previous, Next, FQuat::Identity, ECC_Pawn, Shape, Params))
		{

			return false;
		}

		Previous = Next;
	}

	return true;
}

bool UBTTask_ClownEvade::BeginTactical(UBehaviorTreeComponent& OwnerComp, ABossClown* Clown, AActor* Target)
{
	const UBossClownAnimationData* Data = Clown->GetClownAnimationData();
	UCharacterMovementComponent* Move = Clown->GetCharacterMovement();
	UAnimInstance* Anim = Clown->GetMesh()->GetAnimInstance();

	if (!Data || !Anim || !Move->IsMovingOnGround())
	{
		return false;
	}

	const FVector Forward = (Target->GetActorLocation() - Clown->GetActorLocation()).GetSafeNormal2D();
	const float Side = FMath::RandBool() ? 1.0f : -1.0f;
	FVector Offset = FVector::ZeroVector;
	UAnimMontage* Montage = nullptr;

	switch (Action)
	{
	case EClownEvadeAction::Backstep:
		Offset = -Forward * Data->BackstepDistance;
		Montage = Data->BackstepMontage.Get();
		break;
	case EClownEvadeAction::Backjump:
		Offset = -Forward * Data->BackjumpDistance;
		Montage = Data->BackjumpMontage.Get();
		break;
	case EClownEvadeAction::SideEvade:
		Offset = FVector::CrossProduct(FVector::UpVector, Forward) * Side * Data->SideEvadeDistance;
		Montage = Side < 0.0f ? Data->EvadeLeftMontage.Get() : Data->EvadeRightMontage.Get();
		break;
	}

	if (!Montage && Action != EClownEvadeAction::Orbit)
	{
		return false;
	}

	if (Action != EClownEvadeAction::Orbit &&
		!FindSafeGoal(OwnerComp, Clown, Target, Clown->GetActorLocation() + Offset, MoveGoal))
	{
		return false;
	}

	Duration = Action == EClownEvadeAction::Orbit ? FMath::FRandRange(3.0f, 4.5f) : 0.8f;
	MoveDuration = 0.7f;
	TacticalStart = Clown->GetActorLocation();
	OrbitSign = Side;
	FVector JumpVelocity = FVector::ZeroVector;

	if (Action == EClownEvadeAction::Backjump && !BuildJumpVelocity(Clown, Target, JumpVelocity))
	{
		return false;
	}

	const float Rate = Montage ? Montage->GetPlayLength() / Duration : 1.0f;

	if (Montage && Anim->Montage_Play(Montage, FMath::Max(Rate, 0.1f)) <= 0.0f)
	{
		return false;
	}

	// 계획 검증이 끝난 뒤에만 이동 설정을 변경하고 종료 시 복구.
	AttackingClown = Clown;
	TacticalTarget = Target;
	TacticalMontage = Montage;
	Elapsed = 0.0f;
	NextOrbitProbeTime = 0.0f;
	bTacticalActive = true;
	bSavedOrient = Move->bOrientRotationToMovement;
	bSavedDesired = Move->bUseControllerDesiredRotation;
	bSavedYaw = Clown->bUseControllerRotationYaw;
	OwnerComp.GetAIOwner()->StopMovement();
	Clown->ConsumeMovementInputVector();

	if (Action != EClownEvadeAction::Orbit)
	{
		Move->StopMovementImmediately();
	}

	Move->bOrientRotationToMovement = false;
	Move->bUseControllerDesiredRotation = false;
	Clown->bUseControllerRotationYaw = false;
	Clown->SetEnemyState(Action == EClownEvadeAction::Orbit ? EEnemyState::Strafe : EEnemyState::Chase);

	if (Action == EClownEvadeAction::Backjump)
	{
		Move->MaxWalkSpeed = JumpVelocity.Size2D();
		Clown->LaunchCharacter(JumpVelocity, true, true);
	}
	else if (Action != EClownEvadeAction::Orbit)
	{
		Move->MaxWalkSpeed = Offset.Size2D() / MoveDuration;
	}

	return true;
}

void UBTTask_ClownEvade::FinishTactical(bool bAbort)
{
	if (!bTacticalActive)
	{
		return;
	}

	if (ABossClown* Clown = AttackingClown.Get())
	{
		if (!bAbort && (Action == EClownEvadeAction::Backjump || Action == EClownEvadeAction::Backstep))
		{
			Clown->PreferClownRangedAttack();
		}

		Clown->ConsumeMovementInputVector();

		if (UAttackTraceComponent* Trace = Clown->FindComponentByClass<UAttackTraceComponent>())
		{
			Trace->EndTrace();
		}

		UCharacterMovementComponent* Move = Clown->GetCharacterMovement();
		Move->bOrientRotationToMovement = bSavedOrient;
		Move->bUseControllerDesiredRotation = bSavedDesired;
		Clown->bUseControllerRotationYaw = bSavedYaw;

		if (!bAbort && Clown->IsClownCombatPhase())
		{
			Clown->SetEnemyState(EEnemyState::Chase);
		}

		Clown->RefreshClownMovement();

		if (bAbort)
		{
			Move->StopMovementImmediately();
		}

		if (UAnimInstance* Anim = Clown->GetMesh()->GetAnimInstance())
		{
			if (TacticalMontage.IsValid())
			{
				Anim->Montage_Stop(0.25f, TacticalMontage.Get());
			}
		}
	}

	TacticalMontage.Reset();
	TacticalTarget.Reset();
	bTacticalActive = false;
}
