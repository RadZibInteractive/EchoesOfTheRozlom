// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/SForgePanel.h"
#include "UI/SForgeButton.h"

class AForgeEnemyActor;

// ─── SForgePatrolPanel ────────────────────────────────────────────────────────
//
// Patrol-waypoint editor shown inside SForgeActorInspector when an enemy actor
// is selected.  Inherits SForgePanel (Template Method):
//
//  GetPanelTitle()     → "Patrol Waypoints"
//  GetTitleColor()     → amber
//  BuildHeaderExtras() → [+ Add] [✕ Clear]  (compact header buttons)
//  BuildContent()      → empty-hint label  + waypoint SListView
//  BuildFooter()       → [👁 Visualize Path in Viewport]
//
// Public API:
//  SetEnemy(AForgeEnemyActor*)  — bind to an actor (pass nullptr to clear)

class FORGEEDITOR_API SForgePatrolPanel : public SForgePanel
{
public:
	SLATE_BEGIN_ARGS(SForgePatrolPanel) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	virtual ~SForgePatrolPanel() override;

	/** Bind to a specific enemy actor.  Pass nullptr to clear. */
	void SetEnemy(AForgeEnemyActor* Enemy);

protected:
	// ── SForgePanel overrides ─────────────────────────────────────────────────

	virtual FText        GetPanelTitle()     const override;
	virtual FLinearColor GetTitleColor()     const override;
	virtual TSharedRef<SWidget> BuildHeaderExtras()  override;
	virtual TSharedRef<SWidget> BuildContent()       override;
	virtual TSharedRef<SWidget> BuildFooter()        override;

private:
	// ── State ────────────────────────────────────────────────────────────────

	TWeakObjectPtr<AForgeEnemyActor>          CurrentEnemy;
	TArray<TSharedPtr<int32>>                 WaypointIndices;
	TSharedPtr<SListView<TSharedPtr<int32>>>  WaypointList;
	TSharedPtr<STextBlock>                    EmptyHint;
	bool                                      bVisualizationActive = false;

	// ── Helpers ───────────────────────────────────────────────────────────────

	void RebuildList();
	void VisualizePatrol();
	void ClearVisualization();

	TSharedRef<ITableRow> MakeWaypointRow(
		TSharedPtr<int32> Item,
		const TSharedRef<STableViewBase>& Owner);

	// Button callbacks
	FReply OnAddAtCamera();
	FReply OnRemoveWaypoint(int32 Index);
	FReply OnMoveUp        (int32 Index);
	FReply OnMoveDown      (int32 Index);
	FReply OnClearAll();
	FReply OnVisualize();
};
