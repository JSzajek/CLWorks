using EpicGames.Core;
using UnrealBuildTool;
using System.IO;

public class Assimp : ModuleRules
{
	protected virtual bool IsSupportedWindowsPlatform(ReadOnlyTargetRules Target)
	{
		return Target.Platform == UnrealTargetPlatform.Win64;
	}

	public Assimp(ReadOnlyTargetRules Target) : base(Target)
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
            PublicSystemIncludePaths.Add(ThirdPartyLibsPath + "Assimp-5.4.3/include/");

            // Add TPL Library and Binaries -------------------------------------------------------
            PublicAdditionalLibraries.Add(ThirdPartyLibsPath + "Assimp-5.4.3/lib/assimp-vc143-mt.lib");

            string BinPath = Path.Combine(ThirdPartyLibsPath, "Assimp-5.4.3/bin");
            DirectoryInfo dllinfo2 = new DirectoryInfo(BinPath);
            if (dllinfo2.Exists)
            {
                FileInfo[] files = dllinfo2.GetFiles("*.dll");
                foreach (FileInfo file in files)
                {
                    RuntimeDependencies.Add("$(BinaryOutputDir)/" + file.Name, file.FullName);
                }
            }
            // ------------------------------------------------------------------------------------
        }
	}
}

