#include "CLProgramAssetEditorToolkit.h"

#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "Widgets/Docking/SDockTab.h"
#include "Framework/Docking/TabManager.h"

#include "CLWorksLib.h"

const FName CLProgramEditorTab(TEXT("ShaderSourceTab"));

void FCLProgramAssetEditorToolkit::InitEditor(const EToolkitMode::Type& Mode, 
											  const TSharedPtr<class IToolkitHost>& InitToolkitHost, 
											  TObjectPtr<UCLProgramAsset> InAsset)
{
	mpProgramAsset = InAsset;

	const TSharedRef<FTabManager::FLayout> Layout = FTabManager::NewLayout("CLProgramEditorLayout_v2")
													->AddArea(FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
														->Split(FTabManager::NewStack()
															->AddTab(CLProgramEditorTab, ETabState::OpenedTab)
																->SetHideTabWell(true)));

	FAssetEditorToolkit::InitAssetEditor(Mode, InitToolkitHost, "CLProgramEditorApp", Layout, true, true, Cast<UObject>(InAsset));

	RegenerateMenusAndToolbars();
}

void FCLProgramAssetEditorToolkit::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner(CLProgramEditorTab, FOnSpawnTab::CreateSP(this, &FCLProgramAssetEditorToolkit::SpawnEditorTab))
																				 .SetDisplayName(FText::FromString("Source"))
																				 .SetGroup(WorkspaceMenuCategory.ToSharedRef());
}

TSharedRef<SDockTab> FCLProgramAssetEditorToolkit::SpawnEditorTab(const FSpawnTabArgs& Args)
{
	return SNew(SDockTab).Label(NSLOCTEXT("OpenCL", "ShaderSourceTab", "OpenCL Source"))
		   [
			   SNew(SShaderEditorWidget).ProgramAsset(mpProgramAsset)
		   ];
}
