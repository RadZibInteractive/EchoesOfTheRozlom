// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SButton.h"

// ─── Preset enum ─────────────────────────────────────────────────────────────
// Maps to the named ForgeUIStyle presets.  Use Custom to supply exact colors.

enum class EForgeButtonPreset : uint8
{
	Custom,   // NormalColor / HoverColor supplied by caller
	Ghost,    // Dark panel bg, slightly lighter on hover
	Subtle,   // Fully transparent, dim highlight on hover
	Primary,  // Teal action button
	Confirm,  // Green  — place / confirm
	Danger,   // Red    — destructive action
	Navy,     // Dark navy — browse / back / navigate
	Violet,   // Purple — anomaly-themed
	Amber,    // Orange — patrol-themed
};

// ─── SForgeButton ─────────────────────────────────────────────────────────────
//
// Single configurable Forge button widget.  Eliminates the per-call-site
// repetition of ButtonStyle + STextBlock boilerplate.
//
// Simple usage (preset + text label):
//   SNew(SForgeButton)
//   .Preset(EForgeButtonPreset::Confirm)
//   .Label(LOCTEXT("Place", "▶  Place in World"))
//   .ContentPadding(FMargin(0.f, 9.f))
//   .HAlign(HAlign_Center)
//   .OnClicked(this, &MyPanel::OnPlace)
//
// Custom content slot (icon + text, spinbox, …):
//   SNew(SForgeButton)
//   .Preset(EForgeButtonPreset::Danger)
//   .OnClicked(...)
//   [ SNew(SImage).Image(...) ]
//
// Custom colors:
//   SNew(SForgeButton)
//   .Preset(EForgeButtonPreset::Custom)
//   .NormalColor(FLinearColor(0.08f, 0.38f, 0.36f))
//   .HoverColor (FLinearColor(0.12f, 0.52f, 0.50f))
//   .OnClicked(...)

class FORGEEDITOR_API SForgeButton : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SForgeButton)
		: _Preset        (EForgeButtonPreset::Ghost)
		, _NormalColor   (FLinearColor(0.08f, 0.09f, 0.14f))
		, _HoverColor    (FLinearColor(0.14f, 0.16f, 0.26f))
		, _ContentPadding(FMargin(8.f, 4.f))
		, _HAlign        (HAlign_Fill)
		, _FontSize      (9)
		, _Bold          (false)
		{}

		/** Named style preset — overrides NormalColor/HoverColor unless Custom. */
		SLATE_ARGUMENT(EForgeButtonPreset, Preset)

		/** Normal bg color — only used when Preset == Custom. */
		SLATE_ARGUMENT(FLinearColor, NormalColor)

		/** Hover  bg color — only used when Preset == Custom. */
		SLATE_ARGUMENT(FLinearColor, HoverColor)

		/** Padding inside the button around its content. */
		SLATE_ARGUMENT(FMargin, ContentPadding)

		/** Horizontal alignment of content inside the button. */
		SLATE_ARGUMENT(EHorizontalAlignment, HAlign)

		/**
		 * Short-hand: text rendered inside the button in white.
		 * If a child slot is also provided, the slot takes precedence.
		 */
		SLATE_ATTRIBUTE(FText, Label)

		/** Font size for the Label short-hand (default 9). */
		SLATE_ARGUMENT(int32, FontSize)

		/** Use bold font for the Label short-hand. */
		SLATE_ARGUMENT(bool, Bold)

		/** Tooltip shown on mouse-over. */
		SLATE_ATTRIBUTE(FText, ToolTipText)

		/** Click delegate. */
		SLATE_EVENT(FOnClicked, OnClicked)

		/** Optional custom content — takes precedence over Label if both supplied. */
		SLATE_DEFAULT_SLOT(FArguments, Content)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	// ── Static utility ────────────────────────────────────────────────────────

	/**
	 * Resolve the FButtonStyle for the given preset.
	 * Normal/Hover are only used when Preset == Custom.
	 * Exposed as static so other widgets (e.g., MakeToolButton helpers) can
	 * reuse the same resolution logic without constructing a full SForgeButton.
	 */
	static const FButtonStyle& ResolveStyle(
		EForgeButtonPreset  Preset,
		const FLinearColor& Normal,
		const FLinearColor& Hover);
};
