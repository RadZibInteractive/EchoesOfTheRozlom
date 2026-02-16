# DAValidator

**Custom Editor Validation Plugin**  
Internal tool for validating DataAssets.

---

## Purpose

Provides manual validation for `UDataAsset`-based assets.

- Validates required fields
- Traverses nested structs / arrays / maps
- Reports issues to Message Log

---

## Usage

1. Right-click DataAsset in Content Browser
2. Select **Validate DataAsset**
3. Review results in Message Log

---

## Validation Rules

- `meta = (DARequired)` → treated as Error
- Null references → Warning (unless required)
- Full property path is displayed (e.g. `Effects[2].VFX.WorldLoop`)

---
