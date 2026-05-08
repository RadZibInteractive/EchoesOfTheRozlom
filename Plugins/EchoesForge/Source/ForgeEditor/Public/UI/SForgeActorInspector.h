// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class IDetailsView;
class SForgePatrolPanel;
class SForgeAnomalyPanel;

// Shows the UE property panel for whichever Forge actor is currently
// selected in the viewport. Subscribes to USelection::SelectionChangedEvent.
// When an AForgeEnemyActor is selected, also shows the Patrol Path panel.
// When an AForgeAnomalyActor is selected, also shows the Anomaly Panel.
class FORGEEDITOR_API SForgeActorInspector : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SForgeActorInspector) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	virtual ~SForgeActorInspector() override;

private:
	TSharedPtr<IDetailsView>      DetailsView;
	TSharedPtr<STextBlock>        EmptyLabel;

	// Enemy — patrol path panel
	TSharedPtr<SForgePatrolPanel> PatrolPanel;
	TSharedPtr<SBorder>           PatrolPanelBorder;

	// Anomaly — radius / parameter panel
	TSharedPtr<SForgeAnomalyPanel> AnomalyPanel;
	TSharedPtr<SBorder>            AnomalyPanelBorder;

	void OnSelectionChanged(UObject* Obj);
	void RefreshFromSelection();

	static bool IsForgeActor(const AActor* Actor);
};
