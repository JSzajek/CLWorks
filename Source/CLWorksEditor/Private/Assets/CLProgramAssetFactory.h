#pragma once

#if WITH_EDITOR

#include "CoreMinimal.h"

#include "Factories/Factory.h"

#include "CLProgramAssetFactory.generated.h"


UCLASS()
class CLWORKSEDITOR_API UCLProgramAssetFactory : public UFactory
{
	GENERATED_BODY()
public:
	UCLProgramAssetFactory(const FObjectInitializer& ObjectInitializer);

	virtual UObject* FactoryCreateNew(UClass* Class, 
									  UObject* InParent, 
									  FName Name, 
									  EObjectFlags Flags, 
									  UObject* Context, 
									  FFeedbackContext* Warn) override;
};

#endif