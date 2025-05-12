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
	return SNew(SDockTab)
		.Label(NSLOCTEXT("OpenCL", "ShaderSourceTab", "OpenCL Source"))
		[
			SNew(SVerticalBox)

			// Compile button and result
			+ SVerticalBox::Slot().AutoHeight().Padding(5)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot().AutoWidth()
				[
					SNew(SButton).Text(FText::FromString("Compile"))
								 .OnClicked_Lambda([this]()
					{
						return OnCompileClicked();
					})
				]
				// Status Text ("Success", "Failed", etc.)
				+ SHorizontalBox::Slot().AutoWidth().Padding(10, 0)
				[
					SAssignNew(mpCompileStatusText, STextBlock).Text(FText::FromString(""))
				]
			]

			// Shader source code box
			+ SVerticalBox::Slot().FillHeight(1).Padding(5)
			[
				SAssignNew(mpSourceEditor, SMultiLineEditableTextBox).Text(FText::FromString(mpProgramAsset->SourceCode)).OnTextCommitted_Lambda([this](const FText& NewText, ETextCommit::Type CommitType)
				{
					if (mpProgramAsset)
					{
						mpProgramAsset->Modify();
						mpProgramAsset->SourceCode = NewText.ToString();
					}
				})
				//std::bind(this, &FCLProgramAssetEditorToolkit::OnSourceChanged, std::placeholders::_1, std::placeholders::_2))
			]

			// Error Log Output (Multi-line)
			+ SVerticalBox::Slot().AutoHeight().Padding(5)
			[
				SNew(SBorder).BorderBackgroundColor(FLinearColor(0.1f, 0.1f, 0.1f))
				[
					SAssignNew(mpErrorLogOutput, SMultiLineEditableText).Text(FText::FromString(""))
																		.IsReadOnly(true)
				]
			]
		];
}

void FCLProgramAssetEditorToolkit::OnSourceChanged(const FText& NewText, ETextCommit::Type CommitType)
{
	if (mpProgramAsset)
	{
		mpProgramAsset->Modify();
		mpProgramAsset->SourceCode = NewText.ToString();
	}
}

FReply FCLProgramAssetEditorToolkit::OnCompileClicked()
{
	if (!mpProgramAsset)
		return FReply::Handled();


	// TODO:: Compile Here

	FString CompileLog = "Log...";
	bool success = true;

	// Update status line
	if (mpCompileStatusText.IsValid())
	{
		mpCompileStatusText->SetText(FText::FromString(success ? TEXT("Success") : TEXT("Failed")));
		mpCompileStatusText->SetColorAndOpacity(success ? FLinearColor::Green : FLinearColor::Red);
	}

	// Update error log
	if (mpErrorLogOutput.IsValid())
	{
		mpErrorLogOutput->SetText(FText::FromString(CompileLog));
	}

	return FReply::Handled();
}
