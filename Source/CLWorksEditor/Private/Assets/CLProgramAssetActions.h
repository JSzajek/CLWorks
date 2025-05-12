#pragma once

#include "AssetTypeActions_Base.h"

#include "Assets/CLProgramAsset.h"

class CLWORKSEDITOR_API FCLProgramAssetActions : public FAssetTypeActions_Base
{
public:
	FCLProgramAssetActions(EAssetTypeCategories::Type InCategory)
		: mCategory(InCategory)
	{
	}
public:
	virtual FText GetName() const override 
	{
		return NSLOCTEXT("CLWorks", "CLProgramAssetActions", "CL Program"); 
	}

	virtual FColor GetTypeColor() const override
	{
		return FColor::Orange;
	}

	virtual UClass* GetSupportedClass() const override
	{
		return UCLProgramAsset::StaticClass();
	}

	virtual uint32 GetCategories() override
	{
		return mCategory;
	}

	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, 
								 TSharedPtr<IToolkitHost> ToolkitHost) override;
private:
	EAssetTypeCategories::Type mCategory;
};