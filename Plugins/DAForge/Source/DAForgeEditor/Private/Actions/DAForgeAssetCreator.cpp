// © 2026 RadZib. All rights reserved.

#include "Actions/DAForgeAssetCreator.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "Editor.h"
#include "IAssetTools.h"
#include "Misc/PackageName.h"
#include "Subsystems/AssetEditorSubsystem.h"

#include "ForgeDataAsset.h"
#include "Settings/DAForgeSettings.h"

namespace DAForge
{
	FString FDAForgeAssetCreator::NormalizeGamePathOrDefault(const FString& InPath)
	{
		FString P = InPath;
		P.TrimStartAndEndInline();

		if (!P.StartsWith(TEXT("/Game")))
		{
			return TEXT("/Game");
		}

		while (P.Len() > 5 && P.EndsWith(TEXT("/")))
		{
			P.LeftChopInline(1);
		}

		return P;
	}

	FString FDAForgeAssetCreator::NormalizeRootPathOrDefault(const FString& InPath)
	{
		return NormalizeGamePathOrDefault(InPath.IsEmpty() ? TEXT("/Game") : InPath);
	}

	bool FDAForgeAssetCreator::IsPathUnderRoot(const FString& Path, const FString& Root)
	{
		const FString NP = NormalizeGamePathOrDefault(Path);
		const FString NR = NormalizeRootPathOrDefault(Root);
		return NP == NR || NP.StartsWith(NR + TEXT("/"));
	}

	UClass* FDAForgeAssetCreator::ResolveRuleClassOrNull(const FForgeCategoryRule& Rule)
	{
		UClass* Cls = Rule.AssetClass.LoadSynchronous();
		if (!Cls)
		{
			return nullptr;
		}

		if (!Cls->IsChildOf(UForgeDataAsset::StaticClass()))
		{
			return nullptr;
		}

		return Cls;
	}

	FString FDAForgeAssetCreator::SanitizeSuffix(FString S)
	{
		S.TrimStartAndEndInline();
		S.ReplaceInline(TEXT(" "), TEXT("_"));

		for (int32 i = S.Len() - 1; i >= 0; --i)
		{
			const TCHAR C = S[i];
			const bool bOk =
				(C >= 'a' && C <= 'z') ||
				(C >= 'A' && C <= 'Z') ||
				(C >= '0' && C <= '9') ||
				(C == '_');

			if (!bOk)
			{
				S.RemoveAt(i);
			}
		}

		return S;
	}

	static bool PackageExistsFast(const FString& LongPackageName)
	{
		if (FPackageName::DoesPackageExist(LongPackageName))
		{
			return true;
		}

		const FAssetRegistryModule& ARM = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

		const FString AssetName = FPackageName::GetShortName(LongPackageName);
		const FString ObjectPathStr = FString::Printf(TEXT("%s.%s"), *LongPackageName, *AssetName);
		const FSoftObjectPath ObjPath(ObjectPathStr);

#if ENGINE_MAJOR_VERSION > 5 || (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 4)
		const FAssetData Existing = ARM.Get().GetAssetByObjectPath(ObjPath);
#else
		const FAssetData Existing = ARM.Get().GetAssetByObjectPath(FName(*ObjectPathStr));
#endif
		return Existing.IsValid();
	}

	bool FDAForgeAssetCreator::AssetNameExistsInPath(const FString& PackagePath, const FString& AssetName)
	{
		const FString LongPackage = PackagePath / AssetName;
		return PackageExistsFast(LongPackage);
	}

	FString FDAForgeAssetCreator::MakeNumberedName(const FString& BasePrefix, const FString& PackagePath)
	{
		for (int32 i = 1; i < 10000; ++i)
		{
			const FString Candidate = FString::Printf(TEXT("%s_%03d"), *BasePrefix, i);
			if (!AssetNameExistsInPath(PackagePath, Candidate))
			{
				return Candidate;
			}
		}
		return BasePrefix + TEXT("_9999");
	}

	void FDAForgeAssetCreator::PostFillForgeFields(UObject* NewAsset, const FForgeCategoryRule& Rule, const FString& CleanSuffix, const TSoftObjectPtr<UTexture2D>& OverrideIcon)
	{
		UForgeDataAsset* Forge = Cast<UForgeDataAsset>(NewAsset);
		if (!Forge)
		{
			return;
		}

		Forge->AssetId = NewAsset->GetFName();

		if (!CleanSuffix.IsEmpty())
		{
			Forge->DisplayTitle = CleanSuffix;
		}
		else
		{
			const FString FullName = NewAsset->GetName();
			const FString Prefix = Rule.DisplayTitle.ToString();

			if (FullName.StartsWith(Prefix + TEXT("_")))
			{
				Forge->DisplayTitle = FullName.RightChop(Prefix.Len() + 1);
			}
			else
			{
				Forge->DisplayTitle = FullName;
			}
		}

		if (!OverrideIcon.IsNull())
		{
			Forge->DisplayIcon = OverrideIcon;
		}
		else if (Forge->DisplayIcon.IsNull() && !Rule.DefaultIcon.IsNull())
		{
			Forge->DisplayIcon = Rule.DefaultIcon;
		}

		NewAsset->MarkPackageDirty();
	}

	FCreateResult FDAForgeAssetCreator::CreateAsset(
		const FForgeCategoryRule& Rule,
		const FString& ClickedPath,
		const FString& OptionalSuffix,
		const TSoftObjectPtr<UTexture2D>& OverrideIcon)
	{
		FCreateResult R;

		if (Rule.PrimaryAssetType.IsNone())
		{
			R.Error = FText::FromString(TEXT("DAForge: Category PrimaryAssetType is None."));
			return R;
		}

		UClass* AssetClass = ResolveRuleClassOrNull(Rule);
		if (!AssetClass)
		{
			R.Error = FText::FromString(TEXT("DAForge: Category AssetClass is invalid (must derive from UForgeDataAsset)."));
			return R;
		}

		// Decide final target folder:
		// - prefer clicked folder if it’s under Rule.RootPath
		// - otherwise fallback to Rule.RootPath
		const FString ClickPath = NormalizeGamePathOrDefault(ClickedPath);
		const FString RootPath = NormalizeRootPathOrDefault(Rule.RootPath.Path);

		const FString PackagePath = IsPathUnderRoot(ClickPath, RootPath) ? ClickPath : RootPath;
		const FString TypeName = Rule.DisplayTitle.ToString();

		const FString CleanSuffix = SanitizeSuffix(OptionalSuffix);

		FString AssetName;
		if (!CleanSuffix.IsEmpty())
		{
			AssetName = FString::Printf(TEXT("%s_%s"), *TypeName, *CleanSuffix);
			if (AssetNameExistsInPath(PackagePath, AssetName))
			{
				R.Error = FText::FromString(FString::Printf(TEXT("DAForge: Asset already exists: %s/%s"), *PackagePath, *AssetName));
				return R;
			}
		}
		else
		{
			AssetName = MakeNumberedName(TypeName, PackagePath);
		}

		IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

		UObject* NewAsset = AssetTools.CreateAsset(
			AssetName,
			PackagePath,
			AssetClass,
			nullptr);

		if (!NewAsset)
		{
			R.Error = FText::FromString(TEXT("DAForge: Failed to create asset."));
			return R;
		}

		PostFillForgeFields(NewAsset, Rule, CleanSuffix, OverrideIcon);

		if (Rule.bOpenAfterCreate && GEditor)
		{
			if (UAssetEditorSubsystem* AES = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>())
			{
				AES->OpenEditorForAsset(NewAsset);
			}
		}

		R.NewAsset = NewAsset;
		return R;
	}
}
