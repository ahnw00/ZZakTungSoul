// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ZZakTungSoul : ModuleRules
{
	public ZZakTungSoul(ReadOnlyTargetRules Target) : base(Target)
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
			"ZZakTungSoul",
			"ZZakTungSoul/Variant_Platforming",
			"ZZakTungSoul/Variant_Platforming/Animation",
			"ZZakTungSoul/Variant_Combat",
			"ZZakTungSoul/Variant_Combat/AI",
			"ZZakTungSoul/Variant_Combat/Animation",
			"ZZakTungSoul/Variant_Combat/Gameplay",
			"ZZakTungSoul/Variant_Combat/Interfaces",
			"ZZakTungSoul/Variant_Combat/UI",
			"ZZakTungSoul/Variant_SideScrolling",
			"ZZakTungSoul/Variant_SideScrolling/AI",
			"ZZakTungSoul/Variant_SideScrolling/Gameplay",
			"ZZakTungSoul/Variant_SideScrolling/Interfaces",
			"ZZakTungSoul/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
