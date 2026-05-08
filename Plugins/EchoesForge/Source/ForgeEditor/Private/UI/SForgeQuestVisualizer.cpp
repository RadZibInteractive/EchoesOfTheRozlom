// © 2026 RadZib. All rights reserved.

#include "UI/SForgeQuestVisualizer.h"
#include "UI/SForgeButton.h"
#include "Data/ForgeQuestData.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Layout/SSplitter.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SLeafWidget.h"
#include "Rendering/DrawElements.h"
#include "Styling/AppStyle.h"

#define LOCTEXT_NAMESPACE "ForgeQuestVisualizer"

// ─── State config ─────────────────────────────────────────────────────────────

struct FQuestStateBox
{
	EForgeQuestState State;
	FVector2D        Position;  // normalized 0..1 within chart area
};

static const FQuestStateBox StateBoxes[] =
{
	{ EForgeQuestState::Inactive,    { 0.02f, 0.50f } },
	{ EForgeQuestState::Active,      { 0.24f, 0.50f } },
	{ EForgeQuestState::InProgress,  { 0.50f, 0.50f } },
	{ EForgeQuestState::Completed,   { 0.76f, 0.20f } },
	{ EForgeQuestState::Failed,      { 0.76f, 0.80f } },
};
static constexpr int32 kNumStateBoxes = UE_ARRAY_COUNT(StateBoxes);

static const int32 ArrowPairs[][2] =
{
	{ 0, 1 }, { 1, 2 }, { 2, 3 }, { 2, 4 }
};

// ─── Inner canvas leaf widget ─────────────────────────────────────────────────
// Separate SLeafWidget so it never adds itself as its own child (avoids crash).
// Mouse clicks on a box advance SimulatedState on the parent.

class SForgeQuestCanvas : public SLeafWidget
{
public:
	SLATE_BEGIN_ARGS(SForgeQuestCanvas) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs) {}

	// Non-const — OnMouseButtonDown writes SimulatedState through this pointer.
	// Lifetime: the canvas is always destroyed before the parent visualizer.
	SForgeQuestVisualizer* Visualizer = nullptr;

	virtual FVector2D ComputeDesiredSize(float) const override
	{
		return FVector2D(400.f, 110.f);
	}

	// ── Hit test: return which state box (if any) contains LocalPt ────────────
	int32 HitTestBox(const FVector2D& LocalPt, const FVector2D& Size) const
	{
		static const FVector2D BoxSize(80.f, 36.f);
		for (int32 i = 0; i < kNumStateBoxes; ++i)
		{
			const FVector2D Center(StateBoxes[i].Position.X * Size.X,
				StateBoxes[i].Position.Y * Size.Y);
			const FVector2D TL = Center - BoxSize * 0.5f;
			const FVector2D BR = TL + BoxSize;
			if (LocalPt.X >= TL.X && LocalPt.X <= BR.X &&
				LocalPt.Y >= TL.Y && LocalPt.Y <= BR.Y)
			{
				return i;
			}
		}
		return INDEX_NONE;
	}

	virtual FReply OnMouseButtonDown(
		const FGeometry& MyGeometry,
		const FPointerEvent& MouseEvent) override
	{
		if (!Visualizer || MouseEvent.GetEffectingButton() != EKeys::LeftMouseButton)
			return FReply::Unhandled();

		const FVector2D LocalPt  = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
		const FVector2D Size     = MyGeometry.GetLocalSize();
		const int32     HitIndex = HitTestBox(LocalPt, Size);

		if (HitIndex != INDEX_NONE)
		{
			Visualizer->SimulatedState = StateBoxes[HitIndex].State;
			Invalidate(EInvalidateWidgetReason::Paint);
			return FReply::Handled();
		}
		return FReply::Unhandled();
	}

	virtual int32 OnPaint(
		const FPaintArgs& Args,
		const FGeometry& AllottedGeometry,
		const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements,
		int32 LayerId,
		const FWidgetStyle& InWidgetStyle,
		bool bParentEnabled) const override
	{
		if (!Visualizer) return LayerId;

		const EForgeQuestState CurState = Visualizer->SimulatedState;
		const FVector2D Size    = AllottedGeometry.GetLocalSize();
		const FVector2D BoxSize(80.f, 36.f);

		auto BoxCenter = [&](const FQuestStateBox& B) -> FVector2D
		{
			return FVector2D(B.Position.X * Size.X, B.Position.Y * Size.Y);
		};

		// Draw arrows first (behind boxes)
		for (const auto& Pair : ArrowPairs)
		{
			const FVector2D From = BoxCenter(StateBoxes[Pair[0]]);
			const FVector2D To   = BoxCenter(StateBoxes[Pair[1]]);

			TArray<FVector2D> Points = {
				From + FVector2D(BoxSize.X * 0.5f, 0.f),
				To   - FVector2D(BoxSize.X * 0.5f, 0.f)
			};

			const bool bActivePath =
				(StateBoxes[Pair[0]].State < CurState) ||
				(StateBoxes[Pair[0]].State == CurState);

			FSlateDrawElement::MakeLines(OutDrawElements, LayerId,
				AllottedGeometry.ToPaintGeometry(),
				Points,
				ESlateDrawEffect::None,
				bActivePath
					? FLinearColor(0.20f, 0.62f, 0.85f)   // brand cyan
					: FLinearColor(0.15f, 0.20f, 0.30f),
				true, bActivePath ? 2.f : 1.f);
		}

		// Draw state boxes
		for (const FQuestStateBox& B : StateBoxes)
		{
			const FVector2D Center = BoxCenter(B);
			const FVector2D TL     = Center - BoxSize * 0.5f;
			const bool      bActive = (B.State == CurState);
			const FLinearColor BoxColor = bActive
				? SForgeQuestVisualizer::StateColor(B.State)
				: SForgeQuestVisualizer::StateColor(B.State) * 0.35f;

			// Box background
			FSlateDrawElement::MakeBox(OutDrawElements, LayerId,
				AllottedGeometry.ToPaintGeometry(BoxSize, FSlateLayoutTransform(TL)),
				FAppStyle::Get().GetBrush("WhiteBrush"),
				ESlateDrawEffect::None, BoxColor);

			// Hover hint: thin bright border on active box
			if (bActive)
			{
				// Slightly larger box for the border effect
				const FVector2D BorderTL = TL - FVector2D(1.f, 1.f);
				const FVector2D BorderSz = BoxSize + FVector2D(2.f, 2.f);
				FSlateDrawElement::MakeBox(OutDrawElements, LayerId - 1,
					AllottedGeometry.ToPaintGeometry(BorderSz, FSlateLayoutTransform(BorderTL)),
					FAppStyle::Get().GetBrush("WhiteBrush"),
					ESlateDrawEffect::None,
					SForgeQuestVisualizer::StateColor(B.State));
			}

			// State label
			FSlateDrawElement::MakeText(OutDrawElements, LayerId + 1,
				AllottedGeometry.ToPaintGeometry(BoxSize, FSlateLayoutTransform(TL + FVector2D(4.f, 10.f))),
				SForgeQuestVisualizer::StateLabel(B.State).ToString(),
				FCoreStyle::GetDefaultFontStyle("Bold", 8),
				ESlateDrawEffect::None,
				FLinearColor::White);

			// "Click to set" hint on non-active boxes (tiny text below)
			if (!bActive)
			{
				FSlateDrawElement::MakeText(OutDrawElements, LayerId + 1,
					AllottedGeometry.ToPaintGeometry(
						FVector2D(BoxSize.X, 12.f),
						FSlateLayoutTransform(TL + FVector2D(4.f, BoxSize.Y + 2.f))),
					TEXT("click"),
					FCoreStyle::GetDefaultFontStyle("Italic", 6),
					ESlateDrawEffect::None,
					FLinearColor(0.35f, 0.37f, 0.50f));
			}
		}

		return LayerId + 2;
	}
};

// ─── Construct ───────────────────────────────────────────────────────────────

void SForgeQuestVisualizer::Construct(const FArguments& InArgs)
{
	FPropertyEditorModule& PEM =
		FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs Args;
	Args.bHideSelectionTip = true;
	Args.bAllowSearch      = true;
	Args.NameAreaSettings  = FDetailsViewArgs::HideNameArea;
	DetailsView = PEM.CreateDetailView(Args);

	// Create the canvas and point it back at this visualizer
	SAssignNew(FlowChart, SForgeQuestCanvas);
	FlowChart->Visualizer = this;

	RefreshAssetList();

	ChildSlot
	[
		SNew(SSplitter).Orientation(Orient_Horizontal)

		// LEFT — asset list
		+ SSplitter::Slot().Value(0.28f)
		[
			SNew(SVerticalBox)

			+ SVerticalBox::Slot().AutoHeight().Padding(8.f, 6.f, 8.f, 2.f)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot().FillWidth(1.f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("QuestAssets", "Quest Assets"))
					.Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
					.ColorAndOpacity(FSlateColor(FLinearColor(0.80f, 0.82f, 0.92f)))
				]
				+ SHorizontalBox::Slot().AutoWidth()
				[
					SNew(SForgeButton)
					.Preset(EForgeButtonPreset::Ghost)
					.ContentPadding(FMargin(4.f, 0.f))
					.OnClicked(FOnClicked::CreateLambda([this]() -> FReply
					{
						RefreshAssetList();
						return FReply::Handled();
					}))
					[
						SNew(STextBlock).Text(LOCTEXT("Refresh", "↺"))
						.Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
						.ColorAndOpacity(FSlateColor(FLinearColor(0.5f, 0.6f, 0.9f)))
					]
				]
			]

			+ SVerticalBox::Slot().FillHeight(1.f)
			[
				SAssignNew(AssetListView, SListView<TSharedPtr<FAssetData>>)
				.ListItemsSource(&AssetItems)
				.OnGenerateRow(this, &SForgeQuestVisualizer::MakeAssetRow)
				.OnMouseButtonDoubleClick_Lambda([this](TSharedPtr<FAssetData> Item)
				{
					if (Item.IsValid()) OpenAsset(*Item);
				})
			]
		]

		// RIGHT — flowchart + sim controls + objectives + details
		+ SSplitter::Slot().Value(0.72f)
		[
			SNew(SScrollBox)

			// ── Quest summary ──
			+ SScrollBox::Slot().Padding(8.f, 6.f, 8.f, 2.f)
			[
				SAssignNew(QuestInfoLabel, STextBlock)
				.Text(LOCTEXT("SelectHint", "Double-click a Quest asset to visualize."))
				.Font(FCoreStyle::GetDefaultFontStyle("Italic", 9))
				.ColorAndOpacity(FSlateColor(FLinearColor(0.45f, 0.47f, 0.58f)))
				.AutoWrapText(true)
			]

			// ── State flowchart canvas ──
			+ SScrollBox::Slot()
			[
				SNew(SBox).HeightOverride(120.f)
				[
					FlowChart.ToSharedRef()
				]
			]

			// ── Simulation control buttons ──
			+ SScrollBox::Slot().Padding(8.f, 4.f)
			[
				BuildSimControls()
			]

			+ SScrollBox::Slot().Padding(8.f, 2.f)
			[ SNew(SSeparator) ]

			// ── Objectives checklist ──
			+ SScrollBox::Slot().Padding(0.f, 4.f)
			[
				BuildObjectivesSection()
			]

			+ SScrollBox::Slot().Padding(8.f, 2.f)
			[ SNew(SSeparator) ]

			// ── Full property editor ──
			+ SScrollBox::Slot().Padding(0.f, 4.f)
			[
				SNew(SBox).MinDesiredHeight(180.f)
				[ DetailsView.ToSharedRef() ]
			]
		]
	];
}

// ─── Sim control builder ─────────────────────────────────────────────────────

TSharedRef<SWidget> SForgeQuestVisualizer::BuildSimControls()
{
	auto MakeSimBtn = [this](
		const FText& Label,
		FLinearColor Color,
		FOnClicked Delegate,
		const FText& Tip) -> TSharedRef<SWidget>
	{
		// Normal = dimmed version of state color, Hover = full state color
		const FLinearColor Normal = Color * 0.70f;
		return SNew(SForgeButton)
			.Preset(EForgeButtonPreset::Custom)
			.NormalColor(Normal)
			.HoverColor(Color)
			.ContentPadding(FMargin(6.f, 3.f))
			.ToolTipText(Tip)
			.OnClicked(Delegate)
			.Label(Label)
			.FontSize(7)
			.Bold(true);
	};

	return SAssignNew(SimButtonRow, SHorizontalBox)

		+ SHorizontalBox::Slot().AutoWidth().Padding(2.f, 0.f)
		[
			MakeSimBtn(
				LOCTEXT("SimActivate",  "Activate"),
				StateColor(EForgeQuestState::Active),
				FOnClicked::CreateSP(this, &SForgeQuestVisualizer::OnSimActivate),
				LOCTEXT("SimActivateTip", "Simulate: quest becomes Active"))
		]
		+ SHorizontalBox::Slot().AutoWidth().Padding(2.f, 0.f)
		[
			MakeSimBtn(
				LOCTEXT("SimProgress",  "In Progress"),
				StateColor(EForgeQuestState::InProgress),
				FOnClicked::CreateSP(this, &SForgeQuestVisualizer::OnSimProgress),
				LOCTEXT("SimProgressTip", "Simulate: quest goes InProgress"))
		]
		+ SHorizontalBox::Slot().AutoWidth().Padding(2.f, 0.f)
		[
			MakeSimBtn(
				LOCTEXT("SimComplete",  "Complete"),
				StateColor(EForgeQuestState::Completed),
				FOnClicked::CreateSP(this, &SForgeQuestVisualizer::OnSimComplete),
				LOCTEXT("SimCompleteTip", "Simulate: quest Completed"))
		]
		+ SHorizontalBox::Slot().AutoWidth().Padding(2.f, 0.f)
		[
			MakeSimBtn(
				LOCTEXT("SimFail",      "Fail"),
				StateColor(EForgeQuestState::Failed),
				FOnClicked::CreateSP(this, &SForgeQuestVisualizer::OnSimFail),
				LOCTEXT("SimFailTip", "Simulate: quest Failed"))
		]
		+ SHorizontalBox::Slot().AutoWidth().Padding(8.f, 0.f, 2.f, 0.f)
		[
			MakeSimBtn(
				LOCTEXT("SimReset",     "↺ Reset"),
				FLinearColor(0.18f, 0.20f, 0.28f),
				FOnClicked::CreateSP(this, &SForgeQuestVisualizer::OnSimReset),
				LOCTEXT("SimResetTip", "Reset simulation to Inactive"))
		];
}

// ─── Objectives section builder ───────────────────────────────────────────────

TSharedRef<SWidget> SForgeQuestVisualizer::BuildObjectivesSection()
{
	return SNew(SVerticalBox)

		// Header
		+ SVerticalBox::Slot().AutoHeight().Padding(8.f, 2.f, 8.f, 4.f)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("ObjHeader", "Objectives"))
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", 9))
			.ColorAndOpacity(FSlateColor(FLinearColor(0.75f, 0.77f, 0.90f)))
		]

		// Dynamic objective rows
		+ SVerticalBox::Slot().AutoHeight()
		[
			SAssignNew(ObjectivesContainer, SVerticalBox)
		]

		// Reward row
		+ SVerticalBox::Slot().AutoHeight().Padding(8.f, 6.f, 8.f, 4.f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().AutoWidth().Padding(0.f, 0.f, 6.f, 0.f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("RewardLbl", "Reward"))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 8))
				.ColorAndOpacity(FSlateColor(FLinearColor(0.55f, 0.57f, 0.70f)))
			]
			+ SHorizontalBox::Slot().FillWidth(1.f)
			[
				SAssignNew(RewardLabel, STextBlock)
				.Text(LOCTEXT("RewardNone", "—"))
				.Font(FCoreStyle::GetDefaultFontStyle("Regular", 8))
				.ColorAndOpacity(FSlateColor(FLinearColor(0.70f, 0.72f, 0.85f)))
				.AutoWrapText(true)
			]
		];
}

// ─── RebuildObjectivesPanel ───────────────────────────────────────────────────

void SForgeQuestVisualizer::RebuildObjectivesPanel()
{
	if (!ObjectivesContainer.IsValid()) return;
	ObjectivesContainer->ClearChildren();

	if (!QuestAsset)
	{
		ObjectivesContainer->AddSlot().AutoHeight().Padding(8.f, 2.f)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("NoQuestObjs", "No quest loaded."))
			.Font(FCoreStyle::GetDefaultFontStyle("Italic", 8))
			.ColorAndOpacity(FSlateColor(FLinearColor(0.40f, 0.42f, 0.55f)))
		];
		if (RewardLabel.IsValid())
			RewardLabel->SetText(LOCTEXT("RewardNone", "—"));
		return;
	}

	if (QuestAsset->Objectives.IsEmpty())
	{
		ObjectivesContainer->AddSlot().AutoHeight().Padding(8.f, 2.f)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("NoObjs", "No objectives defined."))
			.Font(FCoreStyle::GetDefaultFontStyle("Italic", 8))
			.ColorAndOpacity(FSlateColor(FLinearColor(0.40f, 0.42f, 0.55f)))
		];
	}
	else
	{
		for (const FForgeObjectiveData& Obj : QuestAsset->Objectives)
		{
			const bool bDone = Obj.IsComplete();
			const FLinearColor RowColor = bDone
				? FLinearColor(0.15f, 0.65f, 0.25f)
				: FLinearColor(0.60f, 0.62f, 0.75f);

			const FText BulletText = FText::FromString(bDone ? TEXT("✓") : TEXT("○"));
			const FText CountText  = FText::Format(
				LOCTEXT("ObjCountFmt", "{0} / {1}"),
				FText::AsNumber(Obj.CurrentCount),
				FText::AsNumber(Obj.RequiredCount));

			ObjectivesContainer->AddSlot().AutoHeight().Padding(8.f, 3.f)
			[
				SNew(SHorizontalBox)

				// Bullet
				+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(0.f, 0.f, 6.f, 0.f)
				[
					SNew(STextBlock)
					.Text(BulletText)
					.Font(FCoreStyle::GetDefaultFontStyle("Bold", 9))
					.ColorAndOpacity(FSlateColor(RowColor))
				]

				// Description
				+ SHorizontalBox::Slot().FillWidth(1.f).VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(Obj.Description)
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 8))
					.ColorAndOpacity(FSlateColor(FLinearColor(0.75f, 0.77f, 0.88f)))
					.AutoWrapText(true)
				]

				// Count badge
				+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(6.f, 0.f, 0.f, 0.f)
				[
					SNew(SBorder)
					.BorderBackgroundColor(RowColor * 0.25f)
					.Padding(FMargin(5.f, 2.f))
					[
						SNew(STextBlock)
						.Text(CountText)
						.Font(FCoreStyle::GetDefaultFontStyle("Bold", 7))
						.ColorAndOpacity(FSlateColor(RowColor))
					]
				]
			];
		}
	}

	// Build reward summary string
	if (RewardLabel.IsValid())
	{
		const FForgeRewardData& R = QuestAsset->Reward;
		FString RewardStr;

		if (R.RZLMAmount > 0)
			RewardStr = FString::Printf(TEXT("%d RZLM"), R.RZLMAmount);

		if (!R.UnlockedQuests.IsEmpty())
		{
			if (!RewardStr.IsEmpty()) RewardStr += TEXT("  +  ");
			RewardStr += FString::Printf(TEXT("Unlocks %d quest(s)"), R.UnlockedQuests.Num());
		}

		if (RewardStr.IsEmpty())
			RewardStr = TEXT("—");

		RewardLabel->SetText(FText::FromString(RewardStr));
	}
}

// ─── Asset loading ────────────────────────────────────────────────────────────

void SForgeQuestVisualizer::OpenAsset(const FAssetData& Asset)
{
	QuestAsset = Cast<UForgeQuestData>(Asset.GetAsset());
	if (!QuestAsset) return;

	SimulatedState = EForgeQuestState::Inactive;

	if (DetailsView.IsValid()) DetailsView->SetObject(QuestAsset);

	if (QuestInfoLabel.IsValid())
	{
		QuestInfoLabel->SetText(FText::Format(
			LOCTEXT("InfoFmt", "📋 {0}  |  {1} objectives  |  ~{2} min  |  Complexity {3}"),
			QuestAsset->QuestTitle,
			FText::AsNumber(QuestAsset->Objectives.Num()),
			FText::AsNumber(FMath::RoundToInt(QuestAsset->EstimatedCompletionMinutes)),
			FText::AsNumber(QuestAsset->MechanicsComplexity)));
	}

	RebuildObjectivesPanel();

	if (FlowChart.IsValid())
	{
		FlowChart->Invalidate(EInvalidateWidgetReason::Paint);
	}
}

void SForgeQuestVisualizer::RefreshAssetList()
{
	AssetItems.Empty();
	IAssetRegistry& AR = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
	FARFilter F;
	F.bRecursivePaths = true;
	F.PackagePaths.Add(FName("/Game"));
	F.ClassPaths.Add(UForgeQuestData::StaticClass()->GetClassPathName());
	TArray<FAssetData> Found;
	AR.GetAssets(F, Found);
	for (const FAssetData& AD : Found)
		AssetItems.Add(MakeShared<FAssetData>(AD));
	if (AssetListView.IsValid()) AssetListView->RequestListRefresh();
}

TSharedRef<ITableRow> SForgeQuestVisualizer::MakeAssetRow(
	TSharedPtr<FAssetData> Item, const TSharedRef<STableViewBase>& Owner)
{
	UForgeQuestData* QD = Cast<UForgeQuestData>(Item->GetAsset());
	const FText Title   = QD ? QD->QuestTitle : FText::FromName(Item->AssetName);

	return SNew(STableRow<TSharedPtr<FAssetData>>, Owner).Padding(FMargin(6.f, 3.f))
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot().AutoHeight()
		[
			SNew(STextBlock).Text(FText::FromName(Item->AssetName))
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", 8))
			.ColorAndOpacity(FSlateColor(FLinearColor(0.82f, 0.84f, 0.92f)))
		]
		+ SVerticalBox::Slot().AutoHeight()
		[
			SNew(STextBlock).Text(Title)
			.Font(FCoreStyle::GetDefaultFontStyle("Italic", 8))
			.ColorAndOpacity(FSlateColor(FLinearColor(0.50f, 0.52f, 0.62f)))
		]
	];
}

// ─── State simulation ─────────────────────────────────────────────────────────

void SForgeQuestVisualizer::SetSimState(EForgeQuestState NewState)
{
	SimulatedState = NewState;
	if (FlowChart.IsValid())
		FlowChart->Invalidate(EInvalidateWidgetReason::Paint);
}

FReply SForgeQuestVisualizer::OnSimActivate()  { SetSimState(EForgeQuestState::Active);     return FReply::Handled(); }
FReply SForgeQuestVisualizer::OnSimProgress()  { SetSimState(EForgeQuestState::InProgress); return FReply::Handled(); }
FReply SForgeQuestVisualizer::OnSimComplete()  { SetSimState(EForgeQuestState::Completed);  return FReply::Handled(); }
FReply SForgeQuestVisualizer::OnSimFail()      { SetSimState(EForgeQuestState::Failed);     return FReply::Handled(); }
FReply SForgeQuestVisualizer::OnSimReset()     { SetSimState(EForgeQuestState::Inactive);   return FReply::Handled(); }

// ─── Helpers ─────────────────────────────────────────────────────────────────

FLinearColor SForgeQuestVisualizer::StateColor(EForgeQuestState S)
{
	switch (S)
	{
	case EForgeQuestState::Active:      return FLinearColor(0.05f, 0.42f, 0.65f);  // cyan
	case EForgeQuestState::InProgress:  return FLinearColor(0.55f, 0.38f, 0.04f);  // gold
	case EForgeQuestState::Completed:   return FLinearColor(0.15f, 0.65f, 0.25f);  // green (success)
	case EForgeQuestState::Failed:      return FLinearColor(0.55f, 0.08f, 0.28f);  // magenta-red
	default:                            return FLinearColor(0.15f, 0.18f, 0.26f);
	}
}

FText SForgeQuestVisualizer::StateLabel(EForgeQuestState S)
{
	switch (S)
	{
	case EForgeQuestState::Inactive:   return LOCTEXT("SInactive",   "Inactive");
	case EForgeQuestState::Active:     return LOCTEXT("SActive",     "Active");
	case EForgeQuestState::InProgress: return LOCTEXT("SInProgress", "In Progress");
	case EForgeQuestState::Completed:  return LOCTEXT("SCompleted",  "Completed");
	case EForgeQuestState::Failed:     return LOCTEXT("SFailed",     "Failed");
	default:                           return FText::GetEmpty();
	}
}

#undef LOCTEXT_NAMESPACE
