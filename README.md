# Echoes of the Rozlom

**Private Unreal Engine project**  
Internal development repository.

---

## Requirements

- Unreal Engine **5.6**
- Visual Studio **2022**
- Git + **Git LFS**

---

## Git LFS (Required)

This project uses **Git LFS** for Unreal Engine assets.

Before working with the repository:

```bash
git lfs install
git lfs pull
 ``` 
 
## Workflow

- `main` — stable development branch
- Feature branches for active work
- No direct commits to `main`

---

## Notes

- Do not remove Git LFS
- Do not commit generated files
- Keep commits clean and scoped

---

## Custom Editor Plugins

This project includes internal editor tools:

- **DAValidator** – DataAsset validation tool  
  [Plugins/DAValidator/README.md](Plugins/DAValidator/README.md)

- **DAForge** – Structured DataAsset creation tool  
  [Plugins/DAForge/README.md](Plugins/DAForge/README.md)