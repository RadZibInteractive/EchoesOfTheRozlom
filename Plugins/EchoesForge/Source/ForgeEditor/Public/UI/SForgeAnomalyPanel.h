// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/SForgePanel.h"
#include "UI/SForgeButton.h"
#include "Data/ForgeAnomalyProfile.h"

class AForgeAnomalyActor;

// ─── SForgeAnomalyPanel ───────────────────────────────────────────────────────
//
// Anomaly info display + radius visualizer.  Inherits SForgePanel:
//
//  GetPanelTitle()     → "Anomaly Parameters"
//  GetTitleColor()     → violet
//  BuildHeaderExtras() → [👁 Visualize] toggle button
//  BuildContent()      → stat grid (Type / Radius / Damage/s / Rozlom×)
//  BuildFooter()       → (empty)
//
// Public API:
//  SetAnomaly(AForgeAnomalyActor*)  — bind to an actor (nullptr to clear)

class FORGEEDITOR_API SForgeAnomalyPanel : public SForgePanel
{
public:
	SLATE_BEGIN_ARGS(SForgeAnomalyPanel) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	virtual ~SForgeAnomalyPanel() override;

	/** Bind to a specific anomaly actor.  Pass nullptr to clear. */
	void SetAnomaly(AForgeAnomalyActor* Anomaly);

protected:
	// ── SForgePanel overrides ─────────────────────────────────────────────────

	virtual FText        GetPanelTitle()          const override;
	virtual FLinearColor GetTitleColor()          const override;
	virtual TSharedRef<SWidget> BuildHeaderExtras()     override;
	virtual TSharedRef<SWidget> BuildContent()          override;

private:
	// ── State ────────────────────────────────────────────────────────────────

	TWeakObjectPtr<AForgeAnomalyActor> CurrentAnomaly;
	bool bVisualizationActive = false;

	// Live stat labels — updated by RefreshInfo()
	TSharedPtr<STextBlock> TypeLabel;
	TSharedPtr<STextBlock> RadiusLabel;
	TSharedPtr<STextBlock> DamageLabel;
	TSharedPtr<STextBlock> RozlomLabel;
	TSharedPtr<STextBlock> VizButtonLabel;

	// ── Helpers ───────────────────────────────────────────────────────────────

	void RefreshInfo();
	void VisualizeRadius();
	void ClearVisualization();

	FReply OnToggleVisualize();

	static FText        AnomalyTypeName (EForgeAnomalyType T);
	static FLinearColor AnomalyTypeColor(EForgeAnomalyType T);
	static FText        DamageTypeName  (EForgeDamageType  D);
};
