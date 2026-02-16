# DAForge

**Custom DataAsset Creation Tool**  
Internal asset factory for Forge-based DataAssets.

---

## Purpose

Streamlined creation of project-specific DataAssets.

- Type-based creation (PrimaryAssetType)
- Auto path routing
- Auto numbering
- Optional icon override per asset

---

## Usage

1. Right-click in Content Browser
2. Select **DA Forge: Create Asset...**
3. Choose type
4. (Optional) set icon
5. (Optional) set suffix
6. Create

---

## Configuration

Configured via:
Editor Settings → Plugins → DAForge

Each category defines:

- PrimaryAssetType
- Display Title
- Asset Class (must derive from UForgeDataAsset)
- Root Path
- Default Icon
- Open After Create

---
