#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Animation/WidgetAnimation.h"
#include "NotificationWidget.generated.h"

class URetainerBox;
class UCurveFloat;
class UTextBlock;
class USoundBase;

UCLASS(Abstract)
class ZZAKTUNGSOUL_API UNotificationWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void ShowNotification();
	void HideNotification();
	void SetNotificationText(const FText& InText);

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;

	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> Anim_Notification;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<URetainerBox> RetainerBox_Glow;

	UPROPERTY(EditDefaultsOnly, Category = "Notification|Glow")
	TObjectPtr<UCurveFloat> BlurRadiusCurve;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_Message;

	UPROPERTY(EditDefaultsOnly, Category = "Notification|Audio")
	TObjectPtr<USoundBase> ShowSound;

private:
	void UpdateBlurRadius(float AnimationTime);
};