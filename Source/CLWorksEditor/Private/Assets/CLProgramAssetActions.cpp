#include "Assets/CLProgramAssetActions.h"


#if WITH_EDITOR

#include "Editor/CLProgramAssetEditorToolkit.h"

void FCLProgramAssetActions::OpenAssetEditor(const TArray<UObject*>& InObjects, 
											 TSharedPtr<IToolkitHost> ToolkitHost)
{
	for (UObject* Obj : InObjects)
	{
		if (UCLProgramAsset* Asset = Cast<UCLProgramAsset>(Obj))
		{
			TSharedRef<FCLProgramAssetEditorToolkit> EditorToolkit = MakeShared<FCLProgramAssetEditorToolkit>();
			EditorToolkit->InitEditor(EToolkitMode::Standalone, ToolkitHost, Asset);
		}
	}
}

#endif