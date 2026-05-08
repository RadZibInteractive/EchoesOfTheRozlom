// © 2026 RadZib. All rights reserved.

#pragma once

#include "Modules/ModuleManager.h"
#include "Framework/Docking/TabManager.h"

class FForgeEditorModule final : public IModuleInterface
{
public:
	static const FName ForgeTabId;

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void RegisterMenus();
	void UnregisterMenus();

	TSharedRef<SDockTab> SpawnForgeTab(const FSpawnTabArgs& Args);
};
