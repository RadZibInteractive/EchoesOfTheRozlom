// © 2026 RadZib. All rights reserved.

#include "UI/SForgeZoneStats.h"
#include "Actors/ForgeWorldActors.h"
#include "UI/SForgeButton.h"
#include "Editor.h"
#include "EngineUtils.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Layout/SBox.h"
#include "Styling/AppStyle.h"

#define LOCTEXT_NAMESPACE "ForgeZoneStats"

// ─── Thresholds ────────────────────────────────────────────────────────────────
//   WarnAt   = soft cap (yellow)
//   DangerAt = hard cap (red)
static constexpr int32 NPC_WARN     = 8,  NPC_DANGER     = 15;
static constexpr int32 ANOMALY_WARN = 5,  ANOMALY_DANGER = 9;
static constexpr int32 ENEMY_WARN   = 12, ENEMY_DANGER   = 20;
static constexpr int32 PROPS_WARN   = 30, PROPS_DANGER   = 50;

// ─── Construct ───────────────────────────────────────────────────────────────

void SForgeZoneStats::Construct(const FArguments& InArgs)
{
	// Initialise entry definitions (counts filled in RefreshStats)
	Entries =
	{
		{ LOCTEXT("LabelNPC",     "NPCs"),      LOCTEXT("IconNPC",     "👤"),
		  0, NPC_WARN,     NPC_DANGER,     FLinearColor(0.05f, 0.52f, 0.72f) },  // cyan
		{ LOCTEXT("LabelAnomaly", "Anomalies"), LOCTEXT("IconAnomaly", "☣"),
		  0, ANOMALY_WARN, ANOMALY_DANGER, FLinearColor(0.62f, 0.10f, 0.56f) },  // magenta
		{ LOCTEXT("LabelEnemy",   "Enemies"),   LOCTEXT("IconEnemy",   "⚔"),
		  0, ENEMY_WARN,   ENEMY_DANGER,   FLinearColor(0.62f, 0.10f, 0.20f) },  // red-magenta
		{ LOCTEXT("LabelProps",   "Props"),     LOCTEXT("IconProps",   "🪨"),
		  0, PROPS_WARN,   PROPS_DANGER,   FLinearColor(0.58f, 0.40f, 0.04f) },  // gold
	};

	ChildSlot
	[
		SNew(SVerticalBox)

		// ── Header ──
		+ SVerticalBox::Slot().AutoHeight().Padding(12.f, 10.f, 12.f, 4.f)
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot().FillWidth(1.f).VAlign(VAlign_Center)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot().AutoHeight()
				[
					SNew(STextBlock)
					.Text(LOCTEXT("Title", "Zone Statistics"))
					.Font(FCoreStyle::GetDefaultFontStyle("Bold", 11))
					.ColorAndOpacity(FSlateColor(FLinearColor(0.80f, 0.90f, 0.95f)))
				]
				+ SVerticalBox::Slot().AutoHeight()
				[
					SAssignNew(ZoneNameLabel, STextBlock)
					.Text(LOCTEXT("NoZone", "No zone loaded"))
					.Font(FCoreStyle::GetDefaultFontStyle("Italic", 8))
					.ColorAndOpacity(FSlateColor(FLinearColor(0.45f, 0.47f, 0.60f)))
				]
			]

			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
			[
				SNew(SForgeButton)
				.Preset(EForgeButtonPreset::Ghost)
				.ContentPadding(FMargin(6.f, 4.f))
				.ToolTipText(LOCTEXT("RefreshTip", "Rescan current level"))
				.OnClicked(FOnClicked::CreateLambda([this]() -> FReply
				{
					RefreshStats();
					return FReply::Handled();
				}))
				[
					SNew(STextBlock)
					.Text(LOCTEXT("RefreshBtn", "↺ Refresh"))
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
					.ColorAndOpacity(FSlateColor(FLinearColor(0.5f, 0.6f, 0.9f)))
				]
			]
		]

		// ── Zone Health badge ──
		+ SVerticalBox::Slot().AutoHeight().Padding(12.f, 0.f, 12.f, 8.f)
		[
			SAssignNew(HealthBorder, SBorder)
			.BorderBackgroundColor(FLinearColor(0.07f, 0.10f, 0.18f))
			.Padding(FMargin(10.f, 6.f))
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot().FillWidth(1.f).VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("HealthTitle", "Zone Balance"))
					.Font(FCoreStyle::GetDefaultFontStyle("Bold", 9))
					.ColorAndOpacity(FSlateColor(FLinearColor(0.60f, 0.62f, 0.72f)))
				]
				+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
				[
					SAssignNew(HealthLabel, STextBlock)
					.Text(LOCTEXT("HealthInit", "—"))
					.Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
					.ColorAndOpacity(FSlateColor(FLinearColor(0.60f, 0.62f, 0.72f)))
				]
			]
		]

		+ SVerticalBox::Slot().AutoHeight().Padding(12.f, 0.f)
		[ SNew(SSeparator).SeparatorImage(FAppStyle::Get().GetBrush("Menu.Separator")) ]

		// ── Stat rows (rebuilt by RebuildRowWidgets) ──
		+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 4.f)
		[
			SAssignNew(RowContainer, SVerticalBox)
		]

		+ SVerticalBox::Slot().AutoHeight().Padding(12.f, 4.f)
		[ SNew(SSeparator).SeparatorImage(FAppStyle::Get().GetBrush("Menu.Separator")) ]

		// ── Total footer ──
		+ SVerticalBox::Slot().AutoHeight().Padding(12.f, 4.f, 12.f, 10.f)
		[
			SAssignNew(TotalLabel, STextBlock)
			.Text(LOCTEXT("TotalInit", "Total actors: 0"))
			.Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
			.ColorAndOpacity(FSlateColor(FLinearColor(0.50f, 0.52f, 0.62f)))
		]
	];

	RefreshStats();
}

// ─── RefreshStats ─────────────────────────────────────────────────────────────

void SForgeZoneStats::RefreshStats()
{
	if (!GEditor) return;
	UWorld* World = GEditor->GetEditorWorldContext().World();

	// Reset counts
	for (FStatEntry& E : Entries) E.Count = 0;

	if (World)
	{
		for (TActorIterator<AForgeNPCActor>    It(World); It; ++It) Entries[0].Count++;
		for (TActorIterator<AForgeAnomalyActor>It(World); It; ++It) Entries[1].Count++;
		for (TActorIterator<AForgeEnemyActor>  It(World); It; ++It) Entries[2].Count++;
		for (TActorIterator<AForgePropActor>   It(World); It; ++It) Entries[3].Count++;

		// Zone name from world
		if (ZoneNameLabel.IsValid())
		{
			ZoneNameLabel->SetText(FText::FromString(World->GetMapName()));
		}
	}
	else
	{
		if (ZoneNameLabel.IsValid())
			ZoneNameLabel->SetText(LOCTEXT("NoZone", "No zone loaded"));
	}

	RebuildRowWidgets();
}

// ─── RebuildRowWidgets ────────────────────────────────────────────────────────

void SForgeZoneStats::RebuildRowWidgets()
{
	if (!RowContainer.IsValid()) return;
	RowContainer->ClearChildren();

	int32 TotalCount    = 0;
	int32 DangerCount   = 0;
	int32 WarnCount     = 0;

	for (const FStatEntry& E : Entries)
	{
		TotalCount += E.Count;
		if (E.Count >= E.DangerAt) DangerCount++;
		else if (E.Count >= E.WarnAt) WarnCount++;

		RowContainer->AddSlot().AutoHeight().Padding(12.f, 5.f, 12.f, 5.f)
		[
			MakeEntryRow(E)
		];
	}

	// Overall health
	if (HealthLabel.IsValid() && HealthBorder.IsValid())
	{
		FLinearColor HealthColor;
		FText HealthText;
		if (DangerCount > 0)
		{
			HealthText  = FText::Format(LOCTEXT("Overloaded", "⚠  OVERLOADED  ({0} over limit)"), FText::AsNumber(DangerCount));
			HealthColor = FLinearColor(0.85f, 0.20f, 0.12f);
		}
		else if (WarnCount > 0)
		{
			HealthText  = FText::Format(LOCTEXT("Crowded", "◐  CROWDED  ({0} near limit)"), FText::AsNumber(WarnCount));
			HealthColor = FLinearColor(0.85f, 0.65f, 0.10f);
		}
		else if (TotalCount == 0)
		{
			HealthText  = LOCTEXT("Empty", "○  EMPTY — add some actors");
			HealthColor = FLinearColor(0.40f, 0.42f, 0.55f);
		}
		else
		{
			HealthText  = LOCTEXT("Balanced", "✓  BALANCED");
			HealthColor = FLinearColor(0.15f, 0.75f, 0.30f);
		}
		HealthLabel->SetText(HealthText);
		HealthLabel->SetColorAndOpacity(FSlateColor(HealthColor));
		HealthBorder->SetBorderBackgroundColor(HealthColor * 0.15f);
	}

	// Total footer
	if (TotalLabel.IsValid())
	{
		TotalLabel->SetText(FText::Format(
			LOCTEXT("TotalFmt", "Total Forge actors in zone: {0}"),
			FText::AsNumber(TotalCount)));
	}
}

TSharedRef<SWidget> SForgeZoneStats::MakeEntryRow(const FStatEntry& E) const
{
	const FLinearColor StatusCol = StatusColor(E.Count, E.WarnAt, E.DangerAt);
	const FText        Badge     = StatusBadge(E.Count, E.WarnAt, E.DangerAt);
	const int32        BarMax    = FMath::Max(E.DangerAt, E.Count + 1);

	return SNew(SVerticalBox)

		// ── Title row ──
		+ SVerticalBox::Slot().AutoHeight()
		[
			SNew(SHorizontalBox)

			// Icon + Label
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(0.f, 0.f, 6.f, 0.f)
			[
				SNew(STextBlock)
				.Text(E.Icon)
				.Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
				.ColorAndOpacity(FSlateColor(E.AccentColor))
			]
			+ SHorizontalBox::Slot().FillWidth(1.f).VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(E.Label)
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 9))
				.ColorAndOpacity(FSlateColor(FLinearColor(0.75f, 0.77f, 0.88f)))
			]

			// Count
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(4.f, 0.f)
			[
				SNew(STextBlock)
				.Text(FText::AsNumber(E.Count))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 11))
				.ColorAndOpacity(FSlateColor(StatusCol))
			]

			// Status badge
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
			[
				SNew(SBorder)
				.BorderBackgroundColor(StatusCol * 0.25f)
				.Padding(FMargin(5.f, 2.f))
				[
					SNew(STextBlock)
					.Text(Badge)
					.Font(FCoreStyle::GetDefaultFontStyle("Bold", 7))
					.ColorAndOpacity(FSlateColor(StatusCol))
				]
			]
		]

		// ── Fill bar ──
		+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 3.f, 0.f, 0.f)
		[
			MakeBarWidget(E.Count, BarMax, E.AccentColor)
		];
}

TSharedRef<SWidget> SForgeZoneStats::MakeBarWidget(int32 Count, int32 Max, FLinearColor Color) const
{
	const float Fill = (Max > 0) ? FMath::Clamp((float)Count / (float)Max, 0.f, 1.f) : 0.f;

	return SNew(SBox).HeightOverride(4.f)
	[
		SNew(SHorizontalBox)

		// Filled portion
		+ SHorizontalBox::Slot().FillWidth(Fill)
		[
			SNew(SBorder)
			.BorderBackgroundColor(Color)
			[ SNullWidget::NullWidget ]
		]

		// Empty portion
		+ SHorizontalBox::Slot().FillWidth(1.f - Fill)
		[
			SNew(SBorder)
			.BorderBackgroundColor(FLinearColor(0.10f, 0.11f, 0.18f))
			[ SNullWidget::NullWidget ]
		]
	];
}

// ─── Helpers ─────────────────────────────────────────────────────────────────

FLinearColor SForgeZoneStats::StatusColor(int32 Count, int32 Warn, int32 Danger)
{
	if (Count >= Danger) return FLinearColor(0.85f, 0.18f, 0.10f);  // red
	if (Count >= Warn)   return FLinearColor(0.85f, 0.65f, 0.10f);  // yellow
	return FLinearColor(0.20f, 0.80f, 0.35f);                        // green
}

FText SForgeZoneStats::StatusBadge(int32 Count, int32 Warn, int32 Danger)
{
	if (Count >= Danger) return LOCTEXT("BadgeDanger", "OVER");
	if (Count >= Warn)   return LOCTEXT("BadgeWarn",   "WARN");
	return LOCTEXT("BadgeOK", "OK");
}

#undef LOCTEXT_NAMESPACE
