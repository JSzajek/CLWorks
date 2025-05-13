#include "Widgets/SShaderEditorWidget.h"

#include "Styling/AppStyle.h"

#include "CLWorksLib.h"

void SShaderEditorWidget::Construct(const FArguments& InArgs)
{
	mpProgramAsset = InArgs._ProgramAsset;

	// Initial Internal Data --------------------
	mpProgramData = MakeShared<ProgramData>();

	mpProgramData->mpDevice = new OpenCL::Device();
	mpProgramData->mpContext = new OpenCL::Context(*mpProgramData->mpDevice);
	// ------------------------------------------

	ChildSlot
	[
		SNew(SVerticalBox)

		// Top row: compile button and status
		+ SVerticalBox::Slot().AutoHeight().Padding(2)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().AutoWidth().Padding(2)
			[
				SNew(SButton).Text(FText::FromString("Compile"))
							 .OnClicked(this, &SShaderEditorWidget::OnCompileClicked)
			]
			+ SHorizontalBox::Slot().FillWidth(1).VAlign(VAlign_Center).Padding(5, 0)
			[
				SAssignNew(mpStatusText, STextBlock).Text(FText::FromString("Idle"))
			]
		]

		// Main text editor with line numbers
		+ SVerticalBox::Slot().FillHeight(1).Padding(2)
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot().AutoWidth().Padding(4)
			[
				SAssignNew(mpLineNumberDisplay, SLineNumberBox)
			]

			+ SHorizontalBox::Slot().FillWidth(1)
			[
				SAssignNew(mpSourceEditor, SMultiLineEditableTextBox).Text(FText::FromString(mpProgramAsset.IsValid() ? mpProgramAsset->SourceCode : TEXT("")))
																     .OnTextChanged(this, &SShaderEditorWidget::OnSourceChanged)
																	 .OnKeyDownHandler(this, &SShaderEditorWidget::OnHandleKeyDown)
																     .Font(FAppStyle::GetFontStyle("MonoFont"))
			]
		]

		// Error log
		+ SVerticalBox::Slot().AutoHeight().Padding(2)
		[
			SAssignNew(mpErrorLogOutput, SMultiLineEditableTextBox).Text(FText::FromString(""))
																   .IsReadOnly(true)
																   .AutoWrapText(true)
		]
	];

	// Check Initial Compile Status
	OnCompileClicked();
	
	if (mpProgramAsset.IsValid())
		UpdateLineNumbers(mpProgramAsset->SourceCode);
}

FReply SShaderEditorWidget::OnCompileClicked()
{
	if (!mpProgramAsset.IsValid())
		return FReply::Handled();

	FString compileLog = "";
	bool success = true;
	if (mpProgramAsset->SourceCode.IsEmpty())
	{
		success = false;
		compileLog = "Empty Program!";
	}
	else
	{
		OpenCL::Program program(*mpProgramData->mpDevice, *mpProgramData->mpContext);

		std::string sourceCode = TCHAR_TO_UTF8(*mpProgramAsset->SourceCode);

		std::string errMsg;
		success = program.ReadFromString(sourceCode, &errMsg);

		if (!success)
			compileLog = UTF8_TO_TCHAR(errMsg.c_str());
	}

	// Update Status Line
	if (mpStatusText.IsValid())
	{
		mpStatusText->SetText(FText::FromString(success ? TEXT("Compiled") : TEXT("Failed")));
		mpStatusText->SetColorAndOpacity(success ? FLinearColor::Green : FLinearColor::Red);
	}

	// Update Error Log
	if (mpErrorLogOutput.IsValid())
		mpErrorLogOutput->SetText(FText::FromString(compileLog));

	return FReply::Handled();
}

void SShaderEditorWidget::OnSourceChanged(const FText& NewText)
{
	if (mpProgramAsset.IsValid())
	{
		mpProgramAsset->Modify();
		mpProgramAsset->SourceCode = NewText.ToString();
	}

	UpdateLineNumbers(NewText.ToString());
}

FReply SShaderEditorWidget::OnHandleKeyDown(const FGeometry& MyGeometry, 
										  const FKeyEvent& InKeyEvent)
{
	if (!mpSourceEditor.IsValid())
		return FReply::Unhandled();

	const FKey key = InKeyEvent.GetKey();
	const bool shiftMod = InKeyEvent.IsShiftDown();

	if (key == EKeys::Tab)
	{
		InsertTabOrUnindent(shiftMod);
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

void SShaderEditorWidget::InsertTabOrUnindent(bool shiftMod)
{
	FText currentText = mpSourceEditor->GetText();
	FString textString = currentText.ToString();

	FTextLocation cursorLocation = mpSourceEditor->GetCursorLocation();

	const int32 cursorLine = cursorLocation.GetLineIndex();
	const int32 cursorColumn = cursorLocation.GetOffset();

	TArray<FString> lines;
	textString.ParseIntoArrayLines(lines);

	// No valid line?
	if (!lines.IsValidIndex(cursorLine))
		return;

	FString& targetLine = lines[cursorLine];
	int32_t modDelta = 0;
	if (shiftMod) // Un-Indent
	{
		if (targetLine.StartsWith(TEXT("\t")))
		{
			targetLine.RemoveAt(0, 1, EAllowShrinking::No);
			modDelta = -1;
		}
		else if (targetLine.StartsWith(TEXT("    ")))
		{
			targetLine.RemoveAt(0, 4, EAllowShrinking::No);
			modDelta = -4;
		}
	}
	else
	{
		targetLine = TEXT("\t") + targetLine;
		modDelta = 1;
	}

	// Reconstruct Full Text --------------------
	const FString rebuiltText = FString::Join(lines, TEXT("\n"));
	mpSourceEditor->SetText(FText::FromString(rebuiltText));

	mpSourceEditor->GoTo(FTextLocation(cursorLocation, modDelta));

	UpdateLineNumbers(rebuiltText);
	// ------------------------------------------
}

void SShaderEditorWidget::UpdateLineNumbers(const FString& Text)
{
	TArray<FString> Lines;
	Text.ParseIntoArrayLines(Lines, false);

	if (mpLineNumberDisplay.IsValid())
	{
		mpLineNumberDisplay->UpdateLineNumbers(Lines.Num());
	}
}

SShaderEditorWidget::ProgramData::~ProgramData()
{
	if (mpDevice)
	{
		delete mpDevice;
		mpDevice = nullptr;
	}

	if (mpContext)
	{
		delete mpContext;
		mpContext = nullptr;
	}
}
