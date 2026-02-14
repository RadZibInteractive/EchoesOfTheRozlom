// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class EchoesOfTheRozlom : ModuleRules
{
	public EchoesOfTheRozlom(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"UMG",
            "GameplayAbilities",
            "GameplayTags",
            "GameplayTasks",
            "NetCore",
			"PhysicsCore",
            "Niagara",
            "DeveloperSettings",
            "AnimGraphRuntime",
			"MotionWarping"
        });

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"EchoesOfTheRozlom",
            "EchoesOfTheRozlom/AI",
            "EchoesOfTheRozlom/Characters",
            "EchoesOfTheRozlom/Characters/Camera",
            "EchoesOfTheRozlom/Characters/Components",
            "EchoesOfTheRozlom/FrameworkBase",
            "EchoesOfTheRozlom/Player",
            "EchoesOfTheRozlom/Weapons",
            "EchoesOfTheRozlom/Systems",
            "EchoesOfTheRozlom/Systems/Anomalies",
            "EchoesOfTheRozlom/Data/DataAssets",
            "EchoesOfTheRozlom/Data/Structs",
            "EchoesOfTheRozlom/Data/Enums",
        });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
