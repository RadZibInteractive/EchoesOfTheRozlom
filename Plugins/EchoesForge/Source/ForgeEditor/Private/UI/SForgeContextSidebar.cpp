// © 2026 RadZib. All rights reserved.

#include "UI/SForgeContextSidebar.h"
#include "UI/SForgeActivityPanel.h"
#include "UI/SForgeProfileBrowser.h"
#include "UI/SForgeActorInspector.h"
#include "UI/SForgeButton.h"
#include "Utils/ForgeUIStyle.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SWidgetSwitcher.h"
#include "Styling/AppStyle.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "FileHelpers.h"
#include "Engine/World.h"

#define LOCTEXT_NAMESPACE "ForgeContextSidebar"

// ─── Tab style ────────────────────────────────────────────────────────────────

namespace SidebarTabStyle
{
	// Brand cyan tabs
	static FLinearColor ActiveBg  (0.06f, 0.20f, 0.30f);
	static FLinearColor InactiveBg(0.05f, 0.06f, 0.10f);
	static FLinearColor ActiveText  (0.55f, 0.90f, 1.00f);
	static FLinearColor InactiveText(0.28f, 0.40f, 0.52f);
}

// ─── Activity definitions ────────────────────────────────────────────────────

const TArray<FForgeActivityEntry>& SForgeContextSidebar::GetActivityEntries()
{
	static TArray<FForgeActivityEntry> Entries =
	{
		{ EForgeSidebarState::NPCSetup,
		  LOCTEXT("AddNPC",     "Add NPC"),
		  FLinearColor(0.04f, 0.25f, 0.36f),   // brand cyan
		  FName("Icons.Actor") },

		{ EForgeSidebarState::AnomalySetup,
		  LOCTEXT("AddAnomaly", "Add Anomaly"),
		  FLinearColor(0.28f, 0.05f, 0.26f),   // brand magenta
		  FName("Icons.Visible") },

		{ EForgeSidebarState::EnemySetup,
		  LOCTEXT("AddEnemies", "Add Enemies"),
		  FLinearColor(0.30f, 0.05f, 0.10f),   // danger red-magenta
		  FName("Icons.Warning") },

		{ EForgeSidebarState::PropsSetup,
		  LOCTEXT("AddProps",   "Add Props"),
		  FLinearColor(0.24f, 0.16f, 0.02f),   // brand gold
		  FName("Icons.Details") },
	};
	return Entries;
}

// ─── Construct ───────────────────────────────────────────────────────────────

void SForgeContextSidebar::Construct(const FArguments& InArgs)
{
	bGhostModeActive    = InArgs._bGhostModeActive;
	OnActivitySelected  = InArgs._OnActivitySelected;
	OnGhostModeToggled  = InArgs._OnGhostModeToggled;
	OnZoneChanged       = InArgs._OnZoneChanged;
	OnDialogueRequested = InArgs._OnDialogueRequested;
	OnQuestRequested    = InArgs._OnQuestRequested;
	OnActorPlaced       = InArgs._OnActorPlaced;

	ChildSlot
	[
		SNew(SBorder)
		.BorderBackgroundColor(FLinearColor(0.05f, 0.06f, 0.10f))
		.Padding(FMargin(0.f))
		[
			SNew(SVerticalBox)

			// ── Sidebar tab bar (Place | Browse | Inspect) ──
			+ SVerticalBox::Slot().AutoHeight()
			[ BuildSidebarTabBar() ]

			// ── Tab content area ──
			+ SVerticalBox::Slot().FillHeight(1.f)
			[
				SAssignNew(SidebarSwitcher, SWidgetSwitcher)

				// Slot 0 — Place
				+ SWidgetSwitcher::Slot()
				[ BuildPlaceTab() ]

				// Slot 1 — Browse (Profile Browser)
				+ SWidgetSwitcher::Slot()
				[ BuildBrowseTab() ]

				// Slot 2 — Inspect (Actor Inspector)
				+ SWidgetSwitcher::Slot()
				[ BuildInspectTab() ]
			]
		]
	];

	// Start on Place tab
	SwitchSidebarTab(0);
}

// ─── Sidebar tab bar ──────────────────────────────────────────────────────────

TSharedRef<SWidget> SForgeContextSidebar::BuildSidebarTabBar()
{
	auto MakeTabBtn = [this](
		TSharedPtr<SButton>& OutBtn, int32 TabIdx,
		const FText& Label, const FText& Tip) -> TSharedRef<SWidget>
	{
		return SAssignNew(OutBtn, SButton)
			// Style provides hover; Lambda drives normal tint for active/inactive state
			.ButtonStyle(&ForgeButtonStyle(SidebarTabStyle::InactiveBg, SidebarTabStyle::ActiveBg))
			.ButtonColorAndOpacity_Lambda([this, TabIdx]()
			{
				return ActiveSidebarTab == TabIdx
					? SidebarTabStyle::ActiveBg
					: SidebarTabStyle::InactiveBg;
			})
			.ContentPadding(FMargin(0.f, 8.f))
			.ToolTipText(Tip)
			.OnClicked_Lambda([this, TabIdx]() -> FReply
			{
				SwitchSidebarTab(TabIdx);
				return FReply::Handled();
			})
			[
				SNew(STextBlock)
				.Text(Label)
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 8))
				.Justification(ETextJustify::Center)
				.ColorAndOpacity_Lambda([this, TabIdx]() -> FSlateColor
				{
					return ActiveSidebarTab == TabIdx
						? SidebarTabStyle::ActiveText
						: SidebarTabStyle::InactiveText;
				})
			];
	};

	return SNew(SHorizontalBox)

		+ SHorizontalBox::Slot().FillWidth(1.f)
		[
			MakeTabBtn(TabBtnPlace, 0,
				LOCTEXT("TabPlace",   "  Place  "),
				LOCTEXT("TabPlaceTip","Place actors in the zone"))
		]

		+ SHorizontalBox::Slot().FillWidth(1.f)
		[
			MakeTabBtn(TabBtnBrowse, 1,
				LOCTEXT("TabBrowse",   "  Browse  "),
				LOCTEXT("TabBrowseTip","Browse data assets"))
		]

		+ SHorizontalBox::Slot().FillWidth(1.f)
		[
			MakeTabBtn(TabBtnInspect, 2,
				LOCTEXT("TabInspect",   "  Inspect  "),
				LOCTEXT("TabInspectTip","Inspect selected actor"))
		];
}

void SForgeContextSidebar::SwitchSidebarTab(int32 Index)
{
	ActiveSidebarTab = Index;
	if (SidebarSwitcher.IsValid())
	{
		SidebarSwitcher->SetActiveWidgetIndex(Index);
	}
	if (TabBtnPlace.IsValid())   TabBtnPlace->Invalidate(EInvalidateWidgetReason::Paint);
	if (TabBtnBrowse.IsValid())  TabBtnBrowse->Invalidate(EInvalidateWidgetReason::Paint);
	if (TabBtnInspect.IsValid()) TabBtnInspect->Invalidate(EInvalidateWidgetReason::Paint);
}

// ─── Place tab ────────────────────────────────────────────────────────────────

TSharedRef<SWidget> SForgeContextSidebar::BuildPlaceTab()
{
	return SNew(SScrollBox)

		// ── Zone section ──
		+ SScrollBox::Slot().Padding(10.f, 8.f, 10.f, 6.f)
		[ BuildZoneSection() ]

		+ SScrollBox::Slot().Padding(10.f, 0.f)
		[ SNew(SSeparator).SeparatorImage(FAppStyle::Get().GetBrush("Menu.Separator")) ]

		// ── Activity grid label ──
		+ SScrollBox::Slot().Padding(10.f, 8.f, 10.f, 4.f)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("ActivityGridLabel", "Activity"))
			.Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
			.ColorAndOpacity(FSlateColor(FLinearColor(0.55f, 0.57f, 0.65f)))
		]

		+ SScrollBox::Slot().Padding(10.f, 0.f, 10.f, 8.f)
		[ BuildActivityGrid() ]

		+ SScrollBox::Slot().Padding(10.f, 0.f)
		[ SNew(SSeparator).SeparatorImage(FAppStyle::Get().GetBrush("Menu.Separator")) ]

		// ── Activity config panel (visible after clicking a grid button) ──
		+ SScrollBox::Slot().Padding(10.f, 4.f, 10.f, 4.f)
		[
			SAssignNew(ActivityPanel, SForgeActivityPanel)
			.ActivityType(EForgeSidebarState::NPCSetup)
			.OnActorPlaced_Lambda([this](AActor* Actor)
			{
				if (Actor)
				{
					SetInspectorVariable(FName("LastPlaced"), Actor->GetActorLabel());
					// Propagate up so SForgeMainPanel can refresh Stats immediately
					OnActorPlaced.ExecuteIfBound(Actor);
				}
			})
			.Visibility(EVisibility::Collapsed)
		]

		+ SScrollBox::Slot().Padding(10.f, 0.f)
		[ SNew(SSeparator).SeparatorImage(FAppStyle::Get().GetBrush("Menu.Separator")) ]

		// ── Forge Logic Inspector ──
		+ SScrollBox::Slot().Padding(10.f, 8.f, 10.f, 0.f)
		[ BuildLogicInspector() ];
}

// ─── Browse tab ───────────────────────────────────────────────────────────────

TSharedRef<SWidget> SForgeContextSidebar::BuildBrowseTab()
{
	return SAssignNew(ProfileBrowser, SForgeProfileBrowser)
		.OnDialogueOpened(FOnForgeAssetOpened::CreateLambda([this](const FAssetData& Asset)
		{
			OnDialogueRequested.ExecuteIfBound(Asset);
		}))
		.OnQuestOpened(FOnForgeAssetOpened::CreateLambda([this](const FAssetData& Asset)
		{
			OnQuestRequested.ExecuteIfBound(Asset);
		}));
}

// ─── Inspect tab ─────────────────────────────────────────────────────────────

TSharedRef<SWidget> SForgeContextSidebar::BuildInspectTab()
{
	return SAssignNew(ActorInspector, SForgeActorInspector);
}

// ─── Zone section ────────────────────────────────────────────────────────────

TSharedRef<SWidget> SForgeContextSidebar::BuildZoneSection()
{
	RefreshZoneList();

	return SNew(SVerticalBox)

		// Label + refresh button
		+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 0.f, 0.f, 3.f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().FillWidth(1.f).VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("ZoneLabel", "Active Zone"))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
				.ColorAndOpacity(FSlateColor(FLinearColor(0.75f, 0.88f, 0.95f)))
			]
			+ SHorizontalBox::Slot().AutoWidth()
			[
				SNew(SForgeButton)
				.Preset(EForgeButtonPreset::Ghost)
				.ContentPadding(FMargin(4.f, 1.f))
				.OnClicked(FOnClicked::CreateLambda([this]() -> FReply
				{
					RefreshZoneList();
					return FReply::Handled();
				}))
				[
					SNew(STextBlock)
					.Text(LOCTEXT("RefreshBtn", "↺"))
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
					.ColorAndOpacity(FSlateColor(FLinearColor(0.5f, 0.6f, 0.9f)))
				]
			]
		]

		// Zone combo box
		+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 0.f, 0.f, 10.f)
		[
			SAssignNew(ZoneComboBox, SComboBox<TSharedPtr<FString>>)
			.OptionsSource(&ZoneNames)
			.OnSelectionChanged(this, &SForgeContextSidebar::OnZoneSelected)
			.OnGenerateWidget_Lambda([](TSharedPtr<FString> Item) -> TSharedRef<SWidget>
			{
				return SNew(STextBlock)
					.Text(FText::FromString(*Item))
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 9));
			})
			[
				SNew(STextBlock)
				.Text_Lambda([this]() -> FText
				{
					return SelectedZone.IsValid()
						? FText::FromString(*SelectedZone)
						: LOCTEXT("NoZone", "Select zone…");
				})
				.Font(FCoreStyle::GetDefaultFontStyle("Mono", 9))
				.ColorAndOpacity(FSlateColor(FLinearColor(0.75f, 0.77f, 0.90f)))
			]
		]

		// AI context block
		+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 0.f, 0.f, 4.f)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("AICtxLabel", "AI Context"))
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
			.ColorAndOpacity(FSlateColor(FLinearColor(0.75f, 0.88f, 0.95f)))
		]

		+ SVerticalBox::Slot().AutoHeight()
		[
			SAssignNew(AIContextBlock, STextBlock)
			.Text(LOCTEXT("AICtxDefault", "Select a zone to load context…"))
			.AutoWrapText(true)
			.Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
			.ColorAndOpacity(FSlateColor(FLinearColor(0.55f, 0.57f, 0.65f)))
		];
}

// ─── Activity Grid ───────────────────────────────────────────────────────────

TSharedRef<SWidget> SForgeContextSidebar::BuildActivityGrid()
{
	const TArray<FForgeActivityEntry>& Entries = GetActivityEntries();

	// 2 rows × 2 columns
	return SNew(SVerticalBox)

		+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 0.f, 0.f, 4.f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().FillWidth(1.f).Padding(0.f, 0.f, 4.f, 0.f)
			[ BuildActivityButton(Entries[0]) ]   // Add NPC
			+ SHorizontalBox::Slot().FillWidth(1.f).Padding(4.f, 0.f, 0.f, 0.f)
			[ BuildActivityButton(Entries[1]) ]   // Add Anomaly
		]

		+ SVerticalBox::Slot().AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().FillWidth(1.f).Padding(0.f, 0.f, 4.f, 0.f)
			[ BuildActivityButton(Entries[2]) ]   // Add Enemies
			+ SHorizontalBox::Slot().FillWidth(1.f).Padding(4.f, 0.f, 0.f, 0.f)
			[ BuildActivityButton(Entries[3]) ]   // Add Props
		];
}

TSharedRef<SWidget> SForgeContextSidebar::BuildActivityButton(const FForgeActivityEntry& Entry)
{
	// Hover = Entry.Color brightened by ~50%, clamped automatically inside ForgeButtonStyle
	const FLinearColor HoverColor(
		FMath::Min(Entry.Color.R * 1.55f, 1.f),
		FMath::Min(Entry.Color.G * 1.55f, 1.f),
		FMath::Min(Entry.Color.B * 1.55f, 1.f));

	return SNew(SForgeButton)
		.Preset(EForgeButtonPreset::Custom)
		.NormalColor(Entry.Color)
		.HoverColor(HoverColor)
		.OnClicked(this, &SForgeContextSidebar::OnActivityClicked, Entry.State)
		.ContentPadding(FMargin(8.f, 14.f))
		[
			SNew(SVerticalBox)

			// Icon
			+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).Padding(0.f, 0.f, 0.f, 4.f)
			[
				SNew(SBox).WidthOverride(22.f).HeightOverride(22.f)
				[
					SNew(SImage)
					.Image(FAppStyle::Get().GetBrush(Entry.IconBrushName))
					.ColorAndOpacity(FSlateColor(FLinearColor::White))
				]
			]

			// Label
			+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center)
			[
				SNew(STextBlock)
				.Text(Entry.Label)
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 9))
				.ColorAndOpacity(FSlateColor(FLinearColor::White))
				.Justification(ETextJustify::Center)
			]
		];
}

// ─── Ghost Mode Block ────────────────────────────────────────────────────────

TSharedRef<SWidget> SForgeContextSidebar::BuildGhostModeBlock()
{
	return SAssignNew(GhostModeBorder, SBorder)
		.BorderBackgroundColor(MakeAttributeLambda([this]() -> FSlateColor
		{
			return bGhostActive
				? FSlateColor(FLinearColor(0.62f, 0.65f, 0.70f))   // light grey when ON
				: FSlateColor(FLinearColor(0.10f, 0.12f, 0.22f));   // dark when OFF
		}))
		.Padding(FMargin(8.f))
		[
			SNew(SButton)
			.ButtonColorAndOpacity(FLinearColor(0.f, 0.f, 0.f, 0.f)) // transparent — border is the bg
			.OnClicked(this, &SForgeContextSidebar::OnGhostModeClicked)
			.ContentPadding(FMargin(0.f))
			[
				SNew(SHorizontalBox)

				// Ghost icon image (large)
				+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(0.f, 0.f, 10.f, 0.f)
				[
					SNew(SBox).WidthOverride(36.f).HeightOverride(36.f)
					[
						SNew(SImage)
						.Image(FAppStyle::Get().GetBrush("Icons.Visibility"))
						.ColorAndOpacity_Lambda([this]()
						{
							return bGhostActive
								? FSlateColor(FLinearColor(0.4f, 0.6f, 1.f))   // blue when active
								: FSlateColor(FLinearColor(0.4f, 0.4f, 0.5f)); // dim when off
						})
					]
				]

				// Text block
				+ SHorizontalBox::Slot().FillWidth(1.f).VAlign(VAlign_Center)
				[
					SNew(SVerticalBox)

					+ SVerticalBox::Slot().AutoHeight()
					[
						SNew(STextBlock)
						.Text(LOCTEXT("GhostModeTitle", "Ghost Mode"))
						.Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
						.ColorAndOpacity(MakeAttributeLambda([this]() -> FSlateColor
						{
							return bGhostActive
								? FSlateColor(FLinearColor(0.08f, 0.09f, 0.14f))
								: FSlateColor(FLinearColor(0.80f, 0.82f, 0.92f));
						}))
					]

					+ SVerticalBox::Slot().AutoHeight()
					[
						SNew(STextBlock)
						.Text_Lambda([this]()
						{
							return bGhostActive
								? LOCTEXT("GhostOn",  "Active — NoClip + Invisibility")
								: LOCTEXT("GhostOff", "Click to enable");
						})
						.Font(FCoreStyle::GetDefaultFontStyle("Italic", 8))
						.ColorAndOpacity(MakeAttributeLambda([this]() -> FSlateColor
						{
							return bGhostActive
								? FSlateColor(FLinearColor(0.20f, 0.22f, 0.30f))
								: FSlateColor(FLinearColor(0.45f, 0.47f, 0.58f));
						}))
					]
				]

				// Active indicator dot
				+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(6.f, 0.f, 0.f, 0.f)
				[
					SNew(SBox).WidthOverride(10.f).HeightOverride(10.f)
					[
						SNew(SImage)
						.Image(FAppStyle::Get().GetBrush("Icons.BulletPoint"))
						.ColorAndOpacity_Lambda([this]()
						{
							return bGhostActive
								? FSlateColor(FLinearColor(0.3f, 0.7f, 1.f))
								: FSlateColor(FLinearColor(0.2f, 0.2f, 0.3f));
						})
					]
				]
			]
		];
}

// ─── Logic Inspector ─────────────────────────────────────────────────────────

TSharedRef<SWidget> SForgeContextSidebar::BuildLogicInspector()
{
	return SNew(SVerticalBox)

		// Collapsible header
		+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 0.f, 0.f, 4.f)
		[
			SNew(SForgeButton)
			.Preset(EForgeButtonPreset::Subtle)
			.ContentPadding(FMargin(0.f))
			.OnClicked(FOnClicked::CreateLambda([this]() -> FReply
			{
				bInspectorExpanded = !bInspectorExpanded;
				if (InspectorVarList.IsValid())
				{
					InspectorVarList->SetVisibility(
						bInspectorExpanded ? EVisibility::Visible : EVisibility::Collapsed);
				}
				return FReply::Handled();
			}))
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(0.f, 0.f, 6.f, 0.f)
				[
					SNew(SImage)
					.Image(FAppStyle::Get().GetBrush("Icons.ChevronDown"))
					.DesiredSizeOverride(FVector2D(10.f, 10.f))
					.ColorAndOpacity(FSlateColor(FLinearColor(0.55f, 0.57f, 0.65f)))
				]

				+ SHorizontalBox::Slot().FillWidth(1.f).VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("InspectorLabel", "Forge Logic Inspector"))
					.Font(FCoreStyle::GetDefaultFontStyle("Bold", 9))
					.ColorAndOpacity(FSlateColor(FLinearColor(0.65f, 0.68f, 0.78f)))
				]
			]
		]

		// Variable list
		+ SVerticalBox::Slot().AutoHeight()
		[
			SNew(SBorder)
			.BorderBackgroundColor(FLinearColor(0.04f, 0.05f, 0.09f))
			.Padding(FMargin(8.f, 6.f))
			[
				SAssignNew(InspectorVarList, SVerticalBox)
				// Populated via SetInspectorVariable()
			]
		];
}

// ─── Public setters ──────────────────────────────────────────────────────────

void SForgeContextSidebar::SetGhostModeActive(bool bActive)
{
	bGhostActive = bActive;
	Invalidate(EInvalidateWidgetReason::Paint);
}

void SForgeContextSidebar::SetInspectorVariable(const FName& VarName, const FString& Value)
{
	if (!InspectorVarList.IsValid()) return;

	const FString Line = FString::Printf(TEXT("%s: %s"), *VarName.ToString(), *Value);

	if (TSharedPtr<STextBlock>* Existing = InspectorVarWidgets.Find(VarName))
	{
		(*Existing)->SetText(FText::FromString(Line));
	}
	else
	{
		TSharedPtr<STextBlock> NewBlock;
		InspectorVarList->AddSlot().AutoHeight().Padding(0.f, 1.f)
		[
			SAssignNew(NewBlock, STextBlock)
			.Text(FText::FromString(Line))
			.Font(FCoreStyle::GetDefaultFontStyle("Mono", 9))
			.ColorAndOpacity(FSlateColor(FLinearColor(0.3f, 0.9f, 0.4f)))
		];
		InspectorVarWidgets.Add(VarName, NewBlock);
	}
}

void SForgeContextSidebar::ClearInspectorVariables()
{
	if (InspectorVarList.IsValid()) InspectorVarList->ClearChildren();
	InspectorVarWidgets.Empty();
}

// ─── Activity click — show config panel ──────────────────────────────────────

FReply SForgeContextSidebar::OnActivityClicked(EForgeSidebarState Activity)
{
	if (ActivityPanel.IsValid())
	{
		ActivityPanel->SetActivityType(Activity);
		ActivityPanel->SetVisibility(EVisibility::Visible);
	}
	OnActivitySelected.ExecuteIfBound(Activity);
	return FReply::Handled();
}

FReply SForgeContextSidebar::OnGhostModeClicked()
{
	bGhostActive = !bGhostActive;
	OnGhostModeToggled.ExecuteIfBound(bGhostActive);
	Invalidate(EInvalidateWidgetReason::Paint);
	return FReply::Handled();
}

// ─── Zone picker ─────────────────────────────────────────────────────────────

void SForgeContextSidebar::RefreshZoneList()
{
	ZoneNames.Empty();

	IAssetRegistry& AR = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();

	FARFilter Filter;
	Filter.ClassPaths.Add(UWorld::StaticClass()->GetClassPathName());
	Filter.bRecursivePaths = true;
	Filter.PackagePaths.Add(FName("/Game"));

	TArray<FAssetData> Maps;
	AR.GetAssets(Filter, Maps);

	for (const FAssetData& Map : Maps)
	{
		ZoneNames.Add(MakeShared<FString>(Map.AssetName.ToString()));
	}

	if (ZoneComboBox.IsValid()) ZoneComboBox->RefreshOptions();
}

void SForgeContextSidebar::OnZoneSelected(TSharedPtr<FString> Item, ESelectInfo::Type /*Reason*/)
{
	if (!Item.IsValid()) return;
	SelectedZone = Item;

	// Update AI context placeholder
	if (AIContextBlock.IsValid())
	{
		AIContextBlock->SetText(FText::Format(
			LOCTEXT("AICtxZone", "Zone '{0}' selected. Describe your scenario idea in the Smart Popup →"),
			FText::FromString(*Item)));
	}

	OnZoneChanged.ExecuteIfBound(*Item);

	// Load the map — UE will prompt to save current if dirty
	IAssetRegistry& AR = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
	FARFilter Filter;
	Filter.ClassPaths.Add(UWorld::StaticClass()->GetClassPathName());
	Filter.bRecursivePaths = true;
	Filter.PackagePaths.Add(FName("/Game"));

	TArray<FAssetData> Maps;
	AR.GetAssets(Filter, Maps);

	for (const FAssetData& Map : Maps)
	{
		if (Map.AssetName.ToString() == *Item)
		{
			FEditorFileUtils::LoadMap(Map.GetSoftObjectPath().ToString(), false, true);
			break;
		}
	}
}

// ─── Public setters ───────────────────────────────────────────────────────────

void SForgeContextSidebar::SetZoneName(const FString& ZoneName)
{
	SelectedZone = MakeShared<FString>(ZoneName);
	if (ZoneComboBox.IsValid()) ZoneComboBox->RefreshOptions();
}

void SForgeContextSidebar::SetAIContextText(const FText& Text)
{
	if (AIContextBlock.IsValid()) AIContextBlock->SetText(Text);
}

FString SForgeContextSidebar::GetCurrentZoneName() const
{
	return SelectedZone.IsValid() ? *SelectedZone : FString();
}

#undef LOCTEXT_NAMESPACE
