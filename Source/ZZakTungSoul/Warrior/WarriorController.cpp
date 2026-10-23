// Fill out your copyright notice in the Description page of Project Settings.


#include "Warrior/WarriorController.h"
#include "Warrior.h"

#include <InputMappingContext.h>
#include <InputAction.h>

#include <EnhancedInputSubsystems.h>
#include <EnhancedInputComponent.h>

#include <TimerManager.h>
#include <Engine/OverlapResult.h>

#include <GamePlay/ZTSGameMode.h>

#include <Blueprint/UserWidget.h>

// [UI]
#include <UI/PlayerHUDWidget.h>
#include <Interface/CharacterStatInterface.h>
#include <UI/StatusWindowWidget.h>
#include <UI/MenuWidget.h>
#include <UI/BonfireMenuWidget.h>

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

	// 상호작용 키
	static ConstructorHelpers::FObjectFinder<UInputAction> ia_interact(TEXT("/Game/Input/IA_Interact.IA_Interact"));
	if (ia_interact.Succeeded()) IA_Interact = ia_interact.Object;

	// 세이브/로드 테스트
	static ConstructorHelpers::FObjectFinder<UInputAction> ia_save(TEXT("/Game/Input/IA_Save.IA_Save"));
	if (ia_save.Succeeded()) IA_Save = ia_save.Object;
	static ConstructorHelpers::FObjectFinder<UInputAction> ia_load(TEXT("/Game/Input/IA_Load.IA_Load"));
	if (ia_load.Succeeded()) IA_Load = ia_load.Object;

	//---------------------------------------------------------------
	// 디버그 용
	// 
	// 
	//---------------------------------------------------------------
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> imc_debug(TEXT("/Game/Input/Debug/IMC_Debug.IMC_Debug"));
	if (imc_debug.Succeeded()) IMC_Debug = imc_debug.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> ia_debugToggle(TEXT("/Game/Input/Debug/IA_Debug_Toggle.IA_Debug_Toggle"));
	if (ia_debugToggle.Succeeded()) IA_DebugToggle = ia_debugToggle.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> ia_debugNum1(TEXT("/Game/Input/Debug/IA_Debug_Num1.IA_Debug_Num1"));
	if (ia_debugNum1.Succeeded()) IA_DebugNum1 = ia_debugNum1.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> ia_debugNum2(TEXT("/Game/Input/Debug/IA_Debug_Num2.IA_Debug_Num2"));
	if (ia_debugNum2.Succeeded()) IA_DebugNum2 = ia_debugNum2.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> ia_debugNum3(TEXT("/Game/Input/Debug/IA_Debug_Num3.IA_Debug_Num3"));
	if (ia_debugNum3.Succeeded()) IA_DebugNum3 = ia_debugNum3.Object;



	// [UI]
	// [플레이어 HUD]
	static ConstructorHelpers::FClassFinder<UPlayerHUDWidget> PlayerHUDClassRef(
		TEXT("/Game/UI/WBP_PlayerHUD.WBP_PlayerHUD_C")
	);
	if (PlayerHUDClassRef.Succeeded())
	{
		PlayerHUDWidgetClass = PlayerHUDClassRef.Class;
	}
	// [스탯창]
	static ConstructorHelpers::FClassFinder<UStatusWindowWidget> StatusWindowClassRef(
		TEXT("/Game/UI/WBP_StatusWindow.WBP_StatusWindow_C") 
	);
	if (StatusWindowClassRef.Succeeded())
	{
		StatusWindowWidgetClass = StatusWindowClassRef.Class;
	}
	// [화톳불]
	static ConstructorHelpers::FClassFinder<UBonfireMenuWidget> BonfireMenuWidgetClassRef(
		TEXT("/Game/UI/WBP_BonfireMenuWidget.WBP_BonfireMenuWidget_C")
	);
	if (BonfireMenuWidgetClassRef.Succeeded())
	{
		BonfireMenuWidgetClass = BonfireMenuWidgetClassRef.Class;
	}
}


	static ConstructorHelpers::FClassFinder<UMenuWidget> MenuWidgetClassRef(
		TEXT("/Game/Blueprints/UI/WBP_Menu.WBP_Menu_C")
	);
	if (MenuWidgetClassRef.Succeeded())
	{
		MenuWidgetClass = MenuWidgetClassRef.Class;
	}
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


	// [HUD 위젯]
	if (PlayerHUDWidgetClass)
	{
		PlayerHUDWidget = CreateWidget<UPlayerHUDWidget>(this, PlayerHUDWidgetClass);
		if (PlayerHUDWidget)
		{
			PlayerHUDWidget->AddToViewport();
		}
	}
	if (MenuWidgetClass)
	{
		MenuWidget = CreateWidget<UMenuWidget>(this, MenuWidgetClass);
		if (MenuWidget)
		{
			MenuWidget->AddToViewport();
		}
	}
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
	EnhancedInput->BindAction(IA_Interact, ETriggerEvent::Completed, this, &AWarriorController::Interact);

	//---------------------------------------------------------------
	// 디버그 용 바인드
	// TriggerDebugToggle 아래 디버그에 사용할 키들을 바인딩 해주면 됨.
	// 
	//---------------------------------------------------------------
	EnhancedInput->BindAction(IA_DebugToggle, ETriggerEvent::Started, this, &AWarriorController::TriggerDebugToggle);
	EnhancedInput->BindAction(IA_DebugNum1, ETriggerEvent::Started, this, &AWarriorController::TriggerDebugNum1);
	EnhancedInput->BindAction(IA_DebugNum2, ETriggerEvent::Started, this, &AWarriorController::TriggerDebugNum2);
	EnhancedInput->BindAction(IA_DebugNum3, ETriggerEvent::Started, this, &AWarriorController::TriggerDebugNum3);
	//---------------------------------------------------------------

	// 세이브/로드 바인드
	EnhancedInput->BindAction(IA_Save, ETriggerEvent::Completed, this, &AWarriorController::Save);
	EnhancedInput->BindAction(IA_Load, ETriggerEvent::Completed, this, &AWarriorController::Load);

	// [UI]
	InputComponent->BindKey(EKeys::C, IE_Pressed, this, &AWarriorController::ToggleStatusWindow);
	InputComponent->BindKey(EKeys::E, IE_Pressed, this, &AWarriorController::Interact);
	InputComponent->BindKey(EKeys::Escape, IE_Pressed, this, &AWarriorController::CloseBonfireMenu);
	InputComponent->BindKey(EKeys::Q, IE_Pressed, this, &AWarriorController::CloseBonfireMenu);
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

void AWarriorController::Interact()
{
	if (AWarrior* Warrior = Cast<AWarrior>(GetPawn()))
	{
		Warrior->Interact();
	}
}

void AWarriorController::Save()
{
	AZTSGameMode* GameMode = Cast<AZTSGameMode>(GetWorld()->GetAuthGameMode());

	if (GameMode)
	{
		GameMode->SaveGame();
	}
}

void AWarriorController::Load()
{
	AZTSGameMode* GameMode = Cast<AZTSGameMode>(GetWorld()->GetAuthGameMode());

	if (GameMode)
	{
		GameMode->LoadGame();
	}
}

void AWarriorController::TriggerDebugToggle()
{
	auto subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	if (!subsystem || !IMC_Debug)
	{
		UE_LOG(LogTemp, Log, TEXT("Debug Mode Failed"));
		return;
	}

	bIsDebugModeActive = ~bIsDebugModeActive;

	if (bIsDebugModeActive)
	{
		UE_LOG(LogTemp, Log, TEXT("Debug Mode Active"));

		subsystem->AddMappingContext(IMC_Debug, 10);
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("디버그 모드 활성화 (1, 2, 3 키 사용 가능)"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Debug Mode Not Active"));

		subsystem->RemoveMappingContext(IMC_Debug);
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("디버그 모드 비활성화"));
	}
}

void AWarriorController::TriggerDebugNum1()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("디버그 모드: 숫자 1"));

	if (AWarrior* Warrior = Cast<AWarrior>(GetPawn()))
	{
		Warrior->SetOverlayState(EOverlayState::None);
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("플레이어 오버레이 상태: 주먹"));
	}
}

void AWarriorController::TriggerDebugNum2()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("디버그 모드: 숫자 2"));

	if (AWarrior* Warrior = Cast<AWarrior>(GetPawn()))
	{
		Warrior->SetOverlayState(EOverlayState::OneHand);
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("플레이어 오버레이 상태: 한손"));
	}
}

void AWarriorController::TriggerDebugNum3()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("디버그 모드: 숫자 3"));

	if (AWarrior* Warrior = Cast<AWarrior>(GetPawn()))
	{
		Warrior->SetOverlayState(EOverlayState::TwoHand);
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("플레이어 오버레이 상태: 양손"));
	}
}





// [UI].
// 사망, 리스폰 상황 때문에 미리 만들었음.
void AWarriorController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// 빙의한 새 캐릭터가 스탯 인터페이스 보유 시 HUD에 컴포넌트 전달.
	if (ICharacterStatInterface* StatOwner = Cast<ICharacterStatInterface>(InPawn))
	{
		if (PlayerHUDWidget)
		{
			PlayerHUDWidget->BindStatComponent(StatOwner->GetCharacterStatComponent());
		}
	}
}

void AWarriorController::ToggleStatusWindow()
{
	// 입력 모드 3종
	// FInputModeGameOnly, FInputModeUIOnly, FInputModeGameAndUI(디아블로)
	// 화면 열려 있으면 닫기.
	if (StatusWindowWidget && StatusWindowWidget->IsInViewport())
	{
		StatusWindowWidget->RemoveFromParent();

		// 마우스 숨기고 조작 모드로 복구.
		bShowMouseCursor = false;
		SetInputMode(FInputModeGameOnly());
		return;
	}

	// 인스턴스 없으면 생성.
	if (!StatusWindowWidget && StatusWindowWidgetClass)
	{
		StatusWindowWidget = CreateWidget<UStatusWindowWidget>(this, StatusWindowWidgetClass);
	}

	// 화면에 열기.
	if (StatusWindowWidget)
	{
		StatusWindowWidget->RefreshStatus();
		StatusWindowWidget->AddToViewport(10);

		bShowMouseCursor = true;
		SetInputMode(FInputModeGameAndUI());
	}
}

void AWarriorController::OpenBonfireMenu()
{
	// 검사.
	if (!BonfireMenuWidget && BonfireMenuWidgetClass)
	{
		BonfireMenuWidget = CreateWidget<UBonfireMenuWidget>(this, BonfireMenuWidgetClass);
	}
	// 조작모드 전환
	if (BonfireMenuWidget && !BonfireMenuWidget->IsInViewport())
	{
		BonfireMenuWidget->AddToViewport(10);
		bShowMouseCursor = true;
		SetInputMode(FInputModeGameAndUI());
	}
}

void AWarriorController::CloseBonfireMenu()
{
	if (BonfireMenuWidget && BonfireMenuWidget->IsInViewport())
	{
		BonfireMenuWidget->RemoveFromParent();
		bShowMouseCursor = false;
		SetInputMode(FInputModeGameOnly());
	}
}

// 상호작용.
void AWarriorController::Interact()
{
	if (AWarrior* Warrior = Cast<AWarrior>(GetPawn()))
	{
		Warrior->Interact();
	}
}