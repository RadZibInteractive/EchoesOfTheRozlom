// © 2026 RadZib. All rights reserved.

#include "UI/SForgeValidationBar.h"
#include "UI/SForgeButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Images/SImage.h"
#include "Styling/AppStyle.h"

#define LOCTEXT_NAMESPACE "ForgeValidationBar"

void SForgeValidationBar::Construct(const FArguments& InArgs)
{
	OnWorldPartitionMapClicked = InArgs._OnWorldPartitionMapClicked;

	ChildSlot
	[
		SNew(SBorder)
		.BorderBackgroundColor(FLinearColor(0.04f, 0.05f, 0.09f))
		.Padding(FMargin(10.f, 4.f))
		[
			SNew(SHorizontalBox)

			// Validation Status label
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(0.f, 0.f, 12.f, 0.f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("StatusLabel", "Validation Status"))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 9))
				.ColorAndOpacity(FSlateColor(FLinearColor(0.55f, 0.57f, 0.65f)))
			]

			// Dynamic items row
			+ SHorizontalBox::Slot().FillWidth(1.f).VAlign(VAlign_Center)
			[
				SAssignNew(ItemsRow, SHorizontalBox)
			]

			// World Partition Map button
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
			[
				SNew(SForgeButton)
				.Preset(EForgeButtonPreset::Navy)
				.ContentPadding(FMargin(8.f, 3.f))
				.OnClicked(FOnClicked::CreateLambda([this]() -> FReply
				{
					OnWorldPartitionMapClicked.ExecuteIfBound();
					return FReply::Handled();
				}))
				[
					SNew(STextBlock)
					.Text(LOCTEXT("WorldMapBtn", "🗺  World Partition Map"))
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
					.ColorAndOpacity(FSlateColor(FLinearColor(0.65f, 0.68f, 0.80f)))
				]
			]
		]
	];
}

void SForgeValidationBar::SetItems(const TArray<FForgeValidationItem>& Items)
{
	RebuildItems(Items);
}

void SForgeValidationBar::ClearItems()
{
	if (ItemsRow.IsValid()) ItemsRow->ClearChildren();
}

void SForgeValidationBar::RebuildItems(const TArray<FForgeValidationItem>& Items)
{
	if (!ItemsRow.IsValid()) return;
	ItemsRow->ClearChildren();

	for (const FForgeValidationItem& Item : Items)
	{
		ItemsRow->AddSlot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		.Padding(0.f, 0.f, 16.f, 0.f)
		[
			SNew(SHorizontalBox)

			// Colored dot
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(0.f, 0.f, 4.f, 0.f)
			[
				SNew(SBox).WidthOverride(8.f).HeightOverride(8.f)
				[
					SNew(SImage)
					.Image(FAppStyle::Get().GetBrush("Icons.BulletPoint"))
					.ColorAndOpacity(GetColor(Item.Status))
				]
			]

			// Prefix [OK] / [Warning] / [Error]
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(0.f, 0.f, 4.f, 0.f)
			[
				SNew(STextBlock)
				.Text(GetPrefix(Item.Status))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 9))
				.ColorAndOpacity(GetColor(Item.Status))
			]

			// Message
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(Item.Message)
				.Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
				.ColorAndOpacity(FSlateColor(FLinearColor(0.65f, 0.66f, 0.72f)))
			]
		];
	}
}

FText SForgeValidationBar::GetPrefix(EForgeValidationStatus Status)
{
	switch (Status)
	{
		case EForgeValidationStatus::Warning: return LOCTEXT("Warn",  "[Warning]");
		case EForgeValidationStatus::Error:   return LOCTEXT("Err",   "[Error]");
		default:                              return LOCTEXT("Ok",    "[OK]");
	}
}

FSlateColor SForgeValidationBar::GetColor(EForgeValidationStatus Status)
{
	switch (Status)
	{
		case EForgeValidationStatus::Warning: return FSlateColor(FLinearColor(0.95f, 0.78f, 0.10f));
		case EForgeValidationStatus::Error:   return FSlateColor(FLinearColor(0.90f, 0.22f, 0.20f));
		default:                              return FSlateColor(FLinearColor(0.25f, 0.82f, 0.35f));
	}
}

#undef LOCTEXT_NAMESPACE
