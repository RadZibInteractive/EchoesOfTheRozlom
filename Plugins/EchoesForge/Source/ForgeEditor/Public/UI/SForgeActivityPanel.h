// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/SForgePanel.h"
#include "UI/SForgeButton.h"
#include "UI/SForgeContextSidebar.h"  // EForgeSidebarState

class AActor;
DECLARE_DELEGATE_OneParam(FOnForgeActorPlaced, AActor* /* PlacedActor */);

// ─── SForgeActivityPanel ──────────────────────────────────────────────────────
//
// Configuration panel shown when an activity grid button is clicked.
// Inherits SForgePanel (Template Method):
//
//  GetPanelTitle()  → dynamic ("Add NPC", "Add Anomaly", …)
//  BuildContent()   → data-asset picker row + per-type parameter widgets
//  BuildFooter()    → [▶ Place in World]  [📦 Create Starter Assets]
//
// Public API:
//  SetActivityType(EForgeSidebarState)  — switch modes without rebuilding slate

class FORGEEDITOR_API SForgeActivityPanel : public SForgePanel
{
public:
	SLATE_BEGIN_ARGS(SForgeActivityPanel)
		: _ActivityType(EForgeSidebarState::NPCSetup)
	{}
		SLATE_ARGUMENT(EForgeSidebarState, ActivityType)
		SLATE_EVENT  (FOnForgeActorPlaced, OnActorPlaced)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	/** Switch the active activity type and rebuild the form content. */
	void SetActivityType(EForgeSidebarState NewType);

protected:
	// ── SForgePanel overrides ─────────────────────────────────────────────────

	virtual FText        GetPanelTitle() const override;
	virtual TSharedRef<SWidget> BuildContent()  override;
	virtual TSharedRef<SWidget> BuildFooter()   override;

private:
	// ── State ────────────────────────────────────────────────────────────────

	EForgeSidebarState  ActivityType;
	FOnForgeActorPlaced OnActorPlaced;

	FString             SelectedAssetPath;
	int32               SpawnCount      = 1;
	float               RadiusOverride  = 0.f;
	bool                bIsInteractable = false;

	TSharedPtr<STextBlock>   AssetNameLabel;
	TSharedPtr<SVerticalBox> FormContainer;

	// ── Form ─────────────────────────────────────────────────────────────────

	void RebuildForm();

	TSharedRef<SWidget> BuildAssetRow();
	TSharedRef<SWidget> BuildNPCExtras();
	TSharedRef<SWidget> BuildAnomalyExtras();
	TSharedRef<SWidget> BuildEnemyExtras();
	TSharedRef<SWidget> BuildPropsExtras();

	// ── Callbacks ────────────────────────────────────────────────────────────

	FReply OnBrowseAsset();
	FReply OnPlaceActor();

	// ── Utilities ────────────────────────────────────────────────────────────

	static FString ActivityLabel(EForgeSidebarState Type);
	static UClass* ProfileClassFor(EForgeSidebarState Type);
	static FVector ViewportCameraLocation();
};
