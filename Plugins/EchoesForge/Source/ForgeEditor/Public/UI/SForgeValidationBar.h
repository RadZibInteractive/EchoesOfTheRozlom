// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Data/ForgeTypes.h"

struct FORGEEDITOR_API FForgeValidationItem
{
	EForgeValidationStatus Status  = EForgeValidationStatus::Valid;
	FText                  Message;
};

DECLARE_DELEGATE(FOnWorldPartitionMapClicked);

// Bottom status bar: [OK] ... [Warning] ... [Error] ... | [World Partition Map]
class FORGEEDITOR_API SForgeValidationBar : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SForgeValidationBar) {}
		SLATE_EVENT(FOnWorldPartitionMapClicked, OnWorldPartitionMapClicked)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	void SetItems(const TArray<FForgeValidationItem>& Items);
	void ClearItems();

private:
	FOnWorldPartitionMapClicked OnWorldPartitionMapClicked;
	TSharedPtr<SHorizontalBox>  ItemsRow;

	void RebuildItems(const TArray<FForgeValidationItem>& Items);

	static FText         GetPrefix(EForgeValidationStatus Status);
	static FSlateColor   GetColor(EForgeValidationStatus Status);
	static const FSlateBrush* GetDotBrush(EForgeValidationStatus Status);
};
