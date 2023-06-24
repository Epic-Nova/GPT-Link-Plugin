/*==========================================================================>
|				GPT-Link - ChatGPT interacting with Unreal Engine			|
| ------------------------------------------------------------------------- |
|					Copyright (C) 2023 Simsalabim Studios.					|
| ------------------------------------------------------------------------- |
<==========================================================================*/

using System.IO;
using UnrealBuildTool;

public class GPTLink : ModuleRules
{
	public GPTLink(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		var PublicIncludeDirectory = System.IO.Path.Combine(ModuleDirectory, "Public");
		var PrivateIncludeDirectory = System.IO.Path.Combine(ModuleDirectory, "Private");
		
		PrivatePCHHeaderFile = Path.Combine(PrivateIncludeDirectory, "GPTLink_PCH.h");
		
		
		PublicIncludePaths.AddRange(new string[] {});
				
		
		PrivateIncludePaths.AddRange(new string[] { });
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore"
			});
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"HTTP",
				"Json",
				"JsonUtilities",
				"ApplicationCore",
				"DeveloperSettings"
			});
		
	}
}
