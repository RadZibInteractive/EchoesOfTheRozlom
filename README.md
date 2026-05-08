# Echoes of the Rozlom

A post-apocalyptic narrative RPG set in Eastern Europe, built on Unreal Engine 5.
Players explore a world fractured by anomalous energy — the Rozlom — navigating factions, anomalies, and consequences.

This repository includes **Echoes Forge** — a custom UE5 editor plugin built for the project that lets designers place actors, manage data assets, edit dialogue trees, visualize quests, and get AI-powered scenario suggestions — without writing code.

---

## Requirements

- Unreal Engine **5.6**
- Visual Studio **2022**
- Git + **Git LFS**

---

## Setup

```bash
# 1. Clone the repository
git clone <repo-url>
cd EchoesOfTheRozlom

# 2. Pull LFS assets (required — large Unreal assets are stored in LFS)
git lfs install
git lfs pull

# 3. Open the project
# Double-click EchoesOfTheRozlom.uproject
# UE5 will prompt to build missing modules — click Yes
```

---

## Opening Echoes Forge

Once the project is open in UE5:

1. Top menu → **Tools** → **Echoes Forge**
2. The editor panel opens as a dockable tab

The panel has three sections:
- **Left** — Place actors, browse data assets, inspect the scene
- **Center** — Viewport · Dialogue editor · Quest visualizer · Zone stats
- **Right** — AI Suggestions panel

---

## AI Suggestions (optional)

The right panel connects to the Claude API for scenario suggestions.
To enable it:

1. Get an API key at [console.anthropic.com](https://console.anthropic.com)
2. Paste it into the **AI API Key** field in the Echoes Forge panel
3. Click **Save**

The panel works without a key — AI suggestions will simply be unavailable.

---

## What to Evaluate

| Feature | Where to find |
|---------|--------------|
| Actor placement (NPC / Anomaly / Enemy / Prop) | Left panel → Place tab → pick activity → Browse → Place in World |
| Data asset browser | Left panel → Browse tab |
| Placed actors in scene | Left panel → Browse → Scene tab — click any actor to focus camera |
| Dialogue tree editor | Center → Dialogue tab |
| Quest state visualizer | Center → Quest tab |
| Zone balance & stats | Center → Stats tab |
| AI scenario suggestions | Right panel → type an idea → Ask AI |
| Validation | Bottom bar → Validate button in toolbar |
| Starter assets | Place tab → Create Starter Assets (generates NPC / Anomaly / Dialogue / Quest) |

---

## Git LFS

This project uses **Git LFS** for Unreal Engine assets.

```bash
git lfs install
git lfs pull
 ``` 
 
## Workflow

| Branch | Purpose |
|--------|---------|
| `main` | Stable — reviewed, releasable state |
| `dev` | Integration branch — features merge here first |
| `echoes-forge/dev` | Active Forge plugin development |
| `feature/<name>` | One branch per feature (e.g. `features/anomalies-interaction`, `features/weapon-system`) |

- All feature work branches off `dev`
- Feature branches merge into `dev` via pull request, never directly into `main`
- `dev` → `main` only when stable and tested
- No direct commits to `main`

---

## Notes

- Do not remove Git LFS
- Do not commit generated files (`Binaries/`, `Intermediate/`, `Saved/`)
- Keep commits clean and scoped

---

## Custom Editor Plugins

- **EchoesForge** — Main editor toolset (Place · Browse · Dialogue · Quest · Stats · AI)
- **DAValidator** – DataAsset validation — [Plugins/DAValidator/README.md](Plugins/DAValidator/README.md)
- **DAForge** – Structured DataAsset creation — [Plugins/DAForge/README.md](Plugins/DAForge/README.md)

---
