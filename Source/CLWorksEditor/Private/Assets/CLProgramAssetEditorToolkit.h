#pragma once

#include "CoreMinimal.h"

#include "Toolkits/AssetEditorToolkit.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"

class UCLProgramAsset;

class FCLProgramAssetEditorToolkit : public FAssetEditorToolkit
{
public:
	void InitEditor(const EToolkitMode::Type& Mode, 
					const TSharedPtr<class IToolkitHost>& InitToolkitHost,
					TObjectPtr<UCLProgramAsset> InAsset);

	void RegisterTabSpawners(const TSharedRef<FTabManager>& TabManager) override;

	virtual FName GetToolkitFName() const override 
	{ 
		return "CLProgramEditor"; 
	}

	virtual FText GetBaseToolkitName() const override 
	{ 
		return NSLOCTEXT("CLWorks", "CLProgramEditor", "CL Program Editor");
	}

	virtual FString GetWorldCentricTabPrefix() const override 
	{ 
		return TEXT("CLProgram"); 
	}

	virtual FLinearColor GetWorldCentricTabColorScale() const override 
	{ 
		return FLinearColor::Red; 
	}
private:
	TSharedRef<SDockTab> SpawnEditorTab(const FSpawnTabArgs& Args);

	void OnSourceChanged(const FText& NewText, ETextCommit::Type CommitType);
	FReply OnCompileClicked();
private:
	TSharedPtr<SMultiLineEditableTextBox> mpSourceEditor;
	TSharedPtr<STextBlock> mpCompileStatusText;
	TSharedPtr<SMultiLineEditableText> mpErrorLogOutput;

	TObjectPtr<UCLProgramAsset> mpProgramAsset;
};