// © 2026 RadZib. All rights reserved.

#include "UI/SForgeValidationOverlay.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBorder.h"

#define LOCTEXT_NAMESPACE "ForgeValidationOverlay"

void SForgeValidationOverlay::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SVerticalBox)

		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.f, 0.f, 0.f, 6.f)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("ValidationTitle", "Validation"))
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", 11))
		]

		+ SVerticalBox::Slot()
		.FillHeight(1.f)
		[
			SNew(SScrollBox)
			+ SScrollBox::Slot()
			[
				SAssignNew(EntryList, SScrollBox)
			]
		]
	];
}

void SForgeValidationOverlay::SetEntries(const TArray<FForgeValidationEntry>& Entries)
{
	ValidationEntries = Entries;
	RebuildList();
}

void SForgeValidationOverlay::ClearEntries()
{
	ValidationEntries.Empty();
	RebuildList();
}

bool SForgeValidationOverlay::HasErrors() const
{
	return ValidationEntries.ContainsByPredicate([](const FForgeValidationEntry& E)
	{
		return E.Status == EForgeValidationStatus::Error;
	});
}

void SForgeValidationOverlay::RebuildList()
{
	if (!EntryList.IsValid()) return;

	EntryList->ClearChildren();

	for (const FForgeValidationEntry& Entry : ValidationEntries)
	{
		EntryList->AddSlot()
		.Padding(2.f)
		[
			BuildEntryRow(Entry)
		];
	}
}

FSlateColor SForgeValidationOverlay::GetColorForStatus(EForgeValidationStatus Status)
{
	switch (Status)
	{
		case EForgeValidationStatus::Warning: return FSlateColor(FLinearColor(1.f, 0.85f, 0.f));
		case EForgeValidationStatus::Error:   return FSlateColor(FLinearColor(1.f, 0.2f,  0.2f));
		default:                              return FSlateColor(FLinearColor(0.3f, 0.9f,  0.3f));
	}
}

FText SForgeValidationOverlay::GetIconForStatus(EForgeValidationStatus Status)
{
	switch (Status)
	{
		case EForgeValidationStatus::Warning: return FText::FromString(TEXT("[!]"));
		case EForgeValidationStatus::Error:   return FText::FromString(TEXT("[X]"));
		default:                              return FText::FromString(TEXT("[OK]"));
	}
}

TSharedRef<SWidget> SForgeValidationOverlay::BuildEntryRow(const FForgeValidationEntry& Entry)
{
	return SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(0.f, 0.f, 6.f, 0.f)
		[
			SNew(STextBlock)
			.Text(GetIconForStatus(Entry.Status))
			.ColorAndOpacity(GetColorForStatus(Entry.Status))
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
		]

		+ SHorizontalBox::Slot()
		.FillWidth(1.f)
		[
			SNew(SVerticalBox)

			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(STextBlock)
				.Text(FText::FromString(Entry.ObjectName))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 9))
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(STextBlock)
				.Text(Entry.Message)
				.AutoWrapText(true)
				.Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
				.ColorAndOpacity(FSlateColor(FLinearColor(0.7f, 0.7f, 0.7f)))
			]
		];
}

#undef LOCTEXT_NAMESPACE
