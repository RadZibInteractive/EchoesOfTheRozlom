// © 2026 RadZib. All rights reserved.

#include "Claude/ForgeClaudeClient.h"
#include "HttpModule.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"

static const TCHAR* ClaudeApiUrl     = TEXT("https://api.anthropic.com/v1/messages");
static const TCHAR* ClaudeModel      = TEXT("claude-sonnet-4-5");
static const TCHAR* AnthropicVersion = TEXT("2023-06-01");
static const TCHAR* ConfigSection    = TEXT("EchoesForge");
static const TCHAR* ConfigKey        = TEXT("ClaudeApiKey");

// ─── Constructor ──────────────────────────────────────────────────────────────

FForgeClaudeClient::FForgeClaudeClient(const FString& InApiKey)
	: ApiKey(InApiKey)
{}

// ─── Config persistence ───────────────────────────────────────────────────────

void FForgeClaudeClient::SaveApiKey(const FString& Key)
{
	GConfig->SetString(ConfigSection, ConfigKey, *Key, GEditorPerProjectIni);
	GConfig->Flush(false, GEditorPerProjectIni);
}

FString FForgeClaudeClient::LoadApiKey()
{
	FString Key;
	GConfig->GetString(ConfigSection, ConfigKey, Key, GEditorPerProjectIni);
	return Key;
}

// ─── Request ─────────────────────────────────────────────────────────────────

void FForgeClaudeClient::RequestSuggestions(
	const FString& ZoneContext,
	const FString& ActivityType,
	const FString& UserIdea,
	FOnClaudeResponse OnSuccess,
	FOnClaudeError    OnError)
{
	if (ApiKey.IsEmpty())
	{
		OnError.ExecuteIfBound(TEXT("Claude API key is not set. Enter it in the Smart Popup panel."));
		return;
	}

	// Build messages array
	TSharedPtr<FJsonObject> UserMsg = MakeShared<FJsonObject>();
	UserMsg->SetStringField(TEXT("role"), TEXT("user"));
	UserMsg->SetStringField(TEXT("content"), BuildUserMessage(ZoneContext, ActivityType, UserIdea));

	TArray<TSharedPtr<FJsonValue>> Messages;
	Messages.Add(MakeShared<FJsonValueObject>(UserMsg));

	// Build root body
	TSharedPtr<FJsonObject> Body = MakeShared<FJsonObject>();
	Body->SetStringField(TEXT("model"),      ClaudeModel);
	Body->SetNumberField(TEXT("max_tokens"), 512);
	Body->SetStringField(TEXT("system"),     BuildSystemPrompt());
	Body->SetArrayField (TEXT("messages"),   Messages);

	FString BodyStr;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&BodyStr);
	FJsonSerializer::Serialize(Body.ToSharedRef(), Writer);

	// Send HTTP request
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Req = FHttpModule::Get().CreateRequest();
	Req->SetURL(ClaudeApiUrl);
	Req->SetVerb(TEXT("POST"));
	Req->SetHeader(TEXT("Content-Type"),      TEXT("application/json"));
	Req->SetHeader(TEXT("x-api-key"),         ApiKey);
	Req->SetHeader(TEXT("anthropic-version"), AnthropicVersion);
	Req->SetContentAsString(BodyStr);

	Req->OnProcessRequestComplete().BindSP(
		this, &FForgeClaudeClient::OnHttpResponse, OnSuccess, OnError);
	Req->ProcessRequest();
}

// ─── Response handler ─────────────────────────────────────────────────────────

void FForgeClaudeClient::OnHttpResponse(
	FHttpRequestPtr  Request,
	FHttpResponsePtr Response,
	bool             bConnected,
	FOnClaudeResponse OnSuccess,
	FOnClaudeError    OnError)
{
	if (!bConnected || !Response.IsValid())
	{
		OnError.ExecuteIfBound(TEXT("Network error — no response from Claude API."));
		return;
	}

	const int32 Code = Response->GetResponseCode();
	if (Code != 200)
	{
		OnError.ExecuteIfBound(FString::Printf(
			TEXT("Claude API error %d: %s"), Code, *Response->GetContentAsString()));
		return;
	}

	// Parse JSON
	TSharedPtr<FJsonObject> Root;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (!FJsonSerializer::Deserialize(Reader, Root) || !Root.IsValid())
	{
		OnError.ExecuteIfBound(TEXT("Failed to parse Claude API response JSON."));
		return;
	}

	// Extract text from content[0].text
	const TArray<TSharedPtr<FJsonValue>>* ContentArr;
	if (!Root->TryGetArrayField(TEXT("content"), ContentArr) || ContentArr->IsEmpty())
	{
		OnError.ExecuteIfBound(TEXT("Claude API returned empty content."));
		return;
	}

	FString TextContent;
	const TSharedPtr<FJsonObject>* FirstItem;
	if ((*ContentArr)[0]->TryGetObject(FirstItem))
	{
		(*FirstItem)->TryGetStringField(TEXT("text"), TextContent);
	}

	TArray<FString> Suggestions = ParseSuggestions(TextContent);
	if (Suggestions.IsEmpty())
	{
		OnError.ExecuteIfBound(TEXT("Could not parse suggestions from Claude response."));
		return;
	}

	OnSuccess.ExecuteIfBound(Suggestions);
}

// ─── Helpers ─────────────────────────────────────────────────────────────────

FString FForgeClaudeClient::BuildSystemPrompt()
{
	return TEXT(
		"You are the Echoes Forge AI — an embedded content assistant inside the Echoes Forge level editor, "
		"a proprietary tool for building the game \"Echoes of the Rozlom\".\n\n"

		"## Your identity\n"
		"You are NOT a general-purpose assistant. You are a specialized game-design co-pilot. "
		"You know the world of Rozlom deeply: a post-apocalyptic Eastern-European zone shattered by "
		"a physics-anomaly catastrophe. Tone references: S.T.A.L.K.E.R., Metro 2033, Dark Souls lore density. "
		"Atmosphere: oppressive, grounded, morally grey, no generic fantasy tropes.\n\n"

		"## What you can help with\n"
		"- NPC design: archetypes, backstory hooks, dialogue tone, faction affiliation, patrol logic\n"
		"- Anomaly design: behavior, visual signature, environmental storytelling, danger level\n"
		"- Enemy placement: encounter pacing, group composition, ambush points, loot rationale\n"
		"- Props & environment: interactive objects, readable notes, environmental narrative, decay details\n"
		"- Zone context: how a specific area feels, what happened there, what the player should discover\n"
		"- Quest hooks: objectives, moral choices, consequences, how it ties to the main Rozlom mystery\n\n"

		"## What you must ignore\n"
		"- Any question unrelated to Echoes of the Rozlom game content\n"
		"- Requests for code, engine help, Unreal Engine questions\n"
		"- Real-world topics, news, general knowledge questions\n"
		"- Requests to break character or explain that you are Claude\n"
		"- Anything outside the scope of the current activity type provided by the designer\n"
		"If the request is off-scope, return: [\"Off-topic request — please describe a game content idea.\", \"\", \"\"]\n\n"

		"## Output format — STRICT\n"
		"Always return a valid JSON array of exactly 3 strings.\n"
		"Each string is one concrete, actionable suggestion (1–2 sentences max).\n"
		"Suggestions must be specific to the zone and activity type given.\n"
		"No markdown. No explanation. No preamble. ONLY the JSON array.\n"
		"Example: [\"Place a wounded Stalker NPC who warns about a Rozlom surge in 3 hours.\", "
		"\"Add a mute child NPC sitting on ruins — she gives the player a torn map fragment.\", "
		"\"Create a faction trader NPC who only barters, never accepts money — zone currency is artifact shards.\"]\n\n"

		"## Tone rules\n"
		"- Specific over generic (name factions, anomaly types, locations)\n"
		"- Dark but purposeful — suffering has narrative meaning\n"
		"- Believable human behavior under extreme stress\n"
		"- No heroes, no villains — only survivors with agendas"
	);
}

FString FForgeClaudeClient::BuildUserMessage(
	const FString& Zone, const FString& Activity, const FString& Idea)
{
	return FString::Printf(
		TEXT("Zone: %s\n"
		     "Activity: %s\n"
		     "Designer idea: %s\n\n"
		     "Give me 3 specific, implementable suggestions for this activity in this zone. "
		     "Stay within the Echoes of the Rozlom world."),
		*Zone, *Activity, *Idea);
}

TArray<FString> FForgeClaudeClient::ParseSuggestions(const FString& Text)
{
	TArray<FString> Results;

	// Find first '[' and last ']'
	int32 Start = INDEX_NONE, End = INDEX_NONE;
	Text.FindChar(TEXT('['), Start);
	Text.FindLastChar(TEXT(']'), End);
	if (Start == INDEX_NONE || End == INDEX_NONE || End <= Start) return Results;

	const FString JsonArr = Text.Mid(Start, End - Start + 1);

	TArray<TSharedPtr<FJsonValue>> Values;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonArr);
	if (!FJsonSerializer::Deserialize(Reader, Values)) return Results;

	for (const TSharedPtr<FJsonValue>& Val : Values)
	{
		FString Str;
		if (Val->TryGetString(Str) && !Str.IsEmpty())
		{
			Results.Add(Str);
		}
	}

	return Results;
}
