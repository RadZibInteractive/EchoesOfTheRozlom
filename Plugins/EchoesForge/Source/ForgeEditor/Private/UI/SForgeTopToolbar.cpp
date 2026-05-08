// © 2026 RadZib. All rights reserved.

#include "UI/SForgeTopToolbar.h"
#include "UI/SForgeButton.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SSeparator.h"
#include "Styling/AppStyle.h"

#define LOCTEXT_NAMESPACE "ForgeTopToolbar"

namespace ForgeToolbarColors
{
	// Brand: Cyan · Magenta · Gold
	static const FLinearColor Simulate  { 0.04f, 0.30f, 0.44f, 1.f }; // cyan
	static const FLinearColor GhostOff  { 0.07f, 0.09f, 0.15f, 1.f }; // dark panel
	static const FLinearColor GhostOn   { 0.06f, 0.24f, 0.36f, 1.f }; // cyan-active
	static const FLinearColor Validate  { 0.06f, 0.12f, 0.22f, 1.f }; // deep cyan
	static const FLinearColor Publish   { 0.36f, 0.22f, 0.02f, 1.f }; // gold
	static const FLinearColor Neutral   { 0.07f, 0.09f, 0.15f, 1.f }; // dark panel
	static const FLinearColor Text      { 0.82f, 0.90f, 0.95f, 1.f }; // cool white
}

void SForgeTopToolbar::Construct(const FArguments& InArgs)
{
	bSimulateActive    = InArgs._bSimulateActive;
	bGhostModeActive   = InArgs._bGhostModeActive;
	OnUndo             = InArgs._OnUndo;
	OnRedo             = InArgs._OnRedo;
	OnSave             = InArgs._OnSave;
	OnSimulate         = InArgs._OnSimulate;
	OnGhostModeToggled = InArgs._OnGhostModeToggled;
	OnValidate         = InArgs._OnValidate;
	OnPublish          = InArgs._OnPublish;

	ChildSlot
	[
		SNew(SBorder)
		.BorderBackgroundColor(FLinearColor(0.04f, 0.05f, 0.09f, 1.f))
		.Padding(FMargin(8.f, 7.f))
		[
			SNew(SHorizontalBox)

			// --- Undo / Redo / Save ---
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(2.f, 0.f)
			[
				MakeToolButton(LOCTEXT("Undo", "↩ Undo"), ForgeToolbarColors::Neutral,
					ForgeToolbarColors::Text,
					FOnClicked::CreateSP(this, &SForgeTopToolbar::HandleUndo))
			]
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(2.f, 0.f)
			[
				MakeToolButton(LOCTEXT("Redo", "↪ Redo"), ForgeToolbarColors::Neutral,
					ForgeToolbarColors::Text,
					FOnClicked::CreateSP(this, &SForgeTopToolbar::HandleRedo))
			]
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(2.f, 0.f)
			[
				MakeToolButton(LOCTEXT("Save", "💾 Save"), ForgeToolbarColors::Neutral,
					ForgeToolbarColors::Text,
					FOnClicked::CreateSP(this, &SForgeTopToolbar::HandleSave))
			]

			// --- Separator ---
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(8.f, 2.f)
			[
				SNew(SSeparator).Orientation(Orient_Vertical)
			]

			// --- Simulate ---
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(2.f, 0.f)
			[
				MakeToolButton(LOCTEXT("Simulate", "▶  Simulate"), ForgeToolbarColors::Simulate,
					FLinearColor::White,
					FOnClicked::CreateSP(this, &SForgeTopToolbar::HandleSimulate))
			]

			// --- Ghost Mode ---
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(2.f, 0.f)
			[
				MakeToolButton(LOCTEXT("GhostMode", "👻  Ghost Mode"),
					bGhostActive ? ForgeToolbarColors::GhostOn : ForgeToolbarColors::GhostOff,
					ForgeToolbarColors::Text,
					FOnClicked::CreateSP(this, &SForgeTopToolbar::HandleGhostMode))
			]

			// --- Spacer ---
			+ SHorizontalBox::Slot().FillWidth(1.f)

			// --- Validate ---
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(2.f, 0.f)
			[
				MakeToolButton(LOCTEXT("Validate", "🔍 Validate"), ForgeToolbarColors::Validate,
					ForgeToolbarColors::Text,
					FOnClicked::CreateSP(this, &SForgeTopToolbar::HandleValidate))
			]

			// --- Publish ---
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(2.f, 0.f)
			[
				MakeToolButton(LOCTEXT("Publish", "📤 Publish"), ForgeToolbarColors::Publish,
					FLinearColor::White,
					FOnClicked::CreateSP(this, &SForgeTopToolbar::HandlePublish))
			]

			// --- Version tag ---
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(12.f, 0.f, 4.f, 0.f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("Version", "v0.1.0-alpha"))
				.Font(FCoreStyle::GetDefaultFontStyle("Regular", 8))
				.ColorAndOpacity(FSlateColor(FLinearColor(0.4f, 0.4f, 0.5f)))
			]
		]
	];
}

void SForgeTopToolbar::SetSimulateActive(bool bActive)
{
	bSimulateActive = bActive;
	Invalidate(EInvalidateWidgetReason::Paint);
}

void SForgeTopToolbar::SetGhostModeActive(bool bActive)
{
	bGhostActive = bActive;
	Invalidate(EInvalidateWidgetReason::Paint);
}

FReply SForgeTopToolbar::HandleUndo()          { OnUndo.ExecuteIfBound();  return FReply::Handled(); }
FReply SForgeTopToolbar::HandleRedo()          { OnRedo.ExecuteIfBound();  return FReply::Handled(); }
FReply SForgeTopToolbar::HandleSave()          { OnSave.ExecuteIfBound();  return FReply::Handled(); }
FReply SForgeTopToolbar::HandleValidate()      { OnValidate.ExecuteIfBound(); return FReply::Handled(); }
FReply SForgeTopToolbar::HandlePublish()       { OnPublish.ExecuteIfBound(); return FReply::Handled(); }

FReply SForgeTopToolbar::HandleSimulate()
{
	OnSimulate.ExecuteIfBound();
	return FReply::Handled();
}

FReply SForgeTopToolbar::HandleGhostMode()
{
	bGhostActive = !bGhostActive;
	OnGhostModeToggled.ExecuteIfBound(bGhostActive);
	Invalidate(EInvalidateWidgetReason::Paint);
	return FReply::Handled();
}

TSharedRef<SWidget> SForgeTopToolbar::MakeToolButton(
	const FText& Label,
	const FLinearColor& BgColor,
	const FLinearColor& TextColor,
	FOnClicked OnClicked,
	const FSlateBrush* /*Icon*/)
{
	// Delegate style resolution to SForgeButton::ResolveStyle (Custom preset)
	// so the toolbar and all other buttons share the same hover-calculation path.
	const FLinearColor HoverColor(
		FMath::Min(BgColor.R * 1.35f + 0.04f, 1.f),
		FMath::Min(BgColor.G * 1.35f + 0.04f, 1.f),
		FMath::Min(BgColor.B * 1.35f + 0.04f, 1.f));

	return SNew(SButton)
		.ButtonStyle(&SForgeButton::ResolveStyle(
			EForgeButtonPreset::Custom, BgColor, HoverColor))
		.OnClicked(OnClicked)
		.ContentPadding(FMargin(14.f, 9.f))
		[
			SNew(STextBlock)
			.Text(Label)
			.Font(FCoreStyle::GetDefaultFontStyle("Regular", 11))
			.ColorAndOpacity(FSlateColor(TextColor))
		];
}

#undef LOCTEXT_NAMESPACE
