// Fill out your copyright notice in the Description page of Project Settings.


#include <Enemy/Boss/Clown/BossClown.h>
#include <Interface/CombatDamageInterface.h>
#include <Enemy/Boss/Clown/Anim/BossClownAnimationInstance.h>
#include <Enemy/Components/AttackTraceComponent.h>
#include <Animation/AnimInstance.h>
#include <Animation/AnimMontage.h>
#include <Components/SkeletalMeshComponent.h>
#include <Engine/SkeletalMesh.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <AIController.h>
#include <Engine/World.h>
#include <UObject/ConstructorHelpers.h>

ABossClown::ABossClown()
{
	//---------------------------------------------------------------
	// 광대 SkeletalMesh 설정.
	//
	//---------------------------------------------------------------
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletonMeshRef(
		TEXT("/Game/Assets/Clown_monster/Meshes/base_mesh/SKM_base_mesh.SKM_base_mesh")
	);

	if (SkeletonMeshRef.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SkeletonMeshRef.Object);

		GetMesh()->SetRelativeLocation(FVector(0, 0, -90));
		GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));
		GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

		static ConstructorHelpers::FClassFinder<UBossClownAnimationInstance> AnimClassRef(
			TEXT("/Game/Blueprints/Enemy/Boss/Clown/Animation/ABP_ClownBoss.ABP_ClownBoss_C")
		);

		if (AnimClassRef.Succeeded())
		{
			GetMesh()->SetAnimInstanceClass(AnimClassRef.Class);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("BossClown: failed to load animation blueprint class."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BossClown: failed to load skeleton mesh."));
	}


}
const UBossClownAnimationData* ABossClown::GetClownAnimationData() const
{
	return ClownAnimationData.Get();
}

const FEnemyAttackData* ABossClown::GetOpeningAttackData() const
{
	const UBossClownAnimationData* Data = GetClownAnimationData();
	return Data ? &Data->OpeningAttack : nullptr;
}

const FEnemyAttackData* ABossClown::GetClownAttackData(EBossClownAttack AttackType) const
{
	const UBossClownAnimationData* Data = GetClownAnimationData();
	if (!Data)
	{
		return nullptr;
	}

	switch (AttackType)
	{
	case EBossClownAttack::HammerSweep:
		return &Data->HammerSweep;
	case EBossClownAttack::HammerSlam:
		return &Data->HammerSlam;
	default:
		return nullptr;
	}
}

bool ABossClown::IsClownCombatPhase() const
{
	return !IsDead() && (BossPhase == EBossPhase::Phase1 || BossPhase == EBossPhase::Phase2);
}

bool ABossClown::StartClownAttack(EBossClownAttack AttackType, AActor* Target, int32& OutExecutionID)
{
	OutExecutionID = INDEX_NONE;

	if (!IsClownCombatPhase() || ActiveAttackID != INDEX_NONE || !IsValid(Target) || !AttackTraceComponent)
	{
		return false;
	}

	const FEnemyAttackData* AttackData = GetClownAttackData(AttackType);
	UAnimInstance* Anim = GetMesh()->GetAnimInstance();

	if (!AttackData || !AttackData->Montage || !AttackData->DamageInfo.IsValidIndex(0) || !Anim)
	{
		UE_LOG(LogTemp, Error, TEXT("BossClown: attack montage, DamageInfo[0] and AnimInstance are required."));
		return false;
	}

	//---------------------------------------------------------------
	// 공격 시작 시 이동 정지와 타깃 방향 정렬.
	//
	//---------------------------------------------------------------
	if (AAIController* AI = Cast<AAIController>(GetController()))
	{
		AI->StopMovement();
		AI->ClearFocus(EAIFocusPriority::Gameplay);
	}

	GetCharacterMovement()->StopMovementImmediately();
	AttackTraceComponent->EndTrace();

	FVector Direction = Target->GetActorLocation() - GetActorLocation();
	Direction.Z = 0.0f;

	if (!Direction.IsNearlyZero())
	{
		SetActorRotation(Direction.Rotation());
	}

	CurrentClownAttack = *AttackData;

	//---------------------------------------------------------------
	// 단타 공격은 첫 번째 데미지를 기본으로 사용.
	// 다단 공격은 Notify에서 인덱스 변경.
	//
	//---------------------------------------------------------------
	CurrentDamageIndex = 0;
	AttackAnimInstance = Anim;
	ActiveAttackID = ++AttackSerial;
	OutExecutionID = ActiveAttackID;

	SetEnemyState(EEnemyState::Attack);

	UAnimMontage* Montage = CurrentClownAttack.Montage.Get();

	if (Anim->Montage_Play(Montage) <= 0.0f)
	{
		FinishClownAttack(EBossClownAttackResult::Failed, false);
		return false;
	}

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &ABossClown::OnClownAttackEnded, ActiveAttackID);
	Anim->Montage_SetEndDelegate(EndDelegate, Montage);

	GetWorld()->GetTimerManager().SetTimer(AttackTimeoutHandle, this, &ABossClown::OnClownAttackTimeout, FMath::Max(AttackTimeout, 0.1f), false);

	return true;
}

void ABossClown::SetClownAttackDamage(int32 DamageIndex, UAnimSequenceBase* SourceAnimation)
{
	if (ActiveAttackID == INDEX_NONE || !IsClownCombatPhase() || SourceAnimation != CurrentClownAttack.Montage.Get())
	{
		return;
	}

	//---------------------------------------------------------------
	// 현재 광대 공격에서 사용할 타격 데미지 인덱스 변경.
	//
	//---------------------------------------------------------------
	if (!CurrentClownAttack.DamageInfo.IsValidIndex(DamageIndex))
	{
		CurrentDamageIndex = INDEX_NONE;
		UE_LOG(LogTemp, Error, TEXT("BossClown: invalid attack DamageIndex %d."), DamageIndex);
		return;
	}

	CurrentDamageIndex = DamageIndex;
}

EBossClownAttackResult ABossClown::GetClownAttackResult(int32 ExecutionID) const
{
	if (ExecutionID != INDEX_NONE && ExecutionID == ActiveAttackID)
	{
		return EBossClownAttackResult::Running;
	}

	return ExecutionID != INDEX_NONE && ExecutionID == LastAttackID ? LastAttackResult : EBossClownAttackResult::Aborted;
}

void ABossClown::CancelClownAttack(int32 ExecutionID)
{
	if (ActiveAttackID != INDEX_NONE && ExecutionID == ActiveAttackID)
	{
		FinishClownAttack(EBossClownAttackResult::Aborted, true);
	}
}

void ABossClown::OnClownAttackEnded(UAnimMontage* Montage, bool bInterrupted, int32 ExecutionID)
{
	if (ActiveAttackID != INDEX_NONE && ExecutionID == ActiveAttackID)
	{
		FinishClownAttack(bInterrupted ? EBossClownAttackResult::Aborted : EBossClownAttackResult::Succeeded, false);
	}
}

void ABossClown::OnClownAttackTimeout()
{
	UE_LOG(LogTemp, Error, TEXT("BossClown: attack timed out."));
	FinishClownAttack(EBossClownAttackResult::Failed, true);
}

void ABossClown::FinishClownAttack(EBossClownAttackResult Result, bool bStopMontage, bool bRestoreState)
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(AttackTimeoutHandle);
	}

	if (ActiveAttackID == INDEX_NONE)
	{
		return;
	}

	UAnimInstance* Anim = AttackAnimInstance.Get();
	UAnimMontage* Montage = CurrentClownAttack.Montage.Get();

	//---------------------------------------------------------------
	// 종료 콜백보다 먼저 실행 번호 무효화.
	//
	//---------------------------------------------------------------
	LastAttackID = ActiveAttackID;
	LastAttackResult = Result;
	ActiveAttackID = INDEX_NONE;
	CurrentDamageIndex = INDEX_NONE;

	if (Anim && Montage)
	{
		FOnMontageEnded EmptyDelegate;
		Anim->Montage_SetEndDelegate(EmptyDelegate, Montage);

		if (bStopMontage)
		{
			Anim->Montage_Stop(0.0f, Montage);
		}
	}

	if (AttackTraceComponent)
	{
		AttackTraceComponent->EndTrace();
	}

	CurrentClownAttack = FEnemyAttackData();
	AttackAnimInstance.Reset();

	if (bRestoreState && !IsActorBeingDestroyed() && IsClownCombatPhase() && GetEnemyState() == EEnemyState::Attack)
	{
		SetEnemyState(EEnemyState::Chase);
	}
}

void ABossClown::ResetBossGimmicks()
{
	FinishClownAttack(EBossClownAttackResult::Aborted, true, false);
	Super::ResetBossGimmicks();
}


void ABossClown::HandleAttackTraceHit(const FHitResult& Hit)
{
	//---------------------------------------------------------------
	// 첫 공격만 공통 보스에서 처리.
	// 일반 적의 피해 전달 함수는 사용하지 않음.
	//
	//---------------------------------------------------------------
	if (BossPhase == EBossPhase::OpeningAttack)
	{
		Super::HandleAttackTraceHit(Hit);
		return;
	}

	//---------------------------------------------------------------
	// 광대 전투 공격의 현재 타격 데이터에서 직접 피해 전달.
	//
	//---------------------------------------------------------------
	if (!IsClownCombatPhase() || ActiveAttackID == INDEX_NONE ||
		!CurrentClownAttack.DamageInfo.IsValidIndex(CurrentDamageIndex))
	{
		return;
	}

	if (ICombatDamageInterface* Target = Cast<ICombatDamageInterface>(Hit.GetActor()))
	{
		const FDamageInfo& Damage = CurrentClownAttack.DamageInfo[CurrentDamageIndex];

		UE_LOG(LogTemp, Log, TEXT("BossClown: Hit %s, DamageIndex %d, Damage %.1f"), *GetNameSafe(Hit.GetActor()), CurrentDamageIndex, Damage.AttackDamage);

		Target->ReceiveDamage(Damage, Hit);
	}
}