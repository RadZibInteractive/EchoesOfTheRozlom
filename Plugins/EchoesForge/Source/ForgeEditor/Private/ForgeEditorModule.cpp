// © 2026 RadZib. All rights reserved.

#include "ForgeEditorModule.h"
#include "UI/SForgeMainPanel.h"
#include "ToolMenus.h"
#include "Widgets/Docking/SDockTab.h"
#include "Framework/Docking/TabManager.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"

#define LOCTEXT_NAMESPACE "ForgeEditor"

const FName FForgeEditorModule::ForgeTabId = FName("EchoesForgeTab");

void FForgeEditorModule::StartupModule()
{
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
		ForgeTabId,
		FOnSpawnTab::CreateRaw(this, &FForgeEditorModule::SpawnForgeTab))
		.SetDisplayName(LOCTEXT("ForgeTabTitle", "Echoes Forge"))
		.SetTooltipText(LOCTEXT("ForgeTabTooltip", "Open the Echoes Forge content creation editor"))
		.SetGroup(WorkspaceMenu::GetMenuStructure().GetToolsCategory());

	RegisterMenus();
}

void FForgeEditorModule::ShutdownModule()
{
	UnregisterMenus();
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(ForgeTabId);
}

void FForgeEditorModule::RegisterMenus()
{
	UToolMenus::RegisterStartupCallback(
		FSimpleMulticastDelegate::FDelegate::CreateLambda([]()
		{
			UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Tools");
			FToolMenuSection& Section = Menu->FindOrAddSection(
				"ForgeTools", LOCTEXT("ForgeSection", "Echoes Forge"));

			Section.AddMenuEntry(
				FName("OpenForgeEditor"),
				LOCTEXT("OpenForge",        "Open Echoes Forge"),
				LOCTEXT("OpenForgeTooltip", "Launch the Echoes Forge narrative content editor"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateLambda([]()
				{
					FGlobalTabmanager::Get()->TryInvokeTab(FForgeEditorModule::ForgeTabId);
				}))
			);
		}));
}

void FForgeEditorModule::UnregisterMenus()
{
	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);
}

TSharedRef<SDockTab> FForgeEditorModule::SpawnForgeTab(const FSpawnTabArgs& Args)
{
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		.Label(LOCTEXT("ForgeTabLabel", "Echoes Forge"))
		[
			SNew(SForgeMainPanel)
		];
}

IMPLEMENT_MODULE(FForgeEditorModule, ForgeEditor)

#undef LOCTEXT_NAMESPACE
