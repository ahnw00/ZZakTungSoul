#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ClownYabawiWidget.generated.h"

class UFont;
class UTextBlock;

UCLASS()
class ZZAKTUNGSOUL_API UClownYabawiWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UClownYabawiWidget(const FObjectInitializer& ObjectInitializer);
	void SetPresentation(const FText& Heading, const FText& Description, FLinearColor Color);

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	UPROPERTY()
	TObjectPtr<UFont> RegionFont;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> Title;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> Detail;
};
