// © 2026 RadZib. All rights reserved.

#include "UI/SForgeDialogueEditor.h"
#include "UI/SForgeButton.h"
#include "Utils/ForgeUIStyle.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"
#include "Misc/FileHelper.h"

#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Layout/SSplitter.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SWidgetSwitcher.h"
#include "Widgets/Images/SImage.h"
#include "Styling/AppStyle.h"

#define LOCTEXT_NAMESPACE "ForgeDialogueEditor"

// ─── Construct ───────────────────────────────────────────────────────────────

void SForgeDialogueEditor::Construct(const FArguments& InArgs)
{
	FPropertyEditorModule& PEM =
		FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs Args;
	Args.bHideSelectionTip = true;
	Args.bAllowSearch      = false;
	Args.NameAreaSettings  = FDetailsViewArgs::HideNameArea;
	AssetDetailsView = PEM.CreateDetailView(Args);

	RefreshAssetList();

	ChildSlot
	[
		SNew(SSplitter).Orientation(Orient_Horizontal)

		// ── LEFT — Asset list + node tree ──
		+ SSplitter::Slot().Value(0.35f)
		[
			SNew(SVerticalBox)

			// Title + refresh
			+ SVerticalBox::Slot().AutoHeight().Padding(8.f, 6.f, 8.f, 2.f)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot().FillWidth(1.f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("AssetsTitle", "Dialogue Assets"))
					.Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
					.ColorAndOpacity(FSlateColor(FLinearColor(0.80f, 0.82f, 0.92f)))
				]
				+ SHorizontalBox::Slot().AutoWidth()
				[
					SNew(SForgeButton)
					.Preset(EForgeButtonPreset::Ghost)
					.ContentPadding(FMargin(4.f,0.f))
					.OnClicked(FOnClicked::CreateLambda([this]()->FReply{ RefreshAssetList(); return FReply::Handled(); }))
					[
						SNew(STextBlock).Text(LOCTEXT("Refresh","↺"))
						.Font(FCoreStyle::GetDefaultFontStyle("Regular",10))
						.ColorAndOpacity(FSlateColor(FLinearColor(0.5f,0.6f,0.9f)))
					]
				]
			]

			// Asset list
			+ SVerticalBox::Slot().AutoHeight().MaxHeight(140.f)
			[
				SAssignNew(AssetListView, SListView<TSharedPtr<FAssetData>>)
				.ListItemsSource(&AssetItems)
				.OnGenerateRow(this, &SForgeDialogueEditor::MakeAssetRow)
				.OnMouseButtonDoubleClick_Lambda([this](TSharedPtr<FAssetData> Item)
				{
					if (Item.IsValid()) OpenAsset(*Item);
				})
			]

			+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 4.f)
			[ SNew(SSeparator) ]

			// Node list header + buttons
			+ SVerticalBox::Slot().AutoHeight().Padding(8.f, 2.f)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot().FillWidth(1.f).VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("NodesTitle","Nodes"))
					.Font(FCoreStyle::GetDefaultFontStyle("Bold", 9))
					.ColorAndOpacity(FSlateColor(FLinearColor(0.65f,0.68f,0.78f)))
				]
				+ SHorizontalBox::Slot().AutoWidth().Padding(2.f,0.f)
				[
					SNew(SForgeButton)
					.Preset(EForgeButtonPreset::Confirm)
					.ContentPadding(FMargin(8.f,3.f))
					.Label(LOCTEXT("Add","+ Node"))
					.Bold(true)
					.FontSize(8)
					.OnClicked(this, &SForgeDialogueEditor::OnAddNode)
				]
				+ SHorizontalBox::Slot().AutoWidth().Padding(2.f,0.f)
				[
					SNew(SForgeButton)
					.Preset(EForgeButtonPreset::Danger)
					.ContentPadding(FMargin(8.f,3.f))
					.Label(LOCTEXT("Del","Delete"))
					.Bold(true)
					.FontSize(8)
					.OnClicked(this, &SForgeDialogueEditor::OnDeleteNode)
				]
			]

			// Node list
			+ SVerticalBox::Slot().FillHeight(1.f)
			[
				SAssignNew(NodeListView, SListView<TSharedPtr<int32>>)
				.ListItemsSource(&NodeIndices)
				.OnGenerateRow(this, &SForgeDialogueEditor::MakeNodeRow)
				.OnSelectionChanged(this, &SForgeDialogueEditor::OnNodeSelected)
			]

			// Save button
			+ SVerticalBox::Slot().AutoHeight().Padding(8.f, 4.f)
			[
				SNew(SForgeButton)
				.Preset(EForgeButtonPreset::Primary)
				.ContentPadding(FMargin(0.f,8.f))
				.HAlign(HAlign_Center)
				.Label(LOCTEXT("Save","💾  Save Dialogue Asset"))
				.Bold(true)
				.FontSize(10)
				.OnClicked(this, &SForgeDialogueEditor::OnSaveAsset)
			]
		]

		// ── RIGHT — SWidgetSwitcher: Edit | Preview ──
		+ SSplitter::Slot().Value(0.65f)
		[
			SNew(SVerticalBox)

			// ▶ Preview / ⏹ Stop toggle button (top of right panel)
			+ SVerticalBox::Slot().AutoHeight().Padding(8.f, 6.f, 8.f, 4.f)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot().FillWidth(1.f).VAlign(VAlign_Center)
				[
					SAssignNew(NodeEditorLabel, STextBlock)
					.Text(LOCTEXT("SelectHint","Double-click a Dialogue asset to start editing."))
					.Font(FCoreStyle::GetDefaultFontStyle("Italic",9))
					.ColorAndOpacity(FSlateColor(FLinearColor(0.45f,0.47f,0.58f)))
					.AutoWrapText(true)
				]
				+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
				[
					SAssignNew(PreviewToggleBtn, SButton)
					// Static style gives a hover response; Lambda drives the per-mode normal tint
					.ButtonStyle(&ForgeButtonStyle(
						FLinearColor(0.12f, 0.38f, 0.20f),   // normal (play/green)
						FLinearColor(0.20f, 0.55f, 0.30f)))  // hover (brighter green)
					.ButtonColorAndOpacity_Lambda([this]() -> FLinearColor
					{
						return bPreviewMode
							? FLinearColor(0.50f, 0.12f, 0.10f)   // red  = stop
							: FLinearColor(0.12f, 0.38f, 0.20f);  // green = play
					})
					.ContentPadding(FMargin(10.f, 4.f))
					.Visibility(EVisibility::Collapsed)   // shown once an asset is loaded
					.ToolTipText(LOCTEXT("PreviewTip", "Walk through the dialogue as a player would"))
					.OnClicked(this, &SForgeDialogueEditor::OnTogglePreview)
					[
						SAssignNew(PreviewToggleBtnLabel, STextBlock)
						.Text(LOCTEXT("PreviewBtn", "▶  Preview"))
						.Font(FCoreStyle::GetDefaultFontStyle("Bold", 9))
						.ColorAndOpacity(FSlateColor(FLinearColor::White))
					]
				]
			]

			// Panel switcher
			+ SVerticalBox::Slot().FillHeight(1.f)
			[
				SAssignNew(RightPanelSwitcher, SWidgetSwitcher)

				// Slot 0 — Edit panel
				+ SWidgetSwitcher::Slot()
				[ BuildEditPanel() ]

				// Slot 1 — Preview panel
				+ SWidgetSwitcher::Slot()
				[ BuildPreviewPanel() ]
			]
		]
	];
}

// ─── Panel builders ───────────────────────────────────────────────────────────

TSharedRef<SWidget> SForgeDialogueEditor::BuildEditPanel()
{
	return AssetDetailsView.ToSharedRef();
}

TSharedRef<SWidget> SForgeDialogueEditor::BuildPreviewPanel()
{
	return SNew(SScrollBox)

		// Speaker + emotion row
		+ SScrollBox::Slot().Padding(12.f, 10.f, 12.f, 4.f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().FillWidth(1.f).VAlign(VAlign_Center)
			[
				SAssignNew(PreviewSpeakerLabel, STextBlock)
				.Text(FText::GetEmpty())
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
				.ColorAndOpacity(FSlateColor(FLinearColor(0.80f, 0.82f, 0.95f)))
			]
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
			[
				SAssignNew(PreviewEmotionLabel, STextBlock)
				.Text(FText::GetEmpty())
				.Font(FCoreStyle::GetDefaultFontStyle("Italic", 8))
				.ColorAndOpacity(FSlateColor(FLinearColor(0.55f, 0.57f, 0.70f)))
			]
		]

		// Dialogue text box
		+ SScrollBox::Slot().Padding(12.f, 0.f, 12.f, 12.f)
		[
			SNew(SBorder)
			.BorderBackgroundColor(FLinearColor(0.06f, 0.08f, 0.14f))
			.Padding(FMargin(14.f, 12.f))
			[
				SAssignNew(PreviewNodeText, STextBlock)
				.Text(LOCTEXT("PreviewEmpty","—"))
				.AutoWrapText(true)
				.Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
				.ColorAndOpacity(FSlateColor(FLinearColor(0.88f, 0.90f, 0.95f)))
			]
		]

		// Response buttons (populated by RefreshPreviewView)
		+ SScrollBox::Slot().Padding(12.f, 0.f, 12.f, 8.f)
		[
			SAssignNew(PreviewResponseBox, SVerticalBox)
		]

		// Back button
		+ SScrollBox::Slot().Padding(12.f, 4.f, 12.f, 12.f)
		[
			SAssignNew(PreviewBackBtn, SButton)
			.ButtonStyle(&ForgeNavyBtn())
			.ContentPadding(FMargin(12.f, 6.f))
			.Visibility(EVisibility::Collapsed)
			.OnClicked(this, &SForgeDialogueEditor::OnPreviewBack)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("BackBtn", "← Back"))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 9))
				.ColorAndOpacity(FSlateColor(FLinearColor(0.60f, 0.70f, 0.90f)))
			]
		];
}

// ─── Asset loading ────────────────────────────────────────────────────────────

void SForgeDialogueEditor::OpenAsset(const FAssetData& Asset)
{
	LoadAsset(Cast<UForgeDialogueData>(Asset.GetAsset()));
}

void SForgeDialogueEditor::LoadAsset(UForgeDialogueData* Asset)
{
	// Exit preview if switching assets
	if (bPreviewMode) OnTogglePreview();

	DialogueAsset     = Asset;
	SelectedNodeIndex = INDEX_NONE;
	RebuildNodeList();

	if (AssetDetailsView.IsValid()) AssetDetailsView->SetObject(Asset);

	if (NodeEditorLabel.IsValid() && Asset)
	{
		NodeEditorLabel->SetText(FText::Format(
			LOCTEXT("EditingFmt","Editing: {0}  |  Entry: {1}  |  {2} nodes"),
			FText::FromName(Asset->DialogueId),
			FText::FromName(Asset->EntryNodeId),
			FText::AsNumber(Asset->Nodes.Num())));
	}

	// Show preview button once an asset is loaded
	if (PreviewToggleBtn.IsValid() && Asset)
	{
		PreviewToggleBtn->SetVisibility(EVisibility::Visible);
	}
}

void SForgeDialogueEditor::RefreshAssetList()
{
	AssetItems.Empty();
	IAssetRegistry& AR = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
	FARFilter F;
	F.bRecursivePaths = true;
	F.PackagePaths.Add(FName("/Game"));
	F.ClassPaths.Add(UForgeDialogueData::StaticClass()->GetClassPathName());
	TArray<FAssetData> Found;
	AR.GetAssets(F, Found);
	for (const FAssetData& AD : Found)
		AssetItems.Add(MakeShared<FAssetData>(AD));
	if (AssetListView.IsValid()) AssetListView->RequestListRefresh();
}

void SForgeDialogueEditor::RebuildNodeList()
{
	NodeIndices.Empty();
	if (DialogueAsset)
	{
		for (int32 i = 0; i < DialogueAsset->Nodes.Num(); ++i)
			NodeIndices.Add(MakeShared<int32>(i));
	}
	if (NodeListView.IsValid()) NodeListView->RequestListRefresh();
}

// ─── Row builders ─────────────────────────────────────────────────────────────

TSharedRef<ITableRow> SForgeDialogueEditor::MakeAssetRow(
	TSharedPtr<FAssetData> Item, const TSharedRef<STableViewBase>& Owner)
{
	return SNew(STableRow<TSharedPtr<FAssetData>>, Owner).Padding(FMargin(6.f,3.f))
	[
		SNew(STextBlock)
		.Text(FText::FromName(Item->AssetName))
		.Font(FCoreStyle::GetDefaultFontStyle("Regular",9))
		.ColorAndOpacity(FSlateColor(FLinearColor(0.75f,0.77f,0.90f)))
	];
}

TSharedRef<ITableRow> SForgeDialogueEditor::MakeNodeRow(
	TSharedPtr<int32> Item, const TSharedRef<STableViewBase>& Owner)
{
	if (!DialogueAsset || !Item.IsValid() || !DialogueAsset->Nodes.IsValidIndex(*Item))
	{
		return SNew(STableRow<TSharedPtr<int32>>, Owner);
	}

	const FForgeDialogueNode& Node = DialogueAsset->Nodes[*Item];
	const bool bIsEntry   = (Node.NodeId == DialogueAsset->EntryNodeId);
	const bool bIsPreview = (bPreviewMode && PreviewNodeIndex == *Item);
	const FLinearColor EmColor = EmotionColor(Node.Emotion);

	return SNew(STableRow<TSharedPtr<int32>>, Owner).Padding(FMargin(4.f, 2.f))
	[
		SNew(SBorder)
		.BorderBackgroundColor(bIsPreview
			? FLinearColor(0.15f, 0.38f, 0.55f, 0.5f)
			: FLinearColor(0.f, 0.f, 0.f, 0.f))
		.Padding(FMargin(2.f, 1.f))
		[
			SNew(SHorizontalBox)

			// Entry marker
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(0.f,0.f,4.f,0.f)
			[
				SNew(STextBlock)
				.Text(bIsEntry ? LOCTEXT("Entry","▶") : FText::GetEmpty())
				.Font(FCoreStyle::GetDefaultFontStyle("Bold",9))
				.ColorAndOpacity(FSlateColor(FLinearColor(0.3f,0.9f,0.4f)))
			]

			// Emotion dot
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(0.f,0.f,6.f,0.f)
			[
				SNew(SBox).WidthOverride(8.f).HeightOverride(8.f)
				[
					SNew(SImage)
					.Image(FAppStyle::Get().GetBrush("Icons.BulletPoint"))
					.ColorAndOpacity(FSlateColor(EmColor))
				]
			]

			// Node ID + text preview
			+ SHorizontalBox::Slot().FillWidth(1.f).VAlign(VAlign_Center)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot().AutoHeight()
				[
					SNew(STextBlock)
					.Text(FText::FromName(Node.NodeId))
					.Font(FCoreStyle::GetDefaultFontStyle("Bold",8))
					.ColorAndOpacity(FSlateColor(FLinearColor(0.82f,0.84f,0.95f)))
				]
				+ SVerticalBox::Slot().AutoHeight()
				[
					SNew(STextBlock)
					.Text(Node.Text)
					.Font(FCoreStyle::GetDefaultFontStyle("Italic",8))
					.ColorAndOpacity(FSlateColor(FLinearColor(0.50f,0.52f,0.62f)))
					.OverflowPolicy(ETextOverflowPolicy::Ellipsis)
				]
			]

			// Response count badge
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(4.f,0.f,0.f,0.f)
			[
				SNew(STextBlock)
				.Text(FText::AsNumber(Node.ResponseNodeIds.Num()))
				.Font(FCoreStyle::GetDefaultFontStyle("Mono",8))
				.ColorAndOpacity(FSlateColor(FLinearColor(0.40f,0.60f,0.90f)))
				.ToolTipText(LOCTEXT("RespTip","Response choices"))
			]
		]
	];
}

// ─── Selection ───────────────────────────────────────────────────────────────

void SForgeDialogueEditor::OnNodeSelected(TSharedPtr<int32> Item, ESelectInfo::Type)
{
	SelectedNodeIndex = (Item.IsValid()) ? *Item : INDEX_NONE;
}

// ─── Edit mode actions ────────────────────────────────────────────────────────

FReply SForgeDialogueEditor::OnAddNode()
{
	if (!DialogueAsset) return FReply::Handled();

	DialogueAsset->Modify();
	FForgeDialogueNode& New = DialogueAsset->Nodes.AddDefaulted_GetRef();
	New.NodeId = FName(*FString::Printf(TEXT("Node_%d"), DialogueAsset->Nodes.Num()));
	New.Text   = LOCTEXT("NewNodeText","...");

	if (DialogueAsset->EntryNodeId.IsNone())
		DialogueAsset->EntryNodeId = New.NodeId;

	RebuildNodeList();
	if (AssetDetailsView.IsValid()) AssetDetailsView->ForceRefresh();
	return FReply::Handled();
}

FReply SForgeDialogueEditor::OnDeleteNode()
{
	if (!DialogueAsset || !DialogueAsset->Nodes.IsValidIndex(SelectedNodeIndex))
		return FReply::Handled();

	DialogueAsset->Modify();
	DialogueAsset->Nodes.RemoveAt(SelectedNodeIndex);
	SelectedNodeIndex = INDEX_NONE;

	RebuildNodeList();
	if (AssetDetailsView.IsValid()) AssetDetailsView->ForceRefresh();
	return FReply::Handled();
}

FReply SForgeDialogueEditor::OnSaveAsset()
{
	if (!DialogueAsset) return FReply::Handled();
	DialogueAsset->MarkPackageDirty();
	{
		FSavePackageArgs SaveArgs;
		SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
		SaveArgs.Error         = GWarn;
		UPackage::SavePackage(
			DialogueAsset->GetPackage(),
			DialogueAsset,
			*FPackageName::LongPackageNameToFilename(
				DialogueAsset->GetPackage()->GetName(),
				FPackageName::GetAssetPackageExtension()),
			SaveArgs);
	}
	return FReply::Handled();
}

// ─── Preview mode ─────────────────────────────────────────────────────────────

FReply SForgeDialogueEditor::OnTogglePreview()
{
	if (!DialogueAsset || DialogueAsset->Nodes.IsEmpty()) return FReply::Handled();

	bPreviewMode = !bPreviewMode;

	// Update button appearance — color comes from ButtonColorAndOpacity_Lambda,
	// so Invalidate(Paint) is enough; we just need to update the stored text widget.
	if (PreviewToggleBtnLabel.IsValid())
	{
		PreviewToggleBtnLabel->SetText(bPreviewMode
			? LOCTEXT("StopBtn",    "⏹  Stop Preview")
			: LOCTEXT("PreviewBtn", "▶  Preview"));
	}
	if (PreviewToggleBtn.IsValid())
	{
		PreviewToggleBtn->Invalidate(EInvalidateWidgetReason::Paint);
	}

	if (bPreviewMode)
	{
		PreviewHistory.Empty();
		// Start at entry node
		const int32 EntryIdx = FindNodeIndex(DialogueAsset->EntryNodeId);
		PreviewNavigateTo(EntryIdx >= 0 ? EntryIdx : 0);
		if (RightPanelSwitcher.IsValid()) RightPanelSwitcher->SetActiveWidgetIndex(1);
	}
	else
	{
		PreviewNodeIndex = INDEX_NONE;
		if (RightPanelSwitcher.IsValid()) RightPanelSwitcher->SetActiveWidgetIndex(0);
	}

	// Rebuild node list to update highlight
	RebuildNodeList();
	return FReply::Handled();
}

FReply SForgeDialogueEditor::OnPreviewBack()
{
	if (PreviewHistory.Num() > 0)
	{
		const int32 Prev = PreviewHistory.Pop();
		PreviewNodeIndex = Prev;
		RefreshPreviewView();
		RebuildNodeList();
	}
	return FReply::Handled();
}

void SForgeDialogueEditor::PreviewNavigateTo(int32 NodeIndex)
{
	if (PreviewNodeIndex != INDEX_NONE)
	{
		PreviewHistory.Push(PreviewNodeIndex);
	}
	PreviewNodeIndex = NodeIndex;
	RefreshPreviewView();
	RebuildNodeList();

	// Scroll the node list to show the current node
	if (NodeListView.IsValid() && NodeIndices.IsValidIndex(NodeIndex))
	{
		NodeListView->RequestScrollIntoView(NodeIndices[NodeIndex]);
	}
}

void SForgeDialogueEditor::RefreshPreviewView()
{
	if (!DialogueAsset || !DialogueAsset->Nodes.IsValidIndex(PreviewNodeIndex)) return;

	const FForgeDialogueNode& Node = DialogueAsset->Nodes[PreviewNodeIndex];

	// Speaker (use Profile display name if node references one, otherwise use dialogue ID)
	if (PreviewSpeakerLabel.IsValid())
	{
		PreviewSpeakerLabel->SetText(FText::FromName(DialogueAsset->DialogueId));
	}

	// Emotion badge
	if (PreviewEmotionLabel.IsValid())
	{
		const FLinearColor EmCol = EmotionColor(Node.Emotion);
		PreviewEmotionLabel->SetText(EmotionLabel(Node.Emotion));
		PreviewEmotionLabel->SetColorAndOpacity(FSlateColor(EmCol));
	}

	// Node text
	if (PreviewNodeText.IsValid())
	{
		PreviewNodeText->SetText(Node.Text);
	}

	// Response buttons
	if (PreviewResponseBox.IsValid())
	{
		PreviewResponseBox->ClearChildren();

		if (Node.ResponseNodeIds.IsEmpty())
		{
			// End of conversation
			PreviewResponseBox->AddSlot().AutoHeight().Padding(0.f, 4.f)
			[
				SNew(SBorder)
				.BorderBackgroundColor(FLinearColor(0.10f, 0.12f, 0.20f))
				.Padding(FMargin(12.f, 8.f))
				[
					SNew(STextBlock)
					.Text(LOCTEXT("EndConvo", "— End of Dialogue —"))
					.Font(FCoreStyle::GetDefaultFontStyle("Italic", 9))
					.Justification(ETextJustify::Center)
					.ColorAndOpacity(FSlateColor(FLinearColor(0.45f, 0.47f, 0.60f)))
				]
			];
		}
		else
		{
			for (int32 r = 0; r < Node.ResponseNodeIds.Num(); ++r)
			{
				const FName ResponseId  = Node.ResponseNodeIds[r];
				const int32 TargetIndex = FindNodeIndex(ResponseId);
				const FText BtnText     = (TargetIndex >= 0)
					? FText::Format(LOCTEXT("RespFmt", "→  {0}"),
						DialogueAsset->Nodes[TargetIndex].Text)
					: FText::Format(LOCTEXT("RespMissing", "→  [{0}] (not found)"),
						FText::FromName(ResponseId));

				PreviewResponseBox->AddSlot().AutoHeight().Padding(0.f, 3.f)
				[
					SNew(SForgeButton)
					.Preset(EForgeButtonPreset::Navy)
					.ContentPadding(FMargin(12.f, 7.f))
					.OnClicked(FOnClicked::CreateLambda([this, TargetIndex]() -> FReply
					{
						if (TargetIndex >= 0) PreviewNavigateTo(TargetIndex);
						return FReply::Handled();
					}))
					[
						SNew(STextBlock)
						.Text(BtnText)
						.AutoWrapText(true)
						.Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
						.ColorAndOpacity(FSlateColor(FLinearColor(0.78f, 0.82f, 0.95f)))
					]
				];
			}
		}
	}

	// Show Back button only if there's history
	if (PreviewBackBtn.IsValid())
	{
		PreviewBackBtn->SetVisibility(
			PreviewHistory.IsEmpty() ? EVisibility::Collapsed : EVisibility::Visible);
	}
}

int32 SForgeDialogueEditor::FindNodeIndex(FName NodeId) const
{
	if (!DialogueAsset) return INDEX_NONE;
	for (int32 i = 0; i < DialogueAsset->Nodes.Num(); ++i)
	{
		if (DialogueAsset->Nodes[i].NodeId == NodeId) return i;
	}
	return INDEX_NONE;
}

// ─── Helpers ─────────────────────────────────────────────────────────────────

FText SForgeDialogueEditor::EmotionLabel(EForgeDialogueEmotion E)
{
	switch (E)
	{
	case EForgeDialogueEmotion::Whisper:   return LOCTEXT("Whisper","Whisper");
	case EForgeDialogueEmotion::Shout:     return LOCTEXT("Shout","Shout");
	case EForgeDialogueEmotion::Sarcastic: return LOCTEXT("Sarcastic","Sarcastic");
	case EForgeDialogueEmotion::Fearful:   return LOCTEXT("Fearful","Fearful");
	case EForgeDialogueEmotion::Joyful:    return LOCTEXT("Joyful","Joyful");
	default:                               return LOCTEXT("Neutral","Neutral");
	}
}

FLinearColor SForgeDialogueEditor::EmotionColor(EForgeDialogueEmotion E)
{
	switch (E)
	{
	case EForgeDialogueEmotion::Whisper:   return FLinearColor(0.40f, 0.55f, 0.80f);
	case EForgeDialogueEmotion::Shout:     return FLinearColor(0.90f, 0.30f, 0.20f);
	case EForgeDialogueEmotion::Sarcastic: return FLinearColor(0.80f, 0.72f, 0.15f);
	case EForgeDialogueEmotion::Fearful:   return FLinearColor(0.70f, 0.35f, 0.80f);
	case EForgeDialogueEmotion::Joyful:    return FLinearColor(0.25f, 0.82f, 0.35f);
	default:                               return FLinearColor(0.55f, 0.57f, 0.65f);
	}
}

#undef LOCTEXT_NAMESPACE
