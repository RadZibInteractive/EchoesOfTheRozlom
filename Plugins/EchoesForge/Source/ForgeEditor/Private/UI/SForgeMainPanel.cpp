// © 2026 RadZib. All rights reserved.

#include "UI/SForgeMainPanel.h"
#include "UI/SForgeTopToolbar.h"
#include "UI/SForgeViewport.h"
#include "UI/SForgeSmartPopup.h"
#include "UI/SForgeValidationBar.h"
#include "UI/SForgeDialogueEditor.h"
#include "UI/SForgeQuestVisualizer.h"
#include "UI/SForgeZoneStats.h"
#include "Validation/ForgeValidator.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SWidgetSwitcher.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Editor.h"
#include "FileHelpers.h"
#include "LevelEditorSubsystem.h"

#define LOCTEXT_NAMESPACE "ForgeMainPanel"

// ─── Tab button style helpers ─────────────────────────────────────────────────

namespace ForgeTabStyle
{
	static FLinearColor ActiveBg  (0.18f, 0.20f, 0.32f);
	static FLinearColor InactiveBg(0.07f, 0.08f, 0.13f);
	static FLinearColor ActiveText  (0.85f, 0.88f, 1.00f);
	static FLinearColor InactiveText(0.45f, 0.47f, 0.60f);
}

// ─── Construct ───────────────────────────────────────────────────────────────

void SForgeMainPanel::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SVerticalBox)

		// ── TOP TOOLBAR ──────────────────────────────────────────────────────
		+ SVerticalBox::Slot().AutoHeight()
		[
			SAssignNew(Toolbar, SForgeTopToolbar)
			.bSimulateActive(false)
			.bGhostModeActive(false)
			.OnUndo(    FOnForgeAction::CreateSP(this, &SForgeMainPanel::OnUndoClicked))
			.OnRedo(    FOnForgeAction::CreateSP(this, &SForgeMainPanel::OnRedoClicked))
			.OnSave(    FOnForgeAction::CreateSP(this, &SForgeMainPanel::OnSaveClicked))
			.OnSimulate(FOnForgeAction::CreateSP(this, &SForgeMainPanel::OnSimulateClicked))
			.OnGhostModeToggled(FOnForgeToggle::CreateSP(this, &SForgeMainPanel::OnGhostModeToggled))
			.OnValidate(FOnForgeAction::CreateSP(this, &SForgeMainPanel::OnValidateClicked))
			.OnPublish( FOnForgeAction::CreateSP(this, &SForgeMainPanel::OnPublishClicked))
		]

		// ── MAIN CONTENT AREA ─────────────────────────────────────────────
		+ SVerticalBox::Slot().FillHeight(1.f)
		[
			SNew(SHorizontalBox)

			// LEFT: Context Sidebar (fixed 280px)
			+ SHorizontalBox::Slot().AutoWidth()
			[
				SNew(SBox).WidthOverride(280.f)
				[
					SAssignNew(Sidebar, SForgeContextSidebar)
					.bGhostModeActive(false)
					.OnActivitySelected(FOnActivitySelected::CreateSP(
						this, &SForgeMainPanel::OnActivitySelected))
					.OnGhostModeToggled(FOnForgeToggle::CreateSP(
						this, &SForgeMainPanel::OnGhostModeToggled))
					.OnZoneChanged(FOnForgeZoneChanged::CreateSP(
						this, &SForgeMainPanel::OnZoneChanged))
					.OnDialogueRequested(FOnForgeDialogueRequested::CreateSP(
						this, &SForgeMainPanel::OnDialogueRequested))
					.OnQuestRequested(FOnForgeQuestRequested::CreateSP(
						this, &SForgeMainPanel::OnQuestRequested))
					.OnActorPlaced(FOnForgeActorPlaced::CreateSP(
						this, &SForgeMainPanel::OnActorPlaced))
				]
			]

			// CENTER: Tab bar + content switcher (Viewport | Dialogue | Quest)
			+ SHorizontalBox::Slot().FillWidth(1.f)
			[
				SNew(SVerticalBox)

				// Tab button bar
				+ SVerticalBox::Slot().AutoHeight()
				[ BuildCenterTabBar() ]

				// Content area
				+ SVerticalBox::Slot().FillHeight(1.f)
				[
					SAssignNew(CenterSwitcher, SWidgetSwitcher)

					// Slot 0 — Viewport
					+ SWidgetSwitcher::Slot()
					[ SAssignNew(Viewport, SForgeViewport) ]

					// Slot 1 — Dialogue Editor
					+ SWidgetSwitcher::Slot()
					[ SAssignNew(DialogueEditor, SForgeDialogueEditor) ]

					// Slot 2 — Quest Visualizer
					+ SWidgetSwitcher::Slot()
					[ SAssignNew(QuestVisualizer, SForgeQuestVisualizer) ]

					// Slot 3 — Zone Statistics
					+ SWidgetSwitcher::Slot()
					[ SAssignNew(ZoneStats, SForgeZoneStats) ]
				]
			]

			// RIGHT: Smart Popup Chat (fixed 280px)
			+ SHorizontalBox::Slot().AutoWidth()
			[
				SNew(SBox).WidthOverride(280.f)
				[
					SAssignNew(SmartPopup, SForgeSmartPopup)
					.Title(LOCTEXT("SmartPopupTitle", "AI Suggestions"))
					.PlaceholderHint(LOCTEXT("SmartPopupHint", "Describe your idea..."))
					.OnIdeaSubmitted(FOnForgeIdeaSubmitted::CreateSP(
						this, &SForgeMainPanel::OnIdeaSubmitted))
					.OnSuggestionPicked(FOnForgeSuggestionPicked::CreateSP(
						this, &SForgeMainPanel::OnSuggestionPicked))
				]
			]
		]

		// ── BOTTOM VALIDATION BAR ─────────────────────────────────────────
		+ SVerticalBox::Slot().AutoHeight()
		[
			SAssignNew(ValidationBar, SForgeValidationBar)
			.OnWorldPartitionMapClicked(FOnWorldPartitionMapClicked::CreateLambda([]()
			{
				UE_LOG(LogTemp, Log, TEXT("Forge: World Partition Map requested"));
			}))
		]
	];

	// Seed the validation bar with a default "no issues" entry
	TArray<FForgeValidationItem> DefaultItems;
	DefaultItems.Add({ EForgeValidationStatus::Valid,
		LOCTEXT("NoIssues", "No issues — open a zone to begin.") });
	ValidationBar->SetItems(DefaultItems);

	// Start on Viewport tab
	SwitchCenterTab(0);
}

// ─── Center tab bar ───────────────────────────────────────────────────────────

TSharedRef<SWidget> SForgeMainPanel::BuildCenterTabBar()
{
	auto MakeTabBtn = [this](
		TSharedPtr<SButton>& OutBtn, int32 TabIdx,
		const FText& Label, const FText& Tip) -> TSharedRef<SWidget>
	{
		return SAssignNew(OutBtn, SButton)
			.ButtonColorAndOpacity_Lambda([this, TabIdx]()
			{
				return ActiveCenterTab == TabIdx
					? ForgeTabStyle::ActiveBg
					: ForgeTabStyle::InactiveBg;
			})
			.ContentPadding(FMargin(14.f, 8.f))
			.ToolTipText(Tip)
			.OnClicked_Lambda([this, TabIdx]() -> FReply
			{
				SwitchCenterTab(TabIdx);
				return FReply::Handled();
			})
			[
				SNew(STextBlock)
				.Text(Label)
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 9))
				.ColorAndOpacity_Lambda([this, TabIdx]() -> FSlateColor
				{
					return ActiveCenterTab == TabIdx
						? ForgeTabStyle::ActiveText
						: ForgeTabStyle::InactiveText;
				})
			];
	};

	return SNew(SHorizontalBox)

		+ SHorizontalBox::Slot().AutoWidth()
		[
			MakeTabBtn(TabBtnViewport, 0,
				LOCTEXT("TabViewport", "🌍  Viewport"),
				LOCTEXT("TabViewportTip", "3D level viewport"))
		]

		+ SHorizontalBox::Slot().AutoWidth()
		[
			MakeTabBtn(TabBtnDialogue, 1,
				LOCTEXT("TabDialogue", "💬  Dialogue"),
				LOCTEXT("TabDialogueTip", "Dialogue tree editor"))
		]

		+ SHorizontalBox::Slot().AutoWidth()
		[
			MakeTabBtn(TabBtnQuest, 2,
				LOCTEXT("TabQuest", "📋  Quest"),
				LOCTEXT("TabQuestTip", "Quest state visualizer"))
		]

		+ SHorizontalBox::Slot().AutoWidth()
		[
			MakeTabBtn(TabBtnStats, 3,
				LOCTEXT("TabStats", "📊  Stats"),
				LOCTEXT("TabStatsTip", "Zone statistics & balance"))
		]

		// Spacer to push tabs to the left
		+ SHorizontalBox::Slot().FillWidth(1.f)
		[
			SNew(SBorder)
			.BorderBackgroundColor(FLinearColor(0.07f, 0.08f, 0.13f))
			[ SNullWidget::NullWidget ]
		];
}

void SForgeMainPanel::SwitchCenterTab(int32 Index)
{
	ActiveCenterTab = Index;
	if (CenterSwitcher.IsValid())
	{
		CenterSwitcher->SetActiveWidgetIndex(Index);
	}
	// Force repaint on tab buttons
	if (TabBtnViewport.IsValid()) TabBtnViewport->Invalidate(EInvalidateWidgetReason::Paint);
	if (TabBtnDialogue.IsValid()) TabBtnDialogue->Invalidate(EInvalidateWidgetReason::Paint);
	if (TabBtnQuest.IsValid())    TabBtnQuest->Invalidate(EInvalidateWidgetReason::Paint);
	if (TabBtnStats.IsValid())    TabBtnStats->Invalidate(EInvalidateWidgetReason::Paint);
}

// ─── Toolbar callbacks ────────────────────────────────────────────────────────

void SForgeMainPanel::OnSimulateClicked()
{
	if (!GEditor) return;

	// Toggle: stop if already playing, start otherwise
	if (GEditor->IsPlaySessionInProgress())
	{
		GEditor->RequestEndPlayMap();
		bSimulateActive = false;
	}
	else
	{
		bSimulateActive = true;
		FRequestPlaySessionParams Params;
		// Default WorldType = PlayInEditor (new window) — works without a registered LevelViewport
		GEditor->RequestPlaySession(Params);
	}

	if (Toolbar.IsValid()) Toolbar->SetSimulateActive(bSimulateActive);
	if (Sidebar.IsValid())
	{
		Sidebar->SetInspectorVariable(FName("Simulate"),
			bSimulateActive ? TEXT("▶ Running") : TEXT("■ Stopped"));
	}
}

void SForgeMainPanel::OnGhostModeToggled(bool bIsActive)
{
	bGhostModeActive = bIsActive;
	if (Toolbar.IsValid()) Toolbar->SetGhostModeActive(bIsActive);
	if (Sidebar.IsValid()) Sidebar->SetGhostModeActive(bIsActive);
}

void SForgeMainPanel::OnValidateClicked()
{
	if (!ValidationBar.IsValid() || !GEditor) return;

	UWorld* World = GEditor->GetEditorWorldContext().World();
	TArray<FForgeValidationResult> Raw = FForgeValidator::ValidateWorld(World);

	TArray<FForgeValidationItem> Items;
	Items.Reserve(Raw.Num());
	for (const FForgeValidationResult& R : Raw)
	{
		const FString Msg = R.Actor.IsEmpty()
			? R.Message
			: FString::Printf(TEXT("[%s] %s"), *R.Actor, *R.Message);
		Items.Add({ R.Status, FText::FromString(Msg) });
	}
	ValidationBar->SetItems(Items);
}

void SForgeMainPanel::OnPublishClicked()
{
	// Save first, then mark package as cooked-ready
	FEditorFileUtils::SaveCurrentLevel();
	UE_LOG(LogTemp, Log, TEXT("Forge: Level saved for publish"));
}

void SForgeMainPanel::OnSaveClicked()
{
	FEditorFileUtils::SaveCurrentLevel();
}

void SForgeMainPanel::OnUndoClicked()
{
	if (GEditor) GEditor->UndoTransaction();
}

void SForgeMainPanel::OnRedoClicked()
{
	if (GEditor) GEditor->RedoTransaction();
}

// ─── Sidebar callbacks ────────────────────────────────────────────────────────

void SForgeMainPanel::OnActivitySelected(EForgeSidebarState Activity)
{
	if (!SmartPopup.IsValid()) return;

	FString ActivityStr;
	switch (Activity)
	{
		case EForgeSidebarState::NPCSetup:     ActivityStr = TEXT("NPC Setup");     break;
		case EForgeSidebarState::AnomalySetup: ActivityStr = TEXT("Anomaly Setup"); break;
		case EForgeSidebarState::EnemySetup:   ActivityStr = TEXT("Enemy Setup");   break;
		case EForgeSidebarState::PropsSetup:   ActivityStr = TEXT("Props Setup");   break;
		default: break;
	}

	const FString Zone = Sidebar.IsValid() ? Sidebar->GetCurrentZoneName() : FString();
	SmartPopup->SetZoneContext(Zone, ActivityStr);
}

void SForgeMainPanel::OnZoneChanged(const FString& ZoneName)
{
	if (SmartPopup.IsValid())
	{
		SmartPopup->SetZoneContext(ZoneName, FString());
	}
	if (Sidebar.IsValid())
	{
		Sidebar->SetInspectorVariable(FName("Zone"), ZoneName);
	}
	// Refresh stats whenever the active zone changes
	if (ZoneStats.IsValid())
	{
		ZoneStats->RefreshStats();
	}
}

void SForgeMainPanel::OnDialogueRequested(const FAssetData& Asset)
{
	SwitchCenterTab(1);
	if (DialogueEditor.IsValid())
	{
		DialogueEditor->OpenAsset(Asset);
	}
}

void SForgeMainPanel::OnQuestRequested(const FAssetData& Asset)
{
	SwitchCenterTab(2);
	if (QuestVisualizer.IsValid())
	{
		QuestVisualizer->OpenAsset(Asset);
	}
}

void SForgeMainPanel::OnActorPlaced(AActor* PlacedActor)
{
	// Immediately update the Stats panel so counts reflect the new actor.
	if (ZoneStats.IsValid()) ZoneStats->RefreshStats();

	UE_LOG(LogTemp, Log, TEXT("Forge: Actor placed — '%s'"),
		PlacedActor ? *PlacedActor->GetActorLabel() : TEXT("?"));
}

// ─── Smart Popup callbacks ────────────────────────────────────────────────────

void SForgeMainPanel::OnIdeaSubmitted(const FString& Idea)
{
	// SmartPopup handles the Claude API call directly; this delegate is
	// kept for any additional main-panel logic (e.g., logging, analytics)
	UE_LOG(LogTemp, Log, TEXT("Forge: Idea submitted — '%s'"), *Idea);
}

void SForgeMainPanel::OnSuggestionPicked(int32 Index)
{
	UE_LOG(LogTemp, Log, TEXT("Forge: Suggestion %d picked"), Index);
}

#undef LOCTEXT_NAMESPACE
