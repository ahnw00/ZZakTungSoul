// [화톳불] 레벨업 서브메뉴.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SaveData/WarriorSaveData.h"
#include "LevelUpWidget.generated.h"

// 전방 선언.
class UButton;
class UTextBlock;
class UWidget;
class AWarriorPlayerState;
class UCharacterStatComponent;

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API ULevelUpWidget : public UUserWidget
{
	GENERATED_BODY()
	

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

public:
	// 초기화 및 가상 포인트 증가 함수.
	void InitializedLevelUp();
	bool CanAddPoint() const;
	
	UFUNCTION(BlueprintCallable, Category = "LevelUp")
	void AddPoint(FName StatName);
	UFUNCTION(BlueprintCallable, Category = "LevelUp")
	void SubPoint(FName StatName);

	// 최종 확정 및 취소.
	UFUNCTION(BlueprintCallable, Category = "LevelUp")
	void CommitLevelUp();
	UFUNCTION(BlueprintCallable, Category = "LevelUp")
	void CancelLevelUp();

	// 모달 팝업 열기/닫기.
	UFUNCTION(BlueprintCallable, Category = "LevelUp")
	void OpenConfirmModal();
	UFUNCTION(BlueprintCallable, Category = "LevelUp")
	void CloseConfirmModal();

protected:
	// 화면의 모든 텍스트를 최신 가상 수치로 갱신.
	void RefreshUI();
	
	// 캐싱된 컴포넌트 가져오기.
	AWarriorPlayerState* GetWarriorPlayerState() const;
	UCharacterStatComponent* GetCharacterStatComponent() const;

	// 원본 데이터.
	int32 BaseLevel = 1;
	int32 BaseSouls = 0;
	FWarriorAttributeData BaseAttributes;
	
	// 가상 장바구니 시뮬레이션 데이터.
	int32 VirtualLevel = 1;
	int32 TotalRequiredSouls = 0;
	FWarriorAttributeData PendingPoints; // 임시로 올린 포인트.

	// UMG 텍스트 슬롯(수치 출력).
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TextBlock_CurrentLevel;
	
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TextBlock_CurrentSouls;
	
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TextBlock_TotalRequiredSouls;
	
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TextBlock_RemainingSouls;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TextBlock_Vigor;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TextBlock_Attunement;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TextBlock_Endurance;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TextBlock_Vitality;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TextBlock_Strength;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TextBlock_Dexterity;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TextBlock_MaxHP;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TextBlock_MaxFP;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TextBlock_MaxStamina;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TextBlock_EquipLoad;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TextBlock_Poise;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TextBlock_AttackPower;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TextBlock_Defense;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TextBlock_Resistance;




	// 결정/취소 버튼.
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> Button_Confirm;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> Button_Cancel;

	// 확인 팝업 패널 및 예/아니오 버튼.
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UWidget> Panel_ConfirmModal;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> Button_ModalYes;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> Button_ModalNo;



};
