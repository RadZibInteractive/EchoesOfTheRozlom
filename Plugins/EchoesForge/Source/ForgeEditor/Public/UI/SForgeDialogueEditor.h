// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "AssetRegistry/AssetData.h"
#include "Data/ForgeDialogueData.h"

class IDetailsView;
class UForgeDialogueData;
class SWidgetSwitcher;

// Dialogue tree editor for UForgeDialogueData assets.
// Left: asset list + node list.
// Right: property editor (Edit mode) or click-through conversation (Preview mode).
class FORGEEDITOR_API SForgeDialogueEditor : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SForgeDialogueEditor) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	// Called from Profile Browser double-click
	void OpenAsset(const FAssetData& Asset);

private:
	// ── Asset / node data ────────────────────────────────────────────────────
	TObjectPtr<UForgeDialogueData>               DialogueAsset;
	int32                                        SelectedNodeIndex = INDEX_NONE;

	TSharedPtr<SListView<TSharedPtr<int32>>>     NodeListView;
	TArray<TSharedPtr<int32>>                    NodeIndices;

	TSharedPtr<IDetailsView>                     AssetDetailsView;
	TSharedPtr<STextBlock>                       NodeEditorLabel;
	TSharedPtr<SVerticalBox>                     NodeEditorBox;

	TArray<TSharedPtr<FAssetData>>               AssetItems;
	TSharedPtr<SListView<TSharedPtr<FAssetData>>> AssetListView;

	// ── Preview mode ─────────────────────────────────────────────────────────
	bool          bPreviewMode     = false;
	int32         PreviewNodeIndex = INDEX_NONE;
	TArray<int32> PreviewHistory;

	TSharedPtr<SWidgetSwitcher> RightPanelSwitcher;   // 0 = Edit, 1 = Preview
	TSharedPtr<STextBlock>      PreviewSpeakerLabel;
	TSharedPtr<STextBlock>      PreviewNodeText;
	TSharedPtr<SVerticalBox>    PreviewResponseBox;
	TSharedPtr<STextBlock>      PreviewEmotionLabel;
	TSharedPtr<SButton>         PreviewBackBtn;
	TSharedPtr<SButton>         PreviewToggleBtn;      // "▶ Preview" / "⏹ Stop"
	TSharedPtr<STextBlock>      PreviewToggleBtnLabel; // label inside the toggle button

	// ── Build helpers ────────────────────────────────────────────────────────
	TSharedRef<SWidget> BuildEditPanel();
	TSharedRef<SWidget> BuildPreviewPanel();

	// ── Asset loading ────────────────────────────────────────────────────────
	void RefreshAssetList();
	void LoadAsset(UForgeDialogueData* Asset);
	void RebuildNodeList();
	void OnNodeSelected(TSharedPtr<int32> Item, ESelectInfo::Type Reason);

	TSharedRef<ITableRow> MakeAssetRow(
		TSharedPtr<FAssetData> Item, const TSharedRef<STableViewBase>& Owner);
	TSharedRef<ITableRow> MakeNodeRow(
		TSharedPtr<int32> Item, const TSharedRef<STableViewBase>& Owner);

	// ── Edit mode actions ────────────────────────────────────────────────────
	FReply OnAddNode();
	FReply OnDeleteNode();
	FReply OnSaveAsset();

	// ── Preview mode actions ─────────────────────────────────────────────────
	FReply OnTogglePreview();
	FReply OnPreviewBack();
	void   PreviewNavigateTo(int32 NodeIndex);
	void   RefreshPreviewView();
	int32  FindNodeIndex(FName NodeId) const;

	// ── Helpers ──────────────────────────────────────────────────────────────
	static FText        EmotionLabel(EForgeDialogueEmotion E);
	static FLinearColor EmotionColor(EForgeDialogueEmotion E);
};
