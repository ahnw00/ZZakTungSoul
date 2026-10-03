// Fill out your copyright notice in the Description page of Project Settings.


#include "Warrior/WarriorController.h"
#include "Warrior.h"

#include <InputMappingContext.h>
#include <InputAction.h>

#include <EnhancedInputSubsystems.h>
#include <EnhancedInputComponent.h>

#include <TimerManager.h>
#include <Engine/OverlapResult.h>

AWarriorController::AWarriorController()
{
	PrimaryActorTick.bCanEverTick = true;

	// ConstructorHelpers는 생성자에서 COD(Class Default Object)를 생성하는 시점에 에셋을 찾아서 연결하기 위함
	// IMC랑 IA 에셋 가져오기
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> imc_warrior(TEXT("/Game/Input/IMC_Warrior.IMC_Warrior"));
	if (imc_warrior.Succeeded()) IMC_Warrior = imc_warrior.Object;
	static ConstructorHelpers::FObjectFinder<UInputAction> ia_move(TEXT("/Game/Input/IA_Move.IA_Move"));
	if (ia_move.Succeeded()) IA_Move = ia_move.Object;
	static ConstructorHelpers::FObjectFinder<UInputAction> ia_look(TEXT("/Game/Input/IA_Look.IA_Look"));
	if (ia_look.Succeeded()) IA_Look = ia_look.Object;
	static ConstructorHelpers::FObjectFinder<UInputAction> ia_dodgedash(TEXT("/Game/Input/IA_DodgeDash.IA_DodgeDash"));
	if (ia_dodgedash.Succeeded()) IA_DodgeDash = ia_dodgedash.Object;
	static ConstructorHelpers::FObjectFinder<UInputAction> ia_block(TEXT("/Game/Input/IA_Block.IA_Block"));
	if (ia_block.Succeeded()) IA_Block = ia_block.Object;
	static ConstructorHelpers::FObjectFinder<UInputAction> ia_attack(TEXT("/Game/Input/IA_Attack.IA_Attack"));
	if (ia_attack.Succeeded()) IA_Attack = ia_attack.Object;
	static ConstructorHelpers::FObjectFinder<UInputAction> ia_lockon(TEXT("/Game/Input/IA_LockOn.IA_LockOn"));
	if (ia_lockon.Succeeded()) IA_LockOn = ia_lockon.Object;
	static ConstructorHelpers::FObjectFinder<UInputAction> ia_changelockon(TEXT("/Game/Input/IA_ChangeLockOn.IA_ChangeLockOn"));
	if (ia_changelockon.Succeeded()) IA_ChangeLockOn = ia_changelockon.Object;
}

void AWarriorController::BeginPlay()
{
	Super::BeginPlay();

	// LocalPlayerSubsystem 중에서도 EnhancedInput에 내가 생성한 IMC 매핑해주기
	auto subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (subsystem)
	{
		subsystem->AddMappingContext(IMC_Warrior, 0);
	}

	if (PlayerCameraManager)
	{
		PlayerCameraManager->ViewPitchMin = -60.f;
		PlayerCameraManager->ViewPitchMax = 45.f;
	}
	else UE_LOG(LogTemp, Log, TEXT("No PlayerCameraManager"));
}

void AWarriorController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bActionPressed)
		return;

	float HoldTime = GetWorld()->GetTimeSeconds() - PressStarted;
	if (HoldTime >= 0.2f)
	{
		if (AWarrior* Warrior = Cast<AWarrior>(GetPawn()))
		{
			if (!Warrior->IsSprint())
				SprintStart();
		}
	}
}

void AWarriorController::SetupInputComponent()
{
	Super::SetupInputComponent();

	auto EnhancedInput = CastChecked<UEnhancedInputComponent>(InputComponent); // cast 성공하면 다음줄로 아니면 크래시 일으켜

	EnhancedInput->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AWarriorController::Move);
	EnhancedInput->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AWarriorController::Look);
	EnhancedInput->BindAction(IA_DodgeDash, ETriggerEvent::Started, this, &AWarriorController::ActionPressed);
	EnhancedInput->BindAction(IA_DodgeDash, ETriggerEvent::Completed, this, &AWarriorController::ActionReleased);
	EnhancedInput->BindAction(IA_Block, ETriggerEvent::Started, this, &AWarriorController::BlockStart);
	EnhancedInput->BindAction(IA_Block, ETriggerEvent::Completed, this, &AWarriorController::BlockEnd);
	EnhancedInput->BindAction(IA_Attack, ETriggerEvent::Started, this, &AWarriorController::Attack);
	EnhancedInput->BindAction(IA_LockOn, ETriggerEvent::Completed, this, &AWarriorController::LockOn);
	EnhancedInput->BindAction(IA_ChangeLockOn, ETriggerEvent::Triggered, this, &AWarriorController::ChangeLockOn);
}

void AWarriorController::Move(const FInputActionValue& InputValue)
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)
	{
		UE_LOG(LogTemp, Log, TEXT("Cannot get ControlledPawn"));
		return;
	}

	const FVector2D MoveVector = InputValue.Get<FVector2D>();
	if (MoveVector.Size() < .1f)
		return;

	if (AWarrior* Warrior = Cast<AWarrior>(ControlledPawn))
	{
		//UE_LOG(LogTemp, Warning, TEXT("State : %d"), (int)Warrior->GetActionState());
		if (Warrior->GetActionState() != EActionState::None)
			return;
	}

	const FRotator YawRotation(0.f, GetControlRotation().Yaw, 0.f);

	const FVector ForwardDirection =
		FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X); // 여기서의 X는 행렬에서 0번째 행(X축을 기준으로 돈 값)을 의미하는거

	const FVector RightDirection =
		FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y); // 여기서의 Y는 행렬에서 1번째 행(Y축을 기준으로 돈 값)을 의미하는거

	ControlDir = ForwardDirection * MoveVector.Y + RightDirection * MoveVector.X;

	ControlledPawn->AddMovementInput(ForwardDirection, MoveVector.Y);
	ControlledPawn->AddMovementInput(RightDirection, MoveVector.X);

	//UE_LOG(LogTemp, Log, TEXT("Move"));
}

void AWarriorController::Idle()
{
}

void AWarriorController::Look(const FInputActionValue& InputValue)
{
	if (AWarrior* Warrior = Cast<AWarrior>(GetPawn()))
	{
		if (Warrior->IsLockOn())
			return;
	}

	const FVector2D LookVector = InputValue.Get<FVector2D>();
	//UE_LOG(LogTemp, Log, TEXT("x: %f, y: %f"), LookVector.X, LookVector.Y);
	if (LookVector.Size() < .1f)
		return;

	AddYawInput(LookVector.X);
	AddPitchInput(LookVector.Y);

	//UE_LOG(LogTemp, Log, TEXT("Look"));
}
void AWarriorController::SprintStart()
{
	if (AWarrior* Warrior = Cast<AWarrior>(GetPawn()))
	{
		Warrior->SprintStart();
	}
}

void AWarriorController::SprintEnd()
{
	if (AWarrior* Warrior = Cast<AWarrior>(GetPawn()))
	{
		Warrior->SprintEnd();
	}
}

void AWarriorController::BlockStart()
{
	if (AWarrior* Warrior = Cast<AWarrior>(GetPawn()))
	{
		Warrior->BlockStart();
	}
	//UE_LOG(LogTemp, Log, TEXT("Block"));
}

void AWarriorController::BlockEnd()
{
	if (AWarrior* Warrior = Cast<AWarrior>(GetPawn()))
	{
		Warrior->BlockEnd();
	}
}

void AWarriorController::Dodge()
{
	if (AWarrior* Warrior = Cast<AWarrior>(GetPawn()))
	{
		if (Warrior->GetActionState() != EActionState::None)
			return;

		Warrior->Dodge(ControlDir);
	}

	//UE_LOG(LogTemp, Log, TEXT("Dodge"));
}

void AWarriorController::Roll()
{
}

void AWarriorController::BackStep()
{
}

void AWarriorController::Attack()
{
	if (AWarrior* Warrior = Cast<AWarrior>(GetPawn()))
	{
		Warrior->Attack();
	}
	//UE_LOG(LogTemp, Log, TEXT("Attack"));
}

void AWarriorController::LockOn()
{
	if (AWarrior* Warrior = Cast<AWarrior>(GetPawn()))
	{
		Warrior->LockOn();
	}
	//UE_LOG(LogTemp, Log, TEXT("LockOn"));
}

void AWarriorController::ChangeLockOn(const FInputActionValue& InputValue)
{
	float InputX = InputValue.Get<FVector2D>().X;
	if (FMath::Abs(InputX) < 0.9f)
		return;

	if (AWarrior* Warrior = Cast<AWarrior>(GetPawn()))
	{
		bool flag = InputX < 0 ? true : false; // true(left), false(right)
		Warrior->ChangeLockOn(flag);
	}
}

void AWarriorController::ActionPressed()
{
	bActionPressed = true;

	PressStarted = GetWorld()->GetTimeSeconds();
}

void AWarriorController::ActionReleased()
{
	bActionPressed = false;

	float HoldTime = GetWorld()->GetTimeSeconds() - PressStarted;

	if (HoldTime < 0.2f)
	{
		Dodge();
	}
	else
	{
		SprintEnd();
	}
}