// © 2026 RadZib. All rights reserved.

using UnrealBuildTool;

public class ForgeRuntime : ModuleRules
{
    public ForgeRuntime(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "GameplayTags",
            "ForgeCore"
        });
    }
}
