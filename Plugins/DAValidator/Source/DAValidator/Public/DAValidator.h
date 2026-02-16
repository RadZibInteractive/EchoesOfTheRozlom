#pragma once

#include "CoreMinimal.h"

class FTokenizedMessage;
class UObject;
class FProperty;

/**
 * DAValidator - minimal public API/.
 */
namespace DAValidator
{
	/** Message log listing name */
	extern const FName LogName;

	/** Validation severity */
	enum class EIssueSeverity : uint8
	{
		Warning,
		Error
	};

	/** One validation issue */
	struct FIssue
	{
		EIssueSeverity Severity = EIssueSeverity::Warning;

		/** Full property path */
		FString Path;

		/** Readable message */
		FString Message;
	};

	/**
	 * Validate a loaded UObject (typically UDataAsset-derived).
	 * Traverses editable/reflected properties recursively.
	 */
	void ValidateObject(UObject* Object, TArray<FIssue>& OutIssues);

	/**
	 * Helper: push issues into MessageLog under DAValidator::LogName.
	 * Creates a page and groups messages per asset.
	 */
	void WriteIssuesToMessageLog(const TArray<UObject*>& ValidatedObjects, const TArray<TArray<FIssue>>& IssuesPerObject);
}