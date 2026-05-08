// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "AssetRegistry/AssetData.h"
#include "Data/ForgeTypes.h"

class UForgeQuestData;
class IDetailsView;
class SForgeQuestCanvas;   // forward-declared; defined in .cpp

// Quest state flowchart + property editor.
// Shows Inactive → Active → InProgress → [Completed | Failed] as painted boxes.
// Canvas boxes are clickable — clicking a box sets SimulatedState.
// Below the canvas: objectives checklist + reward summary.
class FORGEEDITOR_API SForgeQuestVisualizer : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SForgeQuestVisualizer) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	void OpenAsset(const FAssetData& Asset);

	// Current simulated state — read AND written by the inner canvas widget
	EForgeQuestState SimulatedState = EForgeQuestState::Inactive;

	// Called by SForgeQuestCanvas (same .cpp), must be public
	static FLinearColor StateColor(EForgeQuestState S);
	static FText        StateLabel(EForgeQuestState S);

private:
	TObjectPtr<UForgeQuestData>          QuestAsset;

	TSharedPtr<SForgeQuestCanvas>        FlowChart;
	TArray<TSharedPtr<FAssetData>>       AssetItems;
	TSharedPtr<SListView<TSharedPtr<FAssetData>>> AssetListView;
	TSharedPtr<IDetailsView>             DetailsView;
	TSharedPtr<STextBlock>               QuestInfoLabel;

	// Objectives sub-panel
	TSharedPtr<SVerticalBox>             ObjectivesContainer;
	TSharedPtr<STextBlock>               RewardLabel;

	// State simulation controls
	TSharedPtr<SHorizontalBox>           SimButtonRow;

	void RefreshAssetList();
	void RebuildObjectivesPanel();

	// State stepper buttons
	FReply OnSimActivate();
	FReply OnSimProgress();
	FReply OnSimComplete();
	FReply OnSimFail();
	FReply OnSimReset();
	void   SetSimState(EForgeQuestState NewState);

	TSharedRef<ITableRow> MakeAssetRow(
		TSharedPtr<FAssetData> Item, const TSharedRef<STableViewBase>& Owner);

	TSharedRef<SWidget> BuildSimControls();
	TSharedRef<SWidget> BuildObjectivesSection();
};
