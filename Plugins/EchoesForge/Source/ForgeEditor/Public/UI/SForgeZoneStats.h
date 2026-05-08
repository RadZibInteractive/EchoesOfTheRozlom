// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

// Displays per-type Forge actor counts for the active zone with
// colour-coded health thresholds and an overall balance score.
class FORGEEDITOR_API SForgeZoneStats : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SForgeZoneStats) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	// Rescan the current editor world and redraw all rows
	void RefreshStats();

private:
	struct FStatEntry
	{
		FText        Label;
		FText        Icon;
		int32        Count      = 0;
		int32        WarnAt     = 0;   // count >= this → yellow
		int32        DangerAt   = 0;   // count >= this → red
		FLinearColor AccentColor;
	};

	TArray<FStatEntry>       Entries;
	TSharedPtr<SVerticalBox> RowContainer;
	TSharedPtr<STextBlock>   HealthLabel;
	TSharedPtr<SBorder>      HealthBorder;
	TSharedPtr<STextBlock>   TotalLabel;
	TSharedPtr<STextBlock>   ZoneNameLabel;

	void RebuildRowWidgets();
	TSharedRef<SWidget> MakeEntryRow(const FStatEntry& E) const;
	TSharedRef<SWidget> MakeBarWidget(int32 Count, int32 Max, FLinearColor Color) const;

	static FLinearColor StatusColor(int32 Count, int32 Warn, int32 Danger);
	static FText        StatusBadge(int32 Count, int32 Warn, int32 Danger);
};
