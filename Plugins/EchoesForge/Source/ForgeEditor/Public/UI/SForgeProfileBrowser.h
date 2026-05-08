// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "AssetRegistry/AssetData.h"

DECLARE_DELEGATE_OneParam(FOnForgeAssetOpened, const FAssetData& /* Asset */);

// ─── World-actor list entry ────────────────────────────────────────────────────
// Lightweight descriptor built when the Scene tab is refreshed.
struct FWorldActorEntry
{
	TWeakObjectPtr<AActor> Actor;
	FString                Label;      // actor display label in the outliner
	FText                  TypeBadge;  // "NPC" / "Anomaly" / "Enemy" / "Prop"
	FLinearColor           Color;      // accent color matching the type
};

// Asset browser panel for all Forge data assets + placed scene actors.
// Tabs: NPC Profile | Anomaly Profile | Dialogue | Quest | Scene
// Double-click NPC/Anomaly → spawn actor at camera. Dialogue/Quest → open editor tab.
// Scene tab → click to select & focus viewport on the placed actor.
class FORGEEDITOR_API SForgeProfileBrowser : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SForgeProfileBrowser) {}
		SLATE_EVENT(FOnForgeAssetOpened, OnDialogueOpened)
		SLATE_EVENT(FOnForgeAssetOpened, OnQuestOpened)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	void Refresh();

private:
	FOnForgeAssetOpened OnDialogueOpened;
	FOnForgeAssetOpened OnQuestOpened;

	// ── Asset tab state (indices 0–3) ─────────────────────────────────────────
	int32 ActiveFilter = 0;   // 0=NPC 1=Anomaly 2=Dialogue 3=Quest 4=Scene
	TArray<FAssetData>                             AllAssets;
	TArray<TSharedPtr<FAssetData>>                 FilteredItems;
	TSharedPtr<SListView<TSharedPtr<FAssetData>>>  AssetList;
	TSharedPtr<STextBlock>                         EmptyHint;

	// ── Scene tab state (index 4) ─────────────────────────────────────────────
	TArray<TSharedPtr<FWorldActorEntry>>                 WorldItems;
	TSharedPtr<SListView<TSharedPtr<FWorldActorEntry>>>  WorldList;
	TSharedPtr<SBox>                                     AssetListBox;
	TSharedPtr<SBox>                                     WorldListBox;

	// 5 entries: NPC | Anomaly | Dialogue | Quest | Scene
	static const FLinearColor TabColors[5];

	void SetFilter(int32 Index);
	void QueryAssets();
	void QueryWorldActors();

	// Asset list
	TSharedRef<ITableRow> MakeAssetRow(
		TSharedPtr<FAssetData> Item,
		const TSharedRef<STableViewBase>& Owner);
	void OnAssetDoubleClicked(TSharedPtr<FAssetData> Item);
	void SpawnAssetInWorld(const FAssetData& Asset) const;

	// World actor list
	TSharedRef<ITableRow> MakeWorldActorRow(
		TSharedPtr<FWorldActorEntry> Item,
		const TSharedRef<STableViewBase>& Owner);
	void OnWorldActorClicked(TSharedPtr<FWorldActorEntry> Item);

	static FName        ClassNameForFilter(int32 Filter);
	static FLinearColor ColorForFilter(int32 Filter);
	static FText        LabelForFilter(int32 Filter);
};
