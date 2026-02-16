using UnrealBuildTool;

public class DAForgeEditor : ModuleRules
{
    public DAForgeEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "UnrealEd",
            "ToolMenus",
            "AssetTools",
            "AssetRegistry",
            "ContentBrowser",
            "ContentBrowserData",
            "Slate",
            "SlateCore",
            "InputCore",
            "EditorStyle",
            "EditorSubsystem",
            "DeveloperSettings",
            "PropertyEditor",
            "DAForgeRuntime"
        });
    }
}