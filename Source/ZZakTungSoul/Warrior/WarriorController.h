// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "WarriorController.generated.h"

// [전방 선언].
class UPlayerHUDWidget;
class UStatusWindowWidget;
class UMenuWidget;
class UInputAction;
class UBonfireMenuWidget;

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API AWarriorController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AWarriorController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	//virtual void Tick(float DeltaTime) override;

	// [디버그] 소울 지급 함수.
	void DebugAddSouls();

private:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext* IMC_Warrior;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_Move;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_Look;
	//UPROPERTY(EditDefaultsOnly, Category = "Input")
	//UInputAction* IA_Jump;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_DodgeDash;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_Block;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_Attack;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_LockOn;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_ChangeLockOn;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_Interact;

	// 세이브 로드 테스트용
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Save;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Load;

	//---------------------------------------------------------------
	// 디버그 용
	// Insert키를 누르면 디버그 상태.
	// Insert키를 누르면 디버그 컨텍스트가 추가 됨. (우선순위를 높게 주어 기존 키 가로채기 가능)
	// 다시 누르면 제거.
	// 
	//---------------------------------------------------------------
	uint8 bIsDebugModeActive : 1 = false;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext* IMC_Debug;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_DebugToggle;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_DebugNum1;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_DebugNum2;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_DebugNum3;

private:
	void Move(const struct FInputActionValue& InputValue);
	void Idle();
	void Look(const struct FInputActionValue& InputValue);
	//void Jump(const struct FInputActionValue& InputValue);

	UPROPERTY()
	FVector ControlDir;

	void SprintStart();
	void SprintEnd();

	void BlockStart();
	void BlockEnd();

	void Dodge();
	void Roll();
	void BackStep();
	void Attack();

	void LockOn();
	void ChangeLockOn(const struct FInputActionValue& InputValue);

	float PressStarted = 0.f;
	bool bActionPressed = false;

	void ActionPressed();
	void ActionReleased();
	
	// 상호 작용 함수
	void Interact();

	// 세이브/로드 함수
	void Save();
	void Load();

	//---------------------------------------------------------------
	// 디버그 용 토글 함수 및 숫자들 트리거 함수.
	// 
	//---------------------------------------------------------------
	void TriggerDebugToggle();
	void TriggerDebugNum1();
	void TriggerDebugNum2();
	void TriggerDebugNum3();

	// [UI]
public:
	// [스탯 창 토글 함수].
	UFUNCTION(BlueprintCallable, Category = "UI|Status")
	void ToggleStatusWindow();

	// [화톳불 메뉴]
	UFUNCTION(BlueprintCallable, Category = "UI|Bonfire")
	void OpenBonfireMenu();
	UFUNCTION(BlueprintCallable, Category = "UI|Bonfire")
	void CloseBonfireMenu();
	UFUNCTION(BlueprintCallable, Category = "UI|Bonfire")
	bool TeleportToBonfire(const FName& TargetBonfireID);

protected:
	virtual void OnPossess(APawn* InPawn) override;


private:
	// [플레이어 HUD]
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UPlayerHUDWidget> PlayerHUDWidgetClass;
	UPROPERTY()
	TObjectPtr<UPlayerHUDWidget> PlayerHUDWidget;

	// [스탯창]
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UStatusWindowWidget> StatusWindowWidgetClass;
	UPROPERTY()
	TObjectPtr<UStatusWindowWidget> StatusWindowWidget;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UMenuWidget> MenuWidgetClass;
	UPROPERTY()
	TObjectPtr<UMenuWidget> MenuWidget;
	// [화톳불]
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UBonfireMenuWidget> BonfireMenuWidgetClass;
	UPROPERTY()
	TObjectPtr<UBonfireMenuWidget> BonfireMenuWidget;

};
