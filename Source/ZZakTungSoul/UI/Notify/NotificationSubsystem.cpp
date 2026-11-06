#include "UI/Notify/NotificationSubsystem.h"
#include "UI/Notify/NotificationWidget.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "UObject/ConstructorHelpers.h"

UNotificationSubsystem::UNotificationSubsystem()
{
	// 사망 알림 BP
	static ConstructorHelpers::FClassFinder<UNotificationWidget>
		DeathWidgetClassRef(
			TEXT("/Game/UI/Notify/WBP_DeathNotification.WBP_DeathNotification_C")
		);

	if (DeathWidgetClassRef.Succeeded())
	{
		NotificationWidgetClasses.Add(
			ENotificationType::Death,
			DeathWidgetClassRef.Class
		);
	}

	// 보스 클리어 알림 BP
	static ConstructorHelpers::FClassFinder<UNotificationWidget>
		BossClearWidgetClassRef(
			TEXT("/Game/UI/Notify/WBP_BossClearNotification.WBP_BossClearNotification_C")
		);

	if (BossClearWidgetClassRef.Succeeded())
	{
		NotificationWidgetClasses.Add(
			ENotificationType::BossClear,
			BossClearWidgetClassRef.Class
		);
	}

	// 사망 알림 BP
	static ConstructorHelpers::FClassFinder<UNotificationWidget>
		BornFireLitWidgetClassRef(
			TEXT("/Game/UI/Notify/WBP_BornFireLitNotification.WBP_BornFireLitNotification_C")
		);

	if (BornFireLitWidgetClassRef.Succeeded())
	{
		NotificationWidgetClasses.Add(
			ENotificationType::BornFireLit,
			BornFireLitWidgetClassRef.Class
		);
	}

	// 지역 진입 알림 BP
	static ConstructorHelpers::FClassFinder<UNotificationWidget>
		RegionWidgetClassRef(
			TEXT("/Game/UI/Notify/WBP_RegionNotification.WBP_RegionNotification_C")
		);

	if (RegionWidgetClassRef.Succeeded())
	{
		NotificationWidgetClasses.Add(
			ENotificationType::Region,
			RegionWidgetClassRef.Class
		);
	}
}

void UNotificationSubsystem::Initialize(
	FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UNotificationSubsystem::Deinitialize()
{
	for (const auto& Entry : NotificationWidgets)
	{
		UNotificationWidget* Widget = Entry.Value.Get();

		if (IsValid(Widget))
		{
			Widget->RemoveFromParent();
		}
	}

	NotificationWidgets.Empty();

	Super::Deinitialize();
}

void UNotificationSubsystem::ShowNotification(
	ENotificationType Type)
{
	// 요청한 종류의 BP 클래스 조회
	const TSubclassOf<UNotificationWidget> WidgetClass =
		NotificationWidgetClasses.FindRef(Type);

	if (!WidgetClass)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("[Notification] 등록되지 않은 알림 종류: %d"),
			static_cast<int32>(Type)
		);

		return;
	}

	// 요청한 종류로 이미 생성한 위젯 조회
	UNotificationWidget* Widget =
		NotificationWidgets.FindRef(Type).Get();

	// 아직 생성하지 않았다면 생성
	if (!IsValid(Widget))
	{
		Widget = CreateWidget<UNotificationWidget>(GetWorld(), WidgetClass);

		if (!IsValid(Widget))
		{
			UE_LOG(
				LogTemp,
				Warning,
				TEXT("[Notification] 위젯 생성 실패: %d"),
				static_cast<int32>(Type)
			);

			return;
		}

		// 생성한 위젯을 해당 종류로 보관
		NotificationWidgets.Add(Type, Widget);
	}

	if (!Widget->IsInViewport())
	{
		Widget->AddToPlayerScreen(100);
	}

	// 이번에 표시할 위젯을 제외한 나머지 알림 숨기기
	for (const auto& Entry : NotificationWidgets)
	{
		UNotificationWidget* OtherWidget = Entry.Value.Get();

		if (IsValid(OtherWidget) &&
			OtherWidget != Widget &&
			OtherWidget->IsVisible())
		{
			OtherWidget->HideNotification();
		}
	}

	// 요청한 알림 표시
	Widget->ShowNotification();
}

void UNotificationSubsystem::HideNotification(
	ENotificationType Type)
{
	UNotificationWidget* Widget =
		NotificationWidgets.FindRef(Type).Get();

	if (IsValid(Widget))
	{
		Widget->HideNotification();
	}
}

void UNotificationSubsystem::ShowRegionNotification(
	const FText& RegionName)
{
	if (RegionName.IsEmpty())
	{
		return;
	}

	// 기존 생성·재사용·표시 처리 사용
	ShowNotification(ENotificationType::Region);

	UNotificationWidget* Widget =
		NotificationWidgets.FindRef(
			ENotificationType::Region
		).Get();

	if (IsValid(Widget))
	{
		Widget->SetNotificationText(RegionName);
	}
}