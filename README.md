# The Nazarene: Way of the Cross

A 2D arcade action platformer heavily inspired by the original 1987 Sega arcade mechanics of *Shinobi*, set in 1st-century Roman-occupied Judea.

---

## 🎮 Arcade Mechanics & Campaign Overview

- **Internal Resolution:** 320x224 (authentic Sega arcade resolution), rendered to an off-screen render texture with integer / aspect-ratio preserved scaling to 1280x896 (or fullscreen with `FLAG_VSYNC_HINT`).
- **Tactical Jerusalem Mission Map:** Authentic *Shinobi*-style map tracking player progression across 3 distinct campaign areas, with pulsing mission targeting reticles and red Roman `[DEFEATED]` stamps.
- **Three Unique Campaign Stages:**
  - **Stage 1: Via Dolorosa & Roman Garrison** &mdash; Paved stone streets, aqueducts, Roman balconies, and 4 Loaves & Fishes baskets unlocking the Colosseum gate to fight **Centurion Brutus**.
  - **Stage 2: The Inner Temple** &mdash; Vaulted cedar sanctuary, checkered marble floors, menorahs, and multi-tier mezzanines. Collect 4 Sacred Golden Censers while dodging pillar-crawling **Sicarii Shadow Assassins** to confront **Temple Commander Malchus**.
  - **Stage 3: Golgotha Citadel & Fortress Antonia** &mdash; Atmospheric dusk cliffs, Roman standards, roaring iron braziers, and 4 Sacred Relics. Approaching the summit gate causes dark storm clouds to gather and thunder to crash as you face **Tribunal Praetor Longinus**.
- **Dual-Plane & Multi-Tier Platforming:**
  - **Super-Leap (High Jump):** Holding **Up + Jump** executes a high-arc somersault leap enabling the Nazarene to ascend directly between tiers.
  - **Drop-Through:** Holding **Down + Jump** allows dropping down through one-way ledges to return to lower levels.
- **Dynamic Combat System:**
  - **Ranged Spike Toss:** Throws high-velocity iron spikes/nails across the screen.
  - **Automatic Close-Range Melee:** When an enemy is within close proximity (< 24 pixels), attacking automatically triggers an immediate short-range staff strike or palm strike instead of firing a projectile.
  - **Crouch Attacks & Dodging:** Low attacks eliminate ducked archers; crouching ducks underneath spinning Sicarii daggers.
  - **Headshot Jump Challenge:** Bosses feature heavy body armor deflecting frontal shots; defeat them with jump headshots, rear flank attacks, or Divine Wrath.
  - **"FINISH HIM!":** Reaching 1 HP pip on boss battles darkens the sky with apocalyptic lightning and a deep reverberating voice prompt.
  - **Divine Wrath (Ninjutsu Equivalent - 1 use per life):** Freezes the screen, calls down a celestial beam of holy light, manifests the radiant glowing **Lion of Judah**, unleashes a screen-wide white flash, plays a thunderous roar, and eliminates all visible regular enemies on screen (while inflicting heavy damage on the Stage Boss).
  - **Grand Finale & Ascension:** Defeating the Tribunal Praetor parts the storm clouds, reveals a vibrant 7-band celestial rainbow, sings angelic choirs, and ascends Jesus diagonally into the clouds.

---

## 🕹 Controls

| Action | Keyboard | Gamepad (XInput / DirectInput / @Play / Generic) |
|---|---|---|
| **Move Left / Right** | Arrow Keys / `A` / `D` | D-Pad Left/Right, Left Stick, or Hat Axes |
| **Crouch** | Down Arrow / `S` | D-Pad Down, Left Stick Down, or Hat Down |
| **Look Up** | Up Arrow / `W` | D-Pad Up, Left Stick Up, or Hat Up |
| **Jump** | `Spacebar` | Button `A` or Button `B` (Face Down / Right) |
| **Dual-Plane High Jump** | `Up` + `Spacebar` | `Up` + Button `A` / `B` |
| **Drop Through Ledge** | `Down` + `Spacebar` | `Down` + Button `A` / `B` |
| **Attack (Melee < 24px / Ranged)** | `J` or `Z` | Button `X` (Face Left), Right Bumper (`RB`), or Right Trigger (`RT`) |
| **Divine Wrath (1 per life)** | `K` or `X` | Button `Y` (Face Up), Left Bumper (`LB`), or Left Trigger (`LT`) |
| **Pause / Start** | `Enter` / `Escape` | `Start` / `Select` / `Mode` Button |
| **Toggle Fullscreen** | `F11` / `Alt` + `Enter` | - |
| **Quick Restart** | `R` | - |

---

## 🛠 Technology & Build Instructions

- **Engine & Library:** Raylib 5.5 (statically linked for a standalone portable executable).
- **Language Standard:** C++20.
- **Platform:** Windows x64.
- **Audio & Graphics:** 100% procedural wave audio synthesis and procedural pixel art generator built directly into C++ code—runs out of the box with zero external assets required.

### Quick Build & Run

To compile and build `Nazarene.exe`, simply execute:
```bat
build.bat
```
This automatically invokes CMake and Ninja / MinGW, statically compiles Raylib and the game files, and places `Nazarene.exe` in the root project folder.

To run the game:
```bat
Nazarene.exe
```

