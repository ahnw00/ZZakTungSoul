// Fill out your copyright notice in the Description page of Project Settings.


#include "Warrior/WarriorController.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Warrior.h"
#include <Warrior/WarriorPlayerState.h>


#include <EngineUtils.h>
#include <Components/CapsuleComponent.h>

#include <InputMappingContext.h>
#include <InputAction.h>
#include <EnhancedInputComponent.h>
#include <EnhancedInputSubsystems.h>
#include <Blueprint/UserWidget.h>


#include <TimerManager.h>
#include <Engine/OverlapResult.h>

#include <GamePlay/ZTSGameMode.h>


// [UI]
#include <UI/PlayerHUDWidget.h>
#include <Interface/CharacterStatInterface.h>
#include <UI/StatusWindowWidget.h>
#include <UI/MenuWidget.h>
#include <UI/BonfireMenuWidget.h>
#include <Interactable/BonFire.h>
#include <UI/BeltWidget.h>

// [Equipment]
#include <Component/EquipmentComponent.h>

// test
#include <Engine/DamageEvents.h>
#include "Kismet/GameplayStatics.h"
#include <Enemy/Boss/Boss.h>
#include <Enemy/Boss/Clown/BossClown.h>
#include <Enemy/Components/EnemyStatComponent.h>
#include <UI/Notify/NotificationSubsystem.h>
#include "Engine/LocalPlayer.h"

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
	static ConstructorHelpers::FObjectFinder<UInputAction> ia_parry(TEXT("/Game/Input/IA_Parry.IA_Parry"));
	if (ia_parry.Succeeded()) IA_Parry = ia_parry.Object;
	static ConstructorHelpers::FObjectFinder<UInputAction> ia_attack(TEXT("/Game/Input/IA_Attack.IA_Attack"));
	if (ia_attack.Succeeded()) IA_Attack = ia_attack.Object;
	static ConstructorHelpers::FObjectFinder<UInputAction> ia_chargeattack(TEXT("/Game/Input/IA_ChargeAttack.IA_ChargeAttack"));
	if (ia_chargeattack.Succeeded()) IA_ChargeAttack = ia_chargeattack.Object;
	static ConstructorHelpers::FObjectFinder<UInputAction> ia_lockon(TEXT("/Game/Input/IA_LockOn.IA_LockOn"));
	if (ia_lockon.Succeeded()) IA_LockOn = ia_lockon.Object;
	static ConstructorHelpers::FObjectFinder<UInputAction> ia_changelockon(TEXT("/Game/Input/IA_ChangeLockOn.IA_ChangeLockOn"));
	if (ia_changelockon.Succeeded()) IA_ChangeLockOn = ia_changelockon.Object;

	// 상호작용 키
	static ConstructorHelpers::FObjectFinder<UInputAction> ia_interact(TEXT("/Game/Input/IA_Interact.IA_Interact"));
	if (ia_interact.Succeeded()) IA_Interact = ia_interact.Object;

	// 아이템 사용 키
	static ConstructorHelpers::FObjectFinder<UInputAction> ia_upperbelt(TEXT("/Game/Input/IA_UpperBelt.IA_UpperBelt"));
	if (ia_upperbelt.Succeeded()) IA_UpperBelt = ia_upperbelt.Object;
	static ConstructorHelpers::FObjectFinder<UInputAction> ia_lowerbelt(TEXT("/Game/Input/IA_LowerBelt.IA_LowerBelt"));
	if (ia_lowerbelt.Succeeded()) IA_LowerBelt = ia_lowerbelt.Object;

	// 세이브/로드 테스트
	static ConstructorHelpers::FObjectFinder<UInputAction> ia_save(TEXT("/Game/Input/IA_Save.IA_Save"));
	if (ia_save.Succeeded()) IA_Save = ia_save.Object;
	static ConstructorHelpers::FObjectFinder<UInputAction> ia_load(TEXT("/Game/Input/IA_Load.IA_Load"));
	if (ia_load.Succeeded()) IA_Load = ia_load.Object;

	// 메뉴 열고 닫기
	static ConstructorHelpers::FObjectFinder<UInputAction> ia_menu(TEXT("/Game/Input/IA_Menu.IA_Menu"));
	if (ia_menu.Succeeded()) IA_Menu = ia_menu.Object;

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

	static ConstructorHelpers::FObjectFinder<UInputAction> ia_debugNum4(TEXT("/Game/Input/Debug/IA_Debug_Num4.IA_Debug_Num4"));
	if (ia_debugNum4.Succeeded()) IA_DebugNum4 = ia_debugNum4.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> ia_debugNum5(TEXT("/Game/Input/Debug/IA_Debug_Num5.IA_Debug_Num5"));
	if (ia_debugNum5.Succeeded()) IA_DebugNum5 = ia_debugNum5.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> ia_debugNum6(TEXT("/Game/Input/Debug/IA_Debug_Num6.IA_Debug_Num6"));
	if (ia_debugNum6.Succeeded()) IA_DebugNum6 = ia_debugNum6.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> ia_debugNum7(TEXT("/Game/Input/Debug/IA_Debug_Num7.IA_Debug_Num7"));
	if (ia_debugNum7.Succeeded()) IA_DebugNum7 = ia_debugNum7.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> ia_debugNum8(TEXT("/Game/Input/Debug/IA_Debug_Num8.IA_Debug_Num8"));
	if (ia_debugNum8.Succeeded()) IA_DebugNum8 = ia_debugNum8.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> ia_debugNum9(TEXT("/Game/Input/Debug/IA_Debug_Num9.IA_Debug_Num9"));
	if (ia_debugNum9.Succeeded()) IA_DebugNum9 = ia_debugNum9.Object;

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

	// 인게임 진입 시 게임 전용 입력 모드 복구 및 마우스 커서 숨김.
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	SetShowMouseCursor(false);

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

			if (AWarrior* Warrior = Cast<AWarrior>(GetPawn()))
			{
				PlayerHUDWidget->GetBeltWidget()->SetEquipment(Warrior->GetEquipment());
				PlayerHUDWidget->GetBeltWidget()->SetInventory(Warrior->GetInventory());

				Warrior->GetEquipment()->OnEquipItemImageChanged.AddUObject(
					PlayerHUDWidget->GetBeltWidget(), &UBeltWidget::OnEquipmentChanged);
			}
		}
	}
	if (MenuWidgetClass)
	{
		MenuWidget = CreateWidget<UMenuWidget>(this, MenuWidgetClass);

		if (AWarrior* Warrior = Cast<AWarrior>(GetPawn()))
		{
			MenuWidget->SetInventory(Warrior->GetInventory());
			MenuWidget->SetEquipment(Warrior->GetEquipment());

			Warrior->GetEquipment()->OnEquipItemImageChanged.AddUObject(
				MenuWidget, &UMenuWidget::ChangeButtonsImage);
		}
	}

	if (AWarrior* Warrior = Cast<AWarrior>(GetPawn()))
	{
		Warrior->GetEquipment()->OnEquipItemImageChanged.Broadcast();
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
	EnhancedInput->BindAction(IA_Parry, ETriggerEvent::Started, this, &AWarriorController::Parry);
	EnhancedInput->BindAction(IA_Attack, ETriggerEvent::Started, this, &AWarriorController::Attack);
	EnhancedInput->BindAction(IA_ChargeAttack, ETriggerEvent::Started, this, &AWarriorController::ChargeStart);
	EnhancedInput->BindAction(IA_LockOn, ETriggerEvent::Completed, this, &AWarriorController::LockOn);
	EnhancedInput->BindAction(IA_ChangeLockOn, ETriggerEvent::Triggered, this, &AWarriorController::ChangeLockOn);
	EnhancedInput->BindAction(IA_Interact, ETriggerEvent::Completed, this, &AWarriorController::Interact);
	EnhancedInput->BindAction(IA_UpperBelt, ETriggerEvent::Completed, this, &AWarriorController::UseUpperBeltItem);
	EnhancedInput->BindAction(IA_LowerBelt, ETriggerEvent::Completed, this, &AWarriorController::UseLowerBeltItem);

	//---------------------------------------------------------------
	// 디버그 용 바인드
	// TriggerDebugToggle 아래 디버그에 사용할 키들을 바인딩 해주면 됨.
	// 
	//---------------------------------------------------------------
	EnhancedInput->BindAction(IA_DebugToggle, ETriggerEvent::Started, this, &AWarriorController::TriggerDebugToggle);
	EnhancedInput->BindAction(IA_DebugNum1, ETriggerEvent::Started, this, &AWarriorController::TriggerDebugNum1);
	EnhancedInput->BindAction(IA_DebugNum2, ETriggerEvent::Started, this, &AWarriorController::TriggerDebugNum2);
	EnhancedInput->BindAction(IA_DebugNum3, ETriggerEvent::Started, this, &AWarriorController::TriggerDebugNum3);
	EnhancedInput->BindAction(IA_DebugNum4, ETriggerEvent::Started, this, &AWarriorController::TriggerDebugNum4);
	EnhancedInput->BindAction(IA_DebugNum5, ETriggerEvent::Started, this, &AWarriorController::TriggerDebugNum5);
	EnhancedInput->BindAction(IA_DebugNum6, ETriggerEvent::Started, this, &AWarriorController::TriggerDebugNum6);
	EnhancedInput->BindAction(IA_DebugNum7, ETriggerEvent::Started, this, &AWarriorController::TriggerDebugNum7);
	EnhancedInput->BindAction(IA_DebugNum8, ETriggerEvent::Started, this, &AWarriorController::TriggerDebugNum8);
	EnhancedInput->BindAction(IA_DebugNum9, ETriggerEvent::Started, this, &AWarriorController::TriggerDebugNum9);
	//---------------------------------------------------------------

	// 세이브/로드 바인드
	EnhancedInput->BindAction(IA_Save, ETriggerEvent::Completed, this, &AWarriorController::Save);
	EnhancedInput->BindAction(IA_Load, ETriggerEvent::Completed, this, &AWarriorController::Load);

	// 메뉴 열고 닫기
	EnhancedInput->BindAction(IA_Menu, ETriggerEvent::Completed, this, &AWarriorController::ToggleMenuWindow);

	// [UI]
	//InputComponent->BindKey(EKeys::C, IE_Pressed, this, &AWarriorController::ToggleStatusWindow);
	//InputComponent->BindKey(EKeys::E, IE_Pressed, this, &AWarriorController::Interact);
	//InputComponent->BindKey(EKeys::Escape, IE_Pressed, this, &AWarriorController::CloseBonfireMenu);
	//InputComponent->BindKey(EKeys::Q, IE_Pressed, this, &AWarriorController::CloseBonfireMenu);
	// [디버그] 소울 획득 키.
	InputComponent->BindKey(EKeys::F1, IE_Pressed, this, &AWarriorController::DebugAddSouls);
}

void AWarriorController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//sif(!PlayerHUDWidget)
}

void AWarriorController::DebugAddSouls()
{
#if !UE_BUILD_SHIPPING
	if (AWarriorPlayerState* PS = GetPlayerState<AWarriorPlayerState>())
	{
		PS->AddSouls(5000);
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, TEXT("[디버그] 소울 5,000 지급."));
	}
#endif
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
		//if (Warrior->GetActionState() != EActionState::None)
		//	return;

		switch (Warrior->GetActionState())
		{

		case EActionState::Roll:
			[[fallthrough]];
		case EActionState::BackStep:
			[[fallthrough]];
		case EActionState::Attack:
			[[fallthrough]];
		case EActionState::FrontFatalAttack:
			return;
		case EActionState::BackFatalAttack:
			return;
		case EActionState::Hit:
			return;
		case EActionState::Parry:
			return;
		case EActionState::Block:
		{
			if (Warrior->GetWeaponState() == EWeaponState::TwoHand)
			{
				return;
			}

			[[fallthrough]];
		case EActionState::None:
			[[fallthrough]];
		}
		default:
			break;
		}
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

void AWarriorController::Parry()
{
	if (AWarrior* Warrior = Cast<AWarrior>(GetPawn()))
	{
		Warrior->Parry();
	}
}

void AWarriorController::Dodge()
{
	if (AWarrior* Warrior = Cast<AWarrior>(GetPawn()))
	{
		//if (Warrior->GetActionState() != EActionState::None)
		//	return;
		switch (Warrior->GetActionState())
		{

		case EActionState::Roll:
			[[fallthrough]];
		case EActionState::BackStep:
			[[fallthrough]];
		case EActionState::Attack:
			[[fallthrough]];
		case EActionState::Hit:
			return;
		case EActionState::Block:
		{
			if (Warrior->GetWeaponState() == EWeaponState::TwoHand)
			{
				return;
			}

			[[fallthrough]];
		case EActionState::None:
			[[fallthrough]];
		}
		default:
			break;
		}

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

void AWarriorController::ChargeStart()
{
	UE_LOG(LogTemp, Log, TEXT("ChargeStart"));
}

void AWarriorController::ChargeEnd()
{
	if (AWarrior* Warrior = Cast<AWarrior>(GetPawn()))
	{
		Warrior->ChargeAttack();
	}
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
    if (!bActionPressed)
        return;

    bActionPressed = false;
    float HoldTime = GetWorld()->GetTimeSeconds() - PressStarted;
    if (HoldTime < 0.2f)
        Dodge();
    else
        SprintEnd();
}

void AWarriorController::UseUpperBeltItem()
{
	if (AWarrior* Warrior = Cast<AWarrior>(GetPawn()))
	{
		Warrior->UseBeltItem(EEquipmentSlot::UpperBelt);
	}
}

void AWarriorController::UseLowerBeltItem()
{
	if (AWarrior* Warrior = Cast<AWarrior>(GetPawn()))
	{
		Warrior->UseBeltItem(EEquipmentSlot::LowerBelt);
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
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("Debug On"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Debug Mode Not Active"));

		subsystem->RemoveMappingContext(IMC_Debug);
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Debug Off"));
	}
}

void AWarriorController::TriggerDebugNum1()
{
	//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT(""));

	auto Test = Cast<ABoss>(UGameplayStatics::GetActorOfClass(GetWorld(), ABoss::StaticClass()));

	if (!Test)
		return;

	auto ActorCom = Cast<UEnemyStatComponent>(Test->GetComponentByClass(UEnemyStatComponent::StaticClass()));

	if (!ActorCom)
		return;

	ActorCom->SetCurrentHp(10);
}

void AWarriorController::TriggerDebugNum2()
{
	UNotificationSubsystem* NotificationSystem =
		ULocalPlayer::GetSubsystem<UNotificationSubsystem>(
			GetLocalPlayer()
		);

	if (NotificationSystem)
	{
		NotificationSystem->ShowNotification(
			ENotificationType::Death
		);
	}
}

void AWarriorController::TriggerDebugNum3()
{
	UNotificationSubsystem* NotificationSystem =
		ULocalPlayer::GetSubsystem<UNotificationSubsystem>(
			GetLocalPlayer()
		);

	if (NotificationSystem)
	{
		NotificationSystem->ShowNotification(
			ENotificationType::BossClear
		);
	}
}

void AWarriorController::TriggerDebugNum4()
{
	UNotificationSubsystem* NotificationSystem =
		ULocalPlayer::GetSubsystem<UNotificationSubsystem>(
			GetLocalPlayer()
		);

	if (NotificationSystem)
	{
		NotificationSystem->ShowNotification(
			ENotificationType::BornFireLit
		);
	}
}

void AWarriorController::TriggerDebugNum5()
{
	UNotificationSubsystem* NotificationSystem =
		ULocalPlayer::GetSubsystem<UNotificationSubsystem>(
			GetLocalPlayer()
		);

	if (NotificationSystem)
	{
		NotificationSystem->ShowNotification(
			ENotificationType::Region
		);
	}
}

void AWarriorController::TriggerDebugNum6()
{

}

void AWarriorController::TriggerDebugNum7()
{

}

void AWarriorController::TriggerDebugNum8()
{

}

void AWarriorController::TriggerDebugNum9()
{

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

		// 캐릭터 조작 매핑 일시 해제.
		if (auto Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(IMC_Warrior);
		}

		// UI 및 단축키 조작 모드.
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(BonfireMenuWidget->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(InputMode);
	}
}

bool AWarriorController::CloseBonfireMenu()
{
	if (BonfireMenuWidget && BonfireMenuWidget->IsInViewport())
	{
		// 서브메뉴 열려 있다면 서브메뉴만 닫기.
		if (BonfireMenuWidget->CloseCurrentSubMenu())
		{
			// 화톳불 메뉴가 열려있었으면 false 리턴
			return false;
		}

		// 화톳불 메인 메뉴 상태일 때 전체 화톳불 메뉴 닫기.
		BonfireMenuWidget->RemoveFromParent();
		bShowMouseCursor = false;
		
		// 캐릭터 조작 매핑 활성화.
		if (auto Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(IMC_Warrior, 0);
		}
		
		// 게임 전용 조작 모드 복구.
		SetInputMode(FInputModeGameOnly());

		// 열려있던걸 닫은 것도 열려있던거니까 false 리턴
		return false;
	}

	// 열려있지 않았으면 true 리턴
	return true;
}

void AWarriorController::EquipItem(const FGuid& InID, EEquipmentSlot InSlot)
{
	AWarrior* Warrior = Cast<AWarrior>(GetPawn());
	if (!Warrior)
	{
		return;
	}

	// 현재 장비 착용 함수로 구현되어있는데 아이템 장비 함수로 구현해야함
	Warrior->EquipWeapon(InID, InSlot);
}

void AWarriorController::UnequipItem(EEquipmentSlot InSlot)
{
	AWarrior* Warrior = Cast<AWarrior>(GetPawn());
	if (!Warrior)
	{
		return;
	}

	// 현재 장비 착용 함수로 구현되어있는데 아이템 장비 함수로 구현해야함
	Warrior->UnequipItem(InSlot);
}

bool AWarriorController::CheckItemInSlot(const FGuid& InID, EEquipmentSlot InSlot)
{
	AWarrior* Warrior = Cast<AWarrior>(GetPawn());
	if (!Warrior)
	{
		return false;
	}

	return Warrior->CheckItemInSlot(InID, InSlot);
}

void AWarriorController::ToggleMenuWindow()
{
	if (!CloseBonfireMenu())
		return;

	if (!MenuWidget)
	{
		return;
	}

	if (!MenuWidget->IsInViewport())
	{
		MenuWidget->AddToViewport();
	}

	if (MenuWidget->GetVisibility() == ESlateVisibility::Visible)
	{
		if (MenuWidget->ResetMenuWidget())
		{
			MenuWidget->SetVisibility(ESlateVisibility::Collapsed);
			bShowMouseCursor = false;
			SetInputMode(FInputModeGameOnly());
		}
	}
	else
	{
		MenuWidget->SetVisibility(ESlateVisibility::Visible);
		bShowMouseCursor = true;
		SetInputMode(FInputModeGameAndUI());
	}
}



bool AWarriorController::TeleportToBonfire(const FName& TargetBonfireID)
{
	if (TargetBonfireID.IsNone())
	{
		return false;
	}

	// 활성화 화톳불 확인.
	AWarriorPlayerState* PS = GetPlayerState<AWarriorPlayerState>();
	if (!PS || !PS->IsBonfireLit(TargetBonfireID))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Teleport] 점화되지 않은 화톳불: %s"), *TargetBonfireID.ToString());
		return false;
	}

	AWarrior* Warrior = Cast<AWarrior>(GetPawn());
	if (!Warrior)
	{
		return false;
	}


	// ID 가진 화톳불 검색.
	ABonfire* TargetBonfire = nullptr;
	for (TActorIterator<ABonfire> It(GetWorld()); It; ++It)
	{
		if (It->GetBonfireID() == TargetBonfireID)
		{
			TargetBonfire = *It;
			break;
		}
	}
	if (!TargetBonfire)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Teleport] 대상 화톳불을 찾을 수 없음: %s"), *TargetBonfireID.ToString());
		return false;
	}

	// 화톳불 착지점 획득 및 발바닥 착지 높이 보정.
	const FTransform TargetTransform = TargetBonfire->GetTeleportTransform();
	FVector TargetLocation = TargetTransform.GetLocation();
	const FRotator TargetRotation = TargetTransform.GetRotation().Rotator();

	if (UCapsuleComponent* Capsule = Warrior->GetCapsuleComponent())
	{
		TargetLocation.Z += Capsule->GetScaledCapsuleHalfHeight();
	}

	// 캐릭터 위치 및 회전 순간이동(실패시 중단).
	if (!Warrior->TeleportTo(TargetLocation, TargetRotation))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Teleport] 순간이동 실패(목적지 충돌)."));
		return false;
	}

	// 컨트롤러 카메라 시선 정렬.
	SetControlRotation(TargetRotation);

	// 열려 있던 화톳불 메뉴 닫기 및 게임 입력 복구.
	CloseBonfireMenu();

	UE_LOG(LogTemp, Log, TEXT("[Teleport] %s (으)로 이동 완료"), *TargetBonfire->GetBonfireID().ToString());

	return true;

}


void AWarriorController::BeginDeathPresentation()
{
    bActionPressed = false;
    PressStarted = 0.f;
    ControlDir = FVector::ZeroVector;
    FlushPressedKeys();

    if (StatusWindowWidget)
        StatusWindowWidget->RemoveFromParent();
    if (MenuWidget)
    {
        MenuWidget->ResetMenuWidget();
        MenuWidget->RemoveFromParent();
    }
    if (BonfireMenuWidget)
        BonfireMenuWidget->RemoveFromParent();

    DeathHiddenWidgets.Empty();
    TArray<UUserWidget*> Widgets;
    UWidgetBlueprintLibrary::GetAllWidgetsOfClass(this, Widgets, UUserWidget::StaticClass(), true);
    for (UUserWidget* Widget : Widgets)
    {
        if (!IsValid(Widget) || !Widget->IsInViewport())
            continue;
        DeathHiddenWidgets.Add(TWeakObjectPtr<UUserWidget>(Widget), Widget->GetVisibility());
        Widget->SetVisibility(ESlateVisibility::Collapsed);
    }

    SetIgnoreMoveInput(true);
    SetIgnoreLookInput(true);
    SetInputMode(FInputModeUIOnly());
    bShowMouseCursor = false;
}

void AWarriorController::EndDeathPresentation()
{
    bActionPressed = false;
    PressStarted = 0.f;
    ControlDir = FVector::ZeroVector;
    FlushPressedKeys();

    for (const auto& Entry : DeathHiddenWidgets)
    {
        UUserWidget* Widget = Entry.Key.Get();
        if (IsValid(Widget) && Widget->IsInViewport())
            Widget->SetVisibility(Entry.Value);
    }
    DeathHiddenWidgets.Empty();

    if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
            Subsystem->AddMappingContext(IMC_Warrior, 0);
    }

    ResetIgnoreMoveInput();
    ResetIgnoreLookInput();
    SetInputMode(FInputModeGameOnly());
    bShowMouseCursor = false;
}
