// © 2026 RadZib. All rights reserved.

using UnrealBuildTool;

public class ForgeCore : ModuleRules
{
    public ForgeCore(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "GameplayTags"
        });
    }
}
