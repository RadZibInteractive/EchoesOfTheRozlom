using UnrealBuildTool;

public class DAValidator : ModuleRules
{
    public DAValidator(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "UnrealEd",
                "ToolMenus",
                "ContentBrowser",
                "AssetRegistry",
                "MessageLog",
                "Slate",
                "SlateCore",
                "GameplayTags"
            }
        );
    }
}