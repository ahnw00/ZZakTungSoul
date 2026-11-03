// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyBase.h"
#include <Enemy/EnemyAIController.h>
#include <Enemy/Data/EnemyAIData.h>
#include <Enemy/Animation/EnemyBaseAnimInstance.h>
#include <Enemy/Components/AttackTraceComponent.h>
#include <Enemy/Components/EnemyStatComponent.h>
#include <Character/CharacterStatComponent.h>
#include <Interface/CombatDamageInterface.h>
#include <Data/DamageInfo.h>

#include <GameFramework/CharacterMovementComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Components/SkeletalMeshComponent.h>
#include <Components/BoxComponent.h>
#include <Engine/StaticMesh.h>

AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = false;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshRef(
		TEXT("/Game/Assets/InfinityBladeWarriors/Character/CompleteCharacters/sk_CharM_Base.sk_CharM_Base")
	);
	if(MeshRef.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MeshRef.Object);

		GetMesh()->SetRelativeLocation(FVector(0, 0, -88));
		GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));

		GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		static ConstructorHelpers::FClassFinder<UAnimInstance> EnemyAnimAsset
		(TEXT("/Game/Blueprints/Enemy/Animation/ABP_EnemyBase.ABP_EnemyBase_C"));
		if (EnemyAnimAsset.Succeeded())
		{
			GetMesh()->SetAnimInstanceClass(EnemyAnimAsset.Class);
			//UE_LOG(LogTemp, Log, TEXT("AnimInstance Set"));
		}
		else
		{
			//UE_LOG(LogTemp, Log, TEXT("AnimInstance Not Set"));
		}
	}

	FatalAttackBox = CreateDefaultSubobject<UBoxComponent>(TEXT("FatalAttackBox"));
	FatalAttackBox->SetupAttachment(RootComponent);
	FatalAttackBox->SetBoxExtent(FVector(100.f, 32.f, 32.f));

	// 이동 방향 회전 설정
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	// RVO 설정
	GetCharacterMovement()->bUseRVOAvoidance = true;
	GetCharacterMovement()->AvoidanceConsiderationRadius = 100.0f;
	GetCharacterMovement()->AvoidanceWeight = 0.3f;


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

	if (EnemyStatComponet)
	{
		EnemyStatComponet->OnEnemyHpZero.AddUObject(this, &AEnemyBase::SetDead);
	}

	AttackTraceComponent->OnGetParried.AddUObject(this, &AEnemyBase::OnGetParried);
}

void AEnemyBase::ReceiveDamage(const FDamageInfo& DamageInfo, const FHitResult& Hit)
{
	//Super::ReceiveDamage(DamageInfo, Hit);

	if (IsDead())
	{
		return;
	}

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
		Movement->MaxWalkSpeed = EnemyStatComponet->GetWalkSpeed();
		break;

	case EEnemyState::Search:
		Movement->MaxWalkSpeed = EnemyStatComponet->GetWalkSpeed();
		break;

	case EEnemyState::Hit:
		Movement->MaxWalkSpeed = 0.0f;
		break;

	case EEnemyState::Groggy:
		Movement->MaxWalkSpeed = 0.0f;
		break;

	case EEnemyState::Avoid:
		Movement->MaxWalkSpeed = EnemyStatComponet->GetWalkSpeed();
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
	ICombatDamageInterface* HitTarget = Cast<ICombatDamageInterface>(HitActor);
	if (nullptr == HitTarget)
	{
		UE_LOG(LogTemp, Log, TEXT("Attack Noooooooooo Hit!!!!!"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Attack Hit!!!!!@!@!@@@@@@!"));

	// 데미지 처리
	HitTarget->ReceiveDamage(CurrentDamageInfo, Hit);

	UE_LOG(LogTemp, Log, TEXT("Attack %f : %f : %f"), CurrentDamageInfo.AttackDamage, CurrentDamageInfo.PoiseDamage, CurrentDamageInfo.StanceDamage);

	/* 스탯 컴포넌트 가져오기
	TargetStatComp = HitTarget->GetCharacterStatComponent();
	if (nullptr == TargetStatComp)
	{
		return;
	}

	// @Todo: 데미지 임시 처리
	TargetStatComp->ApplyDamage(50.0f);		//*/
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

void AEnemyBase::SetDead()
{
	// 중복 방지
	if (IsDead())
	{
		return;
	}

	// 사망 상태 설정
	SetEnemyState(EEnemyState::Dead);

	// AI 중지 처리
	if (AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController()))
	{
		AIController->StopAIController();
	}

	// 공격 판정 중지
	if (IsValid(AttackTraceComponent))
	{
		AttackTraceComponent->EndTrace();
	}

	// 상위 클래스 공통 로직 실행
	Super::SetDead();

	// 실제 사망 로직 실행
	DeadStart();
}

void AEnemyBase::DeadStart()
{

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

// 현재 쓰이진 않지만 일단 남겨둠 -> SetStrafeMode Task에서 사용하지만 해당 Task도 사용은 안함
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

void AEnemyBase::OnGetParried()
{
	SetEnemyState(EEnemyState::Groggy);

	// 임시 테스트용
	if (!GetController())
		return;
	GetController()->UnPossess();

	UEnemyBaseAnimInstance* AnimInstance = Cast<UEnemyBaseAnimInstance>(GetMesh()->GetAnimInstance());
	if (!AnimInstance)
	{
		return;
	}

	AnimInstance->PlayStunMontage();
}

float AEnemyBase::GetCombatRangeData()
{
	return EnemyAIData->CombatRange;
}

float AEnemyBase::GetAttackRangeData()
{
	return EnemyAIData->AttackRange;
}

float AEnemyBase::GetMinMaintainDistanceData()
{
	return EnemyAIData->MinMaintainDistance;
}

UEnemyAnimationData* AEnemyBase::GetAnimationData()
{
	return EnemyAnimationData;
}

void AEnemyBase::SetCurrentDamageInfoInterface(const FDamageInfo& NewDamageInfo)
{
	CurrentDamageInfo = NewDamageInfo;
}

bool AEnemyBase::CanBeFatalAttacked(AActor* Attacker) const
{
	if (CurrentState != EEnemyState::Groggy)
		return false;

	float Dot = FVector::DotProduct(Attacker->GetActorForwardVector(), GetActorForwardVector());

	// 그로기 상태에 걸린 적과 플레이어의 각이 크면 안돼
	// 두 액터가 정확하게 마주볼 때 Dot값은 -1
	if (Dot > -0.8f)
		return false;

	return true;
}

void AEnemyBase::ReceiveFatalAttack(const FDamageInfo DamageInfo)
{
	// 페이탈 어택 당하는 애니메이션 재생
	UEnemyBaseAnimInstance* Anim = Cast<UEnemyBaseAnimInstance>(GetMesh()->GetAnimInstance());
	if (!Anim)
		return;

	Anim->PlayGetFatalAttackMontage();

	// 페이탈 어택 데미지 적용
	ReceiveDamage(DamageInfo, FHitResult());
}

APatrolPath* AEnemyBase::GetPatrolPathData() const
{
	return nullptr;
}