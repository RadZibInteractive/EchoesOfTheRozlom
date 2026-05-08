// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

DECLARE_DELEGATE_OneParam(FOnClaudeResponse, const TArray<FString>& /* Suggestions */);
DECLARE_DELEGATE_OneParam(FOnClaudeError,    const FString&         /* ErrorMessage */);

// Thin async wrapper around the Claude Messages API.
// Sends zone context + user idea → returns 3 actionable suggestions.
class FORGEEDITOR_API FForgeClaudeClient : public TSharedFromThis<FForgeClaudeClient>
{
public:
	explicit FForgeClaudeClient(const FString& InApiKey);

	void RequestSuggestions(
		const FString& ZoneContext,
		const FString& ActivityType,
		const FString& UserIdea,
		FOnClaudeResponse OnSuccess,
		FOnClaudeError    OnError);

	// Persist / load API key from editor config
	static void    SaveApiKey(const FString& Key);
	static FString LoadApiKey();

private:
	FString ApiKey;

	static FString BuildSystemPrompt();
	static FString BuildUserMessage(const FString& Zone, const FString& Activity, const FString& Idea);
	static TArray<FString> ParseSuggestions(const FString& TextContent);

	void OnHttpResponse(
		FHttpRequestPtr  Request,
		FHttpResponsePtr Response,
		bool             bConnected,
		FOnClaudeResponse OnSuccess,
		FOnClaudeError    OnError);
};
