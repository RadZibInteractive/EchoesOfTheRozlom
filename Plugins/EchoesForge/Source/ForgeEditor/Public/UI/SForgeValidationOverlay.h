// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Data/ForgeTypes.h"

struct FORGEEDITOR_API FForgeValidationEntry
{
	FString ObjectName;
	EForgeValidationStatus Status = EForgeValidationStatus::Valid;
	FText Message;
};

// Overlay panel displaying color-coded validation results before save.
// Matches DD v3.0 section 2.6: Validation Overlay.
// Green = Valid, Yellow = Warning, Red = Error.
class FORGEEDITOR_API SForgeValidationOverlay : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SForgeValidationOverlay) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	void SetEntries(const TArray<FForgeValidationEntry>& Entries);

	void ClearEntries();

	bool HasErrors() const;

private:
	TArray<FForgeValidationEntry> ValidationEntries;
	TSharedPtr<SScrollBox> EntryList;

	void RebuildList();

	static FSlateColor GetColorForStatus(EForgeValidationStatus Status);
	static FText GetIconForStatus(EForgeValidationStatus Status);

	TSharedRef<SWidget> BuildEntryRow(const FForgeValidationEntry& Entry);
};
