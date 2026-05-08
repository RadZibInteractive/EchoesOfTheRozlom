// © 2026 RadZib. All rights reserved.

#include "UI/SForgeButton.h"
#include "Utils/ForgeUIStyle.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Styling/AppStyle.h"

// ─── Construct ───────────────────────────────────────────────────────────────

void SForgeButton::Construct(const FArguments& InArgs)
{
	const FButtonStyle& Style = ResolveStyle(
		InArgs._Preset,
		InArgs._NormalColor,
		InArgs._HoverColor);

	// Content priority: explicit child slot > Label attribute
	TSharedRef<SWidget> ButtonContent = InArgs._Content.Widget;

	if (ButtonContent == SNullWidget::NullWidget && InArgs._Label.IsSet())
	{
		const FString FontFace = InArgs._Bold ? TEXT("Bold") : TEXT("Regular");

		ButtonContent = SNew(STextBlock)
			.Text(InArgs._Label)
			.Font(FCoreStyle::GetDefaultFontStyle(*FontFace, InArgs._FontSize))
			.ColorAndOpacity(FSlateColor(FLinearColor::White));
	}

	ChildSlot
	[
		SNew(SButton)
		.ButtonStyle     (&Style)
		.ContentPadding  (InArgs._ContentPadding)
		.HAlign          (InArgs._HAlign)
		.ToolTipText     (InArgs._ToolTipText)
		.OnClicked       (InArgs._OnClicked)
		[ ButtonContent ]
	];
}

// ─── ResolveStyle ─────────────────────────────────────────────────────────────

// static
const FButtonStyle& SForgeButton::ResolveStyle(
	EForgeButtonPreset  Preset,
	const FLinearColor& Normal,
	const FLinearColor& Hover)
{
	switch (Preset)
	{
	case EForgeButtonPreset::Ghost:   return ForgeGhostBtn();
	case EForgeButtonPreset::Subtle:  return ForgeSubtleBtn();
	case EForgeButtonPreset::Primary: return ForgePrimaryBtn();
	case EForgeButtonPreset::Confirm: return ForgeConfirmBtn();
	case EForgeButtonPreset::Danger:  return ForgeDangerBtn();
	case EForgeButtonPreset::Navy:    return ForgeNavyBtn();
	case EForgeButtonPreset::Violet:  return ForgeVioletBtn();
	case EForgeButtonPreset::Amber:   return ForgeAmberBtn();
	case EForgeButtonPreset::Custom:
	default:
		return ForgeButtonStyle(Normal, Hover);
	}
}
