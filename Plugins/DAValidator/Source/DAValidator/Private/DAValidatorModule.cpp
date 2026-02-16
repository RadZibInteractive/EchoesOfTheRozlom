#include "Modules/ModuleManager.h"

class FDAValidatorModule final : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

IMPLEMENT_MODULE(FDAValidatorModule, DAValidator)

void FDAValidatorModule::StartupModule()
{
	extern void DAValidator_Register();
	DAValidator_Register();
}

void FDAValidatorModule::ShutdownModule()
{
	extern void DAValidator_Unregister();
	DAValidator_Unregister();
}