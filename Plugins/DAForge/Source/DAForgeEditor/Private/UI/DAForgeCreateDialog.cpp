// © 2026 RadZib. All rights reserved.

#include "UI/DAForgeCreateDialog.h"

#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/SBoxPanel.h"
#include "PropertyCustomizationHelpers.h"

#include "Settings/DAForgeSettings.h"
#include "Engine/Texture2D.h"

namespace DAForge::Private
{
	static FText MakeInfoText(const FForgeCategoryRule* Rule)
	{
		if (!Rule) return FText::GetEmpty();

		const FString ClassName = Rule->AssetClass.IsNull()
			? TEXT("None")
			: Rule->AssetClass.ToString();

		return FText::FromString(FString::Printf(
			TEXT("Class: %s  |  Open after create: %s"),
			*ClassName,
			Rule->bOpenAfterCreate ? TEXT("Yes") : TEXT("No")
		));
	}
}

// ----------------------------- GC -----------------------------

void SDAForgeCreateDialog::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(BackgroundTex);
	Collector.AddReferencedObject(TypeIconTex);
	Collector.AddReferencedObject(OverrideTex);
}

// ----------------------------- Helpers -----------------------------

UTexture2D* SDAForgeCreateDialog::LoadSoftTex(const TSoftObjectPtr<UTexture2D>& Soft)
{
	return Soft.IsNull() ? nullptr : Soft.LoadSynchronous();
}

void SDAForgeCreateDialog::InitImageBrush(FSlateBrush& Brush, UTexture2D* Tex, FVector2D Size)
{
	Brush = FSlateBrush();
	Brush.DrawAs = ESlateBrushDrawType::Image;
	Brush.ImageSize = Size;

	if (Tex)
	{
		Tex->UpdateResource();
		Brush.SetResourceObject(Tex);
	}
}

const FForgeCategoryRule* SDAForgeCreateDialog::FindRuleByType(FName Type) const
{
	if (Type.IsNone()) return nullptr;

	for (const FForgeCategoryRule* Rule : Rules)
	{
		if (Rule && Rule->PrimaryAssetType == Type)
		{
			return Rule;
		}
	}
	return nullptr;
}

const FForgeCategoryRule* SDAForgeCreateDialog::GetSelectedRule() const
{
	return FindRuleByType(SelectedType.IsValid() ? *SelectedType : NAME_None);
}

FString SDAForgeCreateDialog::ComputeTargetPathForRule(const FForgeCategoryRule& Rule) const
{
	const FString Root = Rule.RootPath.Path.IsEmpty() ? TEXT("/Game/Data") : Rule.RootPath.Path;
	return ToGamePath(Root);
}

FString SDAForgeCreateDialog::ToGamePath(const FString& InPath)
{
	static const FString AllPrefix = TEXT("/All");
	if (InPath.StartsWith(AllPrefix))
	{
		return InPath.Mid(AllPrefix.Len());
	}
	return InPath;
}

FString SDAForgeCreateDialog::NormalizeForDisplay(const FString& InPath)
{
	if (InPath.StartsWith(TEXT("/Game")))
	{
		return FString(TEXT("/All")) + InPath;
	}
	return InPath;
}

FText SDAForgeCreateDialog::GetSelectedTypeText() const
{
	const FName T = SelectedType.IsValid() ? *SelectedType : NAME_None;
	const FForgeCategoryRule* Rule = FindRuleByType(T);

	if (Rule && !Rule->DisplayTitle.IsEmpty())
	{
		return Rule->DisplayTitle;
	}

	return T.IsNone() ? FText::FromString(TEXT("Select type...")) : FText::FromName(T);
}

bool SDAForgeCreateDialog::CanCreate() const
{
	return GetSelectedRule() != nullptr;
}

TSharedRef<SWidget> SDAForgeCreateDialog::MakeTypeWidget(TSharedPtr<FName> Type) const
{
	const FName T = Type.IsValid() ? *Type : NAME_None;
	const FForgeCategoryRule* Rule = FindRuleByType(T);

	const FText Title =
		(Rule && !Rule->DisplayTitle.IsEmpty())
		? Rule->DisplayTitle
		: (T.IsNone() ? FText::FromString(TEXT("Invalid")) : FText::FromName(T));

	return SNew(STextBlock).Text(Title);
}

// ----------------------------- Visual refresh (retry) -----------------------------

void SDAForgeCreateDialog::RequestVisualRefresh()
{
	VisualRefreshTries = 0;

	if (!VisualRefreshHandle.IsValid())
	{
		VisualRefreshHandle = RegisterActiveTimer(
			0.1f,
			FWidgetActiveTimerDelegate::CreateSP(this, &SDAForgeCreateDialog::TickVisualRefresh));
	}

	UpdateBrushes();
	Invalidate(EInvalidateWidget::PaintAndVolatility);
}

EActiveTimerReturnType SDAForgeCreateDialog::TickVisualRefresh(double, float)
{
	UpdateBrushes();
	Invalidate(EInvalidateWidget::PaintAndVolatility);

	const bool bBgReady = (BackgroundBrush.GetResourceObject() != nullptr);
	const bool bTypeReady = (TypeIconBrush.GetResourceObject() != nullptr);

	if ((bBgReady && bTypeReady) || VisualRefreshTries++ > 40)
	{
		VisualRefreshHandle.Reset();
		return EActiveTimerReturnType::Stop;
	}

	return EActiveTimerReturnType::Continue;
}

void SDAForgeCreateDialog::UpdateBrushes()
{
	// Background (plugin texture)
	if (!BackgroundTex)
	{
		BackgroundTex = LoadObject<UTexture2D>(nullptr, TEXT("/DAForge/UI/Image.Image"));
	}
	InitImageBrush(BackgroundBrush, BackgroundTex, FVector2D(600.f, 280.f));

	// Type icon from settings
	TypeIconTex = nullptr;
	const FForgeCategoryRule* Rule = GetSelectedRule();
	if (Rule)
	{
		TypeIconTex = LoadSoftTex(Rule->DefaultIcon);
	}
	InitImageBrush(TypeIconBrush, TypeIconTex, FVector2D(32.f, 32.f));

	// Override icon
	OverrideTex = LoadSoftTex(OverrideIcon);
	InitImageBrush(OverrideIconBrush, OverrideTex, FVector2D(40.f, 40.f));

	// Text fields
	RefreshTextForSelection();
}

void SDAForgeCreateDialog::RefreshTextForSelection()
{
	const FForgeCategoryRule* Rule = GetSelectedRule();
	if (!Rule)
	{
		if (TargetPathText.IsValid()) TargetPathText->SetText(FText::FromString(TEXT("Target folder: <invalid type>")));
		if (InfoText.IsValid()) InfoText->SetText(FText::GetEmpty());
		return;
	}

	TargetPath = ComputeTargetPathForRule(*Rule);

	if (TargetPathText.IsValid())
	{
		TargetPathText->SetText(
			FText::FromString(FString::Printf(TEXT("Target folder: %s"), *NormalizeForDisplay(TargetPath))));
	}

	if (InfoText.IsValid())
	{
		InfoText->SetText(DAForge::Private::MakeInfoText(Rule));
	}
}

// ----------------------------- Events -----------------------------

void SDAForgeCreateDialog::OnTypeChanged(TSharedPtr<FName> NewValue, ESelectInfo::Type)
{
	SelectedType = NewValue;

	OverrideIcon.Reset();

	RequestVisualRefresh();
}

// ----------------------------- Construct -----------------------------

void SDAForgeCreateDialog::Construct(const FArguments& InArgs)
{
	Rules = InArgs._CategoryRules;
	TargetPath = ToGamePath(InArgs._TargetPath);

	TypeOptions.Reset();
	TypeOptions.Reserve(Rules.Num());

	for (const FForgeCategoryRule* Rule : Rules)
	{
		if (Rule && !Rule->PrimaryAssetType.IsNone())
		{
			TypeOptions.Add(MakeShared<FName>(Rule->PrimaryAssetType));
		}
	}

	if (TypeOptions.Num() > 0)
	{
		SelectedType = TypeOptions[0];
	}

	ChildSlot
		[
			SNew(SOverlay)

				+ SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SNew(SScaleBox)
						.Stretch(EStretch::ScaleToFill)
						[
							SNew(SImage).Image(&BackgroundBrush)
						]
				]

			+ SOverlay::Slot()
				.Padding(0.f)
				[
					SNew(SVerticalBox)

						// Type label + icon
						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(12.f, 10.f, 12.f, 6.f)
						[
							SNew(SHorizontalBox)

								+ SHorizontalBox::Slot()
								.AutoWidth()
								.VAlign(VAlign_Center)
								.Padding(0.f, 0.f, 8.f, 0.f)
								[
									SAssignNew(TypeIconImage, SImage)
										.Image(&TypeIconBrush)
								]

								+ SHorizontalBox::Slot()
								.FillWidth(1.f)
								.VAlign(VAlign_Center)
								[
									SNew(STextBlock).Text(FText::FromString(TEXT("Type")))
								]
						]

					// Type combo
					+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(12.f, 0.f, 12.f, 10.f)
						[
							SAssignNew(Combo, SComboBox<TSharedPtr<FName>>)
								.OptionsSource(&TypeOptions)
								.OnGenerateWidget(this, &SDAForgeCreateDialog::MakeTypeWidget)
								.OnSelectionChanged(this, &SDAForgeCreateDialog::OnTypeChanged)
								[
									SNew(STextBlock).Text(this, &SDAForgeCreateDialog::GetSelectedTypeText)
								]
						]

					// Icon override label
					+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(12.f, 0.f, 12.f, 6.f)
						[
							SNew(STextBlock).Text(FText::FromString(TEXT("Icon (optional override)")))
						]

						// Icon override row
						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(12.f, 0.f, 12.f, 10.f)
						[
							SNew(SHorizontalBox)

								+ SHorizontalBox::Slot()
								.AutoWidth()
								.VAlign(VAlign_Center)
								.Padding(0.f, 0.f, 8.f, 0.f)
								[
									SNew(SBox)
										.WidthOverride(40.f)
										.HeightOverride(40.f)
										[
											SNew(SImage)
												.Image_Lambda([this]() -> const FSlateBrush*
													{
														return OverrideIcon.IsNull() ? &TypeIconBrush : &OverrideIconBrush;
													})
										]
								]

							+ SHorizontalBox::Slot()
								.FillWidth(1.f)
								.VAlign(VAlign_Center)
								[
									SNew(SObjectPropertyEntryBox)
										.AllowedClass(UTexture2D::StaticClass())
										.DisplayThumbnail(false)

										.ObjectPath_Lambda([this]() -> FString
											{
												const FForgeCategoryRule* Rule = GetSelectedRule();
												if (!Rule) return FString();

												const TSoftObjectPtr<UTexture2D> Effective = OverrideIcon.IsNull()
													? Rule->DefaultIcon
													: OverrideIcon;

												return Effective.IsNull() ? FString() : Effective.ToSoftObjectPath().ToString();
											})

										.OnObjectChanged_Lambda([this](const FAssetData& AssetData)
											{
												const FSoftObjectPath Path = AssetData.GetSoftObjectPath();
												OverrideIcon = Path.IsValid() ? TSoftObjectPtr<UTexture2D>(Path) : TSoftObjectPtr<UTexture2D>();
												RequestVisualRefresh();
											})
								]
						]

					// Suffix label
					+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(12.f, 0.f, 12.f, 6.f)
						[
							SNew(STextBlock).Text(FText::FromString(TEXT("Suffix (optional)")))
						]

						// Suffix input
						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(12.f, 0.f, 12.f, 10.f)
						[
							SAssignNew(SuffixBox, SEditableTextBox)
								.MinDesiredWidth(420.f)
								.HintText(FText::FromString(TEXT("e.g. Forge (leave empty to auto-number)")))
								.OnTextChanged_Lambda([this](const FText& T) { Suffix = T.ToString(); })
						]

						// Target path
						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(12.f, 0.f, 12.f, 6.f)
						[
							SAssignNew(TargetPathText, STextBlock)
								.ColorAndOpacity(FSlateColor(FLinearColor(1.f, 1.f, 1.f, 0.6f)))
						]

						// Info
						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(12.f, 0.f, 12.f, 12.f)
						[
							SAssignNew(InfoText, STextBlock)
								.ColorAndOpacity(FSlateColor(FLinearColor(1.f, 1.f, 1.f, 0.45f)))
						]

						// Buttons
						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(12.f, 0.f, 12.f, 12.f)
						.HAlign(HAlign_Right)
						[
							SNew(SUniformGridPanel)
								.SlotPadding(FMargin(6.f, 0.f))

								+ SUniformGridPanel::Slot(0, 0)
								[
									SNew(SButton)
										.Text(FText::FromString(TEXT("Cancel")))
										.OnClicked_Lambda([OnCancel = InArgs._OnCancel]()
											{
												OnCancel.ExecuteIfBound();
												return FReply::Handled();
											})
								]

							+ SUniformGridPanel::Slot(1, 0)
								[
									SNew(SButton)
										.IsEnabled_Lambda([this]() { return CanCreate(); })
										.Text(FText::FromString(TEXT("Create")))
										.OnClicked_Lambda([OnCreate = InArgs._OnCreate]()
											{
												OnCreate.ExecuteIfBound();
												return FReply::Handled();
											})
								]
						]
				]
		];

	RequestVisualRefresh();
}
