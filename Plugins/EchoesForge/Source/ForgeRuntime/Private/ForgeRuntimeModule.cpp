// © 2026 RadZib. All rights reserved.

#include "Modules/ModuleManager.h"

class FForgeRuntimeModule final : public IModuleInterface
{
public:
	virtual void StartupModule() override {}
	virtual void ShutdownModule() override {}
};

IMPLEMENT_MODULE(FForgeRuntimeModule, ForgeRuntime)
