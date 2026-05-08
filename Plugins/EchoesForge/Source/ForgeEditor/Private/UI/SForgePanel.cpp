// © 2026 RadZib. All rights reserved.

#include "UI/SForgePanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Layout/SBorder.h"

// ─── BuildLayout ─────────────────────────────────────────────────────────────
//
// Assembles the shared panel chrome:
//
//  ╔══════════════════════════════════════╗
//  ║  [Title]          [HeaderExtras]     ║  ← GetPanelTitle + BuildHeaderExtras
//  ║  ─────────────────────────────────   ║  ← separator
//  ║  [Content]                           ║  ← BuildContent
//  ║  [Footer]                            ║  ← BuildFooter
//  ╚══════════════════════════════════════╝

void SForgePanel::BuildLayout()
{
	ChildSlot
	[
		SNew(SBorder)
		.BorderBackgroundColor(FLinearColor(0.05f, 0.06f, 0.10f))
		.Padding(FMargin(10.f, 8.f))
		[
			SNew(SVerticalBox)

			// ── Header ──
			+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 0.f, 0.f, 4.f)
			[
				SNew(SHorizontalBox)

				// Title text (left-aligned, fills remaining space)
				+ SHorizontalBox::Slot().FillWidth(1.f).VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					// Lambda so subclasses returning a dynamic title (e.g., based on
					// ActivityType) always produce the up-to-date string.
					.Text_Lambda([this]() { return GetPanelTitle(); })
					.Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
					.ColorAndOpacity_Lambda([this]() -> FSlateColor
					{
						return FSlateColor(GetTitleColor());
					})
				]

				// Optional extra controls (right-aligned within header)
				+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
				[
					BuildHeaderExtras()
				]
			]

			// ── Separator ──
			+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 0.f, 0.f, 8.f)
			[ SNew(SSeparator) ]

			// ── Main content ──
			+ SVerticalBox::Slot().AutoHeight()
			[ BuildContent() ]

			// ── Footer (optional) ──
			+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 6.f, 0.f, 0.f)
			[ BuildFooter() ]
		]
	];
}
