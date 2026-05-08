// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Claude/ForgeClaudeClient.h"

DECLARE_DELEGATE_OneParam(FOnForgeIdeaSubmitted,   const FString&);
DECLARE_DELEGATE_OneParam(FOnForgeSuggestionPicked, int32);

// Smart popup: AI mini-chat powered by Claude API.
// Flow: user types idea → Claude returns 3 suggestions → user picks one.
class FORGEEDITOR_API SForgeSmartPopup : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SForgeSmartPopup)
		: _Title(FText::GetEmpty())
		, _PlaceholderHint(NSLOCTEXT("ForgeEditor", "IdeaHint", "Describe your idea..."))
	{}
		SLATE_ARGUMENT(FText, Title)
		SLATE_ARGUMENT(FText, PlaceholderHint)
		SLATE_EVENT(FOnForgeIdeaSubmitted,   OnIdeaSubmitted)
		SLATE_EVENT(FOnForgeSuggestionPicked, OnSuggestionPicked)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	// Set context passed to Claude (zone name + activity type)
	void SetZoneContext(const FString& ZoneName, const FString& Activity);

	// Called externally to inject suggestions (e.g. from Claude callback)
	void SetSuggestions(const TArray<FString>& Suggestions);
	void SetLoadingState(bool bLoading);

private:
	FText                    Title;
	FText                    PlaceholderHint;
	FOnForgeIdeaSubmitted    OnIdeaSubmitted;
	FOnForgeSuggestionPicked OnSuggestionPicked;

	FString ZoneContext;
	FString ActivityContext;

	TSharedPtr<FForgeClaudeClient>  ClaudeClient;
	TSharedPtr<SEditableTextBox>    IdeaInputBox;
	TSharedPtr<SEditableTextBox>    ApiKeyInput;
	TSharedPtr<SVerticalBox>        SuggestionContainer;
	TSharedPtr<STextBlock>          LoadingSpinner;
	TSharedPtr<STextBlock>          ErrorLabel;
	TArray<FString>                 CurrentSuggestions;

	void RebuildClaudeClient();
	FReply OnSubmitIdea();
	FReply OnPickSuggestion(int32 Index);
};
