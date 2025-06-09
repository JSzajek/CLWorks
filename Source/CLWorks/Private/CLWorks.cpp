// Copyright Epic Games, Inc. All Rights Reserved.

#include "CLWorks.h"

#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FCLWorksModule"

void FCLWorksModule::StartupModule()
{
	FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("CLWorks"))->GetBaseDir(), TEXT("/Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/CLShaders"), PluginShaderDir);
}

void FCLWorksModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCLWorksModule, CLWorks)