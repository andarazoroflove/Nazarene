#include "Mission2Graphics.h"
#include "Constants.h"
#include <vector>
#include <cmath>
#include <algorithm>
#include <cstdlib>

namespace M2Pixel {
    inline void Set(Color* p, int w, int h, int x, int y, Color c) {
        if (x >= 0 && x < w && y >= 0 && y < h && c.a > 0) {
            p[y * w + x] = c;
        }
    }

    inline void Blend(Color* p, int w, int h, int x, int y, Color c) {
        if (x >= 0 && x < w && y >= 0 && y < h && c.a > 0) {
            if (c.a == 255) {
                p[y * w + x] = c;
            } else {
                Color ex = p[y * w + x];
                float a = c.a / 255.0f;
                float invA = 1.0f - a;
                Color bl = {
                    static_cast<unsigned char>(c.r * a + ex.r * invA),
                    static_cast<unsigned char>(c.g * a + ex.g * invA),
                    static_cast<unsigned char>(c.b * a + ex.b * invA),
                    static_cast<unsigned char>(std::max(c.a, ex.a))
                };
                p[y * w + x] = bl;
            }
        }
    }

    inline void FillRect(Color* p, int w, int h, int x, int y, int rw, int rh, Color c) {
        for (int dy = 0; dy < rh; ++dy) {
            for (int dx = 0; dx < rw; ++dx) {
                Set(p, w, h, x + dx, y + dy, c);
            }
        }
    }

    inline void OutlineRect(Color* p, int w, int h, int x, int y, int rw, int rh, Color c) {
        for (int dx = 0; dx < rw; ++dx) {
            Set(p, w, h, x + dx, y, c);
            Set(p, w, h, x + dx, y + rh - 1, c);
        }
        for (int dy = 0; dy < rh; ++dy) {
            Set(p, w, h, x, y + dy, c);
            Set(p, w, h, x + rw - 1, y + dy, c);
        }
    }

    inline void HLine(Color* p, int w, int h, int x0, int x1, int y, Color c) {
        int startX = std::min(x0, x1);
        int endX = std::max(x0, x1);
        for (int x = startX; x <= endX; ++x) Set(p, w, h, x, y, c);
    }

    inline void VLine(Color* p, int w, int h, int x, int y0, int y1, Color c) {
        int startY = std::min(y0, y1);
        int endY = std::max(y0, y1);
        for (int y = startY; y <= endY; ++y) Set(p, w, h, x, y, c);
    }

    inline void FillCircle(Color* p, int w, int h, int cx, int cy, int r, Color c) {
        for (int dy = -r; dy <= r; ++dy) {
            for (int dx = -r; dx <= r; ++dx) {
                if (dx * dx + dy * dy <= r * r) {
                    Set(p, w, h, cx + dx, cy + dy, c);
                }
            }
        }
    }

    inline void OutlineCircle(Color* p, int w, int h, int cx, int cy, int r, Color c) {
        int x = r, y = 0;
        int err = 1 - r;
        while (x >= y) {
            Set(p, w, h, cx + x, cy + y, c);
            Set(p, w, h, cx + y, cy + x, c);
            Set(p, w, h, cx - y, cy + x, c);
            Set(p, w, h, cx - x, cy + y, c);
            Set(p, w, h, cx - x, cy - y, c);
            Set(p, w, h, cx - y, cy - x, c);
            Set(p, w, h, cx + y, cy - x, c);
            Set(p, w, h, cx + x, cy - y, c);
            y++;
            if (err < 0) {
                err += 2 * y + 1;
            } else {
                x--;
                err += 2 * (y - x + 1);
            }
        }
    }

    inline void Line(Color* p, int w, int h, int x0, int y0, int x1, int y1, Color c) {
        int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
        int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
        int err = dx + dy, e2;
        while (true) {
            Set(p, w, h, x0, y0, c);
            if (x0 == x1 && y0 == y1) break;
            e2 = 2 * err;
            if (e2 >= dy) { err += dy; x0 += sx; }
            if (e2 <= dx) { err += dx; y0 += sy; }
        }
    }
}

void Mission2Graphics::GenerateAll(
    Texture2D& bgKidronSky,
    Texture2D& bgKidronRavine,
    Texture2D& bgHedgeSky,
    Texture2D& bgHedgeLabyrinth,
    Texture2D& bgGethsemaneSky,
    Texture2D& bgGethsemaneGrove,
    Texture2D& bgBonusSunset,
    Texture2D& bgBonusTerraces
) {
    using namespace M2Pixel;

    // =========================================================================
    // 1. BONUS STAGE SPRITES (16-Bit MAME Arcade Tier)
    // =========================================================================
    // Villagers (24x30)
    constexpr int VW = 24, VH = 30;
    std::vector<Color> vPix(VW * VH, BLANK);

    auto PaintVillagerArcade = [&](Color* p, int type, int frame, bool satisfied) {
        for (int i = 0; i < VW * VH; ++i) p[i] = BLANK;

        // Rich Multi-Tone Shaded Palettes
        Color skinHi    = Color{ 255, 225, 195, 255 };
        Color skinMid   = Color{ 230, 175, 125, 255 };
        Color skinFlush = Color{ 210, 140,  95, 255 };
        Color skinDark  = Color{ 165, 105,  68, 255 };
        Color outline   = Color{  35,  22,  16, 255 };

        Color tunicHi, tunicMid, tunicDark, tunicFold;
        Color hairHi, hairMid, hairDark;

        if (type == 0) { // Jerusalem Elder (Warm sand/earth tones, gray beard)
            tunicHi   = Color{ 210, 185, 150, 255 };
            tunicMid  = Color{ 165, 135, 100, 255 };
            tunicDark = Color{ 115,  85,  55, 255 };
            tunicFold = Color{  85,  60,  35, 255 };
            hairHi    = Color{ 245, 245, 250, 255 };
            hairMid   = Color{ 200, 205, 215, 255 };
            hairDark  = Color{ 140, 145, 155, 255 };
        } else if (type == 1) { // Galilean Woman (Lapis blue & sapphire drapery, white headscarf)
            tunicHi   = Color{ 105, 170, 235, 255 };
            tunicMid  = Color{  55, 115, 185, 255 };
            tunicDark = Color{  30,  70, 135, 255 };
            tunicFold = Color{  18,  42,  90, 255 };
            hairHi    = Color{ 140,  90,  55, 255 };
            hairMid   = Color{  95,  55,  30, 255 };
            hairDark  = Color{  50,  28,  14, 255 };
        } else if (type == 2) { // Judean Youth (Olive green mantle, swift runner)
            tunicHi   = Color{ 165, 195,  95, 255 };
            tunicMid  = Color{ 120, 150,  60, 255 };
            tunicDark = Color{  75, 100,  35, 255 };
            tunicFold = Color{  45,  65,  20, 255 };
            hairHi    = Color{ 175, 120,  65, 255 };
            hairMid   = Color{ 125,  75,  35, 255 };
            hairDark  = Color{  70,  38,  16, 255 };
        } else { // Temple Beggar (Layered weathered wraps, bandages, rags)
            tunicHi   = Color{ 175, 145, 125, 255 };
            tunicMid  = Color{ 130, 100,  80, 255 };
            tunicDark = Color{  90,  65,  50, 255 };
            tunicFold = Color{  55,  38,  28, 255 };
            hairHi    = Color{ 110,  85,  70, 255 };
            hairMid   = Color{  75,  50,  38, 255 };
            hairDark  = Color{  45,  28,  20, 255 };
        }

        if (satisfied) {
            // SATISFIED / FEASTING POSE (Sitting in joyous prayer holding bread/fish)
            // Head & Peaceful Expression
            FillCircle(p, VW, VH, 12, 8, 4, skinMid);
            HLine(p, VW, VH, 11, 14, 6, skinHi);
            Set(p, VW, VH, 11, 8, Color{ 60, 35, 25, 255 }); // Eyes closed in prayer
            Set(p, VW, VH, 14, 8, Color{ 60, 35, 25, 255 });
            Set(p, VW, VH, 12, 10, skinFlush); // Joyful smile
            Set(p, VW, VH, 13, 10, skinFlush);

            // Hair / Headdress
            FillCircle(p, VW, VH, 12, 6, 4, hairMid);
            HLine(p, VW, VH, 10, 14, 4, hairHi);
            if (type == 0) { // Elder long beard
                FillRect(p, VW, VH, 11, 10, 3, 5, hairHi);
                Set(p, VW, VH, 12, 15, hairMid);
            } else if (type == 1) { // Woman draped scarf
                VLine(p, VW, VH, 8, 7, 18, Color{ 235, 240, 245, 255 });
                VLine(p, VW, VH, 16, 7, 18, Color{ 235, 240, 245, 255 });
            }

            // Draped Torso & Crossed Legs
            FillRect(p, VW, VH, 8, 12, 9, 8, tunicMid);
            OutlineRect(p, VW, VH, 8, 12, 9, 8, tunicDark);
            HLine(p, VW, VH, 9, 15, 13, tunicHi);
            FillRect(p, VW, VH, 5, 20, 15, 8, tunicDark);
            HLine(p, VW, VH, 6, 18, 20, tunicMid);
            HLine(p, VW, VH, 7, 17, 21, tunicHi);

            // Hands holding up the miracle loaf
            FillCircle(p, VW, VH, 12, 14, 3, Color{ 240, 185,  75, 255 });
            Set(p, VW, VH, 12, 13, Color{ 255, 235, 140, 255 }); // Crust highlight
            Set(p, VW, VH, 10, 15, skinHi); // Hands cupping
            Set(p, VW, VH, 14, 15, skinHi);
            return;
        }

        // APPROACHING / SPRINTING POSE
        int headY = 6;
        int headX = 12;

        // Head Anatomy & Expression (Desperate, hungry, focused)
        FillCircle(p, VW, VH, headX, headY, 4, skinMid);
        HLine(p, VW, VH, headX - 1, headX + 2, headY - 2, skinHi);
        // Hair / Head wrap
        FillCircle(p, VW, VH, headX + 1, headY - 2, 4, hairMid);
        HLine(p, VW, VH, headX - 1, headX + 3, headY - 3, hairHi);
        Set(p, VW, VH, headX - 3, headY - 1, hairDark);

        if (type == 0) { // Elder Beard
            FillRect(p, VW, VH, headX - 3, headY + 1, 4, 5, hairHi);
            VLine(p, VW, VH, headX - 2, headY + 2, headY + 5, hairMid);
        } else if (type == 1) { // Woman Veil
            HLine(p, VW, VH, headX - 3, headX + 3, headY - 4, Color{ 245, 248, 255, 255 });
            VLine(p, VW, VH, headX + 3, headY - 3, headY + 12, Color{ 215, 225, 240, 255 });
        }

        // Eager Face Profile
        Set(p, VW, VH, headX - 3, headY - 1, skinDark); // Brow
        Set(p, VW, VH, headX - 3, headY, Color{ 255, 255, 255, 255 }); // Sclera
        Set(p, VW, VH, headX - 2, headY, outline); // Focused pupil
        Set(p, VW, VH, headX - 4, headY + 1, skinHi); // Nose tip
        Set(p, VW, VH, headX - 3, headY + 2, skinFlush); // Open mouth begging
        Set(p, VW, VH, headX - 2, headY + 3, skinDark); // Chin

        // Outstretched Begging Arms (Leading forward with shaded hands)
        FillRect(p, VW, VH, headX - 9, headY + 5, 8, 3, skinMid);
        HLine(p, VW, VH, headX - 9, headX - 2, headY + 5, skinHi);
        HLine(p, VW, VH, headX - 8, headX - 2, headY + 7, skinDark);
        // Cupped begging hands
        Set(p, VW, VH, headX - 10, headY + 4, skinHi);
        Set(p, VW, VH, headX - 10, headY + 5, skinMid);
        Set(p, VW, VH, headX - 9, headY + 6, skinDark);

        // Body Tunic with Dynamic 3-Tone Drapery Folds
        for (int y = headY + 4; y <= headY + 16; ++y) {
            int flare = (y - headY - 4) / 4;
            int lx = headX - 4 - flare;
            int rx = headX + 4 + flare;
            HLine(p, VW, VH, lx, rx, y, tunicMid);
            Set(p, VW, VH, lx, y, tunicDark);
            Set(p, VW, VH, rx, y, tunicFold);
            // Fabric crease highlights
            if (y % 3 == 0) {
                HLine(p, VW, VH, lx + 1, rx - 1, y, tunicHi);
            }
        }
        // Waist Sash / Cord
        HLine(p, VW, VH, headX - 4, headX + 4, headY + 10, (type == 3) ? Color{ 50, 35, 20, 255 } : Color{ 215, 175, 45, 255 });

        // Athletic Running Legs & Shaded Sandals
        if (frame == 0) {
            // Forward stride
            Line(p, VW, VH, headX - 2, headY + 16, headX - 6, headY + 22, skinMid);
            Line(p, VW, VH, headX - 6, headY + 22, headX - 8, headY + 27, skinDark);
            // Foot
            HLine(p, VW, VH, headX - 10, headX - 6, headY + 27, Color{ 75, 45, 25, 255 });
            Set(p, VW, VH, headX - 10, headY + 26, skinHi);

            // Trailing leg
            Line(p, VW, VH, headX + 2, headY + 16, headX + 5, headY + 21, skinMid);
            Line(p, VW, VH, headX + 5, headY + 21, headX + 7, headY + 25, skinDark);
            HLine(p, VW, VH, headX + 6, headX + 9, headY + 25, Color{ 75, 45, 25, 255 });
        } else {
            // Backward push-off stride
            Line(p, VW, VH, headX - 2, headY + 16, headX - 4, headY + 21, skinMid);
            Line(p, VW, VH, headX - 4, headY + 21, headX - 5, headY + 26, skinDark);
            HLine(p, VW, VH, headX - 7, headX - 3, headY + 26, Color{ 75, 45, 25, 255 });

            Line(p, VW, VH, headX + 2, headY + 16, headX + 7, headY + 22, skinMid);
            Line(p, VW, VH, headX + 7, headY + 22, headX + 9, headY + 27, skinDark);
            HLine(p, VW, VH, headX + 8, headX + 11, headY + 27, Color{ 75, 45, 25, 255 });
        }
    };

    // Generate All 4 Villager Types (Walk 0, Walk 1, Satisfied)
    for (int t = 0; t < 4; ++t) {
        PaintVillagerArcade(vPix.data(), t, 0, false);
        SpriteId s0 = (t == 0) ? SPR_VILLAGER_ELDER_RUN_0 :
                      (t == 1) ? SPR_VILLAGER_WOMAN_RUN_0 :
                      (t == 2) ? SPR_VILLAGER_YOUTH_RUN_0 : SPR_VILLAGER_BEGGAR_RUN_0;
        GraphicsManager::SetSpriteTexture(s0, GraphicsManager::CreateTextureFromPixels(VW, VH, vPix.data()));

        PaintVillagerArcade(vPix.data(), t, 1, false);
        SpriteId s1 = (t == 0) ? SPR_VILLAGER_ELDER_RUN_1 :
                      (t == 1) ? SPR_VILLAGER_WOMAN_RUN_1 :
                      (t == 2) ? SPR_VILLAGER_YOUTH_RUN_1 : SPR_VILLAGER_BEGGAR_RUN_1;
        GraphicsManager::SetSpriteTexture(s1, GraphicsManager::CreateTextureFromPixels(VW, VH, vPix.data()));
    }
    PaintVillagerArcade(vPix.data(), 0, 0, true);
    GraphicsManager::SetSpriteTexture(SPR_VILLAGER_SATISFIED, GraphicsManager::CreateTextureFromPixels(VW, VH, vPix.data()));

    // 2. High-Detail Miracle Loaves of Bread (16x12)
    constexpr int BLW = 16, BLH = 12;
    std::vector<Color> breadPix(BLW * BLH, BLANK);
    FillCircle(breadPix.data(), BLW, BLH, 8, 6, 5, Color{ 215, 145,  55, 255 });
    // Golden baked crust & score cuts
    HLine(breadPix.data(), BLW, BLH, 4, 11, 4, Color{ 255, 215, 125, 255 });
    HLine(breadPix.data(), BLW, BLH, 3, 12, 5, Color{ 240, 180,  85, 255 });
    HLine(breadPix.data(), BLW, BLH, 4, 11, 8, Color{ 140,  80,  25, 255 });
    // Flour dusting & crust fissures
    Set(breadPix.data(), BLW, BLH, 6, 4, Color{ 255, 250, 220, 255 });
    Set(breadPix.data(), BLW, BLH, 9, 4, Color{ 255, 250, 220, 255 });
    Set(breadPix.data(), BLW, BLH, 5, 5, Color{ 160,  95,  30, 255 });
    Set(breadPix.data(), BLW, BLH, 8, 5, Color{ 160,  95,  30, 255 });
    Set(breadPix.data(), BLW, BLH, 11, 5, Color{ 160,  95,  30, 255 });
    GraphicsManager::SetSpriteTexture(SPR_BREAD_LOAF, GraphicsManager::CreateTextureFromPixels(BLW, BLH, breadPix.data()));

    // 3. High-Detail Galilee Fish (18x10)
    constexpr int FPW = 18, FPH = 10;
    std::vector<Color> fishPix(FPW * FPH, BLANK);
    // Body torpedo shape
    FillCircle(fishPix.data(), FPW, FPH, 7, 5, 4, Color{ 150, 195, 225, 255 });
    HLine(fishPix.data(), FPW, FPH, 4, 12, 5, Color{ 210, 238, 255, 255 });
    HLine(fishPix.data(), FPW, FPH, 5, 11, 4, Color{ 110, 165, 205, 255 });
    HLine(fishPix.data(), FPW, FPH, 5, 11, 6, Color{  75, 125, 165, 255 });
    HLine(fishPix.data(), FPW, FPH, 6, 10, 7, Color{  45,  85, 120, 255 });
    // Iridescent Scales
    Set(fishPix.data(), FPW, FPH, 6, 5, Color{ 255, 255, 255, 255 });
    Set(fishPix.data(), FPW, FPH, 9, 5, Color{ 255, 255, 255, 255 });
    // Dorsal & pectoral fins
    Line(fishPix.data(), FPW, FPH, 6, 2, 9, 3, Color{ 130, 185, 220, 255 });
    Line(fishPix.data(), FPW, FPH, 7, 6, 5, 8, Color{ 130, 185, 220, 255 });
    // Forked Caudal Fin
    Line(fishPix.data(), FPW, FPH, 13, 5, 17, 2, Color{ 110, 165, 205, 255 });
    Line(fishPix.data(), FPW, FPH, 13, 5, 17, 8, Color{ 110, 165, 205, 255 });
    Line(fishPix.data(), FPW, FPH, 14, 5, 17, 3, Color{ 210, 238, 255, 255 });
    Line(fishPix.data(), FPW, FPH, 14, 5, 17, 7, Color{ 210, 238, 255, 255 });
    // Gleaming Eye
    Set(fishPix.data(), FPW, FPH, 3, 4, Color{  25,  35,  50, 255 });
    Set(fishPix.data(), FPW, FPH, 3, 3, Color{ 255, 255, 255, 255 });
    GraphicsManager::SetSpriteTexture(SPR_FISH_PROJECTILE, GraphicsManager::CreateTextureFromPixels(FPW, FPH, fishPix.data()));

    // 4. Jesus at the Summit Pulpit (Basket Aim & Rapid Miracle Toss) (28x32)
    constexpr int JW = 28, JH = 32;
    std::vector<Color> jPix(JW * JH, BLANK);
    auto PaintJesusBasketArcade = [&](Color* p, bool throwPose) {
        for (int i = 0; i < JW * JH; ++i) p[i] = BLANK;

        Color skinHi    = COLOR_SKIN_HIGHLIGHT;
        Color skinMid   = COLOR_SKIN;
        Color skinDark  = COLOR_SKIN_DARK;
        Color hairLight = Color{ 160, 105,  55, 255 };
        Color hairMid   = Color{  95,  52,  26, 255 };
        Color hairDark  = Color{  45,  22,  12, 255 };

        Color robePure  = COLOR_ROBE_WHITE;
        Color robeLight = Color{ 246, 246, 240, 255 };
        Color robeMid   = Color{ 195, 202, 214, 255 };
        Color robeDark  = Color{ 135, 142, 158, 255 };
        Color sashHi    = COLOR_CRIMSON_BRIGHT;
        Color sashMid   = COLOR_ROMAN_CRIMSON;
        Color sashGold  = COLOR_ROMAN_GOLD;

        // Radiant Celestial Halo & Sunburst Rays
        FillCircle(p, JW, JH, 14, 4, 7, Color{ 255, 235, 130,  85 });
        FillCircle(p, JW, JH, 14, 4, 5, Color{ 255, 245, 180, 180 });
        FillCircle(p, JW, JH, 14, 4, 3, Color{ 255, 255, 230, 240 });

        // Head & Expressive Features
        FillCircle(p, JW, JH, 14, 7, 4, skinMid);
        HLine(p, JW, JH, 13, 16, 5, skinHi);
        FillCircle(p, JW, JH, 13, 5, 4, hairMid);
        HLine(p, JW, JH, 12, 16, 3, hairLight);
        Set(p, JW, JH, 11, 7, hairDark);
        // Face profile
        Set(p, JW, JH, 15, 6, Color{ 255, 255, 255, 255 }); // Eye
        Set(p, JW, JH, 16, 6, Color{  28,  18,  14, 255 }); // Pupil
        Set(p, JW, JH, 17, 7, skinHi);  // Nose
        Set(p, JW, JH, 16, 8, skinDark);// Beard line
        FillRect(p, JW, JH, 14, 9, 3, 2, hairMid); // Trimmed beard

        // White Linen Robes with Multi-Tone Folds
        for (int y = 11; y <= 27; ++y) {
            int flare = (y - 11) / 3;
            int lx = 10 - flare;
            int rx = 18 + flare;
            HLine(p, JW, JH, lx, rx, y, robeLight);
            Set(p, JW, JH, lx, y, robeDark);
            Set(p, JW, JH, rx, y, robeMid);
            if (y % 4 == 0) HLine(p, JW, JH, lx + 2, rx - 2, y, robePure);
        }

        // Royal Crimson Himation Sash across shoulder
        for (int y = 11; y <= 26; ++y) {
            int sx = 14 - (y - 11) / 3;
            Set(p, JW, JH, sx, y, sashHi);
            Set(p, JW, JH, sx + 1, y, sashMid);
            Set(p, JW, JH, sx + 2, y, sashGold);
        }

        // Wicker Miracle Basket (Woven pattern with loaves & fishes)
        FillCircle(p, JW, JH, 7, 18, 5, Color{ 175, 120,  50, 255 });
        OutlineCircle(p, JW, JH, 7, 18, 5, Color{ 115,  70,  25, 255 });
        // Woven crosshatch
        Set(p, JW, JH, 6, 17, Color{ 215, 165,  75, 255 });
        Set(p, JW, JH, 8, 17, Color{ 215, 165,  75, 255 });
        Set(p, JW, JH, 7, 19, Color{ 215, 165,  75, 255 });
        // Bread & fish sticking out of basket
        FillCircle(p, JW, JH, 6, 14, 2, Color{ 245, 195,  85, 255 });
        FillCircle(p, JW, JH, 9, 14, 2, Color{ 150, 205, 235, 255 });

        // Arms & Dynamic Tossing Stance
        if (throwPose) {
            // Forward Miracle Throw Whip!
            Line(p, JW, JH, 14, 13, 22, 9, skinMid);
            Line(p, JW, JH, 14, 14, 22, 10, skinHi);
            Set(p, JW, JH, 23, 8, skinHi); // Open hand
            Set(p, JW, JH, 24, 8, Color{ 255, 255, 200, 255 }); // Divine release spark!
            Set(p, JW, JH, 25, 9, Color{ 255, 235, 120, 200 });
            // Left arm steadying basket
            Line(p, JW, JH, 12, 14, 8, 18, skinDark);
        } else {
            // Aiming Stance (Right hand pointed, left hand supporting basket)
            Line(p, JW, JH, 14, 13, 21, 14, skinHi);
            Set(p, JW, JH, 22, 14, skinHi); // Pointing finger
            Set(p, JW, JH, 22, 13, Color{ 255, 235, 120, 180 }); // Targeting aura
            Line(p, JW, JH, 12, 14, 8, 18, skinDark);
        }

        // Sandaled feet
        FillRect(p, JW, JH, 10, 28, 4, 3, Color{ 110, 72, 40, 255 });
        HLine(p, JW, JH, 9, 14, 30, Color{ 60, 35, 20, 255 });
        FillRect(p, JW, JH, 16, 28, 4, 3, Color{ 110, 72, 40, 255 });
        HLine(p, JW, JH, 15, 20, 30, Color{ 60, 35, 20, 255 });
    };

    PaintJesusBasketArcade(jPix.data(), false);
    GraphicsManager::SetSpriteTexture(SPR_JESUS_BASKET_AIM, GraphicsManager::CreateTextureFromPixels(JW, JH, jPix.data()));
    PaintJesusBasketArcade(jPix.data(), true);
    GraphicsManager::SetSpriteTexture(SPR_JESUS_BASKET_THROW, GraphicsManager::CreateTextureFromPixels(JW, JH, jPix.data()));

    // =========================================================================
    // 2. MISSION 2 ENEMIES (16-Bit MAME Arcade Tier)
    // =========================================================================
    // Tormentor Guard (Roman Torturer with Torch & Spiked Cudgel) (26x32)
    constexpr int TW = 26, TH = 32;
    std::vector<Color> tPix(TW * TH, BLANK);

    auto PaintTormentorArcade = [&](Color* p, int pose) {
        for (int i = 0; i < TW * TH; ++i) p[i] = BLANK;

        Color skinHi    = Color{ 225, 165, 120, 255 };
        Color skinMid   = Color{ 185, 125,  85, 255 };
        Color skinDark  = Color{ 135,  80,  50, 255 };
        Color ironHi    = Color{ 215, 225, 235, 255 };
        Color ironMid   = Color{ 145, 155, 168, 255 };
        Color ironDark  = Color{  75,  85,  98, 255 };
        Color leatherHi = Color{ 145,  95,  55, 255 };
        Color leatherMid= Color{  95,  58,  30, 255 };
        Color leatherDark=Color{  55,  32,  16, 255 };
        Color flameCore = Color{ 255, 255, 210, 255 };
        Color flameMid  = Color{ 255, 150,  30, 255 };
        Color flameEdge = Color{ 210,  40,  25, 255 };

        if (pose == 3) { // Hurt Flash
            ironHi = Color{ 255, 220, 220, 255 };
            ironMid = Color{ 255, 150, 150, 255 };
            skinMid = Color{ 255, 180, 180, 255 };
        }

        // Heavy Spiked Iron Executioner Helm
        FillCircle(p, TW, TH, 13, 6, 4, ironMid);
        HLine(p, TW, TH, 10, 16, 4, ironHi);
        // Spiked horn on helm
        Line(p, TW, TH, 13, 3, 13, 0, ironHi);
        Set(p, TW, TH, 13, 0, Color{ 255, 255, 255, 255 });
        // Sinister Horizontal Slit with Glowing Crimson Visor
        HLine(p, TW, TH, 10, 15, 7, Color{ 25, 18, 22, 255 });
        Set(p, TW, TH, 12, 7, Color{ 255, 30, 45, 255 }); // Glowing Red Eye
        Set(p, TW, TH, 13, 7, Color{ 255, 200, 200, 255 });
        // Chiseled Iron Cheek guards
        VLine(p, TW, TH, 9, 6, 9, ironDark);
        VLine(p, TW, TH, 16, 6, 9, ironDark);

        // Muscular Bare Torso with Studded Gladiator Harness
        FillRect(p, TW, TH, 9, 10, 9, 10, skinMid);
        HLine(p, TW, TH, 10, 16, 11, skinHi); // Pectoral highlights
        Set(p, TW, TH, 13, 12, skinDark);    // Sternum
        Set(p, TW, TH, 11, 15, skinDark);    // Abdominal contours
        Set(p, TW, TH, 15, 15, skinDark);
        // Cross-Chest Leather Straps & Bronze Studs
        Line(p, TW, TH, 9, 10, 17, 19, leatherDark);
        Line(p, TW, TH, 17, 10, 9, 19, leatherDark);
        Set(p, TW, TH, 13, 14, COLOR_ROMAN_GOLD); // Center Medallion

        // Heavy Studded War Belt & Subligaculum
        FillRect(p, TW, TH, 9, 20, 9, 3, leatherMid);
        HLine(p, TW, TH, 9, 17, 20, ironHi);
        FillRect(p, TW, TH, 8, 23, 11, 3, Color{ 140, 20, 28, 255 }); // Red gladiator kilt

        // Left Hand: Blazing Pitch Torch
        VLine(p, TW, TH, 4, 10, 22, Color{ 110, 72, 38, 255 });
        FillRect(p, TW, TH, 3, 8, 3, 3, ironDark); // Iron torch basket
        // Multilayered Fire Plasma
        FillCircle(p, TW, TH, 4, 5, 4, flameEdge);
        FillCircle(p, TW, TH, 4, 5, 2, flameMid);
        Set(p, TW, TH, 4, 4, flameCore);
        Set(p, TW, TH, 3, 2, Color{ 255, 180, 50, 180 }); // Rising ember
        Set(p, TW, TH, 5, 1, Color{ 255, 120, 30, 120 });

        // Right Hand: Massive Spiked Cudgel
        if (pose == 2) {
            // Forward Torch & Mace Smash Lunge!
            Line(p, TW, TH, 15, 12, 23, 7, ironMid);
            FillCircle(p, TW, TH, 23, 6, 4, ironDark);
            Set(p, TW, TH, 23, 6, ironHi);
            // Radial forged spikes
            Set(p, TW, TH, 23, 1, ironHi);
            Set(p, TW, TH, 27, 6, ironHi);
            Set(p, TW, TH, 23, 11, ironHi);
            Set(p, TW, TH, 19, 6, ironHi);
            // Wind trail arc
            HLine(p, TW, TH, 17, 25, 4, Color{ 255, 255, 255, 160 });
        } else {
            // Idle / Walk weapon rest
            Line(p, TW, TH, 15, 13, 20, 20, ironMid);
            FillCircle(p, TW, TH, 20, 20, 3, ironDark);
            Set(p, TW, TH, 20, 20, ironHi);
            Set(p, TW, TH, 23, 20, ironHi);
            Set(p, TW, TH, 20, 23, ironHi);
        }

        // Heavy Armored Greaves & Boots
        int lLeg = (pose == 1) ? 8 : 10;
        int rLeg = (pose == 1) ? 16 : 14;
        FillRect(p, TW, TH, lLeg, 24, 3, 6, ironMid);
        HLine(p, TW, TH, lLeg, lLeg + 2, 25, ironHi);
        FillRect(p, TW, TH, rLeg, 24, 3, 6, ironMid);
        HLine(p, TW, TH, rLeg, rLeg + 2, 25, ironHi);
        FillRect(p, TW, TH, lLeg - 1, 30, 5, 2, leatherDark);
        FillRect(p, TW, TH, rLeg - 1, 30, 5, 2, leatherDark);
    };

    PaintTormentorArcade(tPix.data(), 0);
    GraphicsManager::SetSpriteTexture(SPR_TORMENTOR_WALK_0, GraphicsManager::CreateTextureFromPixels(TW, TH, tPix.data()));
    PaintTormentorArcade(tPix.data(), 1);
    GraphicsManager::SetSpriteTexture(SPR_TORMENTOR_WALK_1, GraphicsManager::CreateTextureFromPixels(TW, TH, tPix.data()));
    PaintTormentorArcade(tPix.data(), 2);
    GraphicsManager::SetSpriteTexture(SPR_TORMENTOR_ATTACK, GraphicsManager::CreateTextureFromPixels(TW, TH, tPix.data()));
    PaintTormentorArcade(tPix.data(), 3);
    GraphicsManager::SetSpriteTexture(SPR_TORMENTOR_HURT, GraphicsManager::CreateTextureFromPixels(TW, TH, tPix.data()));

    // Thorn Sicarii (Hedge Camouflaged Ninja Assassin) (26x30)
    constexpr int SW = 26, SH = 30;
    std::vector<Color> sPix(SW * SH, BLANK);

    auto PaintThornSicariiArcade = [&](Color* p, int pose) {
        for (int i = 0; i < SW * SH; ++i) p[i] = BLANK;

        Color camoHi    = Color{  68, 148,  82, 255 };
        Color camoMid   = COLOR_HEDGE_EMERALD;
        Color camoDark  = COLOR_HEDGE_DARK;
        Color vineBrown = Color{  75,  52,  32, 255 };
        Color bladeHi   = Color{ 255, 255, 255, 255 };
        Color bladeMid  = COLOR_SICARII_BLADE;
        Color bladeDark = Color{ 145, 165, 195, 255 };

        if (pose == 3) { // Hurt Flash
            camoMid = Color{ 255, 180, 180, 255 };
            camoDark = Color{ 180, 80, 80, 255 };
        }

        // Assassin Cowl & Mask
        FillCircle(p, SW, SH, 13, 6, 4, camoMid);
        HLine(p, SW, SH, 11, 15, 4, camoHi);
        // Amber Viper Glowing Eyes through slit
        HLine(p, SW, SH, 10, 15, 6, Color{ 15, 22, 18, 255 });
        Set(p, SW, SH, 11, 6, Color{ 255, 225,  40, 255 });
        Set(p, SW, SH, 14, 6, Color{ 255, 225,  40, 255 });

        // Foliage & Ivy Wrapped Stealth Tunic
        FillRect(p, SW, SH, 9, 10, 8, 10, camoMid);
        OutlineRect(p, SW, SH, 9, 10, 8, 10, camoDark);
        // Braided thorn vines
        Line(p, SW, SH, 9, 11, 16, 18, vineBrown);
        Line(p, SW, SH, 16, 11, 9, 18, vineBrown);
        Set(p, SW, SH, 12, 14, Color{ 215, 45, 55, 255 }); // Red poison thorn berry

        // Dual Curved Damascus Sicarii Blades
        if (pose == 1) { // Somersault Leap pose
            FillCircle(p, SW, SH, 13, 14, 7, camoMid);
            Line(p, SW, SH, 6, 8, 1, 3, bladeMid);
            Set(p, SW, SH, 1, 3, bladeHi);
            Line(p, SW, SH, 20, 8, 25, 3, bladeMid);
            Set(p, SW, SH, 25, 3, bladeHi);
        } else if (pose == 2) { // Cross-Slash Attack
            // Double curved crescent slashes
            Line(p, SW, SH, 15, 12, 25, 13, bladeMid);
            Line(p, SW, SH, 15, 13, 25, 14, bladeHi);
            Line(p, SW, SH, 11, 12, 1, 13, bladeMid);
            Line(p, SW, SH, 11, 13, 1, 14, bladeHi);
            // Slash trails
            HLine(p, SW, SH, 2, 24, 11, Color{ 255, 255, 255, 160 });
        } else { // Crouch / Ambush
            Line(p, SW, SH, 7, 14, 2, 18, bladeMid);
            Set(p, SW, SH, 2, 18, bladeHi);
            Line(p, SW, SH, 19, 14, 24, 18, bladeMid);
            Set(p, SW, SH, 24, 18, bladeHi);
        }

        // Tabi Ninja Boots
        FillRect(p, SW, SH, 10, 21, 3, 7, camoDark);
        FillRect(p, SW, SH, 15, 21, 3, 7, camoDark);
        Set(p, SW, SH, 9, 28, camoMid);
        Set(p, SW, SH, 16, 28, camoMid);
    };

    PaintThornSicariiArcade(sPix.data(), 0);
    GraphicsManager::SetSpriteTexture(SPR_THORN_SICARII_CROUCH, GraphicsManager::CreateTextureFromPixels(SW, SH, sPix.data()));
    PaintThornSicariiArcade(sPix.data(), 1);
    GraphicsManager::SetSpriteTexture(SPR_THORN_SICARII_LEAP, GraphicsManager::CreateTextureFromPixels(SW, SH, sPix.data()));
    PaintThornSicariiArcade(sPix.data(), 2);
    GraphicsManager::SetSpriteTexture(SPR_THORN_SICARII_SLASH, GraphicsManager::CreateTextureFromPixels(SW, SH, sPix.data()));
    PaintThornSicariiArcade(sPix.data(), 3);
    GraphicsManager::SetSpriteTexture(SPR_THORN_SICARII_HURT, GraphicsManager::CreateTextureFromPixels(SW, SH, sPix.data()));

    // Corrupted Priest (Dark Occultist Sorcerer) (24x32)
    constexpr int CW = 24, CH = 32;
    std::vector<Color> cPix(CW * CH, BLANK);

    auto PaintCorruptedPriestArcade = [&](Color* p, int pose) {
        for (int i = 0; i < CW * CH; ++i) p[i] = BLANK;

        Color robeHi    = Color{ 145,  45, 185, 255 };
        Color robeMid   = Color{  85,  22, 115, 255 };
        Color robeDark  = Color{  45,  10,  65, 255 };
        Color goldHi    = Color{ 255, 235, 120, 255 };
        Color goldMid   = COLOR_ROMAN_GOLD;
        Color skinMid   = Color{ 215, 185, 175, 255 };
        Color darkMagic = Color{ 220,  50, 255, 255 };

        if (pose == 2) { // Hurt Flash
            robeMid = Color{ 255, 160, 220, 255 };
            robeDark = Color{ 180, 50, 100, 255 };
        }

        // High Horned Ceremonial Miter Cap with Ruby
        FillRect(p, CW, CH, 9, 1, 6, 5, robeMid);
        OutlineRect(p, CW, CH, 9, 1, 6, 5, goldMid);
        Set(p, CW, CH, 12, 3, Color{ 255, 25, 45, 255 }); // Faceted Ruby
        Set(p, CW, CH, 12, 2, Color{ 255, 200, 200, 255 });

        // Gaunt Pale Face & Glowing Occult Eyes
        FillCircle(p, CW, CH, 12, 8, 4, skinMid);
        HLine(p, CW, CH, 10, 14, 8, Color{ 35, 18, 45, 255 });
        Set(p, CW, CH, 11, 8, Color{ 255, 30, 80, 255 }); // Glowing Red Iris
        Set(p, CW, CH, 13, 8, Color{ 255, 30, 80, 255 });
        // Thin gray goatee
        VLine(p, CW, CH, 12, 11, 13, Color{ 160, 160, 170, 255 });

        // Regal Dark Velvet Vestments with Gold Geometric Embroidery
        for (int y = 12; y <= 27; ++y) {
            int flare = (y - 12) / 3;
            int lx = 8 - flare;
            int rx = 16 + flare;
            HLine(p, CW, CH, lx, rx, y, robeMid);
            Set(p, CW, CH, lx, y, robeDark);
            Set(p, CW, CH, rx, y, robeHi);
        }
        // Gold Central Stole
        VLine(p, CW, CH, 12, 12, 27, goldMid);
        Set(p, CW, CH, 12, 16, goldHi);
        Set(p, CW, CH, 12, 22, goldHi);

        // Arms & Spellcasting
        if (pose == 1) {
            // Levitating Cast Pose! Both arms raised high holding dark magic
            Line(p, CW, CH, 8, 14, 2, 8, skinMid);
            Line(p, CW, CH, 16, 14, 22, 8, skinMid);
            // Glowing Purple Plasma Sphere in hands
            FillCircle(p, CW, CH, 22, 6, 3, darkMagic);
            FillCircle(p, CW, CH, 22, 6, 1, Color{ 255, 255, 255, 255 });
            FillCircle(p, CW, CH, 2, 6, 3, darkMagic);
            FillCircle(p, CW, CH, 2, 6, 1, Color{ 255, 255, 255, 255 });
        } else {
            // Idle Staff rest
            Line(p, CW, CH, 8, 14, 4, 19, skinMid);
            Line(p, CW, CH, 16, 14, 20, 19, skinMid);
            VLine(p, CW, CH, 20, 8, 28, Color{ 145, 115, 60, 255 }); // Antique staff
            FillCircle(p, CW, CH, 20, 7, 2, goldMid);
        }

        // Hemline
        HLine(p, CW, CH, 4, 20, 28, goldMid);
    };

    PaintCorruptedPriestArcade(cPix.data(), 0);
    GraphicsManager::SetSpriteTexture(SPR_CORRUPTED_PRIEST_STAND, GraphicsManager::CreateTextureFromPixels(CW, CH, cPix.data()));
    PaintCorruptedPriestArcade(cPix.data(), 1);
    GraphicsManager::SetSpriteTexture(SPR_CORRUPTED_PRIEST_CAST, GraphicsManager::CreateTextureFromPixels(CW, CH, cPix.data()));
    PaintCorruptedPriestArcade(cPix.data(), 2);
    GraphicsManager::SetSpriteTexture(SPR_CORRUPTED_PRIEST_HURT, GraphicsManager::CreateTextureFromPixels(CW, CH, cPix.data()));

    // Dark Orb (Pulsing Plasma) (14x14)
    constexpr int OW = 14, OH = 14;
    std::vector<Color> orbPix(OW * OH, BLANK);
    FillCircle(orbPix.data(), OW, OH, 7, 7, 6, Color{ 110,  15, 160, 180 });
    FillCircle(orbPix.data(), OW, OH, 7, 7, 4, Color{ 215,  55, 255, 255 });
    FillCircle(orbPix.data(), OW, OH, 7, 7, 2, Color{ 255, 235, 255, 255 });
    Set(orbPix.data(), OW, OH, 7, 7, WHITE);
    GraphicsManager::SetSpriteTexture(SPR_DARK_ORB, GraphicsManager::CreateTextureFromPixels(OW, OH, orbPix.data()));

    // Dark Spike (Obsidian Floor Spikes) (16x8)
    constexpr int DSW = 16, DSH = 8;
    std::vector<Color> dsPix(DSW * DSH, BLANK);
    for (int y = 0; y < DSH; ++y) {
        int w = (DSH - 1 - y) * 2;
        HLine(dsPix.data(), DSW, DSH, 8 - w / 2, 8 + w / 2, y, Color{ 35, 18, 48, 255 });
    }
    VLine(dsPix.data(), DSW, DSH, 8, 0, 7, Color{ 235, 60, 255, 255 });
    Set(dsPix.data(), DSW, DSH, 8, 0, WHITE);
    GraphicsManager::SetSpriteTexture(SPR_DARK_SPIKE, GraphicsManager::CreateTextureFromPixels(DSW, DSH, dsPix.data()));

    // =========================================================================
    // 3. CHRISTUS TENEBRIS (The Evil Genius Clone - Final Boss) (30x34)
    // =========================================================================
    constexpr int EW = 30, EH = 34;
    std::vector<Color> ePix(EW * EH, BLANK);

    auto PaintEvilCloneArcade = [&](Color* p, int pose) {
        for (int i = 0; i < EW * EH; ++i) p[i] = BLANK;

        Color skinHi    = Color{ 240, 205, 185, 255 };
        Color skinMid   = Color{ 205, 160, 140, 255 };
        Color skinDark  = Color{ 145,  95,  80, 255 };

        Color robePure  = Color{  45,  35,  55, 255 };
        Color robeLight = Color{  32,  24,  40, 255 };
        Color robeMid   = Color{  20,  15,  26, 255 };
        Color robeDark  = Color{  10,   8,  14, 255 };

        Color sashHi    = Color{ 245,  40,  60, 255 };
        Color sashMid   = Color{ 185,  18,  32, 255 };
        Color sashGold  = COLOR_ROMAN_GOLD;

        Color eyeRed    = Color{ 255,  30,  50, 255 };
        Color darkAura  = Color{ 120,  15, 150, 140 };

        if (pose == 7) { // Hurt Flash
            robeLight = Color{ 180,  40,  60, 255 };
            robeMid   = Color{ 120,  20,  35, 255 };
            skinMid   = Color{ 255, 180, 180, 255 };
        }

        // 1. Dark Pulsing Shadow Corona around Head
        FillCircle(p, EW, EH, 15, 6, 8, darkAura);
        FillCircle(p, EW, EH, 15, 6, 5, Color{ 40, 10, 60, 220 });

        // 2. Head & Raven Black Hair with Violet Highlights
        FillCircle(p, EW, EH, 15, 8, 4, skinMid);
        HLine(p, EW, EH, 14, 17, 6, skinHi);
        FillCircle(p, EW, EH, 14, 6, 4, Color{ 18, 14, 24, 255 });
        HLine(p, EW, EH, 13, 17, 4, Color{ 90, 45, 120, 255 }); // Violet glint
        Set(p, EW, EH, 12, 8, Color{ 18, 14, 24, 255 });

        // Sinister Face Profile with Glowing Red Eyes
        Set(p, EW, EH, 16, 7, Color{ 35, 12, 18, 255 }); // Dark brow
        Set(p, EW, EH, 17, 7, eyeRed); // Burning crimson eye!
        Set(p, EW, EH, 18, 7, Color{ 255, 220, 220, 255 }); // Specular gleam
        Set(p, EW, EH, 18, 8, skinHi); // Sharp nose bridge
        Set(p, EW, EH, 17, 9, skinDark);
        FillRect(p, EW, EH, 15, 10, 3, 2, Color{ 18, 14, 24, 255 }); // Dark trimmed beard

        // 3. Flowing Inverted Obsidian Robes with Multi-Tone Folds
        for (int y = 12; y <= 28; ++y) {
            int flare = (y - 12) / 3;
            int lx = 11 - flare;
            int rx = 19 + flare;
            HLine(p, EW, EH, lx, rx, y, robeMid);
            Set(p, EW, EH, lx, y, robeLight);
            Set(p, EW, EH, rx, y, robeDark);
            if (y % 4 == 0) HLine(p, EW, EH, lx + 2, rx - 2, y, robePure);
        }

        // 4. Corrupted Crimson Himation Sash with Gold Trim
        for (int y = 12; y <= 27; ++y) {
            int sx = 15 - (y - 12) / 3;
            Set(p, EW, EH, sx, y, sashHi);
            Set(p, EW, EH, sx + 1, y, sashMid);
            Set(p, EW, EH, sx + 2, y, sashGold);
        }

        // 5. Poses & Special Abilities
        if (pose == 3) {
            // Somersault Leap
            FillCircle(p, EW, EH, 15, 17, 10, robeMid);
            FillCircle(p, EW, EH, 15, 17, 7, sashMid);
            FillCircle(p, EW, EH, 15, 17, 3, darkAura);
        } else if (pose == 4) {
            // Throw Stand (Dark Nails)
            Line(p, EW, EH, 15, 14, 25, 10, skinMid);
            Line(p, EW, EH, 15, 15, 25, 11, skinHi);
            Set(p, EW, EH, 26, 9, Color{ 255, 40, 60, 255 }); // Muzzle flash
            Set(p, EW, EH, 27, 9, WHITE);
        } else if (pose == 5) {
            // Melee Strike (Fierce Dark Palm Thrust)
            Line(p, EW, EH, 15, 14, 26, 14, skinHi);
            FillCircle(p, EW, EH, 26, 14, 4, Color{ 180, 25, 230, 220 });
            FillCircle(p, EW, EH, 26, 14, 2, WHITE); // Kinetic shockwave
        } else if (pose == 6) {
            // "Wrath of the Deceiver" Channeling (Arms raised, lightning arcs)
            Line(p, EW, EH, 13, 14, 6, 6, skinHi);
            Line(p, EW, EH, 17, 14, 24, 6, skinHi);
            // Crackling crimson lightning ball above head
            FillCircle(p, EW, EH, 15, 2, 5, Color{ 230, 30, 70, 240 });
            FillCircle(p, EW, EH, 15, 2, 2, Color{ 255, 240, 255, 255 });
            Line(p, EW, EH, 4, 3, 8, 8, Color{ 255, 100, 140, 255 });
            Line(p, EW, EH, 26, 3, 22, 8, Color{ 255, 100, 140, 255 });
        } else if (pose == 8) {
            // Defeated Kneel (Collapsing to knees, darkness dissipating)
            FillCircle(p, EW, EH, 15, 15, 4, skinMid);
            FillRect(p, EW, EH, 11, 19, 9, 10, robeMid);
            FillRect(p, EW, EH, 8, 27, 14, 4, sashMid);
        } else {
            // Idle / Walk
            int step = (pose == 2) ? 1 : 0;
            Line(p, EW, EH, 12, 28, step == 0 ? 9 : 15, 33, Color{ 65, 40, 25, 255 });
            Line(p, EW, EH, 18, 28, step == 0 ? 21 : 15, 33, Color{ 65, 40, 25, 255 });
        }
    };

    PaintEvilCloneArcade(ePix.data(), 0);
    GraphicsManager::SetSpriteTexture(SPR_CLONE_IDLE, GraphicsManager::CreateTextureFromPixels(EW, EH, ePix.data()));
    PaintEvilCloneArcade(ePix.data(), 1);
    GraphicsManager::SetSpriteTexture(SPR_CLONE_WALK_0, GraphicsManager::CreateTextureFromPixels(EW, EH, ePix.data()));
    PaintEvilCloneArcade(ePix.data(), 2);
    GraphicsManager::SetSpriteTexture(SPR_CLONE_WALK_1, GraphicsManager::CreateTextureFromPixels(EW, EH, ePix.data()));
    PaintEvilCloneArcade(ePix.data(), 3);
    GraphicsManager::SetSpriteTexture(SPR_CLONE_JUMP, GraphicsManager::CreateTextureFromPixels(EW, EH, ePix.data()));
    PaintEvilCloneArcade(ePix.data(), 4);
    GraphicsManager::SetSpriteTexture(SPR_CLONE_THROW_STAND, GraphicsManager::CreateTextureFromPixels(EW, EH, ePix.data()));
    PaintEvilCloneArcade(ePix.data(), 4);
    GraphicsManager::SetSpriteTexture(SPR_CLONE_THROW_CROUCH, GraphicsManager::CreateTextureFromPixels(EW, EH, ePix.data()));
    PaintEvilCloneArcade(ePix.data(), 5);
    GraphicsManager::SetSpriteTexture(SPR_CLONE_MELEE_STRIKE, GraphicsManager::CreateTextureFromPixels(EW, EH, ePix.data()));
    PaintEvilCloneArcade(ePix.data(), 6);
    GraphicsManager::SetSpriteTexture(SPR_CLONE_CHANNEL, GraphicsManager::CreateTextureFromPixels(EW, EH, ePix.data()));
    PaintEvilCloneArcade(ePix.data(), 7);
    GraphicsManager::SetSpriteTexture(SPR_CLONE_HURT, GraphicsManager::CreateTextureFromPixels(EW, EH, ePix.data()));
    PaintEvilCloneArcade(ePix.data(), 8);
    GraphicsManager::SetSpriteTexture(SPR_CLONE_KNEEL, GraphicsManager::CreateTextureFromPixels(EW, EH, ePix.data()));

    // =========================================================================
    // 4. MISSION 2 PICKUPS (18x18)
    // =========================================================================
    constexpr int PW = 18, PH = 18;
    std::vector<Color> pickPix(PW * PH, BLANK);

    // Silver Pitcher (Stage 4)
    for (int i = 0; i < PW * PH; ++i) pickPix[i] = BLANK;
    FillCircle(pickPix.data(), PW, PH, 9, 11, 5, Color{ 205, 220, 240, 255 });
    FillRect(pickPix.data(), PW, PH, 7, 4, 5, 5, Color{ 175, 190, 210, 255 });
    HLine(pickPix.data(), PW, PH, 6, 12, 3, Color{ 245, 250, 255, 255 });
    Line(pickPix.data(), PW, PH, 6, 5, 3, 9, Color{ 160, 175, 195, 255 });
    Line(pickPix.data(), PW, PH, 3, 9, 6, 13, Color{ 160, 175, 195, 255 });
    Set(pickPix.data(), PW, PH, 8, 10, WHITE); // Specular reflection
    GraphicsManager::SetSpriteTexture(SPR_SILVER_PITCHER, GraphicsManager::CreateTextureFromPixels(PW, PH, pickPix.data()));

    // Olive Wreath (Stage 5)
    for (int i = 0; i < PW * PH; ++i) pickPix[i] = BLANK;
    OutlineCircle(pickPix.data(), PW, PH, 9, 9, 6, COLOR_HEDGE_EMERALD);
    OutlineCircle(pickPix.data(), PW, PH, 9, 9, 5, COLOR_HEDGE_TRIM);
    Set(pickPix.data(), PW, PH, 9, 3, COLOR_GOLD_BRIGHT);
    Set(pickPix.data(), PW, PH, 15, 9, COLOR_GOLD_BRIGHT);
    Set(pickPix.data(), PW, PH, 9, 15, COLOR_GOLD_BRIGHT);
    Set(pickPix.data(), PW, PH, 3, 9, COLOR_GOLD_BRIGHT);
    Set(pickPix.data(), PW, PH, 6, 6, Color{ 215, 35, 45, 255 }); // Red berries
    Set(pickPix.data(), PW, PH, 12, 12, Color{ 215, 35, 45, 255 });
    GraphicsManager::SetSpriteTexture(SPR_OLIVE_WREATH, GraphicsManager::CreateTextureFromPixels(PW, PH, pickPix.data()));

    // Sacred Chalice (Stage 6)
    for (int i = 0; i < PW * PH; ++i) pickPix[i] = BLANK;
    FillCircle(pickPix.data(), PW, PH, 9, 6, 5, COLOR_GOLD_BRIGHT);
    FillRect(pickPix.data(), PW, PH, 5, 2, 9, 4, COLOR_ROMAN_GOLD);
    VLine(pickPix.data(), PW, PH, 9, 10, 14, COLOR_ROMAN_GOLD);
    FillRect(pickPix.data(), PW, PH, 6, 14, 7, 3, COLOR_GOLD_BRIGHT);
    Set(pickPix.data(), PW, PH, 9, 5, Color{ 255, 35, 45, 255 }); // Faceted Ruby
    Set(pickPix.data(), PW, PH, 9, 4, Color{ 255, 220, 220, 255 });
    GraphicsManager::SetSpriteTexture(SPR_SACRED_CHALICE, GraphicsManager::CreateTextureFromPixels(PW, PH, pickPix.data()));

    // =========================================================================
    // 5. MISSION 2 TILES (16x16)
    // =========================================================================
    constexpr int T_SZ = 16;
    std::vector<Color> tPixTile(T_SZ * T_SZ);

    // Kidron Rock Surface
    for (int y = 0; y < T_SZ; ++y) {
        for (int x = 0; x < T_SZ; ++x) {
            Color c = (y < 4) ? COLOR_KIDRON_ROCK : COLOR_KIDRON_DARK;
            if (y == 0) c = Color{ 115, 105, 125, 255 };
            if ((x + y) % 5 == 0) c = Color{ 48, 44, 55, 255 };
            tPixTile[y * T_SZ + x] = c;
        }
    }
    GraphicsManager::SetTileTexture(TILE_KIDRON_ROCK_SURFACE, GraphicsManager::CreateTextureFromPixels(T_SZ, T_SZ, tPixTile.data()));

    // Kidron Rock Sub
    for (int y = 0; y < T_SZ; ++y) {
        for (int x = 0; x < T_SZ; ++x) {
            tPixTile[y * T_SZ + x] = ((x ^ y) % 4 == 0) ? Color{ 28, 24, 32, 255 } : COLOR_KIDRON_DARK;
        }
    }
    GraphicsManager::SetTileTexture(TILE_KIDRON_ROCK_SUB, GraphicsManager::CreateTextureFromPixels(T_SZ, T_SZ, tPixTile.data()));

    // Kidron Wood Bridge
    for (int i = 0; i < T_SZ * T_SZ; ++i) tPixTile[i] = BLANK;
    FillRect(tPixTile.data(), T_SZ, T_SZ, 0, 0, T_SZ, 6, Color{ 120, 85, 50, 255 });
    HLine(tPixTile.data(), T_SZ, T_SZ, 0, T_SZ - 1, 0, Color{ 160, 115, 70, 255 });
    HLine(tPixTile.data(), T_SZ, T_SZ, 0, T_SZ - 1, 5, Color{ 70, 48, 25, 255 });
    Set(tPixTile.data(), T_SZ, T_SZ, 3, 2, Color{ 40, 25, 15, 255 });
    Set(tPixTile.data(), T_SZ, T_SZ, 12, 2, Color{ 40, 25, 15, 255 });
    GraphicsManager::SetTileTexture(TILE_KIDRON_BRIDGE_WOOD, GraphicsManager::CreateTextureFromPixels(T_SZ, T_SZ, tPixTile.data()));

    // Kidron Ledge
    for (int i = 0; i < T_SZ * T_SZ; ++i) tPixTile[i] = BLANK;
    FillRect(tPixTile.data(), T_SZ, T_SZ, 0, 0, T_SZ, 6, COLOR_KIDRON_ROCK);
    HLine(tPixTile.data(), T_SZ, T_SZ, 0, T_SZ - 1, 0, Color{ 125, 115, 140, 255 });
    HLine(tPixTile.data(), T_SZ, T_SZ, 0, T_SZ - 1, 5, COLOR_KIDRON_DARK);
    GraphicsManager::SetTileTexture(TILE_KIDRON_LEDGE, GraphicsManager::CreateTextureFromPixels(T_SZ, T_SZ, tPixTile.data()));

    // Kidron Column Head
    for (int i = 0; i < T_SZ * T_SZ; ++i) tPixTile[i] = BLANK;
    FillRect(tPixTile.data(), T_SZ, T_SZ, 1, 0, 14, 4, Color{ 135, 125, 145, 255 });
    FillRect(tPixTile.data(), T_SZ, T_SZ, 3, 4, 10, 12, COLOR_KIDRON_ROCK);
    GraphicsManager::SetTileTexture(TILE_KIDRON_COLUMN_HEAD, GraphicsManager::CreateTextureFromPixels(T_SZ, T_SZ, tPixTile.data()));

    // Kidron Column Mid
    for (int i = 0; i < T_SZ * T_SZ; ++i) tPixTile[i] = BLANK;
    FillRect(tPixTile.data(), T_SZ, T_SZ, 3, 0, 10, T_SZ, COLOR_KIDRON_ROCK);
    VLine(tPixTile.data(), T_SZ, T_SZ, 3, 0, T_SZ - 1, Color{ 125, 115, 140, 255 });
    VLine(tPixTile.data(), T_SZ, T_SZ, 12, 0, T_SZ - 1, COLOR_KIDRON_DARK);
    GraphicsManager::SetTileTexture(TILE_KIDRON_COLUMN_MID, GraphicsManager::CreateTextureFromPixels(T_SZ, T_SZ, tPixTile.data()));

    // Kidron Column Base
    for (int i = 0; i < T_SZ * T_SZ; ++i) tPixTile[i] = BLANK;
    FillRect(tPixTile.data(), T_SZ, T_SZ, 3, 0, 10, 11, COLOR_KIDRON_ROCK);
    FillRect(tPixTile.data(), T_SZ, T_SZ, 1, 11, 14, 5, Color{ 90, 80, 100, 255 });
    GraphicsManager::SetTileTexture(TILE_KIDRON_COLUMN_BASE, GraphicsManager::CreateTextureFromPixels(T_SZ, T_SZ, tPixTile.data()));

    // Kidron Torch
    for (int i = 0; i < T_SZ * T_SZ; ++i) tPixTile[i] = BLANK;
    VLine(tPixTile.data(), T_SZ, T_SZ, 8, 5, 15, Color{ 85, 75, 70, 255 });
    FillCircle(tPixTile.data(), T_SZ, T_SZ, 8, 4, 3, COLOR_TORCH_FIRE);
    Set(tPixTile.data(), T_SZ, T_SZ, 8, 3, COLOR_TORCH_EMBER);
    GraphicsManager::SetTileTexture(TILE_KIDRON_TORCH, GraphicsManager::CreateTextureFromPixels(T_SZ, T_SZ, tPixTile.data()));

    // Hedge Maze Top
    for (int y = 0; y < T_SZ; ++y) {
        for (int x = 0; x < T_SZ; ++x) {
            Color c = ((x + y) % 3 == 0) ? COLOR_HEDGE_TRIM : COLOR_HEDGE_EMERALD;
            if (y == 0) c = Color{ 85, 185, 100, 255 };
            tPixTile[y * T_SZ + x] = c;
        }
    }
    GraphicsManager::SetTileTexture(TILE_HEDGE_TOP, GraphicsManager::CreateTextureFromPixels(T_SZ, T_SZ, tPixTile.data()));

    // Hedge Maze Mid
    for (int y = 0; y < T_SZ; ++y) {
        for (int x = 0; x < T_SZ; ++x) {
            Color c = ((x * 3 + y * 7) % 5 == 0) ? COLOR_HEDGE_DARK : COLOR_HEDGE_EMERALD;
            tPixTile[y * T_SZ + x] = c;
        }
    }
    GraphicsManager::SetTileTexture(TILE_HEDGE_MID, GraphicsManager::CreateTextureFromPixels(T_SZ, T_SZ, tPixTile.data()));

    // Hedge Ledges
    for (int i = 0; i < T_SZ * T_SZ; ++i) tPixTile[i] = BLANK;
    FillRect(tPixTile.data(), T_SZ, T_SZ, 0, 0, T_SZ, 6, COLOR_HEDGE_EMERALD);
    HLine(tPixTile.data(), T_SZ, T_SZ, 0, T_SZ - 1, 0, COLOR_HEDGE_TRIM);
    HLine(tPixTile.data(), T_SZ, T_SZ, 0, T_SZ - 1, 5, COLOR_HEDGE_DARK);
    GraphicsManager::SetTileTexture(TILE_HEDGE_LEDGE_1, GraphicsManager::CreateTextureFromPixels(T_SZ, T_SZ, tPixTile.data()));

    for (int i = 0; i < T_SZ * T_SZ; ++i) tPixTile[i] = BLANK;
    FillRect(tPixTile.data(), T_SZ, T_SZ, 0, 0, T_SZ, 6, COLOR_MARBLE_STATUE);
    HLine(tPixTile.data(), T_SZ, T_SZ, 0, T_SZ - 1, 0, WHITE);
    HLine(tPixTile.data(), T_SZ, T_SZ, 0, T_SZ - 1, 5, Color{ 140, 145, 155, 255 });
    GraphicsManager::SetTileTexture(TILE_HEDGE_LEDGE_2, GraphicsManager::CreateTextureFromPixels(T_SZ, T_SZ, tPixTile.data()));

    // Hedge Arch, Urn, Fountain
    for (int i = 0; i < T_SZ * T_SZ; ++i) tPixTile[i] = BLANK;
    FillCircle(tPixTile.data(), T_SZ, T_SZ, 8, 8, 7, COLOR_MARBLE_STATUE);
    FillCircle(tPixTile.data(), T_SZ, T_SZ, 8, 8, 4, BLANK);
    GraphicsManager::SetTileTexture(TILE_HEDGE_ARCH, GraphicsManager::CreateTextureFromPixels(T_SZ, T_SZ, tPixTile.data()));

    for (int i = 0; i < T_SZ * T_SZ; ++i) tPixTile[i] = BLANK;
    FillCircle(tPixTile.data(), T_SZ, T_SZ, 8, 7, 5, COLOR_MARBLE_STATUE);
    FillRect(tPixTile.data(), T_SZ, T_SZ, 5, 12, 7, 4, Color{ 170, 175, 185, 255 });
    GraphicsManager::SetTileTexture(TILE_HEDGE_URN, GraphicsManager::CreateTextureFromPixels(T_SZ, T_SZ, tPixTile.data()));

    for (int i = 0; i < T_SZ * T_SZ; ++i) tPixTile[i] = BLANK;
    FillRect(tPixTile.data(), T_SZ, T_SZ, 2, 8, 12, 7, COLOR_MARBLE_STATUE);
    FillRect(tPixTile.data(), T_SZ, T_SZ, 4, 3, 8, 5, Color{ 140, 205, 245, 255 });
    GraphicsManager::SetTileTexture(TILE_HEDGE_FOUNTAIN, GraphicsManager::CreateTextureFromPixels(T_SZ, T_SZ, tPixTile.data()));

    // Gethsemane Tiles
    for (int y = 0; y < T_SZ; ++y) {
        for (int x = 0; x < T_SZ; ++x) {
            Color c = (y < 3) ? Color{ 55, 75, 45, 255 } : Color{ 45, 36, 30, 255 };
            if (y == 0) c = Color{ 75, 105, 60, 255 };
            tPixTile[y * T_SZ + x] = c;
        }
    }
    GraphicsManager::SetTileTexture(TILE_GETHSEMANE_EARTH_SURFACE, GraphicsManager::CreateTextureFromPixels(T_SZ, T_SZ, tPixTile.data()));

    for (int y = 0; y < T_SZ; ++y) {
        for (int x = 0; x < T_SZ; ++x) {
            tPixTile[y * T_SZ + x] = Color{ 36, 28, 22, 255 };
        }
    }
    GraphicsManager::SetTileTexture(TILE_GETHSEMANE_EARTH_SUB, GraphicsManager::CreateTextureFromPixels(T_SZ, T_SZ, tPixTile.data()));

    for (int y = 0; y < T_SZ; ++y) {
        for (int x = 0; x < T_SZ; ++x) {
            Color c = COLOR_OLIVE_BARK;
            if (x % 4 == 0) c = Color{ 58, 40, 26, 255 };
            tPixTile[y * T_SZ + x] = c;
        }
    }
    GraphicsManager::SetTileTexture(TILE_GETHSEMANE_OLIVE_TRUNK, GraphicsManager::CreateTextureFromPixels(T_SZ, T_SZ, tPixTile.data()));

    for (int i = 0; i < T_SZ * T_SZ; ++i) tPixTile[i] = BLANK;
    FillRect(tPixTile.data(), T_SZ, T_SZ, 0, 4, T_SZ, 8, COLOR_OLIVE_BARK);
    HLine(tPixTile.data(), T_SZ, T_SZ, 0, T_SZ - 1, 4, Color{ 110, 85, 60, 255 });
    GraphicsManager::SetTileTexture(TILE_GETHSEMANE_OLIVE_BRANCH, GraphicsManager::CreateTextureFromPixels(T_SZ, T_SZ, tPixTile.data()));

    for (int y = 0; y < T_SZ; ++y) {
        for (int x = 0; x < T_SZ; ++x) {
            Color c = ((x + y) % 2 == 0) ? COLOR_OLIVE_LEAF : COLOR_OLIVE_SILVER;
            tPixTile[y * T_SZ + x] = c;
        }
    }
    GraphicsManager::SetTileTexture(TILE_GETHSEMANE_OLIVE_LEAVES, GraphicsManager::CreateTextureFromPixels(T_SZ, T_SZ, tPixTile.data()));

    for (int i = 0; i < T_SZ * T_SZ; ++i) tPixTile[i] = BLANK;
    FillRect(tPixTile.data(), T_SZ, T_SZ, 0, 0, T_SZ, 6, Color{ 65, 52, 42, 255 });
    HLine(tPixTile.data(), T_SZ, T_SZ, 0, T_SZ - 1, 0, Color{ 85, 115, 75, 255 });
    GraphicsManager::SetTileTexture(TILE_GETHSEMANE_LEDGE, GraphicsManager::CreateTextureFromPixels(T_SZ, T_SZ, tPixTile.data()));

    // Sacred Altar Stone of Agony
    for (int i = 0; i < T_SZ * T_SZ; ++i) tPixTile[i] = BLANK;
    FillRect(tPixTile.data(), T_SZ, T_SZ, 1, 4, 14, 11, Color{ 85, 80, 95, 255 });
    OutlineRect(tPixTile.data(), T_SZ, T_SZ, 1, 4, 14, 11, COLOR_GOLD_BRIGHT);
    HLine(tPixTile.data(), T_SZ, T_SZ, 3, 12, 8, Color{ 255, 220, 100, 255 });
    GraphicsManager::SetTileTexture(TILE_GETHSEMANE_ALTAR_STONE, GraphicsManager::CreateTextureFromPixels(T_SZ, T_SZ, tPixTile.data()));

    // Gethsemane Lantern
    for (int i = 0; i < T_SZ * T_SZ; ++i) tPixTile[i] = BLANK;
    VLine(tPixTile.data(), T_SZ, T_SZ, 8, 0, 6, Color{ 140, 100, 40, 255 });
    FillCircle(tPixTile.data(), T_SZ, T_SZ, 8, 10, 4, Color{ 255, 195, 65, 255 });
    Set(tPixTile.data(), T_SZ, T_SZ, 8, 10, WHITE);
    GraphicsManager::SetTileTexture(TILE_GETHSEMANE_LANTERN, GraphicsManager::CreateTextureFromPixels(T_SZ, T_SZ, tPixTile.data()));

    // =========================================================================
    // 6. PARALLAX BACKGROUND TEXTURES
    // =========================================================================
    // Bonus Sunset Sky (320x160)
    constexpr int BW = 320, BH = 160;
    std::vector<Color> bSky(BW * BH);
    for (int y = 0; y < BH; ++y) {
        float ty = (float)y / (float)BH;
        Color skyCol;
        if (ty < 0.4f) {
            float t = ty / 0.4f;
            skyCol = Color{ (unsigned char)(165 + t * 70), (unsigned char)(65 + t * 65), (unsigned char)(40 + t * 20), 255 };
        } else if (ty < 0.75f) {
            float t = (ty - 0.4f) / 0.35f;
            skyCol = Color{ (unsigned char)(235 + t * 20), (unsigned char)(130 + t * 60), (unsigned char)(60 + t * 40), 255 };
        } else {
            skyCol = Color{ 180, 135, 95, 255 };
        }
        for (int x = 0; x < BW; ++x) bSky[y * BW + x] = skyCol;
    }
    // Radiant Sun over Sea of Galilee
    FillCircle(bSky.data(), BW, BH, 160, 95, 22, Color{ 255, 245, 180, 255 });
    FillCircle(bSky.data(), BW, BH, 160, 95, 16, WHITE);
    bgBonusSunset = GraphicsManager::CreateTextureFromPixels(BW, BH, bSky.data());

    // Bonus Terraces Backdrop (320x224)
    constexpr int BTW = 320, BTH = 224;
    std::vector<Color> bTerr(BTW * BTH, BLANK);
    // Mountain slope silhouettes & distant Galilean olive trees
    for (int x = 0; x < BTW; ++x) {
        int hill1 = static_cast<int>(110 + sinf(x * 0.02f) * 16.0f);
        for (int y = hill1; y < BTH; ++y) {
            bTerr[y * BTW + x] = Color{ 110, 85, 45, 140 };
        }
        int hill2 = static_cast<int>(145 + cosf(x * 0.035f) * 12.0f);
        for (int y = hill2; y < BTH; ++y) {
            bTerr[y * BTW + x] = Color{ 75, 95, 40, 210 };
        }
        // Cypress & Olive silhouettes on hills
        if (x % 48 == 12) {
            for (int cy = hill2 - 16; cy < hill2; ++cy) {
                int w = (cy - (hill2 - 16)) / 3;
                HLine(bTerr.data(), BTW, BTH, x - w, x + w, cy, Color{ 45, 65, 25, 240 });
            }
        }
    }
    bgBonusTerraces = GraphicsManager::CreateTextureFromPixels(BTW, BTH, bTerr.data());

    // Kidron Gorge Sky (320x160) - Starry nocturnal Judean wilderness
    std::vector<Color> kSky(BW * BH);
    for (int y = 0; y < BH; ++y) {
        float ty = (float)y / (float)BH;
        Color skyCol = {
            static_cast<unsigned char>(10 + ty * 20),
            static_cast<unsigned char>(8  + ty * 18),
            static_cast<unsigned char>(24 + ty * 35),
            255
        };
        for (int x = 0; x < BW; ++x) kSky[y * BW + x] = skyCol;
    }
    // Crescent Moon & Twinkling Stars
    FillCircle(kSky.data(), BW, BH, 250, 40, 14, Color{ 245, 245, 220, 255 });
    FillCircle(kSky.data(), BW, BH, 246, 38, 12, Color{ 15, 12, 32, 255 }); // Crescent cutout
    for (int s = 0; s < 45; ++s) {
        int sx = (s * 47) % BW;
        int sy = (s * 31) % (BH - 50);
        kSky[sy * BW + sx] = ((s % 3 == 0) ? Color{ 255, 255, 255, 255 } : Color{ 180, 195, 235, 180 });
    }
    bgKidronSky = GraphicsManager::CreateTextureFromPixels(BW, BH, kSky.data());

    // Kidron Ravine Midground (640x160)
    constexpr int KRW = 640, KRH = 160;
    std::vector<Color> kRav(KRW * KRH, BLANK);
    for (int x = 0; x < KRW; ++x) {
        float form = sinf(x * 0.025f) * 22.0f + cosf(x * 0.06f) * 10.0f;
        int top = static_cast<int>(55 + form);
        for (int y = top; y < KRH; ++y) {
            Color c = ((x + y) % 6 == 0) ? Color{ 48, 42, 54, 255 } : Color{ 36, 30, 42, 255 };
            if (y == top) c = Color{ 75, 68, 82, 255 };
            kRav[y * KRW + x] = c;
        }
        // Roman fortress watchtowers & tomb monuments (Tomb of Absalom style)
        if (x % 160 >= 20 && x % 160 <= 48) {
            for (int y = 28; y < KRH; ++y) {
                kRav[y * KRW + x] = Color{ 52, 46, 58, 255 };
            }
            if (x % 160 == 34) {
                // Torch on tower
                FillCircle(kRav.data(), KRW, KRH, x, 24, 3, COLOR_TORCH_FIRE);
                Set(kRav.data(), KRW, KRH, x, 23, COLOR_TORCH_EMBER);
            }
        }
    }
    bgKidronRavine = GraphicsManager::CreateTextureFromPixels(KRW, KRH, kRav.data());

    // Hedge Labyrinth Sky (320x160) - Deep twilight with violet royal clouds
    std::vector<Color> hSky(BW * BH);
    for (int y = 0; y < BH; ++y) {
        float ty = (float)y / (float)BH;
        Color skyCol = {
            static_cast<unsigned char>(25 + ty * 45),
            static_cast<unsigned char>(18 + ty * 30),
            static_cast<unsigned char>(55 + ty * 60),
            255
        };
        for (int x = 0; x < BW; ++x) hSky[y * BW + x] = skyCol;
    }
    bgHedgeSky = GraphicsManager::CreateTextureFromPixels(BW, BH, hSky.data());

    // Hedge Labyrinth Midground (640x160)
    std::vector<Color> hLab(KRW * KRH, BLANK);
    for (int x = 0; x < KRW; ++x) {
        // Grand Cypress Topiary rows
        int hedgeH = static_cast<int>(40 + sinf(x * 0.05f) * 12.0f);
        for (int y = hedgeH; y < KRH; ++y) {
            Color c = ((x + y * 2) % 4 == 0) ? COLOR_HEDGE_DARK : COLOR_HEDGE_EMERALD;
            if (y == hedgeH) c = COLOR_HEDGE_TRIM;
            hLab[y * KRW + x] = c;
        }
        // Slender tall Italian Cypress trees
        if (x % 36 == 10) {
            for (int cy = 15; cy < KRH; ++cy) {
                int w = std::min(4, (cy - 15) / 6);
                HLine(hLab.data(), KRW, KRH, x - w, x + w, cy, Color{ 14, 45, 22, 255 });
            }
        }
        // Marble Classical Statues & Gazebos every 160 px
        if (x % 160 == 80) {
            FillRect(hLab.data(), KRW, KRH, x - 8, 35, 16, 25, COLOR_MARBLE_STATUE);
            FillCircle(hLab.data(), KRW, KRH, x, 32, 6, COLOR_MARBLE_STATUE);
        }
    }
    bgHedgeLabyrinth = GraphicsManager::CreateTextureFromPixels(KRW, KRH, hLab.data());

    // Gethsemane Storm Sky (320x160) - Apocalyptic dark churning skies
    std::vector<Color> gSky(BW * BH);
    for (int y = 0; y < BH; ++y) {
        float ty = (float)y / (float)BH;
        Color skyCol = {
            static_cast<unsigned char>(18 + ty * 25),
            static_cast<unsigned char>(15 + ty * 20),
            static_cast<unsigned char>(30 + ty * 35),
            255
        };
        for (int x = 0; x < BW; ++x) gSky[y * BW + x] = skyCol;
    }
    bgGethsemaneSky = GraphicsManager::CreateTextureFromPixels(BW, BH, gSky.data());

    // Gethsemane Ancient Grove Midground (640x160)
    std::vector<Color> gGrove(KRW * KRH, BLANK);
    for (int x = 0; x < KRW; ++x) {
        int hillY = static_cast<int>(60 + sinf(x * 0.03f) * 14.0f);
        for (int y = hillY; y < KRH; ++y) {
            gGrove[y * KRW + x] = Color{ 42, 34, 28, 255 };
        }
        // Massive gnarled ancient olive trunks
        if (x % 80 == 30) {
            FillRect(gGrove.data(), KRW, KRH, x - 6, 20, 12, KRH - 20, COLOR_OLIVE_BARK);
            // Twisting root flares
            Line(gGrove.data(), KRW, KRH, x - 6, KRH - 10, x - 14, KRH - 1, COLOR_OLIVE_BARK);
            Line(gGrove.data(), KRW, KRH, x + 6, KRH - 10, x + 14, KRH - 1, COLOR_OLIVE_BARK);
            // Massive sprawling olive canopy
            FillCircle(gGrove.data(), KRW, KRH, x, 18, 22, COLOR_OLIVE_LEAF);
            FillCircle(gGrove.data(), KRW, KRH, x - 12, 14, 16, COLOR_OLIVE_SILVER);
            FillCircle(gGrove.data(), KRW, KRH, x + 12, 14, 16, COLOR_OLIVE_LEAF);
        }
    }
    bgGethsemaneGrove = GraphicsManager::CreateTextureFromPixels(KRW, KRH, gGrove.data());
}
