// © 2026 RadZib. All rights reserved.

#include "UI/SForgeActorInspector.h"
#include "UI/SForgePatrolPanel.h"
#include "UI/SForgeAnomalyPanel.h"
#include "Actors/ForgeWorldActors.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "Editor.h"
#include "Selection.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Styling/AppStyle.h"

#define LOCTEXT_NAMESPACE "ForgeActorInspector"

void SForgeActorInspector::Construct(const FArguments& InArgs)
{
	FPropertyEditorModule& PEM =
		FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	FDetailsViewArgs Args;
	Args.bHideSelectionTip = true;
	Args.bAllowSearch      = true;
	Args.bShowScrollBar    = true;
	Args.NameAreaSettings  = FDetailsViewArgs::HideNameArea;
	DetailsView = PEM.CreateDetailView(Args);

	ChildSlot
	[
		SNew(SScrollBox)

		// Header
		+ SScrollBox::Slot().Padding(8.f, 6.f, 8.f, 4.f)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("Header", "Selected Actor"))
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
			.ColorAndOpacity(FSlateColor(FLinearColor(0.80f, 0.82f, 0.92f)))
		]

		// Empty-state label
		+ SScrollBox::Slot().Padding(8.f, 4.f)
		[
			SAssignNew(EmptyLabel, STextBlock)
			.Text(LOCTEXT("Empty", "Click a Forge actor in the viewport to inspect it."))
			.AutoWrapText(true)
			.Font(FCoreStyle::GetDefaultFontStyle("Italic", 9))
			.ColorAndOpacity(FSlateColor(FLinearColor(0.45f, 0.47f, 0.58f)))
		]

		// Property panel
		+ SScrollBox::Slot()
		[
			SNew(SBox).MinDesiredHeight(200.f)
			[
				DetailsView.ToSharedRef()
			]
		]

		// ── Patrol section (visible only for AForgeEnemyActor) ──────────────────
		+ SScrollBox::Slot().Padding(8.f, 0.f)
		[
			SAssignNew(PatrolPanelBorder, SBorder)
			.BorderBackgroundColor(FLinearColor(0.08f, 0.10f, 0.18f))
			.Padding(FMargin(8.f, 6.f))
			.Visibility(EVisibility::Collapsed)
			[
				SNew(SVerticalBox)

				+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 0.f, 0.f, 4.f)
				[
					SNew(SSeparator)
					.SeparatorImage(FAppStyle::Get().GetBrush("Menu.Separator"))
				]

				+ SVerticalBox::Slot().AutoHeight()
				[
					SAssignNew(PatrolPanel, SForgePatrolPanel)
				]
			]
		]

		// ── Anomaly section (visible only for AForgeAnomalyActor) ───────────────
		+ SScrollBox::Slot().Padding(8.f, 0.f, 8.f, 8.f)
		[
			SAssignNew(AnomalyPanelBorder, SBorder)
			.BorderBackgroundColor(FLinearColor(0.09f, 0.06f, 0.16f))
			.Padding(FMargin(8.f, 6.f))
			.Visibility(EVisibility::Collapsed)
			[
				SNew(SVerticalBox)

				+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 0.f, 0.f, 4.f)
				[
					SNew(SSeparator)
					.SeparatorImage(FAppStyle::Get().GetBrush("Menu.Separator"))
				]

				+ SVerticalBox::Slot().AutoHeight()
				[
					SAssignNew(AnomalyPanel, SForgeAnomalyPanel)
				]
			]
		]
	];

	// Subscribe to selection changes
	if (GEditor)
	{
		GEditor->GetSelectedActors()->SelectionChangedEvent.AddRaw(
			this, &SForgeActorInspector::OnSelectionChanged);
	}

	RefreshFromSelection();
}

SForgeActorInspector::~SForgeActorInspector()
{
	if (GEditor)
	{
		GEditor->GetSelectedActors()->SelectionChangedEvent.RemoveAll(this);
	}
}

void SForgeActorInspector::OnSelectionChanged(UObject* /*Obj*/)
{
	RefreshFromSelection();
}

void SForgeActorInspector::RefreshFromSelection()
{
	if (!GEditor || !DetailsView.IsValid()) return;

	TArray<AActor*>      SelectedActors;
	AForgeEnemyActor*    SelectedEnemy   = nullptr;
	AForgeAnomalyActor*  SelectedAnomaly = nullptr;

	for (FSelectionIterator It(*GEditor->GetSelectedActors()); It; ++It)
	{
		if (AActor* A = Cast<AActor>(*It))
		{
			if (IsForgeActor(A))
			{
				SelectedActors.Add(A);

				// Only single-selection gets the specialized panels
				if (SelectedActors.Num() == 1)
				{
					SelectedEnemy   = Cast<AForgeEnemyActor>(A);
					SelectedAnomaly = Cast<AForgeAnomalyActor>(A);
				}
			}
		}
	}

	const bool bHasActors = SelectedActors.Num() > 0;

	if (EmptyLabel.IsValid())
	{
		EmptyLabel->SetVisibility(bHasActors ? EVisibility::Collapsed : EVisibility::Visible);
	}
	DetailsView->SetVisibility(bHasActors ? EVisibility::Visible : EVisibility::Collapsed);

	if (bHasActors)
	{
		TArray<UObject*> Objects;
		for (AActor* A : SelectedActors) Objects.Add(A);
		DetailsView->SetObjects(Objects);
	}

	// Patrol panel — enemies only
	if (PatrolPanelBorder.IsValid())
	{
		PatrolPanelBorder->SetVisibility(SelectedEnemy ? EVisibility::Visible : EVisibility::Collapsed);
	}
	if (PatrolPanel.IsValid())
	{
		PatrolPanel->SetEnemy(SelectedEnemy);
	}

	// Anomaly panel — anomaly actors only
	if (AnomalyPanelBorder.IsValid())
	{
		AnomalyPanelBorder->SetVisibility(SelectedAnomaly ? EVisibility::Visible : EVisibility::Collapsed);
	}
	if (AnomalyPanel.IsValid())
	{
		AnomalyPanel->SetAnomaly(SelectedAnomaly);
	}
}

bool SForgeActorInspector::IsForgeActor(const AActor* Actor)
{
	return Actor &&
		(Actor->IsA<AForgeNPCActor>()     ||
		 Actor->IsA<AForgeAnomalyActor>() ||
		 Actor->IsA<AForgeEnemyActor>()   ||
		 Actor->IsA<AForgePropActor>());
}

#undef LOCTEXT_NAMESPACE
