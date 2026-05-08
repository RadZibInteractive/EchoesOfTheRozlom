// © 2026 RadZib. All rights reserved.

#include "UI/SForgeSmartPopup.h"
#include "UI/SForgeButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SSeparator.h"

#define LOCTEXT_NAMESPACE "ForgeSmartPopup"

void SForgeSmartPopup::Construct(const FArguments& InArgs)
{
	Title             = InArgs._Title;
	PlaceholderHint   = InArgs._PlaceholderHint;
	OnIdeaSubmitted   = InArgs._OnIdeaSubmitted;
	OnSuggestionPicked = InArgs._OnSuggestionPicked;

	// Load saved API key and build client
	RebuildClaudeClient();

	ChildSlot
	[
		SNew(SBorder)
		.BorderBackgroundColor(FLinearColor(0.10f, 0.10f, 0.12f))
		.Padding(FMargin(12.f))
		[
			SNew(SVerticalBox)

			// ── Title ──
			+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 0.f, 0.f, 6.f)
			[
				SNew(STextBlock)
				.Text(Title)
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
				.ColorAndOpacity(FSlateColor(FLinearColor(0.85f, 0.87f, 0.95f)))
			]

			+ SVerticalBox::Slot().AutoHeight()[ SNew(SSeparator) ]

			// ── Claude API key ──
			+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 8.f, 0.f, 2.f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("ApiKeyLabel", "AI API Key"))
				.Font(FCoreStyle::GetDefaultFontStyle("Regular", 8))
				.ColorAndOpacity(FSlateColor(FLinearColor(0.45f, 0.47f, 0.58f)))
			]

			+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 0.f, 0.f, 8.f)
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot().FillWidth(1.f).Padding(0.f, 0.f, 4.f, 0.f)
				[
					SAssignNew(ApiKeyInput, SEditableTextBox)
					.Text(FText::FromString(FForgeClaudeClient::LoadApiKey()))
					.HintText(LOCTEXT("ApiKeyHint", "sk-ant-…"))
					.IsPassword(true)
					.Font(FCoreStyle::GetDefaultFontStyle("Mono", 8))
				]

				+ SHorizontalBox::Slot().AutoWidth()
				[
					SNew(SForgeButton)
					.Preset(EForgeButtonPreset::Navy)
					.ContentPadding(FMargin(8.f, 4.f))
					.OnClicked(FOnClicked::CreateLambda([this]() -> FReply
					{
						const FString Key = ApiKeyInput->GetText().ToString();
						FForgeClaudeClient::SaveApiKey(Key);
						RebuildClaudeClient();
						return FReply::Handled();
					}))
					[
						SNew(STextBlock)
						.Text(LOCTEXT("SaveKeyBtn", "Save"))
						.Font(FCoreStyle::GetDefaultFontStyle("Regular", 8))
						.ColorAndOpacity(FSlateColor(FLinearColor(0.70f, 0.75f, 0.95f)))
					]
				]
			]

			+ SVerticalBox::Slot().AutoHeight()[ SNew(SSeparator) ]

			// ── Context indicator ──
			+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 6.f, 0.f, 0.f)
			[
				SNew(STextBlock)
				.Text_Lambda([this]() -> FText
				{
					return ZoneContext.IsEmpty()
						? LOCTEXT("NoZoneCtx", "No zone selected")
						: FText::Format(LOCTEXT("ZoneCtxFmt", "Zone: {0}  |  {1}"),
							FText::FromString(ZoneContext),
							FText::FromString(ActivityContext.IsEmpty() ? TEXT("–") : ActivityContext));
				})
				.Font(FCoreStyle::GetDefaultFontStyle("Italic", 8))
				.ColorAndOpacity(FSlateColor(FLinearColor(0.28f, 0.60f, 0.80f)))
			]

			// ── Idea input ──
			+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 8.f)
			[
				SAssignNew(IdeaInputBox, SEditableTextBox)
				.HintText(PlaceholderHint)
				.OnTextCommitted_Lambda([this](const FText&, ETextCommit::Type Type)
				{
					if (Type == ETextCommit::OnEnter) OnSubmitIdea();
				})
			]

			+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Right).Padding(0.f, 0.f, 0.f, 4.f)
			[
				SNew(SForgeButton)
				.Preset(EForgeButtonPreset::Custom)
				.NormalColor(FLinearColor(0.04f, 0.22f, 0.34f))   // brand cyan
				.HoverColor (FLinearColor(0.06f, 0.38f, 0.55f))
				.ContentPadding(FMargin(14.f, 7.f))
				.Label(LOCTEXT("GenerateBtn", "Ask AI ✦"))
				.Bold(true)
				.FontSize(10)
				.OnClicked(this, &SForgeSmartPopup::OnSubmitIdea)
			]

			// ── Loading ──
			+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).Padding(0.f, 6.f)
			[
				SAssignNew(LoadingSpinner, STextBlock)
				.Text(LOCTEXT("Loading", "Thinking…"))
				.Font(FCoreStyle::GetDefaultFontStyle("Italic", 9))
				.ColorAndOpacity(FSlateColor(FLinearColor(0.4f, 0.6f, 0.9f)))
				.Visibility(EVisibility::Collapsed)
			]

			// ── Error label ──
			+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 2.f, 0.f, 4.f)
			[
				SAssignNew(ErrorLabel, STextBlock)
				.AutoWrapText(true)
				.Font(FCoreStyle::GetDefaultFontStyle("Regular", 8))
				.ColorAndOpacity(FSlateColor(FLinearColor(0.90f, 0.30f, 0.25f)))
				.Visibility(EVisibility::Collapsed)
			]

			+ SVerticalBox::Slot().AutoHeight()[ SNew(SSeparator) ]

			// ── Suggestions list ──
			+ SVerticalBox::Slot().FillHeight(1.f).Padding(0.f, 8.f)
			[
				SNew(SScrollBox)
				+ SScrollBox::Slot()
				[
					SAssignNew(SuggestionContainer, SVerticalBox)
				]
			]
		]
	];
}

// ─── Public API ───────────────────────────────────────────────────────────────

void SForgeSmartPopup::SetZoneContext(const FString& ZoneName, const FString& Activity)
{
	ZoneContext     = ZoneName;
	ActivityContext = Activity;
	Invalidate(EInvalidateWidgetReason::Paint);
}

void SForgeSmartPopup::SetSuggestions(const TArray<FString>& Suggestions)
{
	CurrentSuggestions = Suggestions;
	SetLoadingState(false);

	if (!SuggestionContainer.IsValid()) return;
	SuggestionContainer->ClearChildren();

	if (ErrorLabel.IsValid()) ErrorLabel->SetVisibility(EVisibility::Collapsed);

	for (int32 i = 0; i < Suggestions.Num(); ++i)
	{
		const int32 Idx = i;
		SuggestionContainer->AddSlot().AutoHeight().Padding(0.f, 3.f)
		[
			SNew(SForgeButton)
			.Preset(EForgeButtonPreset::Ghost)
			.ContentPadding(FMargin(8.f, 7.f))
			.OnClicked(this, &SForgeSmartPopup::OnPickSuggestion, Idx)
			[
				SNew(STextBlock)
				.Text(FText::FromString(Suggestions[i]))
				.AutoWrapText(true)
				.Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
				.ColorAndOpacity(FSlateColor(FLinearColor(0.80f, 0.83f, 0.95f)))
			]
		];
	}
}

void SForgeSmartPopup::SetLoadingState(bool bLoading)
{
	if (LoadingSpinner.IsValid())
		LoadingSpinner->SetVisibility(bLoading ? EVisibility::Visible : EVisibility::Collapsed);
}

// ─── Private ──────────────────────────────────────────────────────────────────

void SForgeSmartPopup::RebuildClaudeClient()
{
	ClaudeClient = MakeShared<FForgeClaudeClient>(FForgeClaudeClient::LoadApiKey());
}

FReply SForgeSmartPopup::OnSubmitIdea()
{
	if (!IdeaInputBox.IsValid() || !ClaudeClient.IsValid()) return FReply::Handled();

	const FString Idea = IdeaInputBox->GetText().ToString().TrimStartAndEnd();
	if (Idea.IsEmpty()) return FReply::Handled();

	SetLoadingState(true);
	if (SuggestionContainer.IsValid()) SuggestionContainer->ClearChildren();
	if (ErrorLabel.IsValid()) ErrorLabel->SetVisibility(EVisibility::Collapsed);

	OnIdeaSubmitted.ExecuteIfBound(Idea);

	// Fire async request to Claude
	ClaudeClient->RequestSuggestions(
		ZoneContext.IsEmpty() ? TEXT("Unknown zone") : ZoneContext,
		ActivityContext.IsEmpty() ? TEXT("General") : ActivityContext,
		Idea,
		FOnClaudeResponse::CreateSP(this, &SForgeSmartPopup::SetSuggestions),
		FOnClaudeError::CreateLambda([this](const FString& Err)
		{
			SetLoadingState(false);
			if (ErrorLabel.IsValid())
			{
				ErrorLabel->SetText(FText::FromString(Err));
				ErrorLabel->SetVisibility(EVisibility::Visible);
			}
		})
	);

	return FReply::Handled();
}

FReply SForgeSmartPopup::OnPickSuggestion(int32 Index)
{
	OnSuggestionPicked.ExecuteIfBound(Index);
	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
