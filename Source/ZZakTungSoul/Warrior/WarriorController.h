// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "WarriorController.generated.h"

// [전방 선언].
class UPlayerHUDWidget;
class UStatusWindowWidget;

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

private:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext* IMC_Warrior;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Move;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Look;
	//UPROPERTY(EditDefaultsOnly, Category = "Input")
	//class UInputAction* IA_Jump;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_DodgeDash;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Block;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Attack;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_LockOn;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_ChangeLockOn;

	// 세이브 로드 테스트용
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Save;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Load;

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
	

	// 세이브/로드 함수
	void Save();
	void Load();







	// [UI]
public:
	// [스탯 창 토글 함수].
	UFUNCTION(BlueprintCallable, Category = "UI|Status")
	void ToggleStatusWindow();

protected:
	virtual void OnPossess(APawn* InPawn) override;


private:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UPlayerHUDWidget> PlayerHUDWidgetClass;
	UPROPERTY()
	TObjectPtr<UPlayerHUDWidget> PlayerHUDWidget;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UStatusWindowWidget> StatusWindowWidgetClass;
	UPROPERTY()
	TObjectPtr<UStatusWindowWidget> StatusWindowWidget;

};
