#pragma once

#include "Widgets/Text/STextBlock.h"
#include "Widgets//SLineNumberBox.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"

namespace OpenCL
{
	class Device;
	class Context;
}

class UCLProgramAsset;

class SShaderEditorWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SShaderEditorWidget) {}
		SLATE_ARGUMENT(TWeakObjectPtr<UCLProgramAsset>, ProgramAsset)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
private:
	FReply OnCompileClicked();
	void OnSourceChanged(const FText& NewText);
	FReply OnHandleKeyDown(const FGeometry& MyGeometry,
						   const FKeyEvent& InKeyEvent);

	void InsertTabOrUnindent(bool shiftMod);
	void UpdateLineNumbers(const FString& Text);
private:
	TWeakObjectPtr<UCLProgramAsset> mpProgramAsset;

	TSharedPtr<SMultiLineEditableTextBox> mpSourceEditor;
	TSharedPtr<STextBlock> mpStatusText;
	TSharedPtr<SMultiLineEditableTextBox> mpErrorLogOutput;
	TSharedPtr<SLineNumberBox> mpLineNumberDisplay;

	struct ProgramData
	{
	public:
		ProgramData() = default;
		~ProgramData();
	public:
		OpenCL::Device* mpDevice = nullptr;
		OpenCL::Context* mpContext = nullptr;
	};
	TSharedPtr<ProgramData> mpProgramData;
};