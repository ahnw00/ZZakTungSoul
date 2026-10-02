// Fill out your copyright notice in the Description page of Project Settings.


#include "Warrior/Warrior.h"

//Camera
#include "GameFrameWork/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFrameWork/CharacterMovementComponent.h"

#include "Components/ArrowComponent.h"

//DataTable
//#include "Data/CharacterStatData.h"

//#include "Anim/TestAnimInstance.h"
//#include "Enemy/TestAIController.h"
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

	//LockOnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("LockOnPoint"));
	//LockOnPoint->SetupAttachment(RootComponent);

	// Character Movement Setting
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;

	// Load DataTable
	//CharacterDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/DataTable/DT_CharacterStat.DT_CharacterStat"));

	// Player가 소유하지 않으면 AI가 소유해
	//AIControllerClass = ATestAIController::StaticClass();
	//AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AWarrior::BeginPlay()
{
	Super::BeginPlay();
}

void AWarrior::PostInitializeComponents()
{
	Super::PostInitializeComponents();


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

void AWarrior::UpdateLockOnCamera(float DeltaTime)
{
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

void AWarrior::UpdateLook(float DeltaTime)
{
}

void AWarrior::OnActionMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
}

void AWarrior::UpdateCharacterStats(int32 CharacterLevel)
{
}

void AWarrior::SprintStart()
{
}

void AWarrior::SprintEnd()
{
}

void AWarrior::BlockStart()
{
}

void AWarrior::BlockEnd()
{
}

void AWarrior::Attack()
{
}

void AWarrior::Dodge(FVector ControlDir)
{
}

void AWarrior::LockOn()
{
}

void AWarrior::ChangeLockOn(bool flag)
{
}

bool AWarrior::GetOverlaps()
{
	return false;
}

void AWarrior::SetLockOnTarget(ACharacter* Target)
{
}

void AWarrior::ResetLockOnTarget()
{
}

void AWarrior::SetMoveState(EMoveState NewState)
{
}

void AWarrior::SetActionState(EActionState NewState)
{
}

void AWarrior::SetOverlayState(EOverlayState NewState)
{
}
