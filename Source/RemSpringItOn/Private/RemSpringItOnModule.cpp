// Copyright RemRemRemRe, All Rights Reserved.

#include "RemSpringItOnModule.h"
#include "Modules/ModuleManager.h"

IRemSpringItOnModule& IRemSpringItOnModule::Get()
{
	return FModuleManager::LoadModuleChecked< IRemSpringItOnModule >( "RemSpringItOn" );
}

bool IRemSpringItOnModule::IsAvailable()
{
	return FModuleManager::Get().IsModuleLoaded( "RemSpringItOn" );
}


class FRemSpringItOnModule final : public IRemSpringItOnModule
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

IMPLEMENT_MODULE(FRemSpringItOnModule, RemSpringItOn)

void FRemSpringItOnModule::StartupModule()
{
	// This code will execute after your module is loaded into memory (but after global variables are initialized, of course.)
	IRemSpringItOnModule::StartupModule();
}

void FRemSpringItOnModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	IRemSpringItOnModule::ShutdownModule();
}
