// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyBase.h"
#include <Enemy/EnemyAIController.h>
#include <Enemy/Data/EnemyAIData.h>
#include <Enemy/Components/AttackTraceComponent.h>
#include <Enemy/Components/EnemyStatComponent.h>
#include <Character/CharacterStatComponent.h>
#include <Interface/CharacterStatInterface.h>
#include <Data/DamageInfo.h>

#include <GameFramework/CharacterMovementComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Components/SkeletalMeshComponent.h>
#include <Engine/StaticMesh.h>

AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = false;

	// 이동 방향 회전 설정
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	// 초기 이동 속도 설정
	//GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;

	// AIController 설정
	AIControllerClass = AEnemyAIController::StaticClass();

	// AI제어 타입 설정
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// Trace Component 설정
	AttackTraceComponent = CreateDefaultSubobject<UAttackTraceComponent>(TEXT("AttackTraceComponent"));

	// Stat Component 설정
	EnemyStatComponet = CreateDefaultSubobject<UEnemyStatComponent>(TEXT("EnemyStatComponent"));
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	// 초기 상태에 따른 이동속도 설정
	UpdateMovementSpeed();

	// 무기 Mesh 초기화
	InitializeWeapon();

	// Delegate 연결
	if (AttackTraceComponent)
	{
		AttackTraceComponent->OnTraceHit.AddUObject(this, &AEnemyBase::HandleAttackTraceHit);
	}
}

void AEnemyBase::ReceiveDamage(const FDamageInfo& DamageInfo, const FHitResult& Hit)
{
	Super::ReceiveDamage(DamageInfo, Hit);

	if (nullptr == EnemyStatComponet)
	{
		return;
	}

	bool bInIsUnBlockable = DamageInfo.bUnblockable;
	bool bInIsIgnoreInvincibility = DamageInfo.bIgnoreInvincibility;

	// 무적 상태이고 무적무시공격이 아닐때 데미지 X
	if (bIsInvincibility && !bInIsIgnoreInvincibility)
	{
		return;
	}

	// 방어 하는 중이고 가드불가공격이 아닐때 데미지 X
	if (bIsGuard && !bInIsUnBlockable)
	{
		// 데미지 안주고 그로기 수치만 보내고 싶으면 DamageInfo에서 StanceDamage 수치만 보내면 됨
		return;
	}

	EnemyStatComponet->ApplyDamage(DamageInfo);
}

void AEnemyBase::UpdateMovementSpeed()
{
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	if (!Movement || !EnemyStatComponet)
	{
		return;
	}

	switch (CurrentState)
	{
	case EEnemyState::Idle:
		Movement->MaxWalkSpeed = 0.0f;
		break;

	case EEnemyState::Patrol:
		Movement->MaxWalkSpeed = EnemyStatComponet->GetWalkSpeed();
		break;

	case EEnemyState::Chase:
		Movement->MaxWalkSpeed = EnemyStatComponet->GetChaseSpeed();
		break;

	case EEnemyState::Strafe:
		Movement->MaxWalkSpeed = EnemyStatComponet->GetStrafeSpeed();
		break;

	case EEnemyState::Attack:
		Movement->MaxWalkSpeed = 0.0f;
		break;

	case EEnemyState::Search:
		Movement->MaxWalkSpeed = EnemyStatComponet->GetWalkSpeed();
		break;

	case EEnemyState::Hit:
		Movement->MaxWalkSpeed = 0.0f;
		break;

	case EEnemyState::Dead:
		Movement->MaxWalkSpeed = 0.0f;
		break;

	default:
		break;
	}
}

void AEnemyBase::UpdateRotationMode()
{
	// Strafe상태 인지 확인
	const bool bFaceToTarget = CurrentState == EEnemyState::Strafe;

	// 회전 방향 설정
	bUseControllerRotationYaw = bFaceToTarget;

	// 이동 방향으로 자동 회전 설정
	GetCharacterMovement()->bOrientRotationToMovement = !bFaceToTarget;
}

void AEnemyBase::HandleAttackTraceHit(const FHitResult& Hit)
{
	AActor* HitActor = Hit.GetActor();
	if (!IsValid(HitActor))
	{
		return;
	}

	//UE_LOG(LogTemp, Log, TEXT("Attack Hit: %s"), *HitActor->GetName());

	// 인터페이스 형변환
	ICharacterStatInterface* StatOwner = Cast<ICharacterStatInterface>(HitActor);
	if (nullptr == StatOwner)
	{
		UE_LOG(LogTemp, Log, TEXT("Attack Noooooooooo Hit!!!!!"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Attack Hit!!!!!@!@!@@@@@@!"));

	// 스탯 컴포넌트 가져오기
	TargetStatComp = StatOwner->GetCharacterStatComponent();
	if (nullptr == TargetStatComp)
	{
		return;
	}

	// @Todo: 데미지 임시 처리
	TargetStatComp->ApplyDamage(50.0f);
}

void AEnemyBase::InitializeWeapon()
{
	// 중복 초기화 방지
	if (IsValid(WeaponMeshComponent) || !IsValid(WeaponMesh))
	{
		return;
	}

	// 캐릭터 SkeletalMesh 확인
	USkeletalMeshComponent* CharacterSKMesh = GetMesh();
	if (!IsValid(CharacterSKMesh))
	{
		return;
	}

	// Socket 유효성 검사
	// 캐릭터의 무기 부착 Socket 확인
	if (!CharacterSKMesh->DoesSocketExist(WeaponAttachSocketName))
	{
		return;
	}

	// 무기의 Socket 확인
	if (!WeaponMesh->FindSocket(TEXT("TraceStart")) || !WeaponMesh->FindSocket(TEXT("TraceEnd")))
	{
		return;
	}

	// 무기 컴포넌트 생성
	UStaticMeshComponent* NewMeshComp = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass(), TEXT("EquippedWeaponMesh"));
	if (!IsValid(NewMeshComp))
	{
		return;
	}

	// 무기 Mesh 적용
	NewMeshComp->SetStaticMesh(WeaponMesh.Get());

	// 실제 충돌 검사는 TraceComponent에서 함
	NewMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	NewMeshComp->SetGenerateOverlapEvents(false);
	NewMeshComp->SetSimulatePhysics(false);

	// Component 추가 -> 런타임 설정
	AddInstanceComponent(NewMeshComp);

	// 부착 계층 설정
	NewMeshComp->SetupAttachment(CharacterSKMesh, WeaponAttachSocketName);

	// Transform 설정 Socket의 설정된 값 사용
	NewMeshComp->SetRelativeTransform(FTransform::Identity);

	// Component 등록 -> 런타임 설정
	NewMeshComp->RegisterComponent();

	// 실제 무기 Mesh Component 참조 저장
	WeaponMeshComponent = NewMeshComp;
	if (IsValid(WeaponMeshComponent))
	{
		AttackTraceComponent->SetTraceMesh(WeaponMeshComponent.Get());
	}

}

void AEnemyBase::SetEnemyState(EEnemyState NewState)
{
	// 상태 검사
	if (IsDead())
	{
		return;
	}

	if (CurrentState == NewState)
	{
		return;
	}

	// 이전 상태 저장
	const EEnemyState PreviousState = CurrentState;

	// 현재 상태 변경
	CurrentState = NewState;

	// 변경된 상태에 따라 이동속도 갱신
	UpdateMovementSpeed();

	// 변경된 상태에 따라 회전 상태 갱신
	UpdateRotationMode();

	// AIController에 상태 변경 갱신 호출
	if (AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController()))
	{
		AIController->UpdateAIFocus(PreviousState, NewState);
	}
}

void AEnemyBase::SetStrafeMode(bool bEnable)
{
	// 현재 Strafe 상태와 설정할 상태가 같으면 건너뜀
	if (bIsStrafeMode == bEnable)
	{
		return;
	}

	bIsStrafeMode = bEnable;

	if (bEnable)
	{
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}
	else
	{
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}
}
