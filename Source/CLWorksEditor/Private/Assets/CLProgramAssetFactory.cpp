#include "CLProgramAssetFactory.h"

#include "Assets/CLProgramAsset.h"

UCLProgramAssetFactory::UCLProgramAssetFactory(const FObjectInitializer& ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UCLProgramAsset::StaticClass();
}

UObject* UCLProgramAssetFactory::FactoryCreateNew(UClass* Class, 
												  UObject* InParent, 
												  FName Name, 
												  EObjectFlags Flags, 
												  UObject* Context, 
												  FFeedbackContext* Warn)
{
	UCLProgramAsset* NewAsset = NewObject<UCLProgramAsset>(InParent, Class, Name, Flags);
	if (NewAsset)
	{
		NewAsset->ProgramName = Name.ToString();
		NewAsset->SourceCode = "__kernel void example()\n{\n}";
	}

	return NewAsset;
}

