// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include <ItemData/ItemDataAsset.h>
#include "Components/SlateWrapperTypes.h"
#include "WarriorController.generated.h"

class UUserWidget;

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

	void BeginDeathPresentation();
	void EndDeathPresentation();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	//virtual void Tick(float DeltaTime) override;

	virtual void PostInitializeComponents() override;

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
	UInputAction* IA_Parry;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_Attack;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_ChargeAttack;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_LockOn;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_ChangeLockOn;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_Interact;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_UpperBelt;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_LowerBelt;

	// 세이브 로드 테스트용
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Save;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Load;

	// 메뉴 창(ESC랑 나중에 한번에 하도록 묶어주는 함수 만들어야 함)
	// 화톳불 메뉴에서 ESC를 눌러도 닫고 메뉴 창을 열고 닫을 때도 ESC를 눌러야 하니까
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Menu;

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
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_DebugNum4;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_DebugNum5;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_DebugNum6;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_DebugNum7;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_DebugNum8;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_DebugNum9;

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
	void Parry();

	void Dodge();
	void Roll();
	void BackStep();
	void Attack();
	void ChargeStart();
	void ChargeEnd();

	void LockOn();
	void ChangeLockOn(const struct FInputActionValue& InputValue);

	float PressStarted = 0.f;
	bool bActionPressed = false;
	TMap<TWeakObjectPtr<UUserWidget>, ESlateVisibility> DeathHiddenWidgets;

	void ActionPressed();
	void ActionReleased();

	// 아이템 사용 함수
	void UseUpperBeltItem();
	void UseLowerBeltItem();
	
	// 상호 작용 함수
	void Interact();

	// 세이브/로드 함수
	void Save();
	void Load();

	// [메뉴 창 토글 함수]
	void ToggleMenuWindow();

	//---------------------------------------------------------------
	// 디버그 용 토글 함수 및 숫자들 트리거 함수.
	// 
	//---------------------------------------------------------------
	void TriggerDebugToggle();
	void TriggerDebugNum0();
	void TriggerDebugNum1();
	void TriggerDebugNum2();
	void TriggerDebugNum3();
	void TriggerDebugNum4();
	void TriggerDebugNum5();
	void TriggerDebugNum6();
	void TriggerDebugNum7();
	void TriggerDebugNum8();
	void TriggerDebugNum9();

	class ABossClown* FindDebugClownBoss() const;
	void TriggerDebugClownFatalAttack(bool bFront);

	// [UI]
public:
	// [스탯 창 토글 함수].
	UFUNCTION(BlueprintCallable, Category = "UI|Status")
	void ToggleStatusWindow();

	// [화톳불 메뉴]
	UFUNCTION(BlueprintCallable, Category = "UI|Bonfire")
	void OpenBonfireMenu();
	UFUNCTION(BlueprintCallable, Category = "UI|Bonfire")
	bool CloseBonfireMenu();
	UFUNCTION(BlueprintCallable, Category = "UI|Bonfire")
	bool TeleportToBonfire(const FName& TargetBonfireID);

	void EquipItem(const FGuid& InID, EEquipmentSlot InSlot);

	void UnequipItem(EEquipmentSlot InSlot);

	bool CheckItemInSlot(const FGuid& InID, EEquipmentSlot InSlot);


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
