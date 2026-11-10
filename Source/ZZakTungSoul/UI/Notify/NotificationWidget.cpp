#include "UI/Notify/NotificationWidget.h"
#include "Components/RetainerBox.h"
#include "Curves/CurveFloat.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/TextBlock.h"
#include <Sound/SoundBase.h>
#include "Kismet/GameplayStatics.h"

void UNotificationWidget::ShowNotification()
{
	SetVisibility(ESlateVisibility::HitTestInvisible);
	UpdateBlurRadius(0.f);

	if (Anim_Notification)
	{
		PlayAnimation(
			Anim_Notification,
			0.f,
			1,
			EUMGSequencePlayMode::Forward,
			1.f
		);

		if (ShowSound)
		{
			UGameplayStatics::PlaySound2D(this, ShowSound);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[Notification] 애니메이션 연결 없음"));
	}

}

void UNotificationWidget::SetNotificationText(const FText& InText)
{
	if (Text_Message)
	{
		Text_Message->SetText(InText);
	}
}

void UNotificationWidget::HideNotification()
{
	if (Anim_Notification)
	{
		StopAnimation(Anim_Notification);
	}

	SetVisibility(ESlateVisibility::Collapsed);
}

void UNotificationWidget::OnAnimationFinished_Implementation(
	const UWidgetAnimation* Animation)
{
	Super::OnAnimationFinished_Implementation(Animation);

	if (Animation == Anim_Notification.Get() &&
		!IsAnimationPlaying(Anim_Notification))
	{
		SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UNotificationWidget::NativeTick(
	const FGeometry& MyGeometry,
	float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!Anim_Notification ||
		!IsAnimationPlaying(Anim_Notification))
	{
		return;
	}

	const float AnimationTime =
		GetAnimationCurrentTime(Anim_Notification);

	UpdateBlurRadius(AnimationTime);
}

void UNotificationWidget::UpdateBlurRadius(float AnimationTime)
{
	if (!RetainerBox_Glow || !BlurRadiusCurve)
	{
		return;
	}

	UMaterialInstanceDynamic* Material =
		RetainerBox_Glow->GetEffectMaterial();

	if (!Material)
	{
		return;
	}

	const float Radius = FMath::Max(
		0.f,
		BlurRadiusCurve->GetFloatValue(AnimationTime)
	);

	Material->SetScalarParameterValue(
		TEXT("BlurRadius"),
		Radius
	);
}