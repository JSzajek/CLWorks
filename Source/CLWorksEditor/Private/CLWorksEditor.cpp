// Copyright Epic Games, Inc. All Rights Reserved.

#include "CLWorksEditor.h"

#include "AssetToolsModule.h"

#include "Assets/CLProgramAssetActions.h"
#include "Editor/CLProgramAssetEditorToolkit.h"

#define LOCTEXT_NAMESPACE "FCLWorksEditorModule"

void FCLWorksEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

	EAssetTypeCategories::Type CLCategory = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("CLWorksEditor")),		// Internal category name
																					 FText::FromString(TEXT("CL")));	// Display name

	// Register Custom Asset Actions
	AssetActions = MakeShareable(new FCLProgramAssetActions(CLCategory));
	AssetTools.RegisterAssetTypeActions(AssetActions.ToSharedRef());
}

void FCLWorksEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FCLWorksEditorModule, CLWorksEditor)