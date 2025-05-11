using UnrealBuildTool;
using System.IO;

public class OpenCL : ModuleRules
{
	protected virtual bool IsSupportedWindowsPlatform(ReadOnlyTargetRules Target)
	{
		return Target.Platform == UnrealTargetPlatform.Win64;
	}

	public OpenCL(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;

		if (IsSupportedWindowsPlatform(Target))
		{
            System.Console.WriteLine("Linking OpenCL Library.");

            string ThirdPartyLibsPath = Path.Combine(PluginDirectory, "ThirdParty/");
            string ModulePath = ModuleDirectory + "/";

            // Include UE Library Includes Path
            PublicSystemIncludePaths.Add(ModulePath + "Include/");

            // Include TPL includes path
            PublicSystemIncludePaths.Add(ThirdPartyLibsPath + "OpenCL/include/");

            // Add TPL Library and Binaries -------------------------------------------------------
            PublicAdditionalLibraries.Add(ThirdPartyLibsPath + "OpenCL/lib/OpenCL.lib");
            PublicAdditionalLibraries.Add(ThirdPartyLibsPath + "OpenCL/lib/OpenCLExt.lib");
            PublicAdditionalLibraries.Add(ThirdPartyLibsPath + "OpenCL/lib/OpenCLUtils.lib");
            PublicAdditionalLibraries.Add(ThirdPartyLibsPath + "OpenCL/lib/OpenCLUtilsCpp.lib");

            string BinPath = Path.Combine(ThirdPartyLibsPath, "OpenCL/bin");
            DirectoryInfo binDirectory = new DirectoryInfo(BinPath);
            if (binDirectory.Exists)
            {
                FileInfo[] files = binDirectory.GetFiles();
                foreach (FileInfo file in files)
                {
                    RuntimeDependencies.Add("$(BinaryOutputDir)/" + file.Name, file.FullName);
                }
            }
            // ------------------------------------------------------------------------------------
        }
	}
}

