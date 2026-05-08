// © 2026 RadZib. All rights reserved.

#include "UI/SForgeAnomalyPanel.h"
#include "Actors/ForgeWorldActors.h"
#include "Utils/ForgeUIStyle.h"
#include "Editor.h"
#include "DrawDebugHelpers.h"

#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "Styling/AppStyle.h"

#define LOCTEXT_NAMESPACE "ForgeAnomalyPanel"

// ─── Construct ───────────────────────────────────────────────────────────────

void SForgeAnomalyPanel::Construct(const FArguments& /*InArgs*/)
{
	BuildLayout();
}

SForgeAnomalyPanel::~SForgeAnomalyPanel()
{
	ClearVisualization();
}

// ─── SForgePanel overrides ────────────────────────────────────────────────────

FText SForgeAnomalyPanel::GetPanelTitle() const
{
	return LOCTEXT("Title", "Anomaly Parameters");
}

FLinearColor SForgeAnomalyPanel::GetTitleColor() const
{
	return FLinearColor(0.72f, 0.12f, 0.64f);   // brand magenta
}

TSharedRef<SWidget> SForgeAnomalyPanel::BuildHeaderExtras()
{
	// Toggle button: "👁 Visualize" ↔ "✕ Clear"
	// VizButtonLabel is stored so OnToggleVisualize() can swap the text.
	return SNew(SForgeButton)
		.Preset(EForgeButtonPreset::Violet)
		.ContentPadding(FMargin(6.f, 2.f))
		.ToolTipText(LOCTEXT("VizTip", "Draw anomaly radius sphere in viewport"))
		.OnClicked(this, &SForgeAnomalyPanel::OnToggleVisualize)
		[
			SAssignNew(VizButtonLabel, STextBlock)
			.Text(LOCTEXT("VizBtn", "👁 Visualize"))
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", 8))
			.ColorAndOpacity(FSlateColor(FLinearColor(0.80f, 0.60f, 1.00f)))
		];
}

TSharedRef<SWidget> SForgeAnomalyPanel::BuildContent()
{
	// Two-column label / value grid
	auto MakeRow = [](const FText& Key,
		TSharedPtr<STextBlock>& ValueLabel) -> TSharedRef<SWidget>
	{
		return SNew(SHorizontalBox)

			+ SHorizontalBox::Slot().FillWidth(0.45f)
			[
				SNew(STextBlock)
				.Text(Key)
				.Font(FCoreStyle::GetDefaultFontStyle("Regular", 8))
				.ColorAndOpacity(FSlateColor(FLinearColor(0.45f, 0.47f, 0.58f)))
			]

			+ SHorizontalBox::Slot().FillWidth(0.55f)
			[
				SAssignNew(ValueLabel, STextBlock)
				.Text(LOCTEXT("Dash", "—"))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 8))
				.ColorAndOpacity(FSlateColor(FLinearColor(0.80f, 0.82f, 0.92f)))
			];
	};

	return SNew(SVerticalBox)

		+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 2.f)
		[ MakeRow(LOCTEXT("TypeLbl",   "Type"),     TypeLabel)   ]

		+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 2.f)
		[ MakeRow(LOCTEXT("RadiusLbl", "Radius"),   RadiusLabel) ]

		+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 2.f)
		[ MakeRow(LOCTEXT("DmgLbl",    "Damage/s"), DamageLabel) ]

		+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 2.f)
		[ MakeRow(LOCTEXT("RozlomLbl", "Rozlom ×"), RozlomLabel) ];
}

// ─── SetAnomaly ───────────────────────────────────────────────────────────────

void SForgeAnomalyPanel::SetAnomaly(AForgeAnomalyActor* Anomaly)
{
	ClearVisualization();
	CurrentAnomaly = Anomaly;
	RefreshInfo();
}

// ─── RefreshInfo ─────────────────────────────────────────────────────────────

void SForgeAnomalyPanel::RefreshInfo()
{
	AForgeAnomalyActor*   A = CurrentAnomaly.Get();
	UForgeAnomalyProfile* P = A ? A->Profile : nullptr;

	const FText Dash = LOCTEXT("None", "—");

	if (!A || !P)
	{
		if (TypeLabel.IsValid())   TypeLabel->SetText(Dash);
		if (RadiusLabel.IsValid()) RadiusLabel->SetText(Dash);
		if (DamageLabel.IsValid()) DamageLabel->SetText(Dash);
		if (RozlomLabel.IsValid()) RozlomLabel->SetText(Dash);
		return;
	}

	const float EffRadius = A->RadiusOverride > 0.f ? A->RadiusOverride : P->Radius;

	if (TypeLabel.IsValid())
	{
		TypeLabel->SetText(AnomalyTypeName(P->AnomalyType));
		TypeLabel->SetColorAndOpacity(FSlateColor(AnomalyTypeColor(P->AnomalyType)));
	}
	if (RadiusLabel.IsValid())
	{
		RadiusLabel->SetText(FText::FromString(
			FString::Printf(TEXT("%.0f cm%s"),
				EffRadius,
				A->RadiusOverride > 0.f ? TEXT(" (override)") : TEXT(""))));
	}
	if (DamageLabel.IsValid())
	{
		DamageLabel->SetText(FText::FromString(
			FString::Printf(TEXT("%.1f  [%s]"),
				P->DamagePerSecond, *DamageTypeName(P->DamageType).ToString())));
	}
	if (RozlomLabel.IsValid())
	{
		// Red if above balance threshold (mirrors ForgeValidator warning)
		const FLinearColor RColor = P->RozlomRechargeMultiplier > 5.f
			? FLinearColor(0.85f, 0.25f, 0.10f)
			: FLinearColor(0.80f, 0.82f, 0.92f);

		RozlomLabel->SetText(FText::AsNumber(P->RozlomRechargeMultiplier));
		RozlomLabel->SetColorAndOpacity(FSlateColor(RColor));
	}
}

// ─── Visualization ────────────────────────────────────────────────────────────

FReply SForgeAnomalyPanel::OnToggleVisualize()
{
	if (bVisualizationActive)
	{
		ClearVisualization();
		if (VizButtonLabel.IsValid())
			VizButtonLabel->SetText(LOCTEXT("VizBtn", "👁 Visualize"));
	}
	else
	{
		VisualizeRadius();
		if (VizButtonLabel.IsValid())
			VizButtonLabel->SetText(LOCTEXT("VizBtnStop", "✕ Clear"));
	}
	return FReply::Handled();
}

void SForgeAnomalyPanel::VisualizeRadius()
{
	AForgeAnomalyActor*   A = CurrentAnomaly.Get();
	UForgeAnomalyProfile* P = A ? A->Profile : nullptr;
	if (!A || !GEditor) return;

	UWorld* World = GEditor->GetEditorWorldContext().World();
	if (!World) return;

	FlushPersistentDebugLines(World);

	const float   EffRadius = (A->RadiusOverride > 0.f) ? A->RadiusOverride : (P ? P->Radius : 500.f);
	const float   Falloff   = P ? P->Falloff : 0.5f;
	const FVector Origin    = A->GetActorLocation();

	// Outer boundary — brand magenta
	DrawDebugSphere(World, Origin, EffRadius, 24, FColor(210, 40, 190), true, -1.f, 0, 2.f);

	// Inner falloff zone — magenta dim
	if (Falloff > 0.01f)
	{
		DrawDebugSphere(World, Origin, EffRadius * (1.f - Falloff),
			16, FColor(145, 25, 130), true, -1.f, 0, 1.f);
	}

	// Centre point + vertical axis guide
	DrawDebugPoint(World, Origin, 10.f, FColor(240, 80, 220), true);
	DrawDebugLine(World,
		Origin - FVector(0, 0, EffRadius),
		Origin + FVector(0, 0, EffRadius),
		FColor(210, 40, 190), true, -1.f, 0, 1.f);

	bVisualizationActive = true;
}

void SForgeAnomalyPanel::ClearVisualization()
{
	if (!bVisualizationActive || !GEditor) return;

	if (UWorld* World = GEditor->GetEditorWorldContext().World())
	{
		FlushPersistentDebugLines(World);
	}
	bVisualizationActive = false;
}

// ─── Static helpers ──────────────────────────────────────────────────────────

FText SForgeAnomalyPanel::AnomalyTypeName(EForgeAnomalyType T)
{
	switch (T)
	{
	case EForgeAnomalyType::Gravitational: return LOCTEXT("AGrav",  "Gravitational");
	case EForgeAnomalyType::Chemical:      return LOCTEXT("AChem",  "Chemical");
	case EForgeAnomalyType::Electrical:    return LOCTEXT("AElec",  "Electrical");
	case EForgeAnomalyType::Thermal:       return LOCTEXT("ATherm", "Thermal");
	case EForgeAnomalyType::Magnetic:      return LOCTEXT("AMag",   "Magnetic");
	default:                               return LOCTEXT("AUnk",   "Unknown");
	}
}

FLinearColor SForgeAnomalyPanel::AnomalyTypeColor(EForgeAnomalyType T)
{
	switch (T)
	{
	case EForgeAnomalyType::Gravitational: return FLinearColor(0.65f, 0.10f, 0.58f);  // magenta
	case EForgeAnomalyType::Chemical:      return FLinearColor(0.20f, 0.75f, 0.20f);  // green (keep)
	case EForgeAnomalyType::Electrical:    return FLinearColor(0.10f, 0.65f, 0.88f);  // cyan
	case EForgeAnomalyType::Thermal:       return FLinearColor(0.80f, 0.50f, 0.05f);  // gold
	case EForgeAnomalyType::Magnetic:      return FLinearColor(0.05f, 0.45f, 0.72f);  // deep cyan
	default:                               return FLinearColor(0.35f, 0.40f, 0.48f);
	}
}

FText SForgeAnomalyPanel::DamageTypeName(EForgeDamageType D)
{
	switch (D)
	{
	case EForgeDamageType::Physical: return LOCTEXT("DPhys",   "Physical");
	case EForgeDamageType::Fire:     return LOCTEXT("DFire",   "Fire");
	case EForgeDamageType::Electric: return LOCTEXT("DElec",   "Electric");
	case EForgeDamageType::Chemical: return LOCTEXT("DChem",   "Chemical");
	case EForgeDamageType::Rozlom:   return LOCTEXT("DRozlom", "Rozlom");
	default:                         return LOCTEXT("DUnk",    "Unknown");
	}
}

#undef LOCTEXT_NAMESPACE
