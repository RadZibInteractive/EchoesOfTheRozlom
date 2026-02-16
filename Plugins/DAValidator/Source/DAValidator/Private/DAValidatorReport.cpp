#include "DAValidator.h"

#include "Logging/TokenizedMessage.h"
#include "Logging/MessageLog.h"
#include "Misc/UObjectToken.h"
#include "Misc/StringBuilder.h"

#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

const FName DAValidator::LogName(TEXT("DAValidation"));

namespace DAValidator::Private
{
	static const FSlateBrush* PickToastBrush(bool bIsError, bool bIsWarning)
	{
		const ISlateStyle& Style = FAppStyle::Get();

		auto Try = [&Style](const ANSICHAR* Name) -> const FSlateBrush*
			{
				return Style.GetOptionalBrush(FName(Name));
			};

		if (bIsError)
		{
			if (const FSlateBrush* B = Try("Icons.ErrorWithColor")) return B;
			if (const FSlateBrush* B = Try("MessageLog.Error")) return B;
			if (const FSlateBrush* B = Try("Icons.Error")) return B;
		}
		else if (bIsWarning)
		{
			if (const FSlateBrush* B = Try("Icons.WarningWithColor")) return B;
			if (const FSlateBrush* B = Try("MessageLog.Warning")) return B;
			if (const FSlateBrush* B = Try("Icons.Warning")) return B;
		}
		else
		{
			if (const FSlateBrush* B = Try("Icons.SuccessWithColor")) return B;
			if (const FSlateBrush* B = Try("MessageLog.Note")) return B;
			if (const FSlateBrush* B = Try("Icons.Check")) return B;
		}

		return nullptr;
	}

	static void ShowToast(const FText& Text, bool bIsError, bool bIsWarning)
	{
		FNotificationInfo Info(Text);
		Info.bFireAndForget = true;
		Info.FadeInDuration = 0.1f;
		Info.FadeOutDuration = 0.2f;
		Info.ExpireDuration = 4.0f;

		if (const FSlateBrush* Brush = PickToastBrush(bIsError, bIsWarning))
		{
			Info.Image = Brush;
		}

		TSharedPtr<SNotificationItem> Item = FSlateNotificationManager::Get().AddNotification(Info);
		if (Item.IsValid())
		{
			const SNotificationItem::ECompletionState State =
				bIsError ? SNotificationItem::CS_Fail :
				(bIsWarning ? SNotificationItem::CS_None : SNotificationItem::CS_Success);

			Item->SetCompletionState(State);
			Item->ExpireAndFadeout();
		}
	}

	static EMessageSeverity::Type ToMsgSeverity(DAValidator::EIssueSeverity S)
	{
		return (S == DAValidator::EIssueSeverity::Error) ? EMessageSeverity::Error : EMessageSeverity::Warning;
	}

	static const TCHAR* Plural(int32 Count, const TCHAR* One, const TCHAR* Many)
	{
		return (Count == 1) ? One : Many;
	}
}

void DAValidator::WriteIssuesToMessageLog(const TArray<UObject*>& ValidatedObjects, const TArray<TArray<FIssue>>& IssuesPerObject)
{
	FMessageLog Log(DAValidator::LogName);
	Log.NewPage(FText::FromString(TEXT("DA Validation")));

	int32 TotalWarnings = 0;
	int32 TotalErrors = 0;

	const int32 Num = FMath::Min(ValidatedObjects.Num(), IssuesPerObject.Num());

	for (int32 i = 0; i < Num; ++i)
	{
		UObject* Obj = ValidatedObjects[i];
		const TArray<FIssue>& Issues = IssuesPerObject[i];

		if (!Obj)
		{
			continue;
		}

		{
			TSharedRef<FTokenizedMessage> Header = FTokenizedMessage::Create(EMessageSeverity::Info);
			Header->AddToken(FTextToken::Create(FText::FromString(TEXT("Asset: "))));
			Header->AddToken(FAssetNameToken::Create(Obj->GetPathName()));
			Header->AddToken(FTextToken::Create(FText::FromString(TEXT(" "))));
			Header->AddToken(FUObjectToken::Create(Obj));
			Log.AddMessage(Header);
		}

		if (Issues.Num() == 0)
		{
			TSharedRef<FTokenizedMessage> OkMsg = FTokenizedMessage::Create(EMessageSeverity::Info);
			OkMsg->AddToken(FTextToken::Create(FText::FromString(TEXT("  OK - No issues found."))));
			Log.AddMessage(OkMsg);
			continue;
		}

		for (const FIssue& Issue : Issues)
		{
			if (Issue.Severity == EIssueSeverity::Error) { ++TotalErrors; }
			else { ++TotalWarnings; }

			TStringBuilder<256> B;
			B.Append(TEXT("  "));
			B.Append(Issue.Path);
			B.Append(TEXT(" : "));
			B.Append(Issue.Message);

			TSharedRef<FTokenizedMessage> Msg = FTokenizedMessage::Create(DAValidator::Private::ToMsgSeverity(Issue.Severity));
			Msg->AddToken(FTextToken::Create(FText::FromString(FString(B))));
			Log.AddMessage(Msg);
		}
	}

	const bool bHasErrors = (TotalErrors > 0);
	const bool bHasWarnings = (TotalWarnings > 0);

	if (!bHasErrors && !bHasWarnings)
	{
		DAValidator::Private::ShowToast(FText::FromString(TEXT("DA validation finished: OK.")), false, false);
		return;
	}

	TStringBuilder<128> Summary;
	Summary.Append(TEXT("DA validation finished: "));

	bool bNeedComma = false;
	if (bHasErrors)
	{
		Summary.Appendf(TEXT("%d %s"), TotalErrors, DAValidator::Private::Plural(TotalErrors, TEXT("error"), TEXT("errors")));
		bNeedComma = true;
	}
	if (bHasWarnings)
	{
		if (bNeedComma) { Summary.Append(TEXT(", ")); }
		Summary.Appendf(TEXT("%d %s"), TotalWarnings, DAValidator::Private::Plural(TotalWarnings, TEXT("warning"), TEXT("warnings")));
	}

	DAValidator::Private::ShowToast(FText::FromString(FString(Summary)), bHasErrors, bHasWarnings);

	Log.Open(bHasErrors ? EMessageSeverity::Error : EMessageSeverity::Warning, bHasErrors);
}