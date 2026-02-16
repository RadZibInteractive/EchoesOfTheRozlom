// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "UObject/GCObject.h"
#include "UObject/ObjectPtr.h"

struct FForgeCategoryRule;
class UTexture2D;
class STextBlock;
template <typename OptionType> class SComboBox;
class SEditableTextBox;
class SImage;

class SDAForgeCreateDialog : public SCompoundWidget, public FGCObject
{
public:
	SLATE_BEGIN_ARGS(SDAForgeCreateDialog) {}
		SLATE_ARGUMENT(TArray<const FForgeCategoryRule*>, CategoryRules)
		SLATE_ARGUMENT(FString, TargetPath)
		SLATE_EVENT(FSimpleDelegate, OnCancel)
		SLATE_EVENT(FSimpleDelegate, OnCreate)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	const FForgeCategoryRule* GetSelectedRule() const;
	const FString& GetSuffix() const { return Suffix; }
	const FString& GetTargetPath() const { return TargetPath; }
	TSoftObjectPtr<UTexture2D> GetOverrideIcon() const { return OverrideIcon; }

private:
	// FGCObject
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual FString GetReferencerName() const override { return TEXT("SDAForgeCreateDialog"); }

private:
	// Data
	TArray<const FForgeCategoryRule*> Rules;
	TArray<TSharedPtr<FName>> TypeOptions;
	TSharedPtr<FName> SelectedType;

	FString TargetPath;
	FString Suffix;

	// Widgets
	TSharedPtr<STextBlock> TargetPathText;
	TSharedPtr<STextBlock> InfoText;
	TSharedPtr<SComboBox<TSharedPtr<FName>>> Combo;
	TSharedPtr<SEditableTextBox> SuffixBox;
	TSharedPtr<SImage> TypeIconImage;

	// Textures
	TObjectPtr<UTexture2D> BackgroundTex = nullptr;
	TObjectPtr<UTexture2D> TypeIconTex = nullptr;
	TObjectPtr<UTexture2D> OverrideTex = nullptr;

	// Soft override
	TSoftObjectPtr<UTexture2D> OverrideIcon;

	// Brushes
	FSlateBrush BackgroundBrush;
	FSlateBrush TypeIconBrush;
	FSlateBrush OverrideIconBrush;

	// Visual refresh retry
	TSharedPtr<FActiveTimerHandle> VisualRefreshHandle;
	int32 VisualRefreshTries = 0;

private:
	// UI helpers
	const FForgeCategoryRule* FindRuleByType(FName Type) const;
	TSharedRef<SWidget> MakeTypeWidget(TSharedPtr<FName> Type) const;
	void OnTypeChanged(TSharedPtr<FName> NewValue, ESelectInfo::Type);

	void RefreshTextForSelection();
	FString ComputeTargetPathForRule(const FForgeCategoryRule& Rule) const;

	static FString NormalizeForDisplay(const FString& InPath);
	static FString ToGamePath(const FString& InPath);
	FText GetSelectedTypeText() const;
	bool CanCreate() const;

	// Visuals
	void RequestVisualRefresh();
	EActiveTimerReturnType TickVisualRefresh(double, float);

	void UpdateBrushes();
	static UTexture2D* LoadSoftTex(const TSoftObjectPtr<UTexture2D>& Soft);
	static void InitImageBrush(FSlateBrush& Brush, UTexture2D* Tex, FVector2D Size);
};
