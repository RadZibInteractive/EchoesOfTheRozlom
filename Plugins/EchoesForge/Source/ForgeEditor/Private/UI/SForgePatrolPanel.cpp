// © 2026 RadZib. All rights reserved.

#include "UI/SForgePatrolPanel.h"
#include "Actors/ForgeWorldActors.h"
#include "Utils/ForgeUIStyle.h"
#include "Editor.h"
#include "EditorViewportClient.h"
#include "DrawDebugHelpers.h"

#include "Widgets/Text/STextBlock.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Layout/SBox.h"
#include "Styling/AppStyle.h"

#define LOCTEXT_NAMESPACE "ForgePatrolPanel"

// ─── Construct ───────────────────────────────────────────────────────────────

void SForgePatrolPanel::Construct(const FArguments& /*InArgs*/)
{
	// SForgePanel::BuildLayout() calls all virtual Build* methods,
	// so member state must be ready before we call it.
	BuildLayout();
}

SForgePatrolPanel::~SForgePatrolPanel()
{
	ClearVisualization();
}

// ─── SForgePanel overrides ────────────────────────────────────────────────────

FText SForgePatrolPanel::GetPanelTitle() const
{
	return LOCTEXT("Title", "Patrol Waypoints");
}

FLinearColor SForgePatrolPanel::GetTitleColor() const
{
	return FLinearColor(0.78f, 0.56f, 0.06f);   // brand gold
}

TSharedRef<SWidget> SForgePatrolPanel::BuildHeaderExtras()
{
	// [+ Add]  [✕ Clear] — compact, live in the title row
	return SNew(SHorizontalBox)

		+ SHorizontalBox::Slot().AutoWidth().Padding(2.f, 0.f)
		[
			SNew(SForgeButton)
			.Preset(EForgeButtonPreset::Confirm)
			.Label(LOCTEXT("AddBtn", "+ Add"))
			.ContentPadding(FMargin(6.f, 2.f))
			.Bold(true)
			.FontSize(8)
			.ToolTipText(LOCTEXT("AddTip", "Add waypoint at camera position"))
			.OnClicked(this, &SForgePatrolPanel::OnAddAtCamera)
		]

		+ SHorizontalBox::Slot().AutoWidth().Padding(2.f, 0.f)
		[
			SNew(SForgeButton)
			.Preset(EForgeButtonPreset::Danger)
			.Label(LOCTEXT("ClearBtn", "✕ Clear"))
			.ContentPadding(FMargin(6.f, 2.f))
			.Bold(true)
			.FontSize(8)
			.ToolTipText(LOCTEXT("ClearTip", "Remove all waypoints"))
			.OnClicked(this, &SForgePatrolPanel::OnClearAll)
		];
}

TSharedRef<SWidget> SForgePatrolPanel::BuildContent()
{
	return SNew(SVerticalBox)

		// Empty-state hint
		+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 4.f)
		[
			SAssignNew(EmptyHint, STextBlock)
			.Text(LOCTEXT("Empty",
				"No waypoints. Click '+ Add' to place one at the camera position."))
			.AutoWrapText(true)
			.Font(FCoreStyle::GetDefaultFontStyle("Italic", 8))
			.ColorAndOpacity(FSlateColor(FLinearColor(0.40f, 0.42f, 0.55f)))
			.Visibility(EVisibility::Visible)
		]

		// Waypoint list
		+ SVerticalBox::Slot().AutoHeight().MaxHeight(140.f)
		[
			SAssignNew(WaypointList, SListView<TSharedPtr<int32>>)
			.ListItemsSource(&WaypointIndices)
			.OnGenerateRow(this, &SForgePatrolPanel::MakeWaypointRow)
			.SelectionMode(ESelectionMode::Single)
		];
}

TSharedRef<SWidget> SForgePatrolPanel::BuildFooter()
{
	return SNew(SForgeButton)
		.Preset(EForgeButtonPreset::Amber)
		.Label(LOCTEXT("VizBtn", "👁  Visualize Path in Viewport"))
		.ContentPadding(FMargin(0.f, 5.f))
		.HAlign(HAlign_Center)
		.Bold(true)
		.FontSize(8)
		.ToolTipText(LOCTEXT("VizTip",
			"Draw patrol path in viewport (persistent until cleared)"))
		.OnClicked(this, &SForgePatrolPanel::OnVisualize);
}

// ─── SetEnemy ────────────────────────────────────────────────────────────────

void SForgePatrolPanel::SetEnemy(AForgeEnemyActor* Enemy)
{
	ClearVisualization();
	CurrentEnemy = Enemy;
	RebuildList();
}

// ─── RebuildList ─────────────────────────────────────────────────────────────

void SForgePatrolPanel::RebuildList()
{
	WaypointIndices.Empty();

	if (AForgeEnemyActor* E = CurrentEnemy.Get())
	{
		for (int32 i = 0; i < E->PatrolWaypoints.Num(); ++i)
		{
			WaypointIndices.Add(MakeShared<int32>(i));
		}
	}

	const bool bEmpty = WaypointIndices.IsEmpty();
	if (EmptyHint.IsValid())
	{
		EmptyHint->SetVisibility(bEmpty ? EVisibility::Visible : EVisibility::Collapsed);
	}
	if (WaypointList.IsValid())
	{
		WaypointList->RequestListRefresh();
	}
}

// ─── MakeWaypointRow ─────────────────────────────────────────────────────────

TSharedRef<ITableRow> SForgePatrolPanel::MakeWaypointRow(
	TSharedPtr<int32>              Item,
	const TSharedRef<STableViewBase>& Owner)
{
	if (!Item.IsValid() || !CurrentEnemy.IsValid())
	{
		return SNew(STableRow<TSharedPtr<int32>>, Owner);
	}

	const int32 Idx = *Item;
	AForgeEnemyActor* E = CurrentEnemy.Get();

	if (!E->PatrolWaypoints.IsValidIndex(Idx))
	{
		return SNew(STableRow<TSharedPtr<int32>>, Owner);
	}

	const FVector& WP = E->PatrolWaypoints[Idx];
	const FText CoordText = FText::FromString(
		FString::Printf(TEXT("[%d]  X=%.0f  Y=%.0f  Z=%.0f"), Idx, WP.X, WP.Y, WP.Z));

	return SNew(STableRow<TSharedPtr<int32>>, Owner).Padding(FMargin(2.f))
	[
		SNew(SHorizontalBox)

		// Index badge
		+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(0.f, 0.f, 6.f, 0.f)
		[
			SNew(SBorder)
			.BorderBackgroundColor(FLinearColor(0.52f, 0.36f, 0.04f))
			.Padding(FMargin(5.f, 1.f))
			[
				SNew(STextBlock)
				.Text(FText::AsNumber(Idx))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 7))
				.ColorAndOpacity(FSlateColor(FLinearColor::White))
			]
		]

		// Coordinates
		+ SHorizontalBox::Slot().FillWidth(1.f).VAlign(VAlign_Center)
		[
			SNew(STextBlock)
			.Text(CoordText)
			.Font(FCoreStyle::GetDefaultFontStyle("Mono", 8))
			.ColorAndOpacity(FSlateColor(FLinearColor(0.75f, 0.78f, 0.88f)))
		]

		// ↑ ↓ ✕  (subtle transparent buttons — hover reveals them)
		+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot().AutoWidth()
			[
				SNew(SForgeButton)
				.Preset(EForgeButtonPreset::Subtle)
				.ContentPadding(FMargin(3.f, 0.f))
				.OnClicked(this, &SForgePatrolPanel::OnMoveUp, Idx)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("↑")))
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
					.ColorAndOpacity(FSlateColor(FLinearColor(0.5f, 0.6f, 0.8f)))
				]
			]

			+ SHorizontalBox::Slot().AutoWidth()
			[
				SNew(SForgeButton)
				.Preset(EForgeButtonPreset::Subtle)
				.ContentPadding(FMargin(3.f, 0.f))
				.OnClicked(this, &SForgePatrolPanel::OnMoveDown, Idx)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("↓")))
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
					.ColorAndOpacity(FSlateColor(FLinearColor(0.5f, 0.6f, 0.8f)))
				]
			]

			+ SHorizontalBox::Slot().AutoWidth()
			[
				// Transparent→danger on hover — the ✕ appears when you need it
				SNew(SForgeButton)
				.Preset(EForgeButtonPreset::Custom)
				.NormalColor(FLinearColor(0.f, 0.f, 0.f, 0.f))
				.HoverColor (FLinearColor(0.30f, 0.06f, 0.05f))
				.ContentPadding(FMargin(3.f, 0.f))
				.OnClicked(this, &SForgePatrolPanel::OnRemoveWaypoint, Idx)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("✕")))
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
					.ColorAndOpacity(FSlateColor(FLinearColor(0.8f, 0.3f, 0.2f)))
				]
			]
		]
	];
}

// ─── Button callbacks ─────────────────────────────────────────────────────────

FReply SForgePatrolPanel::OnAddAtCamera()
{
	AForgeEnemyActor* E = CurrentEnemy.Get();
	if (!E || !GEditor) return FReply::Handled();

	FVector Loc = E->GetActorLocation();
	if (FViewport* VP = GEditor->GetActiveViewport())
	{
		if (FEditorViewportClient* C = static_cast<FEditorViewportClient*>(VP->GetClient()))
		{
			Loc = C->GetViewLocation() + C->GetViewRotation().Vector() * 200.f;
		}
	}

	E->Modify();
	E->PatrolWaypoints.Add(Loc);
	RebuildList();
	if (bVisualizationActive) VisualizePatrol();
	return FReply::Handled();
}

FReply SForgePatrolPanel::OnRemoveWaypoint(int32 Index)
{
	AForgeEnemyActor* E = CurrentEnemy.Get();
	if (!E || !E->PatrolWaypoints.IsValidIndex(Index)) return FReply::Handled();

	E->Modify();
	E->PatrolWaypoints.RemoveAt(Index);
	RebuildList();
	if (bVisualizationActive) VisualizePatrol();
	return FReply::Handled();
}

FReply SForgePatrolPanel::OnMoveUp(int32 Index)
{
	AForgeEnemyActor* E = CurrentEnemy.Get();
	if (!E || Index <= 0 || !E->PatrolWaypoints.IsValidIndex(Index)) return FReply::Handled();

	E->Modify();
	E->PatrolWaypoints.Swap(Index, Index - 1);
	RebuildList();
	if (bVisualizationActive) VisualizePatrol();
	return FReply::Handled();
}

FReply SForgePatrolPanel::OnMoveDown(int32 Index)
{
	AForgeEnemyActor* E = CurrentEnemy.Get();
	if (!E || Index >= E->PatrolWaypoints.Num() - 1) return FReply::Handled();

	E->Modify();
	E->PatrolWaypoints.Swap(Index, Index + 1);
	RebuildList();
	if (bVisualizationActive) VisualizePatrol();
	return FReply::Handled();
}

FReply SForgePatrolPanel::OnClearAll()
{
	AForgeEnemyActor* E = CurrentEnemy.Get();
	if (!E) return FReply::Handled();

	E->Modify();
	E->PatrolWaypoints.Empty();
	ClearVisualization();
	RebuildList();
	return FReply::Handled();
}

FReply SForgePatrolPanel::OnVisualize()
{
	VisualizePatrol();
	return FReply::Handled();
}

// ─── Visualization ────────────────────────────────────────────────────────────

void SForgePatrolPanel::VisualizePatrol()
{
	AForgeEnemyActor* E = CurrentEnemy.Get();
	if (!E || !GEditor) return;

	UWorld* World = GEditor->GetEditorWorldContext().World();
	if (!World) return;

	FlushPersistentDebugLines(World);

	const TArray<FVector>& WPs = E->PatrolWaypoints;
	if (WPs.IsEmpty()) return;

	const FColor LineColor  (220, 160, 20);   // gold
	const FColor LoopColor  (170, 115, 12);   // gold dim
	const FColor SphereColor(255, 200, 40);   // gold bright

	for (int32 i = 0; i < WPs.Num(); ++i)
	{
		DrawDebugSphere(World, WPs[i], 28.f, 10, SphereColor, /*bPersistent=*/true);
		DrawDebugPoint (World, WPs[i], 8.f, SphereColor, true);

		if (i < WPs.Num() - 1)
		{
			DrawDebugLine(World, WPs[i], WPs[i + 1], LineColor, true, -1.f, 0, 3.f);
		}
	}

	if (E->bLoopPatrol && WPs.Num() > 1)
	{
		DrawDebugLine(World, WPs.Last(), WPs[0], LoopColor, true, -1.f, 0, 2.f);
	}

	bVisualizationActive = true;
}

void SForgePatrolPanel::ClearVisualization()
{
	if (!bVisualizationActive || !GEditor) return;

	if (UWorld* World = GEditor->GetEditorWorldContext().World())
	{
		FlushPersistentDebugLines(World);
	}
	bVisualizationActive = false;
}

#undef LOCTEXT_NAMESPACE
