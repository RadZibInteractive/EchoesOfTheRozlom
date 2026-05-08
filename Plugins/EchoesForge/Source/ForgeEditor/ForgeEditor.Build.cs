// © 2026 RadZib. All rights reserved.

using UnrealBuildTool;

public class ForgeEditor : ModuleRules
{
    public ForgeEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "UnrealEd",
            "ToolMenus",
            "Slate",
            "SlateCore",
            "InputCore",
            "EditorStyle",
            "EditorSubsystem",
            "PropertyEditor",
            "AssetTools",
            "AssetRegistry",
            "WorkspaceMenuStructure",
            "EditorFramework",
            "LevelEditor",
            "EditorWidgets",
            "ContentBrowser",
            "ContentBrowserData",
            "HTTP",
            "Json",
            "JsonUtilities",
            "ForgeCore",
            "ForgeRuntime"
        });
    }
}
