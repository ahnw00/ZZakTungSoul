#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "Subsystems/SubsystemCollection.h"
#include "UI/Notify/NotificationTypes.h"
#include "NotificationSubsystem.generated.h"
class UNotificationWidget;

UCLASS(BlueprintType)
class ZZAKTUNGSOUL_API UNotificationSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	UNotificationSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "UI|Notification")
	void ShowNotification(
		ENotificationType Type = ENotificationType::Death
	);

	UFUNCTION(BlueprintCallable, Category = "UI|Notification")
	void HideNotification(
		ENotificationType Type = ENotificationType::Death
	);

	UFUNCTION(BlueprintCallable, Category = "UI|Notification")
	void ShowRegionNotification(const FText& RegionName);

private:
	UPROPERTY()
	TMap<ENotificationType, TSubclassOf<UNotificationWidget>>
		NotificationWidgetClasses;

	UPROPERTY(Transient)
	TMap<ENotificationType, TObjectPtr<UNotificationWidget>>
		NotificationWidgets;
};