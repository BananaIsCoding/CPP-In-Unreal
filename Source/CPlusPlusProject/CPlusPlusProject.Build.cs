// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CPlusPlusProject : ModuleRules
{
	public CPlusPlusProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"CPlusPlusProject",
			"CPlusPlusProject/Variant_Platforming",
			"CPlusPlusProject/Variant_Platforming/Animation",
			"CPlusPlusProject/Variant_Combat",
			"CPlusPlusProject/Variant_Combat/AI",
			"CPlusPlusProject/Variant_Combat/Animation",
			"CPlusPlusProject/Variant_Combat/Gameplay",
			"CPlusPlusProject/Variant_Combat/Interfaces",
			"CPlusPlusProject/Variant_Combat/UI",
			"CPlusPlusProject/Variant_SideScrolling",
			"CPlusPlusProject/Variant_SideScrolling/AI",
			"CPlusPlusProject/Variant_SideScrolling/Gameplay",
			"CPlusPlusProject/Variant_SideScrolling/Interfaces",
			"CPlusPlusProject/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
