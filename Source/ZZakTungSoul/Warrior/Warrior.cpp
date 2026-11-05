// Fill out your copyright notice in the Description page of Project Settings.

#include "Warrior/Warrior.h"

// Camera
#include "GameFrameWork/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFrameWork/CharacterMovementComponent.h"

#include "Components/ArrowComponent.h"
#include <Components/CapsuleComponent.h>

// DataTable
// #include "Data/CharacterStatData.h"
#include <Warrior/WarriorPlayerState.h>
#include <Character/CharacterStatComponent.h>

// Item
#include <ItemData/WeaponDataAsset.h>
#include <Component/EquipmentComponent.h>
#include <Component/InventoryComponent.h>

#include "Anim/WarriorAnimInstance.h"
#include "Enemy/WarriorAIController.h"
#include "Engine/OverlapResult.h"
#include "Interface/InteractionInterface.h"
#include <Interface/FatalAttackableInterface.h>

// Damage
#include <Data/DamageInfo.h>
#include <Interface/CombatDamageInterface.h>
#include <Enemy/Components/AttackTraceComponent.h>
#include <Enemy/Boss/Clown/Gimmick/ClownYabawiBowl.h>

AWarrior::AWarrior()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need
	// it.
	PrimaryActorTick.bCanEverTick = true;

	// Mesh setting
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(
		TEXT("/Game/Assets/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard"));
	if (TempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(TempMesh.Object);

		GetMesh()->SetRelativeLocation(FVector(0, 0, -95));
		GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));

		GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		static ConstructorHelpers::FClassFinder<UAnimInstance> WarriorAnimAsset(
			TEXT("/Game/Blueprints/Anim/ABP_Warrior.ABP_Warrior_C"));
		if (WarriorAnimAsset.Succeeded())
		{
			GetMesh()->SetAnimInstanceClass(WarriorAnimAsset.Class);
			// UE_LOG(LogTemp, Log, TEXT("AnimInstance Set"));
		}
		else
		{
			// UE_LOG(LogTemp, Log, TEXT("AnimInstance Not Set"));
		}
	}

	// SpringArm Setting
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 300.f;
	SpringArm->SetRelativeLocation(FVector(0.f, 0.f, 70.f));
	SpringArm->SocketOffset = FVector(0.f, 30.f, 0.f);
	SpringArm->bUsePawnControlRotation = true;

	// Camera Setting
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera->bUsePawnControlRotation = false;

	LockOnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("LockOnPoint"));
	LockOnPoint->SetupAttachment(RootComponent);

	// Character Movement Setting
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;

	// 무기
	// 스켈레탈 메시 컴포넌트 생성
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	Weapon->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));

	LeftHandWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LeftHandWeapon"));
	LeftHandWeapon->SetupAttachment(GetMesh(), TEXT("hand_lSocket"));

	// TraceComponent가 판정할 수 있는 캡슐 컴포넌트(무기마다 크기 조정 필요)
	// 방패를 들때만 방패 크기에 맞춰 커질거고
	// 만약 왼손에 무기나 아무것도 안들고 있다면 그냥 기본 캡슐 크기가 되어야함
	LeftHandCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("LeftHandCapsule"));
	LeftHandCapsule->SetupAttachment(GetMesh(), TEXT("hand_lSocket"));
	LeftHandCapsule->SetCapsuleHalfHeight(70.f);
	LeftHandCapsule->SetCapsuleRadius(70.f);
	LeftHandCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftHandCapsule->SetCollisionObjectType(ECC_Pawn);
	LeftHandCapsule->ComponentTags.Add(TEXT("ParryCollision"));

	// [스탯 컴포넌트].
	CharacterStatComponent = CreateDefaultSubobject<UCharacterStatComponent>(TEXT("CharacterStatComponent"));
	// [장비 컴포넌트]
	Equipment = CreateDefaultSubobject<UEquipmentComponent>(TEXT("Equipment"));
	// [인벤토리 컴포넌트]
	Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));

	// Trace 무기 판정 -> 장착 가능한 무기 수만큼 컴포너트 추가하면 됩니다. EX) 쌍검 쓸거면 하나 더 추가...
	// Trace Component 설정
	AttackTraceComponent = CreateDefaultSubobject<UAttackTraceComponent>(TEXT("RightAttackTraceComponent"));

	// Player가 소유하지 않으면 AI가 소유해
	AIControllerClass = AWarriorAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AWarrior::BeginPlay()
{
	Super::BeginPlay();

	UpdateCharacterStats();

	// Delegate 연결
	if (AttackTraceComponent)
	{
		AttackTraceComponent->OnTraceHit.AddUObject(this, &AWarrior::HandleAttackTraceHit);

		// Test
		AttackTraceComponent->SetTraceMesh(Weapon);
	}
}

void AWarrior::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AnimInstance = Cast<UWarriorAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->OnMontageEnded.AddDynamic(this, &AWarrior::OnActionMontageEnded);
	}

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AWarrior::OnBeginOverlap);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &AWarrior::OnEndOverlap);

	Equipment->OnUnequipItem.AddUObject(this, &AWarrior::UnequipItem);

	// if (Inventory)
	//{
	//	Inventory->OnItemObtained.BindUObject(this, &AWarrior::ObtainItem);
	// }
}

void AWarrior::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float Speed = GetVelocity().Size2D();

	// UE_LOG(LogTemp, Log, TEXT("Speed: %f"), Speed);

	if (Speed < 10.f)
	{
		MoveState = EMoveState::Idle;
	}
	else if (bIsSprint)
	{
		MoveState = EMoveState::Run;
	}
	else
	{
		MoveState = EMoveState::Walk;
	}

	// 락온 상태
	if (bLockOn)
	{
		UpdateLockOnCamera(DeltaTime);

		if (ActionState != EActionState::Roll)
			UpdateLook(DeltaTime);

		// DrawDebugPoint(GetWorld(), LockOnTarget->GetActorLocation(), 50.f, FColor::Purple, false, 1.f);
	}
}

void AWarrior::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AWarrior::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	CurrentFatalAttackable = Cast<IFatalAttackableInterface>(OtherActor);
}

void AWarrior::OnEndOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	CurrentFatalAttackable = nullptr;
}

// 락온 시에 카메라가 대상에 고정이 되도록(Tick에서 호출될 함수)
void AWarrior::UpdateLockOnCamera(float DeltaTime)
{
	// 락온 컴포넌트 아직 없어서 일단은 그냥 캐릭터 생성하면 붙어있는 Arrow 컴포넌트로 한거
	// Todo: 나중에 LockOn 컴포넌트 만들어서 UI로도 보여주게 해야할거 같아요
	if (LockOnTarget && !LockOnTarget->GetArrowComponent())
		return;
	// else UE_LOG(LogTemp, Log, TEXT("ArrowExists"));

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC)
		return;

	// 스크린 상에서 타겟의 위치
	FVector2D ScreenPos;
	PC->ProjectWorldLocationToScreen(LockOnTarget->GetArrowComponent()->GetComponentLocation(), ScreenPos);

	// 화면 크기
	int32 SizeX, SizeY;
	PC->GetViewportSize(SizeX, SizeY);

	// 락온 기준 위치 구하기
	FVector PlayerLoc = GetActorLocation();
	FVector TargetLoc = LockOnTarget->GetActorLocation();

	float Distance = FVector::Dist(PlayerLoc, TargetLoc);

	constexpr float MinDistance = 300.f;  // 이 거리보다 가까우면
	constexpr float MaxDistance = 1000.f; // 이 거리보다 멀면

	constexpr float MinOffset = 50.f;	// 가까울 때 적용할 오프셋 (절댓값 작음)
	constexpr float MaxOffset = -150.f; // 멀 때 적용할 오프셋 (절댓값 큼)

	float ScreenOffset = FMath::GetMappedRangeValueClamped(
		FVector2D(MinDistance, MaxDistance), FVector2D(MinOffset, MaxOffset), Distance);

	// 거리가 멀면 락온 기준 위치를 중앙에서 좀 위쪽으로
	// 가까우면 락온 기준 위치를 중앙에서 좀 아래쪽으로
	FVector2D LockOnCenter(SizeX * 0.5f, SizeY * 0.5f + ScreenOffset);

	// 화면상에서 락온 기준 위치로부터 타겟이 얼만큼 떨어졌는지
	FVector2D Offset = ScreenPos - LockOnCenter;

	// 현재 카메라의 회전값
	FRotator ControlRot = PC->GetControlRotation();
	// 카메라의 최종 회전값을 나타낼 변수
	FRotator TargetRot = ControlRot;

	// 카메라의 위아래
	TargetRot.Pitch -= Offset.Y * DeltaTime;

	// 화면 상의 거리 차이에 따라 보정 속도 조정
	constexpr float MinScreenOffset = 0.f;	 // 이 차이보다 작으면
	constexpr float MaxScreenOffset = 400.f; // 이 차이보다 크면

	constexpr float MinSpeed = 5.f; // 가까울 때 적용할 속도
	constexpr float MaxSpeed = 7.f; // 멀 때 적용할 속도

	// 타겟이 화면 상에서 락온 기준 위치로부터 많이 떨어져 있으면 회전 속도를 더 빠르게 해
	float RotateSpeed = FMath::GetMappedRangeValueClamped(
		FVector2D(MinScreenOffset, MaxScreenOffset), FVector2D(MinSpeed, MaxSpeed), Offset.Size());

	// 캐릭터가 타겟을 바라보는 방향
	FVector LookVec = TargetLoc - PlayerLoc;
	FRotator LookRot = LookVec.Rotation();

	// 카메라와 (캐릭터가 타겟을 바라보는 방향)에서 Yaw값 차이
	float YawDiff = FMath::FindDeltaAngleDegrees(ControlRot.Yaw, LookRot.Yaw);

	// 중앙에서 좌우로 벌어진 차이의 허용 범위
	constexpr float AllowYaw = 10.f;

	// 카메라의 좌우
	if (YawDiff > AllowYaw)
	{
		TargetRot.Yaw += (YawDiff - AllowYaw) * RotateSpeed * DeltaTime;
	}
	else if (YawDiff < -AllowYaw)
	{
		TargetRot.Yaw += (YawDiff + AllowYaw) * RotateSpeed * DeltaTime;
	}

	TargetRot.Pitch = FMath::ClampAngle(TargetRot.Pitch, -10.f, 45.f);
	PC->SetControlRotation(TargetRot);
}

// 락온 시에 몸이 락온 대상을 향하도록(지금 캐릭터 회전과 SpringArm 회전 비동기 상태라서)
void AWarrior::UpdateLook(float DeltaTime)
{
	if (!LockOnTarget)
		return;
	if (!LockOnTarget->GetArrowComponent())
		return;

	FVector TargetLocation = LockOnTarget->GetArrowComponent()->GetComponentLocation();
	FVector LookVec = TargetLocation - GetActorLocation();

	FRotator TargetRotation = LookVec.Rotation();
	TargetRotation.Pitch = 0.f;
	TargetRotation.Roll = 0.f;

	FRotator CurrentRotation = GetActorRotation();

	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, 5.f);
	SetActorRotation(NewRotation);
}

void AWarrior::OnActionMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// if (Montage == AnimInstance->GetBlockMontage())
	//	return;

	// if (bIsAttacking)
	//{
	//	AttackCombo = 0;
	//	bIsAttacking = false;
	//	bComboQueued = false;
	//	bCanComboInput = false;
	// }

	if (ActionState != EActionState::Attack)
		SetActionState(EActionState::None);

	// if (ActionState == EActionState::Attack)
	//{
	//	SetMoveState(EMoveState::Idle);
	// }
}

void AWarrior::UpdateCharacterStats()
{
	// if (CharacterDataTable == nullptr)
	//	return;

	//// 데이터 테이블에서 정보 가져오기
	// FName RowName = FName(*FString::FromInt(CharacterLevel));
	// CharacterStats =
	//	CharacterDataTable->FindRow<FCharacterStatData>(
	//		RowName,
	//		TEXT("Character Stat Lookup")
	//	);

	// if (CharacterStats)
	//{
	//	GetCharacterMovement()->MaxAcceleration = CharacterStats->MaxAcceleration;
	//	GetCharacterMovement()->MaxWalkSpeed = CharacterStats->MaxWalkSpeed;
	//	GetCharacterMovement()->BrakingDecelerationWalking = CharacterStats->BrakingDecelerationWalking;
	// }

	// 임시로 직접 값 넣어준거
	GetCharacterMovement()->MaxAcceleration = 600.f;
	GetCharacterMovement()->MaxWalkSpeed = 400.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 600.f;

	const FWarriorSaveData* SaveData = GetSaveData();
	if (!SaveData || !CharacterStatComponent)
	{
		return;
	}

	CharacterStatComponent->UpdateCharacterStat(SaveData->Attributes);
}

void AWarrior::SprintStart()
{
	if (bIsSprint)
		return;

	if (ActionState == EActionState::Block)
		return;

	// if (OverlayState == EOverlayState::Block)
	//	return;

	bIsSprint = true;
	// 임시로 값 직접 넣어준거(달리는 속도 600)
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
}

void AWarrior::SprintEnd()
{
	if (!bIsSprint)
		return;

	bIsSprint = false;
	// 다시 원래 걷는 속도로
	GetCharacterMovement()->MaxWalkSpeed = 400.f;
}

void AWarrior::BlockStart()
{
	// 다른 액션을 하고 있으면 무시
	if (ActionState != EActionState::None)
		return;

	// if (OverlayState != EOverlayState::None)
	//	return;

	if (bIsSprint)
		SprintEnd();

	// OverlayState = EOverlayState::Block;
	SetActionState(EActionState::Block);
	// SetOverlayState(EOverlayState::Block);
	// AnimInstance->PlayBlockMontage();
}

void AWarrior::BlockEnd()
{
	if (ActionState != EActionState::Block)
		return;
	// if (OverlayState != EOverlayState::Block)
	//	return;

	// OverlayState = EOverlayState::None;
	SetActionState(EActionState::None);
	// AnimInstance->StopBlockMontage();
}

void AWarrior::Parry()
{
	// 다른 행동을 하고 있으면 무시
	if (ActionState != EActionState::None)
		return;

	SetActionState(EActionState::Parry);
	AnimInstance->PlayActionMontage();
}

void AWarrior::Attack()
{
	switch (ActionState)
	{
	case EActionState::None:
	{
		if (FatalAttack())
			return;
		break;
	}
	case EActionState::Block:
		BlockEnd();
		[[fallthrough]];
	case EActionState::Roll:
		[[fallthrough]];
	case EActionState::BackStep:
		[[fallthrough]];
	case EActionState::Attack:
	{
		if (bCanComboInput)
		{
			bComboQueued = true;
			// UE_LOG(LogTemp, Warning, TEXT("Attack()== ComboQueued : %d"), bComboQueued);
			return;
		}
		break;
	}
	case EActionState::Hit:
		return;
	default:
		break;
	}

	// if (OverlayState != EWeaponState::None)
	//	BlockEnd();

	AttackCombo = 0;
	bIsAttacking = true;
	bCanComboInput = false;
	bComboQueued = false;

	SetActionState(EActionState::Attack);
	if (!AnimInstance || !AnimInstance->PlayActionMontage())
	{
		FinishAttack();
	}
}

void AWarrior::FinishAttack()
{
	if (ActionState == EActionState::Attack)
	{
		AttackCombo = 0;
		bIsAttacking = false;
		bComboQueued = false;
		bCanComboInput = false;

		// SetMoveState(EMoveState::Idle);
		SetActionState(EActionState::None);
	}
}

bool AWarrior::FatalAttack()
{
	if (!CurrentFatalAttackable)
		return false;

	if (!CurrentFatalAttackable->CanBeFatalAttacked(this))
		return false;

	FDamageInfo DamageInfo;
	DamageInfo.AttackDamage = CharacterStatComponent->GetAttackPower() * 2.0f;
	CurrentFatalAttackable->ReceiveFatalAttack(DamageInfo);

	SetActionState(EActionState::FatalAttack);

	// Todo: 위치 보정 필요
	// 적의 Transform 가져오기
	AActor* Enemy = Cast<AActor>(CurrentFatalAttackable);

	const FTransform EnemyTransform = Enemy->GetActorTransform();

	// 적 기준 상대적인 위치
	const FVector RelativeLocation(70.f, 0.f, 0.f);

	// 적의 로컬 좌표를 월드 좌표로 변환
	const FVector TargetLocation = EnemyTransform.TransformPosition(RelativeLocation);

	// 플레이어 위치 보정
	SetActorLocation(TargetLocation);

	// 적을 바라보는 방향으로 회전
	const FRotator TargetRotation = (Enemy->GetActorLocation() - GetActorLocation()).Rotation();

	SetActorRotation(TargetRotation);

	// 플레이어 페이탈 어택 애니메이션 재생
	AnimInstance->PlayActionMontage();

	return true;
}

void AWarrior::SetComboWindow(bool bOpen)
{
	bCanComboInput = bOpen && ActionState == EActionState::Attack && !bComboQueued;
}

void AWarrior::OnComboTransition()
{
	if (ActionState != EActionState::Attack || !bComboQueued)
	{
		// UE_LOG(LogTemp, Warning, TEXT("Can not Combo"));
		FinishAttack();
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("ComboQueued: %d"), bComboQueued);

	bCanComboInput = false;
	bComboQueued = false;

	AttackCombo++;

	if (!AnimInstance || !AnimInstance->PlayActionMontage())
	{
		FinishAttack();
	}
}

void AWarrior::ChargeAttack()
{
	UE_LOG(LogTemp, Log, TEXT("ChargeAttack End"));
}

// 이때 당시에 애니메이션 애셋이 이상해서 위치를 직접 조정해주었는데
// 애셋이 제대로 되어있다면 이렇게 안하고 애니메이션 재생시켜주는거 더 좋을거 같아요
void AWarrior::Dodge(FVector ControlDir)
{
	// UE_LOG(LogTemp, Warning, TEXT("ActionState : %d"), (int)GetActionState());
	// UE_LOG(LogTemp, Warning, TEXT("MoveState : %d"), (int)GetMoveState());
	if (ActionState != EActionState::None)
		return;

	// [스테미너 소모]
	if (CharacterStatComponent && !CharacterStatComponent->UseStamina(25.f))
	{
		return;
	}

	if (MoveState == EMoveState::Idle)
	{
		// ActionState = EActionState::BackStep;
		SetActionState(EActionState::BackStep);
		AnimInstance->PlayActionMontage();
		// LaunchCharacter(-GetActorForwardVector() * 2400.f, true, false);
		return;
	}

	SetActionState(EActionState::Roll);
	AnimInstance->PlayActionMontage();

	// 구를때는 이동 방향으로 굴러야해
	// 회전 안잡아주면 카메라가 바라보는 방향으로만 구르게 돼(특히 락온 상태에서)
	FRotator RollRot = ControlDir.Rotation();
	SetActorRotation(RollRot);
}

void AWarrior::LockOn()
{
	if (!bLockOn)
	{
		if (!GetOverlaps())
			return;

		float BestDot = -1.f;
		FVector CameraLocation = Camera->GetComponentLocation();
		FVector CameraForward = Camera->GetForwardVector();

		for (const auto& OverlapResult : OverlapResults)
		{
			ACharacter* Detected = Cast<ACharacter>(OverlapResult.GetActor());

			if (!Detected)
				continue;

			AController* DetectedController = Detected->GetController();
			if (!DetectedController)
				continue;

			if (DetectedController->IsPlayerController())
				continue;

			FVector Dir = (Detected->GetActorLocation() - CameraLocation).GetSafeNormal();
			float Dot = FVector::DotProduct(Dir, CameraForward);

			// cos60 = 0.5 -> 60도보다 크면 무시
			if (Dot < 0.5f)
				continue;

			// DrawDebugPoint(GetWorld(), Detected->GetActorLocation(), 50.f, FColor::Purple, false, 10.f);
			if (Dot > BestDot)
			{
				BestDot = Dot;
				SetLockOnTarget(Detected);
			}
		}
	}
	else
	{
		ResetLockOnTarget();
	}
}

void AWarrior::ChangeLockOn(bool flag)
{
	if (!bLockOn)
		return;

	if (!GetOverlaps())
		return;

	float BestDot = -1.f;
	FVector CameraLocation = Camera->GetComponentLocation();
	FVector CameraForward = Camera->GetForwardVector();

	for (const auto& OverlapResult : OverlapResults)
	{
		ACharacter* Detected = Cast<ACharacter>(OverlapResult.GetActor());

		if (!Detected)
			continue;
		if (Detected == LockOnTarget)
			continue;
		if (Detected->GetController()->IsPlayerController())
			continue;

		APlayerController* PC = Cast<APlayerController>(GetController());
		if (!PC)
			return;

		// 스크린 상에서 타겟의 위치
		FVector2D ScreenPos;
		PC->ProjectWorldLocationToScreen(Detected->GetArrowComponent()->GetComponentLocation(), ScreenPos);

		// 화면 크기
		int32 SizeX, SizeY;
		PC->GetViewportSize(SizeX, SizeY);

		if (flag && ScreenPos.X > SizeX / 2)
			continue; // 왼쪽키 눌렀는데 감지된게 오른쪽에 있으면
		if (!flag && ScreenPos.X < SizeX / 2)
			continue; // 오른쪽키 눌렀는데 감지된게 왼쪽에 있으면

		FVector Dir = (Detected->GetActorLocation() - CameraLocation).GetSafeNormal();
		float Dot = FVector::DotProduct(Dir, CameraForward);

		// cos60 = 0.5 -> 60도보다 크면 무시
		if (Dot < 0.5f)
			continue;

		// DrawDebugPoint(GetWorld(), Detected->GetActorLocation(), 50.f, FColor::Purple, false, 10.f);
		if (Dot > BestDot)
		{
			BestDot = Dot;
			SetLockOnTarget(Detected);
		}
	}
	UE_LOG(LogTemp, Log, TEXT("Change LockOn"));
}

bool AWarrior::GetOverlaps()
{
	UWorld* World = GetWorld();
	if (!World)
		return false;

	FVector Center = this->GetActorLocation();
	float DetectRadius = 600.f;

	// TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionQueryParam(NAME_None, false, this);
	bool bResult = World->OverlapMultiByChannel(OverlapResults, Center, FQuat::Identity, ECollisionChannel::ECC_Pawn,
		FCollisionShape::MakeSphere(DetectRadius), CollisionQueryParam);

	DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Red, false, 0.2f);

	return bResult;
}

void AWarrior::Interact()
{
	if (DetectedActors.Num() == 0)
		return;

	int32 lastIndex = DetectedActors.Num() - 1;
	AActor* Target = DetectedActors[lastIndex].Get();
	// WeakObjectPtr이라서 유효성 검사
	if (!IsValid(Target))
	{
		DetectedActors.RemoveAt(lastIndex);
		return;
	}

	IInteractableInterface* Interactable = Cast<IInteractableInterface>(Target);
	if (!Interactable)
		return;

	Interactable->Interact(this);
}

void AWarrior::SetLockOnTarget(ACharacter* Target)
{
	bLockOn = true;
	LockOnTarget = Target;
	GetCharacterMovement()->bOrientRotationToMovement = false;
}

void AWarrior::ResetLockOnTarget()
{
	bLockOn = false;
	LockOnTarget = nullptr;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void AWarrior::SetMoveState(EMoveState NewState)
{
	if (MoveState == NewState)
		return;

	MoveState = NewState;
}

void AWarrior::SetActionState(EActionState NewState)
{
	if (NewState != EActionState::None && ActionState != EActionState::None)
		return;

	ActionState = NewState;
}

void AWarrior::SetWeaponState(EWeaponState NewState)
{
	if (WeaponState == NewState)
		return;

	WeaponState = NewState;
}

// void AWarrior::Interact()
//{
//	//if (CurrentInteractable)
//	//{
//	//	CurrentInteractable->Interact(this);
//	//}
// }

const FWarriorSaveData* AWarrior::GetSaveData() const
{
	if (AWarriorPlayerState* WarriorPlayerState = GetPlayerState<AWarriorPlayerState>())
	{
		return &WarriorPlayerState->GetSaveData();
	}
	return nullptr;
}

void AWarrior::ReceiveDamage(const FDamageInfo& DamageInfo, const FHitResult& Hit)
{
	if (nullptr == CharacterStatComponent)
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

	if (!AnimInstance && !CharacterStatComponent)
	{
		return;
	}

	float Poise = CharacterStatComponent->GetPoise();
	float CurrentStamina = CharacterStatComponent->GetCurrentStamina();

	if (Poise < DamageInfo.PoiseDamage)
	{
		// 스텐스 낮추기.
		bool StanceIsZero = CharacterStatComponent->TakeStanceDamage(DamageInfo.StanceDamage);

		if (StanceIsZero)
		{
			SetActionState(EActionState::Stun);
			AnimInstance->PlayActionMontage();
			CharacterStatComponent->ApplyDamage(DamageInfo);
		}
		else if (CurrentStamina == 0 && GetActionState() == EActionState::Block)
		{
			CharacterStatComponent->SetCurrentStance(0);
			SetActionState(EActionState::Stun);
			AnimInstance->PlayActionMontage();
			CharacterStatComponent->ApplyDamage(DamageInfo);
		}
		else
		{
			SetActionState(EActionState::Hit);
			AnimInstance->PlayActionMontage();
			CharacterStatComponent->ApplyDamage(DamageInfo);
		}
	}
	else
	{
		// 맞았는데, 강인도가 높아서 액션 취소 안됨.
		SetActionState(EActionState::Hit);
		AnimInstance->PlayActionMontage();
		CharacterStatComponent->ApplyDamage(DamageInfo);
	}
}

void AWarrior::HandleAttackTraceHit(const FHitResult& Hit)
{
	AActor* HitActor = Hit.GetActor();
	if (!IsValid(HitActor))
	{
		return;
	}
	if (AClownYabawiBowl* Bowl = Cast<AClownYabawiBowl>(HitActor))
	{
		Bowl->ReceivePlayerAttack(this, Hit);
		return;
	}

	// UE_LOG(LogTemp, Log, TEXT("Attack Hit: %s"), *HitActor->GetName());

	// 인터페이스 형변환
	ICombatDamageInterface* HitTarget = Cast<ICombatDamageInterface>(HitActor);
	if (nullptr == HitTarget)
	{
		UE_LOG(LogTemp, Log, TEXT("Player Attack Noooooooooo Hit!!!!!"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Player Attack Hit!!!!!########!"));

	// 공격 데이터 설정
	FDamageInfo DamageInfo;

	// @@Todo: 나중에 수치 하드 코딩이 아니라 외부에서 데이터 설정해서 가져오도록 구현 EX: AnimNotify, AssetData 등등
	DamageInfo.AttackDamage = 50.0f;

	// 데미지 처리
	HitTarget->ReceiveDamage(DamageInfo, Hit);
}

void AWarrior::EquipWeapon(const FGuid& InID, EEquipmentSlot Slot)
{
	Equipment->EquipItem(InID, Slot);

	const UItemDataAsset* InItem = Inventory->FindItemDataByInstanceID(InID);
	const UWeaponDataAsset* WeaponData = Cast<UWeaponDataAsset>(InItem);

	if (!WeaponData)
		return;

	// 아직 로드되지 않았다면
	if (WeaponData->Mesh.IsPending())
	{
		// 동기 로딩
		WeaponData->Mesh.LoadSynchronous();
	}

	// Todo: 각각의 슬롯 위치에 따라 장착을 달리해줘야해
	// 이 함수가 호출되기 전에 장착 가능한 슬롯은 이미 걸러져서 또 확인 안해도 돼
	// AttackTraceComponent도 왼쪽 오른쪽 다 있어야해

	// 임시
	if (Slot == EEquipmentSlot::RightHand)
	{
		Weapon->SetSkeletalMesh(WeaponData->Mesh.Get());
		Weapon->SetRelativeRotation(WeaponData->EquipRotation);
		AttackTraceComponent->SetTraceMesh(Weapon);
	}
	else if (Slot == EEquipmentSlot::LeftHand)
	{
		LeftHandWeapon->SetSkeletalMesh(WeaponData->Mesh.Get());
		LeftHandWeapon->SetRelativeRotation(WeaponData->EquipRotation);
		// Todo: 왼쪽 AttackTraceComponent에 메시 설정해주기
	}

	// Todo: 무기 스탯 연동해주기
}

void AWarrior::UnequipItem(EEquipmentSlot Slot)
{
	switch (Slot)
	{
	case EEquipmentSlot::None:
		break;
	case EEquipmentSlot::RightHand:
		Weapon->SetSkeletalMesh(nullptr);
		break;
	case EEquipmentSlot::LeftHand:
		LeftHandWeapon->SetSkeletalMesh(nullptr);
		break;
	case EEquipmentSlot::Head:
		break;
	case EEquipmentSlot::Chest:
		break;
	case EEquipmentSlot::Arms:
		break;
	case EEquipmentSlot::Legs:
		break;
	case EEquipmentSlot::UpperBelt:
		break;
	case EEquipmentSlot::LowerBelt:
		break;
	default:
		break;
	}
}

void AWarrior::ObtainItem(UItemDataAsset* detectedItem)
{
	if (DetectedActors.Num() == 0)
		return;

	// 마지막으로 추가된걸 상호작용
	int32 lastIndex = DetectedActors.Num() - 1;
	AItemObject* ItemObject = Cast<AItemObject>(DetectedActors[lastIndex].Get());
	if (!ItemObject)
		return;

	// 인벤토리에 추가
	Inventory->AddItem(ItemObject->GetItemData(), ItemObject->GetInstanceID());
}

// float AWarrior::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor*
// DamageCauser)
//{
//	float ret = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
//
//	if (!AnimInstance && !CharacterStatComponent)
//	{
//		CharacterStatComponent->ApplyDamage(Damage);
//
//		//SetActionState(EActionState::Hit);
//		//SetActionState(EActionState::Stun);
//		AnimInstance->PlayActionMontage();
//	}
//
//	return ret;
// }

float AWarrior::TakeDamaged(
	float Damage, float PoiseDamage, float StanceDamage, bool bUnblockable, bool bIgnoreInvincibility)
{
	if (!AnimInstance && !CharacterStatComponent)
	{
		float Poise = CharacterStatComponent->GetPoise();
		float CurrentStamina = CharacterStatComponent->GetCurrentStamina();

		if (Poise < PoiseDamage)
		{
			// 스텐스 낮추기.
			bool StanceIsZero = CharacterStatComponent->TakeStanceDamage(StanceDamage);

			if (StanceIsZero)
			{
				// SetActionState(EActionState::Stun);
			}
			else if (CurrentStamina == 0 && GetActionState() == EActionState::Block)
			{
				CharacterStatComponent->SetCurrentStance(0);
				SetActionState(EActionState::Stun);
				AnimInstance->PlayActionMontage();
			}
			else
			{
				SetActionState(EActionState::Hit);
				AnimInstance->PlayActionMontage();
			}
		}
		else
		{
			// 맞았는데, 강인도가 높아서 액션 취소 안됨.
		}

		// CharacterStatComponent->ApplyDamage(Damage);
	}

	return 0.0f;
}

float AWarrior::TakeDamageTest_1(
	float Damage, float PoiseDamage, float StanceDamage, bool bUnblockable, bool bIgnoreInvincibility)
{
	SetActionState(EActionState::Stun);
	AnimInstance->PlayActionMontage();
	return 0.0f;
}

float AWarrior::TakeDamageTest_2(
	float Damage, float PoiseDamage, float StanceDamage, bool bUnblockable, bool bIgnoreInvincibility)
{
	SetActionState(EActionState::Hit);
	AnimInstance->PlayActionMontage();
	return 0.0f;
}

void AWarrior::ReceiveDamageTest_1(const FDamageInfo& DamageInfo, const FHitResult& Hit)
{
	SetActionState(EActionState::Stun);
	AnimInstance->PlayActionMontage();
}

void AWarrior::ReceiveDamageTest_2(const FDamageInfo& DamageInfo, const FHitResult& Hit)
{
	SetActionState(EActionState::Hit);
	AnimInstance->PlayActionMontage();
}
