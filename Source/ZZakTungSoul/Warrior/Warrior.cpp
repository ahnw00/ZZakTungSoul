// Fill out your copyright notice in the Description page of Project Settings.


#include "Warrior/Warrior.h"

//Camera
#include "GameFrameWork/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFrameWork/CharacterMovementComponent.h"

#include "Components/ArrowComponent.h"

//DataTable
//#include "Data/CharacterStatData.h"
#include <Warrior/WarriorPlayerState.h>
#include <Character/CharacterStatComponent.h>

#include "Anim/WarriorAnimInstance.h"
#include "Enemy/WarriorAIController.h"
#include "Engine/OverlapResult.h"

AWarrior::AWarrior()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Mesh setting
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh
	(TEXT("/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard"));
	if (TempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(TempMesh.Object);

		GetMesh()->SetRelativeLocation(FVector(0, 0, -88));
		GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));

		GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		static ConstructorHelpers::FClassFinder<UAnimInstance> WarriorAnimAsset
		(TEXT("/Game/Blueprints/Anim/ABP_Warrior.ABP_Warrior_C"));
		if (WarriorAnimAsset.Succeeded())
		{
			GetMesh()->SetAnimInstanceClass(WarriorAnimAsset.Class);
			//UE_LOG(LogTemp, Log, TEXT("AnimInstance Set"));
		}
		else
		{
			//UE_LOG(LogTemp, Log, TEXT("AnimInstance Not Set"));
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

	// Load DataTable
	//CharacterDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/DataTable/DT_CharacterStat.DT_CharacterStat"));

	// [스탯 컴포넌트].
	CharacterStatComponent = CreateDefaultSubobject<UCharacterStatComponent>(TEXT("CharacterStatComponent"));
	

	// Player가 소유하지 않으면 AI가 소유해
	AIControllerClass = AWarriorAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AWarrior::BeginPlay()
{
	Super::BeginPlay();

	UpdateCharacterStats();
}

void AWarrior::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AnimInstance = Cast<UWarriorAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->OnMontageEnded.AddDynamic(this, &AWarrior::OnActionMontageEnded);
	}
}

void AWarrior::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float Speed = GetVelocity().Size2D();

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

	// 구를때 제자리에서 구르는 몽타주라서 직접 이동해주기
	if (ActionState == EActionState::Roll)
	{
		AddActorWorldOffset(GetActorForwardVector() * 400.f * DeltaTime, true);
	}

	// 락온 상태
	if (bLockOn)
	{
		UpdateLockOnCamera(DeltaTime);

		if (ActionState != EActionState::Roll)
			UpdateLook(DeltaTime);

		//DrawDebugPoint(GetWorld(), LockOnTarget->GetActorLocation(), 50.f, FColor::Purple, false, 1.f);
	}
}

void AWarrior::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


}

// 락온 시에 카메라가 대상에 고정이 되도록(Tick에서 호출될 함수)
void AWarrior::UpdateLockOnCamera(float DeltaTime)
{
	// 락온 컴포넌트 아직 없어서 일단은 그냥 캐릭터 생성하면 붙어있는 Arrow 컴포넌트로 한거
	// Todo: 나중에 LockOn 컴포넌트 만들어서 UI로도 보여주게 해야할거 같아요
	if (!LockOnTarget->GetArrowComponent())
		return;
	//else UE_LOG(LogTemp, Log, TEXT("ArrowExists"));

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

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

	constexpr float MinOffset = 50.f;     // 가까울 때 적용할 오프셋 (절댓값 작음)
	constexpr float MaxOffset = -150.f;   // 멀 때 적용할 오프셋 (절댓값 큼)

	float ScreenOffset = FMath::GetMappedRangeValueClamped(
		FVector2D(MinDistance, MaxDistance),
		FVector2D(MinOffset, MaxOffset),
		Distance
	);

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
	constexpr float MinScreenOffset = 0.f;   // 이 차이보다 작으면
	constexpr float MaxScreenOffset = 400.f; // 이 차이보다 크면

	constexpr float MinSpeed = 5.f; // 가까울 때 적용할 속도
	constexpr float MaxSpeed = 7.f;  // 멀 때 적용할 속도

	// 타겟이 화면 상에서 락온 기준 위치로부터 많이 떨어져 있으면 회전 속도를 더 빠르게 해
	float RotateSpeed = FMath::GetMappedRangeValueClamped(
		FVector2D(MinScreenOffset, MaxScreenOffset),
		FVector2D(MinSpeed, MaxSpeed),
		Offset.Size()
	);

	// 캐릭터가 타겟을 바라보는 방향
	FVector LookVec = TargetLoc - PlayerLoc;
	FRotator LookRot = LookVec.Rotation();

	// 카메라와 (캐릭터가 타겟을 바라보는 방향)에서 Yaw값 차이
	float YawDiff = FMath::FindDeltaAngleDegrees(
		ControlRot.Yaw,
		LookRot.Yaw
	);


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
	if (!LockOnTarget) return;
	if (!LockOnTarget->GetArrowComponent()) return;

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
	if (Montage == AnimInstance->GetBlockMontage())
		return;

	if (bIsAttacking)
	{
		bIsAttacking = false;
		AttackCombo = 0;
	}
	SetActionState(EActionState::None);
}

void AWarrior::UpdateCharacterStats()
{
	//if (CharacterDataTable == nullptr)
	//	return;

	//// 데이터 테이블에서 정보 가져오기
	//FName RowName = FName(*FString::FromInt(CharacterLevel));
	//CharacterStats =
	//	CharacterDataTable->FindRow<FCharacterStatData>(
	//		RowName,
	//		TEXT("Character Stat Lookup")
	//	);

	//if (CharacterStats)
	//{
	//	GetCharacterMovement()->MaxAcceleration = CharacterStats->MaxAcceleration;
	//	GetCharacterMovement()->MaxWalkSpeed = CharacterStats->MaxWalkSpeed;
	//	GetCharacterMovement()->BrakingDecelerationWalking = CharacterStats->BrakingDecelerationWalking;
	//}

	// 임시로 직접 값 넣어준거
	GetCharacterMovement()->MaxAcceleration = 600.f;
	GetCharacterMovement()->MaxWalkSpeed = 400.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 600.f;

	const FWarriorSaveData* SaveData = GetSaveData();
	if (!SaveData || !CharacterStatComponent) { return; }

	CharacterStatComponent->UpdateCharacterStat(SaveData->Attributes);
}

void AWarrior::SprintStart()
{
	if (bIsSprint)
		return;

	bIsSprint = true;
	// 임시로 값 직접 넣어준거(달리는 속도 600)
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
}

void AWarrior::SprintEnd()
{
	bIsSprint = false;
	// 다시 원래 걷는 속도로
	GetCharacterMovement()->MaxWalkSpeed = 400.f;
}

void AWarrior::BlockStart()
{
	if (ActionState != EActionState::None)
		return;
	if (OverlayState != EOverlayState::None)
		return;

	//OverlayState = EOverlayState::Block;
	SetOverlayState(EOverlayState::Block);
	AnimInstance->PlayBlockMontage();
}

void AWarrior::BlockEnd()
{
	if (OverlayState != EOverlayState::Block)
		return;

	//OverlayState = EOverlayState::None;
	SetOverlayState(EOverlayState::None);
	AnimInstance->StopBlockMontage();
}

void AWarrior::Attack()
{
	if (!bIsAttacking)
		bIsAttacking = true;
	else // if (bIsAttacking)
		AttackCombo++;

	if (ActionState != EActionState::None)
		return;
	if (OverlayState != EOverlayState::None)
		BlockEnd();

	//ActionState = EActionState::Attack;
	SetActionState(EActionState::Attack);
	AnimInstance->PlayAttackMontage();
}

// 이때 당시에 애니메이션 애셋이 이상해서 위치를 직접 조정해주었는데
// 애셋이 제대로 되어있다면 이렇게 안하고 애니메이션 재생시켜주는거 더 좋을거 같아요
void AWarrior::Dodge(FVector ControlDir)
{
	//UE_LOG(LogTemp, Warning, TEXT("ActionState : %d"), (int)GetActionState());
	//UE_LOG(LogTemp, Warning, TEXT("MoveState : %d"), (int)GetMoveState());
	if (ActionState != EActionState::None)
		return;

	// [스테미너 소모]
	if (CharacterStatComponent && !CharacterStatComponent->UseStamina(25.f))
	{
		return;
	}

	if (MoveState == EMoveState::Idle)
	{
		//ActionState = EActionState::BackStep;
		SetActionState(EActionState::BackStep);
		AnimInstance->PlayBackStepMontage();
		LaunchCharacter(-GetActorForwardVector() * 2400.f, true, false);
		return;
	}

	SetActionState(EActionState::Roll);
	AnimInstance->PlayRollMontage();

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

			if (!Detected) continue;
			if (Detected->GetController()->IsPlayerController()) continue;

			FVector Dir = (Detected->GetActorLocation() - CameraLocation).GetSafeNormal();
			float Dot = FVector::DotProduct(Dir, CameraForward);

			// cos60 = 0.5 -> 60도보다 크면 무시
			if (Dot < 0.5f) continue;

			//DrawDebugPoint(GetWorld(), Detected->GetActorLocation(), 50.f, FColor::Purple, false, 10.f);
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
	if (!bLockOn) return;

	if (!GetOverlaps())
		return;

	float BestDot = -1.f;
	FVector CameraLocation = Camera->GetComponentLocation();
	FVector CameraForward = Camera->GetForwardVector();

	for (const auto& OverlapResult : OverlapResults)
	{
		ACharacter* Detected = Cast<ACharacter>(OverlapResult.GetActor());

		if (!Detected) continue;
		if (Detected == LockOnTarget) continue;
		if (Detected->GetController()->IsPlayerController()) continue;

		APlayerController* PC = Cast<APlayerController>(GetController());
		if (!PC) return;

		// 스크린 상에서 타겟의 위치
		FVector2D ScreenPos;
		PC->ProjectWorldLocationToScreen(Detected->GetArrowComponent()->GetComponentLocation(), ScreenPos);

		// 화면 크기
		int32 SizeX, SizeY;
		PC->GetViewportSize(SizeX, SizeY);

		if (flag && ScreenPos.X > SizeX / 2) continue; // 왼쪽키 눌렀는데 감지된게 오른쪽에 있으면
		if (!flag && ScreenPos.X < SizeX / 2) continue;// 오른쪽키 눌렀는데 감지된게 왼쪽에 있으면

		FVector Dir = (Detected->GetActorLocation() - CameraLocation).GetSafeNormal();
		float Dot = FVector::DotProduct(Dir, CameraForward);

		// cos60 = 0.5 -> 60도보다 크면 무시
		if (Dot < 0.5f) continue;

		//DrawDebugPoint(GetWorld(), Detected->GetActorLocation(), 50.f, FColor::Purple, false, 10.f);
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
	if (!World) return false;

	FVector Center = this->GetActorLocation();
	float DetectRadius = 600.f;

	//TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionQueryParam(NAME_None, false, this);
	bool bResult = World->OverlapMultiByChannel(
		OverlapResults,
		Center,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(DetectRadius),
		CollisionQueryParam
	);

	DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Red, false, 0.2f);

	return bResult;
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

void AWarrior::SetOverlayState(EOverlayState NewState)
{
	if (OverlayState == NewState)
		return;

	OverlayState = NewState;
}

const FWarriorSaveData* AWarrior::GetSaveData() const
{
	if (AWarriorPlayerState* WarriorPlayerState = GetPlayerState<AWarriorPlayerState>())
	{
		return &WarriorPlayerState->GetSaveData();
	}
	return nullptr;
}
