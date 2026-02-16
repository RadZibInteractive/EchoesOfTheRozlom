#include "DAValidator.h"

#include "AssetRegistry/AssetData.h"
#include "ContentBrowserMenuContexts.h"
#include "MessageLogModule.h"
#include "Modules/ModuleManager.h"
#include "ToolMenus.h"
#include "Engine/DataAsset.h"

namespace
{
	static const FName MenuName_AssetContext(TEXT("ContentBrowser.AssetContextMenu"));
	static const FName OwnerName(TEXT("DAValidator"));
	static const FName SectionName(TEXT("DAValidator"));
	static const FName EntryName(TEXT("DAValidator_ValidateDataAssets"));

	static FDelegateHandle ToolMenusStartupHandle;

	static void RegisterMessageLog()
	{
		FMessageLogModule& MessageLogModule = FModuleManager::LoadModuleChecked<FMessageLogModule>("MessageLog");

		FMessageLogInitializationOptions InitOptions;
		InitOptions.bShowFilters = true;
		InitOptions.bShowPages = true;
		InitOptions.bAllowClear = true;

		MessageLogModule.RegisterLogListing(DAValidator::LogName, FText::FromString(TEXT("DA Validation")), InitOptions);
	}

	static bool IsDataAssetClass(const UClass* Class)
	{
		return Class && Class->IsChildOf(UDataAsset::StaticClass());
	}

	static void ExecuteValidateSelectedAssets(const FToolMenuContext& MenuContext)
	{
		const UContentBrowserAssetContextMenuContext* Ctx = MenuContext.FindContext<UContentBrowserAssetContextMenuContext>();
		if (!Ctx)
		{
			return;
		}

		TArray<FAssetData> Selected;
		Selected.Reserve(Ctx->SelectedAssets.Num());

		for (const FAssetData& AssetData : Ctx->SelectedAssets)
		{
			if (IsDataAssetClass(AssetData.GetClass()))
			{
				Selected.Add(AssetData);
			}
		}

		if (Selected.Num() == 0)
		{
			return;
		}

		TArray<UObject*> LoadedObjects;
		LoadedObjects.Reserve(Selected.Num());

		TArray<TArray<DAValidator::FIssue>> IssuesPerObject;
		IssuesPerObject.Reserve(Selected.Num());

		for (const FAssetData& AssetData : Selected)
		{
			UObject* Obj = AssetData.GetAsset();
			if (!Obj)
			{
				continue;
			}

			LoadedObjects.Add(Obj);

			TArray<DAValidator::FIssue> Issues;
			DAValidator::ValidateObject(Obj, Issues);

			IssuesPerObject.Add(MoveTemp(Issues));
		}

		DAValidator::WriteIssuesToMessageLog(LoadedObjects, IssuesPerObject);
	}

	static void RegisterMenus()
	{
		UToolMenus* ToolMenus = UToolMenus::Get();
		if (!ToolMenus)
		{
			return;
		}

		UToolMenu* Menu = ToolMenus->ExtendMenu(MenuName_AssetContext);
		if (!Menu)
		{
			return;
		}

		FToolMenuOwnerScoped OwnerScope(OwnerName);

		FToolMenuSection& Section = Menu->FindOrAddSection(SectionName, FText::FromString(TEXT("DA Validator")));

		const FToolMenuExecuteAction ExecuteAction = FToolMenuExecuteAction::CreateStatic(&ExecuteValidateSelectedAssets);

		const FToolUIAction UIAction(ExecuteAction, FToolMenuGetActionCheckState());

		Section.AddEntry(
			FToolMenuEntry::InitMenuEntry(
				EntryName,
				FText::FromString(TEXT("Validate DataAssets")),
				FText::FromString(TEXT("Validate selected DataAssets via reflection traversal (no DataValidation plugin).")),
				FSlateIcon(),
				FToolUIActionChoice(UIAction)
			)
		);
	}

	static void UnregisterMenus()
	{
		if (UToolMenus* ToolMenus = UToolMenus::Get())
		{
			ToolMenus->UnregisterOwnerByName(OwnerName);
		}
	}
}

void DAValidator_Register()
{
	RegisterMessageLog();

	ToolMenusStartupHandle = UToolMenus::RegisterStartupCallback(
		FSimpleMulticastDelegate::FDelegate::CreateStatic(&RegisterMenus)
	);
}

void DAValidator_Unregister()
{
	if (ToolMenusStartupHandle.IsValid())
	{
		UToolMenus::UnRegisterStartupCallback(ToolMenusStartupHandle);
		ToolMenusStartupHandle.Reset();
	}

	UnregisterMenus();
}