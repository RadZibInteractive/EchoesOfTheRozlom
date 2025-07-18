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
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
			"GameplayMessageRuntime",
			"ModularGameplay",
			"NetCore",
			"PhysicsCore",
            "IrisCore"
        });

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"EchoesOfTheRozlom",
            "EchoesOfTheRozlom/AbilitySystem",
            "EchoesOfTheRozlom/AbilitySystem/Abilities",
            "EchoesOfTheRozlom/AbilitySystem/Attributes",
            "EchoesOfTheRozlom/AbilitySystem/Executions",
            "EchoesOfTheRozlom/AI",
			"EchoesOfTheRozlom/Characters",
            "EchoesOfTheRozlom/Characters/Camera",
            "EchoesOfTheRozlom/Characters/Components",
            "EchoesOfTheRozlom/GameModes",
            "EchoesOfTheRozlom/Items",
            "EchoesOfTheRozlom/Player",
            "EchoesOfTheRozlom/Weapons",
            "EchoesOfTheRozlom/System",
            "EchoesOfTheRozlom/Messages",
            "EchoesOfTheRozlom/Physics"
        });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
