// [화톳불] 텔레포트 서브메뉴.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TeleportWidget.generated.h"

class UScrollBox;
class UButton;
class UBonfireMenuButtonWidget;

// 전송 목적지 화톳불 정보 구조체.
USTRUCT(BlueprintType)
struct FBonfireTeleportEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Bonfire")
	FName BonfireID = NAME_None;

	UPROPERTY(BlueprintReadOnly, Category = "Bonfire")
	FText DisplayName;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTeleportCancelDelegate);

/**
 * 
 */
UCLASS()
class ZZAKTUNGSOUL_API UTeleportWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 전송 목록 갱신 및 UI 버튼 생성.
	UFUNCTION(BlueprintCallable, Category = "Bonfire|Teleport")
	void RefreshTeleportList();
	// 전송 가능 화톳불 목록 조회.
	UFUNCTION(BlueprintCallable, Category = "Bonfire|Teleport")
	TArray<FBonfireTeleportEntry> GetAvailableBonfires() const;
	// 특정 화톳불로 전송 요청.
	UFUNCTION(BlueprintCallable, Category = "Bonfire|Teleport")
	void RequestTeleport(const FName& TargetBonfireID);

	UPROPERTY(BlueprintAssignable, Category = "Bonfire|Events")
	FOnTeleportCancelDelegate OnTeleportCanceled;

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	// 취소/뒤로가기 버튼 클릭 콜백.
	UFUNCTION()
	void OnCancelClicked();

protected:
	// 에디터에서 지정할 화톳불 버튼 위젯 블루프린트 클래스.
	UPROPERTY(EditDefaultsOnly, Category = "Bonfire|Teleport")
	TSubclassOf<UBonfireMenuButtonWidget> BonfireButtonClass;

	// 점화된 화톳불 버튼들이 나열될 스크롤 박스.
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UScrollBox> ScrollBox_BonfireList;

	// 닫기/뒤로가기 버튼.
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> Button_Cancel;
	
};
