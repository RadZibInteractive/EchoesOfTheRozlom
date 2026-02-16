#include "Actions/DAForgeAssetManagerSync.h"
#include "Settings/DAForgeSettings.h"
#include "Engine/AssetManagerSettings.h"
#include "Misc/MessageDialog.h"

static bool IsValidGamePath(const FString& P)
{
	return P.StartsWith(TEXT("/Game"));
}

static bool SetPrimaryAssetTypeInfoDirectories(FPrimaryAssetTypeInfo& Info, const TArray<FDirectoryPath>& NewDirs)
{
	UScriptStruct* Struct = FPrimaryAssetTypeInfo::StaticStruct();
	if (!Struct) return false;

	FArrayProperty* DirsProp = FindFProperty<FArrayProperty>(Struct, TEXT("Directories"));
	if (!DirsProp) return false;

	void* StructData = &Info;
	void* ArrayPtr = DirsProp->ContainerPtrToValuePtr<void>(StructData);

	FScriptArrayHelper Helper(DirsProp, ArrayPtr);
	Helper.EmptyValues();

	for (const FDirectoryPath& Dir : NewDirs)
	{
		const int32 NewIndex = Helper.AddValue();
		void* ElemPtr = Helper.GetRawPtr(NewIndex);

		if (FStructProperty* ElemStructProp = CastField<FStructProperty>(DirsProp->Inner))
		{
			ElemStructProp->Struct->CopyScriptStruct(ElemPtr, &Dir);
		}
		else
		{
			return false;
		}
	}

	return true;
}

void FDAForgeAssetManagerSync::SyncToAssetManager(const TArray<FForgeCategoryRule>& Categories)
{
	UAssetManagerSettings* AMS = GetMutableDefault<UAssetManagerSettings>();
	if (!AMS)
		return;

	bool bAnyChange = false;

	for (const FForgeCategoryRule& Rule : Categories)
	{
		if (Rule.PrimaryAssetType.IsNone() || Rule.AssetClass.IsNull())
			continue;

		const FString Path = Rule.RootPath.Path;
		if (!IsValidGamePath(Path))
			continue;

		FPrimaryAssetTypeInfo* Existing =
			AMS->PrimaryAssetTypesToScan.FindByPredicate(
				[&](const FPrimaryAssetTypeInfo& Info)
				{
					return Info.PrimaryAssetType == Rule.PrimaryAssetType;
				});

		if (!Existing)
		{
			TSoftClassPtr<UObject> BaseClass;
			BaseClass = Rule.AssetClass.ToSoftObjectPath();
			FPrimaryAssetTypeInfo NewInfo;
			NewInfo.PrimaryAssetType = Rule.PrimaryAssetType;
			NewInfo.SetAssetBaseClass(BaseClass);
			NewInfo.bHasBlueprintClasses = false;
			NewInfo.bIsEditorOnly = false;

			TArray<FDirectoryPath> Dirs;
			Dirs.Add(FDirectoryPath{ Rule.RootPath.Path });
			SetPrimaryAssetTypeInfoDirectories(NewInfo, Dirs);

			AMS->PrimaryAssetTypesToScan.Add(MoveTemp(NewInfo));
			bAnyChange = true;
		}
		else
		{
			const bool bHasDir = Existing->GetDirectories().ContainsByPredicate(
				[&](const FDirectoryPath& D) { return D.Path == Path; });

			if (!bHasDir)
			{
				TArray<FDirectoryPath> Dirs = Existing->GetDirectories();
				Dirs.Add(FDirectoryPath{ Path });

				SetPrimaryAssetTypeInfoDirectories(*Existing, Dirs);
				bAnyChange = true;
			}
		}
	}

	if (bAnyChange)
	{
		AMS->SaveConfig();
		AMS->TryUpdateDefaultConfigFile();

		FMessageDialog::Open(EAppMsgType::Ok,
			FText::FromString(TEXT("Asset Manager updated.\nRestart recommended.")));
	}
	else
	{
		FMessageDialog::Open(EAppMsgType::Ok,
			FText::FromString(TEXT("No changes needed.")));
	}
}
