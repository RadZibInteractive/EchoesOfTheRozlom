// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "AssetRegistry/AssetData.h"

UENUM()
enum class EForgeSidebarState : uint8
{
	Initialization,
	ActivitySelection,
	NPCSetup,
	AnomalySetup,
	EnemySetup,
	PropsSetup,
};

DECLARE_DELEGATE_OneParam(FOnActivitySelected,        EForgeSidebarState);
DECLARE_DELEGATE_OneParam(FOnForgeToggle,             bool);
DECLARE_DELEGATE_OneParam(FOnForgeZoneChanged,        const FString& /* ZoneName */);
DECLARE_DELEGATE_OneParam(FOnForgeDialogueRequested,  const FAssetData& /* Asset */);
DECLARE_DELEGATE_OneParam(FOnForgeQuestRequested,     const FAssetData& /* Asset */);
DECLARE_DELEGATE_OneParam(FOnForgeActorPlaced,        AActor* /* PlacedActor */);

struct FForgeActivityEntry
{
	EForgeSidebarState State;
	FText              Label;
	FLinearColor       Color;
	FName              IconBrushName;
};

class SForgeActivityPanel;
class SForgeProfileBrowser;
class SForgeActorInspector;
class SWidgetSwitcher;

// Left sidebar with three tabs:
//   Place   — Zone picker + AI context + 2×2 activity grid + config panel + inspector
//   Browse  — Profile Browser (NPC / Anomaly / Dialogue / Quest assets)
//   Inspect — Actor Inspector (selected actor properties)
class FORGEEDITOR_API SForgeContextSidebar : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SForgeContextSidebar)
		: _bGhostModeActive(false)
	{}
		SLATE_ATTRIBUTE(bool, bGhostModeActive)
		SLATE_EVENT(FOnActivitySelected,       OnActivitySelected)
		SLATE_EVENT(FOnForgeToggle,            OnGhostModeToggled)
		SLATE_EVENT(FOnForgeZoneChanged,       OnZoneChanged)
		SLATE_EVENT(FOnForgeDialogueRequested, OnDialogueRequested)
		SLATE_EVENT(FOnForgeQuestRequested,    OnQuestRequested)
		SLATE_EVENT(FOnForgeActorPlaced,       OnActorPlaced)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	void SetGhostModeActive(bool bActive);
	void SetAIContextText(const FText& Text);
	void SetZoneName(const FString& ZoneName);

	// Logic Inspector live variable feed
	void SetInspectorVariable(const FName& VarName, const FString& Value);
	void ClearInspectorVariables();

	FString GetCurrentZoneName() const;

private:
	TAttribute<bool>           bGhostModeActive;
	FOnActivitySelected        OnActivitySelected;
	FOnForgeToggle             OnGhostModeToggled;
	FOnForgeZoneChanged        OnZoneChanged;
	FOnForgeDialogueRequested  OnDialogueRequested;
	FOnForgeQuestRequested     OnQuestRequested;
	FOnForgeActorPlaced        OnActorPlaced;

	// Sidebar tab switcher (Place | Browse | Inspect)
	TSharedPtr<SWidgetSwitcher>       SidebarSwitcher;
	int32                             ActiveSidebarTab = 0;

	// Tab buttons for highlight
	TSharedPtr<SButton>  TabBtnPlace;
	TSharedPtr<SButton>  TabBtnBrowse;
	TSharedPtr<SButton>  TabBtnInspect;

	// Zone picker
	TArray<TSharedPtr<FString>>            ZoneNames;
	TSharedPtr<SComboBox<TSharedPtr<FString>>> ZoneComboBox;
	TSharedPtr<FString>                    SelectedZone;

	TSharedPtr<STextBlock>              AIContextBlock;
	TSharedPtr<SVerticalBox>            InspectorVarList;
	TSharedPtr<SBorder>                 GhostModeBorder;
	TSharedPtr<SForgeActivityPanel>     ActivityPanel;
	TSharedPtr<SForgeProfileBrowser>    ProfileBrowser;
	TSharedPtr<SForgeActorInspector>    ActorInspector;

	bool bGhostActive       = false;
	bool bInspectorExpanded = true;

	TMap<FName, TSharedPtr<STextBlock>> InspectorVarWidgets;

	// --- Build helpers ---
	void SwitchSidebarTab(int32 Index);
	TSharedRef<SWidget> BuildSidebarTabBar();
	TSharedRef<SWidget> BuildPlaceTab();
	TSharedRef<SWidget> BuildBrowseTab();
	TSharedRef<SWidget> BuildInspectTab();

	TSharedRef<SWidget> BuildZoneSection();
	TSharedRef<SWidget> BuildActivityGrid();
	TSharedRef<SWidget> BuildActivityButton(const FForgeActivityEntry& Entry);
	TSharedRef<SWidget> BuildGhostModeBlock();
	TSharedRef<SWidget> BuildLogicInspector();

	void RefreshZoneList();
	void OnZoneSelected(TSharedPtr<FString> Item, ESelectInfo::Type Reason);

	FReply OnActivityClicked(EForgeSidebarState Activity);
	FReply OnGhostModeClicked();

	static const TArray<FForgeActivityEntry>& GetActivityEntries();
};
