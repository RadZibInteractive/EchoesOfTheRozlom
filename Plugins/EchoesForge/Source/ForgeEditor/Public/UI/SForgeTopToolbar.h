// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "UI/SForgeButton.h"

DECLARE_DELEGATE(FOnForgeAction);
DECLARE_DELEGATE_OneParam(FOnForgeToggle, bool /* bIsActive */);

// Top toolbar: Undo/Redo/Save | Simulate | Ghost Mode | Validate | Publish
class FORGEEDITOR_API SForgeTopToolbar : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SForgeTopToolbar)
		: _bSimulateActive(false)
		, _bGhostModeActive(false)
	{}
		SLATE_ATTRIBUTE(bool, bSimulateActive)
		SLATE_ATTRIBUTE(bool, bGhostModeActive)

		SLATE_EVENT(FOnForgeAction, OnUndo)
		SLATE_EVENT(FOnForgeAction, OnRedo)
		SLATE_EVENT(FOnForgeAction, OnSave)
		SLATE_EVENT(FOnForgeAction, OnSimulate)
		SLATE_EVENT(FOnForgeToggle, OnGhostModeToggled)
		SLATE_EVENT(FOnForgeAction, OnValidate)
		SLATE_EVENT(FOnForgeAction, OnPublish)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	void SetSimulateActive(bool bActive);
	void SetGhostModeActive(bool bActive);

private:
	TAttribute<bool> bSimulateActive;
	TAttribute<bool> bGhostModeActive;

	FOnForgeAction  OnUndo;
	FOnForgeAction  OnRedo;
	FOnForgeAction  OnSave;
	FOnForgeAction  OnSimulate;
	FOnForgeToggle  OnGhostModeToggled;
	FOnForgeAction  OnValidate;
	FOnForgeAction  OnPublish;

	bool bGhostActive = false;

	FReply HandleSimulate();
	FReply HandleGhostMode();
	FReply HandleValidate();
	FReply HandlePublish();
	FReply HandleSave();
	FReply HandleUndo();
	FReply HandleRedo();

	static TSharedRef<SWidget> MakeToolButton(
		const FText& Label,
		const FLinearColor& BgColor,
		const FLinearColor& TextColor,
		FOnClicked OnClicked,
		const FSlateBrush* Icon = nullptr);
};
