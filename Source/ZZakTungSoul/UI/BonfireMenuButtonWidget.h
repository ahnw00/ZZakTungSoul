// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BonfireMenuButtonWidget.generated.h"

class UButton;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBonfireButtonClickedDelegate, const FName&, BonfireID);
DECLARE_DELEGATE(FOnButtonClickedDelegate);

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API UBonfireMenuButtonWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// 버튼의 화톳불 ID와 표시 이름 세팅.
	UFUNCTION(BlueprintCallable, Category = "Bonfire")
	void InitializeBonfireButton(const FName& InBonfireID, const FText& InDisplayName);
	// 클릭시 부모에게 알리는 델리게이트.
	UPROPERTY(BlueprintAssignable, Category = "Bonfire")
	FOnBonfireButtonClickedDelegate OnBonfireButtonClicked;

	FOnButtonClickedDelegate OnUIButtonClicked;

protected:
	virtual void NativeOnInitialized() override;

	// 버튼 클릭 콜백.
	UFUNCTION()
	void OnButtonClicked();

protected:
	// 에디터 연결용 위젯(WBP 이름 일치).
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> Button_Bonfire;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TextBlock_BonfireName;

	// 버튼이 가리키는 화톳불 ID.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bonfire")
	FName AssignedBonfireID = NAME_None;


};
