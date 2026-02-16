// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"

struct FForgeCategoryRule;

namespace DAForge
{
	struct FCreateResult
	{
		UObject* NewAsset = nullptr;
		FText Error;
		bool bSucceeded() const { return NewAsset != nullptr; }
	};

	class FDAForgeAssetCreator
	{
	public:
		static FCreateResult CreateAsset(
			const FForgeCategoryRule& Rule,
			const FString& ClickedPath,
			const FString& OptionalSuffix,
			const TSoftObjectPtr<UTexture2D>& OverrideIcon);

	private:
		static FString NormalizeGamePathOrDefault(const FString& InPath);
		static FString NormalizeRootPathOrDefault(const FString& InPath);

		static bool IsPathUnderRoot(const FString& Path, const FString& Root);

		static UClass* ResolveRuleClassOrNull(const FForgeCategoryRule& Rule);

		static FString SanitizeSuffix(FString S);
		static bool AssetNameExistsInPath(const FString& PackagePath, const FString& AssetName);
		static FString MakeNumberedName(const FString& BasePrefix, const FString& PackagePath);

		static void PostFillForgeFields(UObject* NewAsset, const FForgeCategoryRule& Rule, const FString& CleanSuffix, const TSoftObjectPtr<UTexture2D>& OverrideIcon);
	};
}
