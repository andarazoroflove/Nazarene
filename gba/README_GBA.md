# The Nazarene: Way of the Cross - Game Boy Advance (GBA) Version

An authentic bare-metal Game Boy Advance (ARM7TDMI) port of **The Nazarene: Way of the Cross**, running natively on real GBA hardware, flash carts (EverDrive GBA, EZ-Flash Omega), and GBA emulators (mGBA, VisualBoyAdvance, Delta, RetroArch).

---

## 🎮 GBA Controls

| GBA Button | Action |
|---|---|
| **D-Pad Left / Right** | Move Left / Right |
| **D-Pad Down** | Crouch |
| **D-Pad Up** | Look Up / High Jump Modifier |
| **Button A** | Jump |
| **Up + Button A** | **Dual-Plane Somersault High Jump** (Leap between street and upper balconies) |
| **Down + Button A** | **Drop-Through** (Descend through one-way ledges) |
| **Button B** | **Attack** (Ranged Iron Spike toss or Close-Range Olive Staff / Palm Melee Strike) |
| **Button L / R** | **Divine Wrath** (Screen freeze, celestial light, glowing Lion of Judah transformation, wipes regular enemies!) |
| **START** | Start Game / Resume |
| **SELECT** | **Add Credit** (Arcade Coin Insert) |

---

## 💾 Flashcart Compatibility (SuperCard, EZ-Flash, EverDrive)

For flashcarts like the **SuperCard SD / MiniSD / CF / Lite** that require standard 32Mbit (4MB) ROM alignment to load, convert, or patch without errors:
- Use **`Nazarene-padded.gba`** (or **`Nazarene-padded.zip`**).
- This build is padded to exactly 4MB (4,194,304 bytes) with `0xFF` filler and signed with valid Nintendo GBA header complement checksums.
- Compatible with official SuperCard software, SCFW/SuperFW, and all hardware flashers.

---

## 🕹 Features & Technical Architecture

- **Custom Bare-Metal GBA Engine:** Written in C and ARM assembly, targeting the 16.78 MHz ARM7TDMI.
- **Hardware Mode 4 (240x160):** Double-buffered 60 FPS rendering with zero screen-tearing.
- **4-Layer Parallax Background System:**
  1. *Judean Sunset Sky:* Color-gradient sunset, blazing low sun with God-rays, Mountains of Moab silhouettes, and soaring peaceful doves.
  2. *The Monumental Temple & Citadel of David (Migdal David):* Featuring 4 distinct architectural tiers across screens 3–4 (Bedrock Megaliths, Fortified Zion Gate Terrace, Palace of King David with Proto-Aeolic columns and Davidic standards, and the soaring Citadel Keep with a polished bronze cupola).
  3. *Jerusalem Villas & Aqueducts:* Flat-roofed stucco villas, terracotta tiles, and Italian cypress groves.
  4. *Roman Paved Road:* Cobblestones, curbs, and wooden platforms.
- **Ground Drop Shadows:** Soft ambient occlusion grounding all characters.
- **Authentic GBA Hardware PSG Sound:**
  - Square channels 1 and 2 for authentic Phrygian melody, basslines, and sound effects.
  - Channel 4 noise for spear swooshes, whip cracks, and thunderous Divine Wrath roars.
- **Mission 1 Jerusalem Objective:** Collect 4 Loaves and Fishes wicker baskets to open the heavy Iron Portcullis.
- **Stage 1 Boss:** Centurion Gladiator **Brutus** with barbed combat whip, headshot vulnerability, kneeling defeat, withering to ash, and heavenly Ascension.

---

## 🛠 Building the GBA ROM

Requires the `arm-none-eabi-gcc` toolchain. To compile:
```bat
cd gba
build_gba.bat
```
This compiles the assembly startup code (`crt0.s`), C source code, links using `gba.ld`, converts to raw binary with `arm-none-eabi-objcopy`, and patches the official Nintendo GBA header using `gbafix`.

