#include "Enemy/Boss/Clown/UI/ClownYabawiWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Engine/Font.h"
#include "UObject/ConstructorHelpers.h"

UClownYabawiWidget::UClownYabawiWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UFont> Font(TEXT("/Game/UI/Fonts/HS1_0_Regular_Font.HS1_0_Regular_Font"));
	RegionFont = Font.Object;
}

TSharedRef<SWidget> UClownYabawiWidget::RebuildWidget()
{
	if (!WidgetTree)
	{
		WidgetTree = NewObject<UWidgetTree>(this, TEXT("WidgetTree"));
	}

	if (!WidgetTree->RootWidget)
	{
		USizeBox* Size = WidgetTree->ConstructWidget<USizeBox>();
		Size->SetWidthOverride(500.0f);

		UBorder* Panel = WidgetTree->ConstructWidget<UBorder>();
		Panel->SetBrushColor(FLinearColor(0.015f, 0.012f, 0.01f, 0.85f));
		Panel->SetPadding(FMargin(24.0f, 14.0f));

		UVerticalBox* Content = WidgetTree->ConstructWidget<UVerticalBox>();

		Title = WidgetTree->ConstructWidget<UTextBlock>();
		Title->SetFont(FSlateFontInfo(RegionFont, 26));
		Title->SetJustification(ETextJustify::Center);
		Title->SetAutoWrapText(true);
		Title->SetShadowOffset(FVector2D(0.0f, 2.0f));
		Title->SetShadowColorAndOpacity(FLinearColor::Black);
		Content->AddChildToVerticalBox(Title);

		Detail = WidgetTree->ConstructWidget<UTextBlock>();
		Detail->SetFont(FSlateFontInfo(RegionFont, 18));
		Detail->SetJustification(ETextJustify::Center);
		Detail->SetColorAndOpacity(FSlateColor(FLinearColor(0.8f, 0.77f, 0.68f)));
		Content->AddChildToVerticalBox(Detail)->SetPadding(FMargin(0.0f, 8.0f, 0.0f, 0.0f));

		Panel->SetContent(Content);
		Size->SetContent(Panel);
		WidgetTree->RootWidget = Size;

		SetVisibility(ESlateVisibility::HitTestInvisible);
	}

	return Super::RebuildWidget();
}

void UClownYabawiWidget::SetPresentation(const FText& Heading, const FText& Description, FLinearColor Color)
{
	if (Title)
	{
		Title->SetText(Heading);
		Title->SetColorAndOpacity(FSlateColor(Color));
	}

	if (Detail)
	{
		Detail->SetText(Description);
	}
}
