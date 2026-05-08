// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "UI/SForgeContextSidebar.h"
#include "AssetRegistry/AssetData.h"

class SForgeTopToolbar;
class SForgeViewport;
class SForgeSmartPopup;
class SForgeValidationBar;
class SForgeDialogueEditor;
class SForgeQuestVisualizer;
class SForgeZoneStats;
class SWidgetSwitcher;

// Root layout widget for the Echoes Forge editor tab.
// Owns all sub-panels and routes delegate callbacks between them.
class FORGEEDITOR_API SForgeMainPanel : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SForgeMainPanel) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	TSharedPtr<SForgeTopToolbar>      Toolbar;
	TSharedPtr<SForgeContextSidebar>  Sidebar;
	TSharedPtr<SForgeViewport>        Viewport;
	TSharedPtr<SForgeSmartPopup>      SmartPopup;
	TSharedPtr<SForgeValidationBar>   ValidationBar;

	// Center tab system
	TSharedPtr<SWidgetSwitcher>       CenterSwitcher;
	TSharedPtr<SForgeDialogueEditor>  DialogueEditor;
	TSharedPtr<SForgeQuestVisualizer> QuestVisualizer;
	TSharedPtr<SForgeZoneStats>       ZoneStats;
	int32                             ActiveCenterTab = 0;

	// Tab buttons for visual highlight
	TSharedPtr<SButton>  TabBtnViewport;
	TSharedPtr<SButton>  TabBtnDialogue;
	TSharedPtr<SButton>  TabBtnQuest;
	TSharedPtr<SButton>  TabBtnStats;

	bool bGhostModeActive = false;
	bool bSimulateActive  = false;

	// Center tab switching
	void SwitchCenterTab(int32 Index);
	TSharedRef<SWidget> BuildCenterTabBar();

	// Toolbar callbacks
	void OnSimulateClicked();
	void OnGhostModeToggled(bool bIsActive);
	void OnValidateClicked();
	void OnPublishClicked();
	void OnSaveClicked();
	void OnUndoClicked();
	void OnRedoClicked();

	// Sidebar callbacks
	void OnActivitySelected(EForgeSidebarState Activity);
	void OnZoneChanged(const FString& ZoneName);
	void OnDialogueRequested(const FAssetData& Asset);
	void OnQuestRequested(const FAssetData& Asset);
	void OnActorPlaced(AActor* PlacedActor);

	// Smart Popup callbacks
	void OnIdeaSubmitted(const FString& Idea);
	void OnSuggestionPicked(int32 Index);
};
