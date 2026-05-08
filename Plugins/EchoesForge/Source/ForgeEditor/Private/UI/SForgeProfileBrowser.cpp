// © 2026 RadZib. All rights reserved.

#include "UI/SForgeProfileBrowser.h"
#include "Actors/ForgeWorldActors.h"
#include "Data/ForgeNPCProfile.h"
#include "Data/ForgeAnomalyProfile.h"
#include "Data/ForgeDialogueData.h"
#include "Data/ForgeQuestData.h"
#include "UI/SForgeButton.h"
#include "Utils/ForgeUIStyle.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "Editor.h"
#include "EditorViewportClient.h"
#include "EngineUtils.h"

#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Images/SImage.h"
#include "Styling/AppStyle.h"

#define LOCTEXT_NAMESPACE "ForgeProfileBrowser"

// 5 tab accent colors: NPC=cyan | Anomaly=magenta | Dialogue=deep-cyan | Quest=gold | Scene=teal
const FLinearColor SForgeProfileBrowser::TabColors[5] =
{
	FLinearColor(0.04f, 0.25f, 0.36f),   // NPC      — brand cyan
	FLinearColor(0.28f, 0.05f, 0.26f),   // Anomaly  — brand magenta
	FLinearColor(0.06f, 0.12f, 0.22f),   // Dialogue — deep cyan / navy
	FLinearColor(0.28f, 0.18f, 0.02f),   // Quest    — brand gold
	FLinearColor(0.04f, 0.20f, 0.20f),   // Scene    — teal (world actors)
};

// ─── Construct ───────────────────────────────────────────────────────────────

void SForgeProfileBrowser::Construct(const FArguments& InArgs)
{
	OnDialogueOpened = InArgs._OnDialogueOpened;
	OnQuestOpened    = InArgs._OnQuestOpened;

	auto MakeTab = [this](int32 Idx) -> TSharedRef<SWidget>
	{
		const FLinearColor ActiveCol = TabColors[Idx];
		const FLinearColor HoverCol  = FLinearColor(
			FMath::Min(ActiveCol.R * 1.40f, 1.f),
			FMath::Min(ActiveCol.G * 1.40f, 1.f),
			FMath::Min(ActiveCol.B * 1.40f, 1.f));
		const FLinearColor InactiveBg(0.10f, 0.11f, 0.16f);

		return SNew(SButton)
			.ButtonStyle(&ForgeButtonStyle(InactiveBg, HoverCol))
			.ButtonColorAndOpacity_Lambda([this, Idx, ActiveCol, InactiveBg]() -> FLinearColor
			{
				return ActiveFilter == Idx ? ActiveCol : InactiveBg;
			})
			.ContentPadding(FMargin(0.f, 6.f))
			.HAlign(HAlign_Center)
			.OnClicked_Lambda([this, Idx]() -> FReply
			{
				SetFilter(Idx);
				return FReply::Handled();
			})
			[
				SNew(STextBlock)
				.Text(LabelForFilter(Idx))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 8))
				.ColorAndOpacity(FSlateColor(FLinearColor::White))
				.Justification(ETextJustify::Center)
			];
	};

	ChildSlot
	[
		SNew(SVerticalBox)

		// ── Filter tabs (5) ──
		+ SVerticalBox::Slot().AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().FillWidth(1.f).Padding(1.f, 0.f)[ MakeTab(0) ]
			+ SHorizontalBox::Slot().FillWidth(1.f).Padding(1.f, 0.f)[ MakeTab(1) ]
			+ SHorizontalBox::Slot().FillWidth(1.f).Padding(1.f, 0.f)[ MakeTab(2) ]
			+ SHorizontalBox::Slot().FillWidth(1.f).Padding(1.f, 0.f)[ MakeTab(3) ]
			+ SHorizontalBox::Slot().FillWidth(1.f).Padding(1.f, 0.f)[ MakeTab(4) ]
		]

		// ── Refresh button ──
		+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Right).Padding(4.f, 3.f)
		[
			SNew(SForgeButton)
			.Preset(EForgeButtonPreset::Ghost)
			.ContentPadding(FMargin(4.f, 0.f))
			.OnClicked(FOnClicked::CreateLambda([this]() -> FReply { Refresh(); return FReply::Handled(); }))
			[
				SNew(STextBlock)
				.Text(LOCTEXT("Refresh", "↺ Refresh"))
				.Font(FCoreStyle::GetDefaultFontStyle("Regular", 8))
				.ColorAndOpacity(FSlateColor(FLinearColor(0.5f, 0.6f, 0.9f)))
			]
		]

		// ── Asset list (visible for tabs 0–3) ──
		+ SVerticalBox::Slot().FillHeight(1.f)
		[
			SAssignNew(AssetListBox, SBox)
			[
				SAssignNew(AssetList, SListView<TSharedPtr<FAssetData>>)
				.ListItemsSource(&FilteredItems)
				.OnGenerateRow(this, &SForgeProfileBrowser::MakeAssetRow)
				.OnMouseButtonDoubleClick(this, &SForgeProfileBrowser::OnAssetDoubleClicked)
				.SelectionMode(ESelectionMode::Single)
			]
		]

		// ── World actor list (tab 4 — Scene) ──
		+ SVerticalBox::Slot().FillHeight(1.f)
		[
			SAssignNew(WorldListBox, SBox)
			.Visibility(EVisibility::Collapsed)
			[
				SAssignNew(WorldList, SListView<TSharedPtr<FWorldActorEntry>>)
				.ListItemsSource(&WorldItems)
				.OnGenerateRow(this, &SForgeProfileBrowser::MakeWorldActorRow)
				.OnMouseButtonClick(this, &SForgeProfileBrowser::OnWorldActorClicked)
				.SelectionMode(ESelectionMode::Single)
			]
		]

		// ── Empty-state hint ──
		+ SVerticalBox::Slot().AutoHeight().Padding(10.f, 12.f)
		[
			SAssignNew(EmptyHint, STextBlock)
			.Visibility(EVisibility::Collapsed)
			.AutoWrapText(true)
			.Font(FCoreStyle::GetDefaultFontStyle("Italic", 8))
			.ColorAndOpacity(FSlateColor(FLinearColor(0.45f, 0.47f, 0.58f)))
		]
	];

	QueryAssets();
	SetFilter(0);
}

// ─── Filter / query ───────────────────────────────────────────────────────────

void SForgeProfileBrowser::SetFilter(int32 Index)
{
	ActiveFilter = Index;

	// ── Scene tab (index 4) ───────────────────────────────────────────────────
	if (Index == 4)
	{
		if (AssetListBox.IsValid()) AssetListBox->SetVisibility(EVisibility::Collapsed);
		if (WorldListBox.IsValid()) WorldListBox->SetVisibility(EVisibility::Visible);

		QueryWorldActors();
		if (WorldList.IsValid()) WorldList->RequestListRefresh();

		if (EmptyHint.IsValid())
		{
			const bool bEmpty = WorldItems.IsEmpty();
			EmptyHint->SetVisibility(bEmpty ? EVisibility::Visible : EVisibility::Collapsed);
			if (bEmpty)
				EmptyHint->SetText(LOCTEXT("EmptyScene",
					"No Forge actors found in the current level.\n"
					"Place some actors via the Place tab first."));
		}

		Invalidate(EInvalidateWidgetReason::Paint);
		return;
	}

	// ── Asset tabs 0–3 ────────────────────────────────────────────────────────
	if (AssetListBox.IsValid()) AssetListBox->SetVisibility(EVisibility::Visible);
	if (WorldListBox.IsValid()) WorldListBox->SetVisibility(EVisibility::Collapsed);

	FilteredItems.Empty();

	for (const FAssetData& AD : AllAssets)
	{
		const FName AssetClass = AD.AssetClassPath.GetAssetName();
		bool bMatch = false;

		if (Index == 1)
		{
			// Anomaly tab: match both the Forge profile AND the game-native EotRAnomalyProfile
			bMatch = (AssetClass == FName("ForgeAnomalyProfile") ||
			          AssetClass == FName("EotRAnomalyProfile"));
		}
		else
		{
			bMatch = (AssetClass == ClassNameForFilter(Index));
		}

		if (bMatch) FilteredItems.Add(MakeShared<FAssetData>(AD));
	}

	if (AssetList.IsValid()) AssetList->RequestListRefresh();

	if (EmptyHint.IsValid())
	{
		const bool bEmpty = FilteredItems.IsEmpty();
		EmptyHint->SetVisibility(bEmpty ? EVisibility::Visible : EVisibility::Collapsed);
		if (bEmpty)
		{
			static const FText Hints[4] =
			{
				LOCTEXT("EmptyNPC",
					"No ForgeNPCProfile assets found.\n"
					"Right-click in Content Browser → Forge Core → ForgeNPCProfile to create one."),
				LOCTEXT("EmptyAnomaly",
					"No anomaly profile assets found.\n"
					"Create a ForgeAnomalyProfile or use your EotRAnomalyProfile assets."),
				LOCTEXT("EmptyDialogue",
					"No ForgeDialogueData assets found.\n"
					"Right-click in Content Browser → Forge Core → ForgeDialogueData to create one."),
				LOCTEXT("EmptyQuest",
					"No ForgeQuestData assets found.\n"
					"Right-click in Content Browser → Forge Core → ForgeQuestData to create one."),
			};
			EmptyHint->SetText(Hints[FMath::Clamp(Index, 0, 3)]);
		}
	}

	Invalidate(EInvalidateWidgetReason::Paint);
}

void SForgeProfileBrowser::QueryAssets()
{
	AllAssets.Empty();
	IAssetRegistry& AR = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();

	FARFilter Filter;
	Filter.bRecursivePaths   = true;
	Filter.bRecursiveClasses = true;
	Filter.PackagePaths.Add(FName("/Game"));
	Filter.ClassPaths.Add(UForgeNPCProfile::StaticClass()->GetClassPathName());
	Filter.ClassPaths.Add(UForgeAnomalyProfile::StaticClass()->GetClassPathName());
	Filter.ClassPaths.Add(UForgeDialogueData::StaticClass()->GetClassPathName());
	Filter.ClassPaths.Add(UForgeQuestData::StaticClass()->GetClassPathName());
	// Game-native anomaly profiles (EotRAnomalyProfile from EchoesOfTheRozlom module)
	Filter.ClassPaths.Add(FTopLevelAssetPath(TEXT("/Script/EchoesOfTheRozlom"), TEXT("EotRAnomalyProfile")));

	AR.GetAssets(Filter, AllAssets);
}

void SForgeProfileBrowser::QueryWorldActors()
{
	WorldItems.Empty();

	if (!GEditor) return;
	UWorld* World = GEditor->GetEditorWorldContext().World();
	if (!World) return;

	const FLinearColor CyanCol   (0.05f, 0.52f, 0.72f);
	const FLinearColor MagentaCol(0.62f, 0.10f, 0.56f);
	const FLinearColor RedCol    (0.62f, 0.10f, 0.20f);
	const FLinearColor GoldCol   (0.58f, 0.40f, 0.04f);

	auto Add = [this](AActor* A, const FText& Badge, const FLinearColor& Color)
	{
		if (!IsValid(A)) return;
		auto Entry       = MakeShared<FWorldActorEntry>();
		Entry->Actor     = A;
		Entry->Label     = A->GetActorLabel();
		Entry->TypeBadge = Badge;
		Entry->Color     = Color;
		WorldItems.Add(Entry);
	};

	for (TActorIterator<AForgeNPCActor>     It(World); It; ++It) Add(*It, LOCTEXT("BadgeNPC",     "NPC"),    CyanCol);
	for (TActorIterator<AForgeAnomalyActor> It(World); It; ++It) Add(*It, LOCTEXT("BadgeAnomaly", "Anomaly"),MagentaCol);
	for (TActorIterator<AForgeEnemyActor>   It(World); It; ++It) Add(*It, LOCTEXT("BadgeEnemy",   "Enemy"),  RedCol);
	for (TActorIterator<AForgePropActor>    It(World); It; ++It) Add(*It, LOCTEXT("BadgeProp",    "Prop"),   GoldCol);
}

void SForgeProfileBrowser::Refresh()
{
	QueryAssets();
	SetFilter(ActiveFilter);
}

// ─── Asset row widget ─────────────────────────────────────────────────────────

TSharedRef<ITableRow> SForgeProfileBrowser::MakeAssetRow(
	TSharedPtr<FAssetData> Item,
	const TSharedRef<STableViewBase>& Owner)
{
	const FLinearColor Dot = ColorForFilter(ActiveFilter);

	FString ActionHint;
	FText   BadgeText = FText::GetEmpty();
	FText   SubText   = FText::GetEmpty();

	if (ActiveFilter == 0) // NPC
	{
		ActionHint = TEXT("Double-click to spawn NPC actor");
		if (UForgeNPCProfile* Prof = Cast<UForgeNPCProfile>(Item->GetAsset()))
		{
			if (!Prof->DialogueData.IsNull())
			{
				BadgeText  = FText::FromString(TEXT("💬"));
				ActionHint += TEXT(" + open linked Dialogue");
			}
			if (!Prof->DisplayName.IsEmpty())
				SubText = Prof->DisplayName;
		}
	}
	else if (ActiveFilter == 1) // Anomaly
	{
		ActionHint = TEXT("Double-click to spawn Anomaly actor");
		if (UForgeAnomalyProfile* Prof = Cast<UForgeAnomalyProfile>(Item->GetAsset()))
		{
			if (!Prof->DisplayName.IsEmpty())
				SubText = Prof->DisplayName;
		}
		// Badge for game-native EotRAnomalyProfile assets
		if (Item->AssetClassPath.GetAssetName() == FName("EotRAnomalyProfile"))
			BadgeText = FText::FromString(TEXT("🔬"));
	}
	else
	{
		ActionHint = TEXT("Double-click to open");
	}

	return SNew(STableRow<TSharedPtr<FAssetData>>, Owner)
		.Padding(FMargin(4.f, 3.f))
		.ToolTipText(FText::FromString(ActionHint))
		[
			SNew(SHorizontalBox)

			// Colored dot
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(0.f, 0.f, 6.f, 0.f)
			[
				SNew(SBox).WidthOverride(8.f).HeightOverride(8.f)
				[
					SNew(SImage)
					.Image(FAppStyle::Get().GetBrush("Icons.BulletPoint"))
					.ColorAndOpacity(FSlateColor(Dot))
				]
			]

			// Asset name + optional display name sub-label
			+ SHorizontalBox::Slot().FillWidth(1.f).VAlign(VAlign_Center)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot().AutoHeight()
				[
					SNew(STextBlock)
					.Text(FText::FromName(Item->AssetName))
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
					.ColorAndOpacity(FSlateColor(FLinearColor(0.82f, 0.84f, 0.92f)))
				]
				+ SVerticalBox::Slot().AutoHeight()
				[
					SNew(STextBlock)
					.Text(SubText)
					.Visibility(SubText.IsEmpty() ? EVisibility::Collapsed : EVisibility::Visible)
					.Font(FCoreStyle::GetDefaultFontStyle("Italic", 7))
					.ColorAndOpacity(FSlateColor(FLinearColor(0.50f, 0.52f, 0.65f)))
				]
			]

			// Badge (💬 dialogue link | 🔬 game-native)
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(4.f, 0.f, 0.f, 0.f)
			[
				SNew(STextBlock)
				.Text(BadgeText)
				.Visibility(BadgeText.IsEmpty() ? EVisibility::Collapsed : EVisibility::Visible)
				.Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
				.ToolTipText(LOCTEXT("BadgeTip", "Game-native EotRAnomalyProfile asset"))
			]
		];
}

// ─── World actor row widget ───────────────────────────────────────────────────

TSharedRef<ITableRow> SForgeProfileBrowser::MakeWorldActorRow(
	TSharedPtr<FWorldActorEntry> Item,
	const TSharedRef<STableViewBase>& Owner)
{
	if (!Item.IsValid())
		return SNew(STableRow<TSharedPtr<FWorldActorEntry>>, Owner);

	return SNew(STableRow<TSharedPtr<FWorldActorEntry>>, Owner)
		.Padding(FMargin(4.f, 4.f))
		.ToolTipText(LOCTEXT("WorldActorTip", "Click to select and focus camera on this actor"))
		[
			SNew(SHorizontalBox)

			// Type-colored bullet
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(0.f, 0.f, 6.f, 0.f)
			[
				SNew(SBox).WidthOverride(8.f).HeightOverride(8.f)
				[
					SNew(SImage)
					.Image(FAppStyle::Get().GetBrush("Icons.BulletPoint"))
					.ColorAndOpacity(FSlateColor(Item->Color))
				]
			]

			// Actor label
			+ SHorizontalBox::Slot().FillWidth(1.f).VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(FText::FromString(Item->Label))
				.Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
				.ColorAndOpacity(FSlateColor(FLinearColor(0.82f, 0.84f, 0.92f)))
			]

			// Type badge
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(4.f, 0.f, 0.f, 0.f)
			[
				SNew(SBorder)
				.BorderBackgroundColor(Item->Color * 0.28f)
				.Padding(FMargin(5.f, 2.f))
				[
					SNew(STextBlock)
					.Text(Item->TypeBadge)
					.Font(FCoreStyle::GetDefaultFontStyle("Bold", 7))
					.ColorAndOpacity(FSlateColor(Item->Color))
				]
			]
		];
}

// ─── Click — Scene tab ────────────────────────────────────────────────────────

void SForgeProfileBrowser::OnWorldActorClicked(TSharedPtr<FWorldActorEntry> Item)
{
	if (!Item.IsValid() || !GEditor) return;
	AActor* Actor = Item->Actor.Get();
	if (!IsValid(Actor)) return;

	// Select the actor and move ALL perspective viewports to it
	GEditor->SelectNone(true, true);
	GEditor->SelectActor(Actor, true, true);
	GEditor->NoteSelectionChange();
	GEditor->MoveViewportCamerasToActor(*Actor, /*bActiveViewportOnly=*/false);
}

// ─── Double-click — asset tabs ────────────────────────────────────────────────

void SForgeProfileBrowser::OnAssetDoubleClicked(TSharedPtr<FAssetData> Item)
{
	if (!Item.IsValid()) return;

	if (ActiveFilter == 0) // NPC — spawn actor + open linked dialogue
	{
		SpawnAssetInWorld(*Item);

		if (UForgeNPCProfile* Prof = Cast<UForgeNPCProfile>(Item->GetAsset()))
		{
			if (!Prof->DialogueData.IsNull())
			{
				const FSoftObjectPath& SoftPath = Prof->DialogueData.ToSoftObjectPath();
				IAssetRegistry& AR = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
				const FAssetData DialogueAD = AR.GetAssetByObjectPath(SoftPath);
				if (DialogueAD.IsValid())
					OnDialogueOpened.ExecuteIfBound(DialogueAD);
			}
		}
	}
	else if (ActiveFilter == 1) // Anomaly — spawn actor only
	{
		SpawnAssetInWorld(*Item);
	}
	else if (ActiveFilter == 2)
	{
		OnDialogueOpened.ExecuteIfBound(*Item);
	}
	else
	{
		OnQuestOpened.ExecuteIfBound(*Item);
	}
}

void SForgeProfileBrowser::SpawnAssetInWorld(const FAssetData& Asset) const
{
	if (!GEditor) return;
	UWorld* World = GEditor->GetEditorWorldContext().World();
	if (!World) return;

	FVector Loc = FVector::ZeroVector;
	if (FViewport* VP = GEditor->GetActiveViewport())
	{
		if (FEditorViewportClient* C = static_cast<FEditorViewportClient*>(VP->GetClient()))
			Loc = C->GetViewLocation() + C->GetViewRotation().Vector() * 200.f;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActor* Spawned = nullptr;

	if (ActiveFilter == 0) // NPC
	{
		AForgeNPCActor* A = World->SpawnActor<AForgeNPCActor>(Loc, FRotator::ZeroRotator, Params);
		if (A) { A->Profile = Cast<UForgeNPCProfile>(Asset.GetAsset()); Spawned = A; }
	}
	else if (ActiveFilter == 1) // Anomaly (ForgeAnomalyProfile or EotRAnomalyProfile)
	{
		AForgeAnomalyActor* A = World->SpawnActor<AForgeAnomalyActor>(Loc, FRotator::ZeroRotator, Params);
		if (A)
		{
			if (UForgeAnomalyProfile* Prof = Cast<UForgeAnomalyProfile>(Asset.GetAsset()))
				A->Profile = Prof;
			else
				A->EotProfile = Cast<UPrimaryDataAsset>(Asset.GetAsset());
			Spawned = A;
		}
	}

	if (Spawned)
	{
		GEditor->SelectNone(true, true);
		GEditor->SelectActor(Spawned, true, true);
		GEditor->NoteSelectionChange();
	}
}

// ─── Helpers ─────────────────────────────────────────────────────────────────

FName SForgeProfileBrowser::ClassNameForFilter(int32 Filter)
{
	switch (Filter)
	{
	case 0:  return UForgeNPCProfile::StaticClass()->GetFName();
	case 1:  return UForgeAnomalyProfile::StaticClass()->GetFName();
	case 2:  return UForgeDialogueData::StaticClass()->GetFName();
	default: return UForgeQuestData::StaticClass()->GetFName();
	}
}

FLinearColor SForgeProfileBrowser::ColorForFilter(int32 Filter)
{
	return TabColors[FMath::Clamp(Filter, 0, 4)];
}

FText SForgeProfileBrowser::LabelForFilter(int32 Filter)
{
	switch (Filter)
	{
	case 0:  return LOCTEXT("TabNPC",      "NPC");
	case 1:  return LOCTEXT("TabAnomaly",  "Anomaly");
	case 2:  return LOCTEXT("TabDialogue", "Dialogue");
	case 3:  return LOCTEXT("TabQuest",    "Quest");
	default: return LOCTEXT("TabScene",    "Scene");
	}
}

#undef LOCTEXT_NAMESPACE
