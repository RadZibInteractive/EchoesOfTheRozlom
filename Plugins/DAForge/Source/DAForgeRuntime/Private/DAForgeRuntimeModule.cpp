// © 2026 RadZib. All rights reserved.

#include "Modules/ModuleManager.h"

class FDAForgeRuntimeModule final : public IModuleInterface
{
public:
	virtual void StartupModule() override {}
	virtual void ShutdownModule() override {}
};

IMPLEMENT_MODULE(FDAForgeRuntimeModule, DAForgeRuntime)