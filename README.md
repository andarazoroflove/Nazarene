# The Nazarene: Way of the Cross (Mission 1 Prototype)

A 2D arcade action platformer heavily inspired by the original 1987 Sega arcade mechanics of *Shinobi*, set in 1st-century Roman-occupied Judea.

---

## 🎮 Arcade Mechanics Overview

- **Internal Resolution:** 320x224 (authentic Sega arcade resolution), rendered to an off-screen render texture with integer / aspect-ratio preserved scaling to 1280x896 (or fullscreen with `FLAG_VSYNC_HINT`).
- **Dual-Plane Platforming:** The level features two primary platforming planes:
  - **Lower Tier:** Roman paved stone streets, market stalls, and fortified barricades.
  - **Upper Tier:** Elevated stone balconies, Roman aqueduct spans, and rooftop terraces.
  - **Super-Leap (High Jump):** Holding **Up + Jump** executes a high-arc somersault leap enabling the Nazarene to ascend directly between tiers.
  - **Drop-Through:** Holding **Down + Jump** allows dropping down through one-way ledges to return to lower levels.
- **Dynamic Combat System:**
  - **Ranged Spike Toss:** Throws high-velocity iron spikes/nails across the screen.
  - **Automatic Close-Range Melee:** When an enemy is within close proximity (< 24 pixels), attacking automatically triggers an immediate short-range staff strike or palm strike instead of firing a projectile.
  - **Crouch Attacks:** Low nail throws can eliminate low targets and ducked archers.
  - **Divine Wrath (Ninjutsu Equivalent - 1 use per life):** Freezes the screen, calls down a celestial beam of holy light, manifests the radiant glowing **Lion of Judah**, unleashes a screen-wide white flash, plays a thunderous roar, and eliminates all visible regular enemies on screen (while inflicting heavy damage on the Stage Boss).
- **Mission Objective (Streets of Jerusalem):**
  - 4 **Loaves and Fishes** wicker basket pick-ups are hidden across both upper and lower tiers.
  - All 4 baskets must be collected to raise the heavy Roman Iron Portcullis and gain entry to the Colosseum Arena at the far right.
  - Each basket restored 1 pip of life and yields bonus score.
- **Enemies:**
  1. **Patrol Guard (Melee):** Legionary patrolling ledges; lunges forward with spear when player is close; defeated in 1 hit.
  2. **Auxiliary Archer (Ranged):** Ducks behind sandbags/crates to avoid standard standing projectiles; rises periodically to fire horizontal arrows; defeated in 1 hit.
  3. **Heavy Legionary:** Carries a large Roman Scutum shield that deflects frontal attacks with a metallic ping; vulnerable only from behind (by jumping over him) or while swinging his gladius; takes 2 hits.
  4. **Stage 1 Boss - The Gladiator / Armored Centurion:** Wields a barbed Roman combat whip (flagrum). Cracks his whip horizontally at mid-height (forces crouching or high-jumping), slams overhead, stalks forward, and is vulnerable to head strikes or during his recovery cooldown.

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

