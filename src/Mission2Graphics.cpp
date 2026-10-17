#include "Mission2Graphics.h"
#include "Constants.h"
#include <vector>
#include <cmath>
#include <algorithm>

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
    // 1. BONUS STAGE SPRITES
    // =========================================================================
    // Villagers (22x28)
    constexpr int VW = 22, VH = 28;
    std::vector<Color> vPix(VW * VH, BLANK);

    auto PaintVillager = [&](Color* p, int type, int frame, bool satisfied) {
        for (int i = 0; i < VW * VH; ++i) p[i] = BLANK;

        Color skin = COLOR_SKIN;
        Color skinDark = COLOR_SKIN_DARK;
        Color hair = (type == 0) ? Color{ 220, 220, 220, 255 } : Color{ 70, 45, 25, 255 };
        Color tunic = COLOR_ROBE_WHITE;

        if (type == 0) tunic = Color{ 150, 110, 75, 255 };      // Elder (Brown)
        else if (type == 1) tunic = Color{ 65, 115, 165, 255 };  // Woman (Blue)
        else if (type == 2) tunic = Color{ 120, 145, 75, 255 };  // Youth (Olive)
        else if (type == 3) tunic = Color{ 125, 95, 80, 255 };   // Beggar (Rags)

        if (satisfied) {
            // Sitting cross-legged praising
            FillCircle(p, VW, VH, 11, 7, 4, skin);
            FillCircle(p, VW, VH, 11, 5, 4, hair);
            FillRect(p, VW, VH, 8, 12, 7, 10, tunic);
            FillRect(p, VW, VH, 5, 20, 13, 5, tunic);
            // Holding loaf/fish in hands
            FillCircle(p, VW, VH, 11, 14, 3, Color{ 225, 165, 55, 255 });
            return;
        }

        // Running / Begging
        int headY = 5;
        FillCircle(p, VW, VH, 11, headY, 4, skin);
        FillCircle(p, VW, VH, 11, headY - 2, 4, hair);
        if (type == 0) FillRect(p, VW, VH, 10, headY + 2, 3, 4, Color{ 220, 220, 220, 255 }); // Beard

        // Outstretched begging arms forward
        HLine(p, VW, VH, 3, 10, 11, skin);
        HLine(p, VW, VH, 2, 9, 13, skin);

        // Body tunic
        FillRect(p, VW, VH, 8, 10, 7, 11, tunic);
        if (type == 3) {
            // Patched rags
            Set(p, VW, VH, 9, 13, Color{ 60, 40, 30, 255 });
            Set(p, VW, VH, 12, 17, Color{ 170, 130, 90, 255 });
        }

        // Running legs
        if (frame == 0) {
            VLine(p, VW, VH, 9, 21, 26, skinDark);
            VLine(p, VW, VH, 13, 21, 25, skinDark);
            Set(p, VW, VH, 8, 26, Color{ 50, 35, 25, 255 });
            Set(p, VW, VH, 14, 25, Color{ 50, 35, 25, 255 });
        } else {
            Line(p, VW, VH, 10, 21, 7, 26, skinDark);
            Line(p, VW, VH, 12, 21, 15, 26, skinDark);
            Set(p, VW, VH, 6, 26, Color{ 50, 35, 25, 255 });
            Set(p, VW, VH, 16, 26, Color{ 50, 35, 25, 255 });
        }
    };

    // Generate Villagers
    PaintVillager(vPix.data(), 0, 0, false);
    GraphicsManager::SetSpriteTexture(SPR_VILLAGER_ELDER_RUN_0, GraphicsManager::CreateTextureFromPixels(VW, VH, vPix.data()));
    PaintVillager(vPix.data(), 0, 1, false);
    GraphicsManager::SetSpriteTexture(SPR_VILLAGER_ELDER_RUN_1, GraphicsManager::CreateTextureFromPixels(VW, VH, vPix.data()));

    PaintVillager(vPix.data(), 1, 0, false);
    GraphicsManager::SetSpriteTexture(SPR_VILLAGER_WOMAN_RUN_0, GraphicsManager::CreateTextureFromPixels(VW, VH, vPix.data()));
    PaintVillager(vPix.data(), 1, 1, false);
    GraphicsManager::SetSpriteTexture(SPR_VILLAGER_WOMAN_RUN_1, GraphicsManager::CreateTextureFromPixels(VW, VH, vPix.data()));

    PaintVillager(vPix.data(), 2, 0, false);
    GraphicsManager::SetSpriteTexture(SPR_VILLAGER_YOUTH_RUN_0, GraphicsManager::CreateTextureFromPixels(VW, VH, vPix.data()));
    PaintVillager(vPix.data(), 2, 1, false);
    GraphicsManager::SetSpriteTexture(SPR_VILLAGER_YOUTH_RUN_1, GraphicsManager::CreateTextureFromPixels(VW, VH, vPix.data()));

    PaintVillager(vPix.data(), 3, 0, false);
    GraphicsManager::SetSpriteTexture(SPR_VILLAGER_BEGGAR_RUN_0, GraphicsManager::CreateTextureFromPixels(VW, VH, vPix.data()));
    PaintVillager(vPix.data(), 3, 1, false);
    GraphicsManager::SetSpriteTexture(SPR_VILLAGER_BEGGAR_RUN_1, GraphicsManager::CreateTextureFromPixels(VW, VH, vPix.data()));

    PaintVillager(vPix.data(), 0, 0, true);
    GraphicsManager::SetSpriteTexture(SPR_VILLAGER_SATISFIED, GraphicsManager::CreateTextureFromPixels(VW, VH, vPix.data()));

    // Bread Loaf (14x10)
    constexpr int BLW = 14, BLH = 10;
    std::vector<Color> breadPix(BLW * BLH, BLANK);
    FillCircle(breadPix.data(), BLW, BLH, 7, 5, 4, Color{ 210, 150, 65, 255 });
    HLine(breadPix.data(), BLW, BLH, 4, 10, 3, Color{ 245, 205, 125, 255 });
    HLine(breadPix.data(), BLW, BLH, 4, 10, 7, Color{ 145, 95, 35, 255 });
    Set(breadPix.data(), BLW, BLH, 5, 4, Color{ 255, 240, 200, 255 });
    Set(breadPix.data(), BLW, BLH, 9, 4, Color{ 255, 240, 200, 255 });
    GraphicsManager::SetSpriteTexture(SPR_BREAD_LOAF, GraphicsManager::CreateTextureFromPixels(BLW, BLH, breadPix.data()));

    // Fish Projectile (16x8)
    constexpr int FPW = 16, FPH = 8;
    std::vector<Color> fishPix(FPW * FPH, BLANK);
    FillCircle(fishPix.data(), FPW, FPH, 7, 4, 3, Color{ 160, 205, 225, 255 });
    HLine(fishPix.data(), FPW, FPH, 4, 11, 4, Color{ 220, 240, 255, 255 });
    HLine(fishPix.data(), FPW, FPH, 5, 10, 3, Color{ 110, 165, 195, 255 });
    HLine(fishPix.data(), FPW, FPH, 5, 10, 5, Color{ 85, 135, 165, 255 });
    // Tail fin
    Line(fishPix.data(), FPW, FPH, 12, 4, 15, 1, Color{ 130, 185, 215, 255 });
    Line(fishPix.data(), FPW, FPH, 12, 4, 15, 7, Color{ 130, 185, 215, 255 });
    Set(fishPix.data(), FPW, FPH, 3, 3, Color{ 30, 45, 60, 255 }); // Eye
    GraphicsManager::SetSpriteTexture(SPR_FISH_PROJECTILE, GraphicsManager::CreateTextureFromPixels(FPW, FPH, fishPix.data()));

    // Jesus Basket Aim & Throw (28x32)
    constexpr int JW = 28, JH = 32;
    std::vector<Color> jPix(JW * JH, BLANK);
    auto PaintJesusBasket = [&](Color* p, bool throwPose) {
        for (int i = 0; i < JW * JH; ++i) p[i] = BLANK;
        // Head
        FillCircle(p, JW, JH, 14, 6, 4, COLOR_SKIN);
        FillCircle(p, JW, JH, 13, 5, 5, Color{ 95, 52, 26, 255 }); // Hair
        FillCircle(p, JW, JH, 14, 2, 5, COLOR_GOLD_BRIGHT);        // Radiant Halo
        FillCircle(p, JW, JH, 14, 2, 3, Color{ 255, 245, 180, 255 });
        // Tunic & Crimson Mantle
        FillRect(p, JW, JH, 10, 10, 9, 18, COLOR_ROBE_WHITE);
        FillRect(p, JW, JH, 13, 11, 6, 17, COLOR_ROMAN_CRIMSON);
        // Wicker Basket with bread/fish
        FillCircle(p, JW, JH, 7, 18, 5, Color{ 165, 115, 45, 255 });
        FillCircle(p, JW, JH, 7, 15, 3, Color{ 235, 185, 75, 255 });
        // Arms
        if (throwPose) {
            Line(p, JW, JH, 13, 13, 21, 6, COLOR_SKIN);
            FillCircle(p, JW, JH, 22, 5, 2, Color{ 220, 160, 60, 255 }); // Releasing bread
        } else {
            Line(p, JW, JH, 13, 13, 8, 16, COLOR_SKIN);
            Line(p, JW, JH, 15, 13, 19, 14, COLOR_SKIN);
        }
        // Sandaled feet
        FillRect(p, JW, JH, 10, 28, 4, 3, Color{ 90, 60, 30, 255 });
        FillRect(p, JW, JH, 15, 28, 4, 3, Color{ 90, 60, 30, 255 });
    };

    PaintJesusBasket(jPix.data(), false);
    GraphicsManager::SetSpriteTexture(SPR_JESUS_BASKET_AIM, GraphicsManager::CreateTextureFromPixels(JW, JH, jPix.data()));
    PaintJesusBasket(jPix.data(), true);
    GraphicsManager::SetSpriteTexture(SPR_JESUS_BASKET_THROW, GraphicsManager::CreateTextureFromPixels(JW, JH, jPix.data()));

    // =========================================================================
    // 2. MISSION 2 ENEMY SPRITES
    // =========================================================================
    // Tormentor Guard (24x32)
    constexpr int TW = 24, TH = 32;
    std::vector<Color> tPix(TW * TH, BLANK);
    auto PaintTormentor = [&](Color* p, int pose) {
        for (int i = 0; i < TW * TH; ++i) p[i] = BLANK;
        Color skin = Color{ 190, 135, 95, 255 };
        Color armor = Color{ 65, 55, 60, 255 };
        Color bronze = Color{ 185, 125, 45, 255 };
        Color fire = COLOR_TORCH_FIRE;

        // Helmet & Face
        FillCircle(p, TW, TH, 11, 7, 4, skin);
        FillRect(p, TW, TH, 8, 3, 7, 4, bronze);
        Set(p, TW, TH, 10, 8, Color{ 255, 40, 40, 255 }); // Red glowing sinister eye

        // Torso / Cuirass
        FillRect(p, TW, TH, 8, 11, 7, 10, armor);
        OutlineRect(p, TW, TH, 8, 11, 7, 10, bronze);

        // Torch in Left Hand
        VLine(p, TW, TH, 4, 8, 20, Color{ 110, 75, 40, 255 });
        FillCircle(p, TW, TH, 4, 6, 3, fire);
        Set(p, TW, TH, 4, 5, COLOR_TORCH_EMBER);

        // Spiked Club in Right Hand
        if (pose == 2) {
            // Attack swing!
            Line(p, TW, TH, 14, 12, 21, 6, Color{ 85, 80, 85, 255 });
            FillCircle(p, TW, TH, 21, 6, 3, Color{ 40, 35, 40, 255 });
            // Fiery torch swing flare
            FillCircle(p, TW, TH, 1, 10, 4, fire);
        } else {
            Line(p, TW, TH, 14, 12, 18, 19, Color{ 85, 80, 85, 255 });
            FillCircle(p, TW, TH, 18, 19, 3, Color{ 40, 35, 40, 255 });
        }

        // Legs
        if (pose == 1) {
            Line(p, TW, TH, 9, 21, 7, 28, bronze);
            Line(p, TW, TH, 13, 21, 16, 28, bronze);
        } else {
            VLine(p, TW, TH, 9, 21, 28, bronze);
            VLine(p, TW, TH, 13, 21, 28, bronze);
        }
        if (pose == 3) {
            // Hurt flash
            for (int i = 0; i < TW * TH; ++i) if (p[i].a > 0) p[i] = WHITE;
        }
    };

    PaintTormentor(tPix.data(), 0);
    GraphicsManager::SetSpriteTexture(SPR_TORMENTOR_WALK_0, GraphicsManager::CreateTextureFromPixels(TW, TH, tPix.data()));
    PaintTormentor(tPix.data(), 1);
    GraphicsManager::SetSpriteTexture(SPR_TORMENTOR_WALK_1, GraphicsManager::CreateTextureFromPixels(TW, TH, tPix.data()));
    PaintTormentor(tPix.data(), 2);
    GraphicsManager::SetSpriteTexture(SPR_TORMENTOR_ATTACK, GraphicsManager::CreateTextureFromPixels(TW, TH, tPix.data()));
    PaintTormentor(tPix.data(), 3);
    GraphicsManager::SetSpriteTexture(SPR_TORMENTOR_HURT, GraphicsManager::CreateTextureFromPixels(TW, TH, tPix.data()));

    // Thorn Sicarii (Hedge Camouflaged Assassin) (24x30)
    constexpr int SW = 24, SH = 30;
    std::vector<Color> sPix(SW * SH, BLANK);
    auto PaintThornSicarii = [&](Color* p, int pose) {
        for (int i = 0; i < SW * SH; ++i) p[i] = BLANK;
        Color camo = COLOR_HEDGE_EMERALD;
        Color camoDark = COLOR_HEDGE_DARK;
        Color blade = COLOR_SICARII_BLADE;

        // Hood & Mask
        FillCircle(p, SW, SH, 12, 6, 4, camo);
        Set(p, SW, SH, 11, 6, Color{ 255, 220, 50, 255 }); // Amber viper eye

        // Body with Leaf Foliage Wraps
        FillRect(p, SW, SH, 8, 10, 8, 10, camo);
        Set(p, SW, SH, 9, 12, camoDark);
        Set(p, SW, SH, 12, 14, camoDark);

        // Dual Sickles
        if (pose == 1) { // Leap
            Line(p, SW, SH, 6, 8, 2, 4, blade);
            Line(p, SW, SH, 18, 8, 22, 4, blade);
            Line(p, SW, SH, 9, 20, 6, 26, camo);
            Line(p, SW, SH, 15, 20, 18, 26, camo);
        } else if (pose == 2) { // Slash
            Line(p, SW, SH, 14, 12, 22, 14, blade);
            Line(p, SW, SH, 10, 12, 3, 14, blade);
            VLine(p, SW, SH, 9, 20, 27, camo);
            VLine(p, SW, SH, 15, 20, 27, camo);
        } else { // Crouch / Stand
            Line(p, SW, SH, 6, 14, 3, 18, blade);
            Line(p, SW, SH, 18, 14, 21, 18, blade);
            HLine(p, SW, SH, 6, 18, 24, camoDark);
        }

        if (pose == 3) { // Hurt
            for (int i = 0; i < SW * SH; ++i) if (p[i].a > 0) p[i] = Color{ 255, 220, 220, 255 };
        }
    };

    PaintThornSicarii(sPix.data(), 0);
    GraphicsManager::SetSpriteTexture(SPR_THORN_SICARII_CROUCH, GraphicsManager::CreateTextureFromPixels(SW, SH, sPix.data()));
    PaintThornSicarii(sPix.data(), 1);
    GraphicsManager::SetSpriteTexture(SPR_THORN_SICARII_LEAP, GraphicsManager::CreateTextureFromPixels(SW, SH, sPix.data()));
    PaintThornSicarii(sPix.data(), 2);
    GraphicsManager::SetSpriteTexture(SPR_THORN_SICARII_SLASH, GraphicsManager::CreateTextureFromPixels(SW, SH, sPix.data()));
    PaintThornSicarii(sPix.data(), 3);
    GraphicsManager::SetSpriteTexture(SPR_THORN_SICARII_HURT, GraphicsManager::CreateTextureFromPixels(SW, SH, sPix.data()));

    // Corrupted Priest (22x32)
    constexpr int CW = 22, CH = 32;
    std::vector<Color> cPix(CW * CH, BLANK);
    auto PaintCorruptedPriest = [&](Color* p, int pose) {
        for (int i = 0; i < CW * CH; ++i) p[i] = BLANK;
        Color vestment = Color{ 70, 22, 90, 255 };
        Color gold = COLOR_ROMAN_GOLD;
        Color skin = Color{ 210, 190, 180, 255 };

        // Head & Miter
        FillCircle(p, CW, CH, 11, 7, 4, skin);
        FillRect(p, CW, CH, 8, 1, 6, 4, vestment);
        OutlineRect(p, CW, CH, 8, 1, 6, 4, gold);
        Set(p, CW, CH, 10, 7, Color{ 255, 30, 60, 255 });

        // Flowing Dark Robes
        FillRect(p, CW, CH, 7, 11, 8, 15, vestment);
        VLine(p, CW, CH, 11, 11, 26, gold);

        if (pose == 1) { // Cast
            Line(p, CW, CH, 7, 13, 2, 8, skin);
            Line(p, CW, CH, 15, 13, 20, 8, skin);
            // Floating dark spark
            FillCircle(p, CW, CH, 20, 6, 2, Color{ 175, 45, 230, 255 });
        } else {
            Line(p, CW, CH, 7, 13, 4, 18, skin);
            Line(p, CW, CH, 15, 13, 18, 18, skin);
        }

        if (pose == 2) { // Hurt
            for (int i = 0; i < CW * CH; ++i) if (p[i].a > 0) p[i] = WHITE;
        }
    };

    PaintCorruptedPriest(cPix.data(), 0);
    GraphicsManager::SetSpriteTexture(SPR_CORRUPTED_PRIEST_STAND, GraphicsManager::CreateTextureFromPixels(CW, CH, cPix.data()));
    PaintCorruptedPriest(cPix.data(), 1);
    GraphicsManager::SetSpriteTexture(SPR_CORRUPTED_PRIEST_CAST, GraphicsManager::CreateTextureFromPixels(CW, CH, cPix.data()));
    PaintCorruptedPriest(cPix.data(), 2);
    GraphicsManager::SetSpriteTexture(SPR_CORRUPTED_PRIEST_HURT, GraphicsManager::CreateTextureFromPixels(CW, CH, cPix.data()));

    // Dark Orb (12x12)
    constexpr int OW = 12, OH = 12;
    std::vector<Color> orbPix(OW * OH, BLANK);
    FillCircle(orbPix.data(), OW, OH, 6, 6, 5, Color{ 120, 20, 160, 200 });
    FillCircle(orbPix.data(), OW, OH, 6, 6, 3, Color{ 210, 70, 255, 255 });
    Set(orbPix.data(), OW, OH, 6, 6, WHITE);
    GraphicsManager::SetSpriteTexture(SPR_DARK_ORB, GraphicsManager::CreateTextureFromPixels(OW, OH, orbPix.data()));

    // Dark Spike (14x6)
    constexpr int DSW = 14, DSH = 6;
    std::vector<Color> dsPix(DSW * DSH, BLANK);
    HLine(dsPix.data(), DSW, DSH, 0, 11, 2, Color{ 25, 20, 30, 255 });
    HLine(dsPix.data(), DSW, DSH, 0, 11, 3, Color{ 145, 30, 185, 255 });
    Set(dsPix.data(), DSW, DSH, 12, 2, Color{ 255, 60, 60, 255 });
    Set(dsPix.data(), DSW, DSH, 13, 3, Color{ 255, 200, 200, 255 });
    GraphicsManager::SetSpriteTexture(SPR_DARK_SPIKE, GraphicsManager::CreateTextureFromPixels(DSW, DSH, dsPix.data()));

    // =========================================================================
    // 3. THE EVIL GENIUS CLONE ("CHRISTUS TENEBRIS") (28x32)
    // =========================================================================
    constexpr int EW = 28, EH = 32;
    std::vector<Color> ePix(EW * EH, BLANK);

    auto PaintEvilClone = [&](Color* p, int pose) {
        for (int i = 0; i < EW * EH; ++i) p[i] = BLANK;

        Color skin = Color{ 215, 175, 150, 255 };
        Color hair = Color{ 18, 16, 22, 255 };
        Color tunic = COLOR_CLONE_TUNIC;
        Color mantle = COLOR_CLONE_CRIMSON;
        Color eye = COLOR_CLONE_EYE_RED;

        // Dark Corona / Corrupted Aura
        FillCircle(p, EW, EH, 14, 3, 5, Color{ 110, 15, 140, 180 });
        FillCircle(p, EW, EH, 14, 3, 3, Color{ 40, 10, 60, 240 });

        // Head
        FillCircle(p, EW, EH, 14, 7, 4, skin);
        FillCircle(p, EW, EH, 13, 6, 5, hair);
        Set(p, EW, EH, 13, 7, eye); // Piercing red evil eye

        // Body
        FillRect(p, EW, EH, 10, 11, 9, 17, tunic);
        FillRect(p, EW, EH, 13, 12, 6, 16, mantle);
        OutlineRect(p, EW, EH, 13, 12, 6, 16, Color{ 220, 175, 55, 255 });

        if (pose == 3) { // Jump Somersault
            FillCircle(p, EW, EH, 14, 16, 9, tunic);
            FillCircle(p, EW, EH, 14, 16, 6, mantle);
        } else if (pose == 4) { // Throw Stand
            Line(p, EW, EH, 13, 14, 22, 10, skin);
            Set(p, EW, EH, 23, 9, eye);
            VLine(p, EW, EH, 11, 28, 30, hair);
            VLine(p, EW, EH, 16, 28, 30, hair);
        } else if (pose == 5) { // Melee Strike
            Line(p, EW, EH, 13, 14, 24, 14, skin);
            FillCircle(p, EW, EH, 24, 14, 3, Color{ 180, 25, 220, 255 });
            VLine(p, EW, EH, 11, 28, 30, hair);
            VLine(p, EW, EH, 16, 28, 30, hair);
        } else if (pose == 6) { // Channeling Dark Wrath
            Line(p, EW, EH, 11, 14, 5, 5, skin);
            Line(p, EW, EH, 17, 14, 23, 5, skin);
            FillCircle(p, EW, EH, 14, 1, 4, Color{ 220, 30, 70, 255 });
            VLine(p, EW, EH, 11, 28, 30, hair);
            VLine(p, EW, EH, 16, 28, 30, hair);
        } else if (pose == 7) { // Hurt
            for (int i = 0; i < EW * EH; ++i) if (p[i].a > 0) p[i] = Color{ 255, 200, 240, 255 };
        } else if (pose == 8) { // Kneel / Dissolve
            FillCircle(p, EW, EH, 14, 14, 4, skin);
            FillRect(p, EW, EH, 10, 18, 9, 10, tunic);
            FillRect(p, EW, EH, 7, 26, 14, 4, mantle);
        } else { // Walk
            int step = pose % 2;
            Line(p, EW, EH, 11, 27, step == 0 ? 8 : 14, 31, hair);
            Line(p, EW, EH, 16, 27, step == 0 ? 19 : 13, 31, hair);
        }
    };

    PaintEvilClone(ePix.data(), 0);
    GraphicsManager::SetSpriteTexture(SPR_CLONE_IDLE, GraphicsManager::CreateTextureFromPixels(EW, EH, ePix.data()));
    PaintEvilClone(ePix.data(), 1);
    GraphicsManager::SetSpriteTexture(SPR_CLONE_WALK_0, GraphicsManager::CreateTextureFromPixels(EW, EH, ePix.data()));
    PaintEvilClone(ePix.data(), 2);
    GraphicsManager::SetSpriteTexture(SPR_CLONE_WALK_1, GraphicsManager::CreateTextureFromPixels(EW, EH, ePix.data()));
    PaintEvilClone(ePix.data(), 3);
    GraphicsManager::SetSpriteTexture(SPR_CLONE_JUMP, GraphicsManager::CreateTextureFromPixels(EW, EH, ePix.data()));
    PaintEvilClone(ePix.data(), 4);
    GraphicsManager::SetSpriteTexture(SPR_CLONE_THROW_STAND, GraphicsManager::CreateTextureFromPixels(EW, EH, ePix.data()));
    PaintEvilClone(ePix.data(), 4);
    GraphicsManager::SetSpriteTexture(SPR_CLONE_THROW_CROUCH, GraphicsManager::CreateTextureFromPixels(EW, EH, ePix.data()));
    PaintEvilClone(ePix.data(), 5);
    GraphicsManager::SetSpriteTexture(SPR_CLONE_MELEE_STRIKE, GraphicsManager::CreateTextureFromPixels(EW, EH, ePix.data()));
    PaintEvilClone(ePix.data(), 6);
    GraphicsManager::SetSpriteTexture(SPR_CLONE_CHANNEL, GraphicsManager::CreateTextureFromPixels(EW, EH, ePix.data()));
    PaintEvilClone(ePix.data(), 7);
    GraphicsManager::SetSpriteTexture(SPR_CLONE_HURT, GraphicsManager::CreateTextureFromPixels(EW, EH, ePix.data()));
    PaintEvilClone(ePix.data(), 8);
    GraphicsManager::SetSpriteTexture(SPR_CLONE_KNEEL, GraphicsManager::CreateTextureFromPixels(EW, EH, ePix.data()));

    // =========================================================================
    // 4. MISSION 2 PICKUPS (18x18)
    // =========================================================================
    constexpr int PW = 18, PH = 18;
    std::vector<Color> pickPix(PW * PH, BLANK);

    // Silver Pitcher (Stage 4)
    for (int i = 0; i < PW * PH; ++i) pickPix[i] = BLANK;
    FillCircle(pickPix.data(), PW, PH, 9, 11, 5, Color{ 200, 215, 235, 255 });
    FillRect(pickPix.data(), PW, PH, 7, 4, 5, 5, Color{ 170, 185, 205, 255 });
    HLine(pickPix.data(), PW, PH, 6, 12, 3, Color{ 235, 245, 255, 255 });
    Line(pickPix.data(), PW, PH, 6, 5, 3, 9, Color{ 160, 175, 195, 255 });
    Line(pickPix.data(), PW, PH, 3, 9, 6, 13, Color{ 160, 175, 195, 255 });
    GraphicsManager::SetSpriteTexture(SPR_SILVER_PITCHER, GraphicsManager::CreateTextureFromPixels(PW, PH, pickPix.data()));

    // Olive Wreath (Stage 5)
    for (int i = 0; i < PW * PH; ++i) pickPix[i] = BLANK;
    OutlineCircle(pickPix.data(), PW, PH, 9, 9, 6, COLOR_HEDGE_EMERALD);
    OutlineCircle(pickPix.data(), PW, PH, 9, 9, 5, COLOR_HEDGE_TRIM);
    Set(pickPix.data(), PW, PH, 9, 3, COLOR_GOLD_BRIGHT);
    Set(pickPix.data(), PW, PH, 15, 9, COLOR_GOLD_BRIGHT);
    Set(pickPix.data(), PW, PH, 9, 15, COLOR_GOLD_BRIGHT);
    Set(pickPix.data(), PW, PH, 3, 9, COLOR_GOLD_BRIGHT);
    GraphicsManager::SetSpriteTexture(SPR_OLIVE_WREATH, GraphicsManager::CreateTextureFromPixels(PW, PH, pickPix.data()));

    // Sacred Chalice (Stage 6)
    for (int i = 0; i < PW * PH; ++i) pickPix[i] = BLANK;
    FillCircle(pickPix.data(), PW, PH, 9, 6, 5, COLOR_GOLD_BRIGHT);
    FillRect(pickPix.data(), PW, PH, 5, 2, 9, 4, COLOR_ROMAN_GOLD);
    VLine(pickPix.data(), PW, PH, 9, 10, 14, COLOR_ROMAN_GOLD);
    FillRect(pickPix.data(), PW, PH, 6, 14, 7, 3, COLOR_GOLD_BRIGHT);
    Set(pickPix.data(), PW, PH, 9, 5, Color{ 255, 35, 45, 255 }); // Ruby
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
            if (y == 0) c = Color{ 105, 95, 115, 255 };
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
    HLine(tPixTile.data(), T_SZ, T_SZ, 0, T_SZ - 1, 0, Color{ 115, 105, 130, 255 });
    HLine(tPixTile.data(), T_SZ, T_SZ, 0, T_SZ - 1, 5, COLOR_KIDRON_DARK);
    GraphicsManager::SetTileTexture(TILE_KIDRON_LEDGE, GraphicsManager::CreateTextureFromPixels(T_SZ, T_SZ, tPixTile.data()));

    // Kidron Columns (Head, Mid, Base)
    for (int i = 0; i < T_SZ * T_SZ; ++i) tPixTile[i] = BLANK;
    FillRect(tPixTile.data(), T_SZ, T_SZ, 1, 0, 14, 4, Color{ 130, 120, 140, 255 });
    FillRect(tPixTile.data(), T_SZ, T_SZ, 3, 4, 10, 12, COLOR_KIDRON_ROCK);
    GraphicsManager::SetTileTexture(TILE_KIDRON_COLUMN_HEAD, GraphicsManager::CreateTextureFromPixels(T_SZ, T_SZ, tPixTile.data()));

    for (int i = 0; i < T_SZ * T_SZ; ++i) tPixTile[i] = BLANK;
    FillRect(tPixTile.data(), T_SZ, T_SZ, 3, 0, 10, T_SZ, COLOR_KIDRON_ROCK);
    VLine(tPixTile.data(), T_SZ, T_SZ, 3, 0, T_SZ - 1, Color{ 115, 105, 130, 255 });
    VLine(tPixTile.data(), T_SZ, T_SZ, 12, 0, T_SZ - 1, COLOR_KIDRON_DARK);
    GraphicsManager::SetTileTexture(TILE_KIDRON_COLUMN_MID, GraphicsManager::CreateTextureFromPixels(T_SZ, T_SZ, tPixTile.data()));

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

    // Hedge Ledges (1 and 2)
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
    // Surface & Sub
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

    // Olive Tree Trunk, Branch, Leaves
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

    // Gethsemane Ledge
    for (int i = 0; i < T_SZ * T_SZ; ++i) tPixTile[i] = BLANK;
    FillRect(tPixTile.data(), T_SZ, T_SZ, 0, 0, T_SZ, 6, Color{ 65, 52, 42, 255 });
    HLine(tPixTile.data(), T_SZ, T_SZ, 0, T_SZ - 1, 0, Color{ 85, 115, 75, 255 });
    GraphicsManager::SetTileTexture(TILE_GETHSEMANE_LEDGE, GraphicsManager::CreateTextureFromPixels(T_SZ, T_SZ, tPixTile.data()));

    // Sacred Altar Stone of Agony
    for (int i = 0; i < T_SZ * T_SZ; ++i) tPixTile[i] = BLANK;
    FillRect(tPixTile.data(), T_SZ, T_SZ, 1, 4, 14, 11, Color{ 85, 80, 95, 255 });
    OutlineRect(tPixTile.data(), T_SZ, T_SZ, 1, 4, 14, 11, COLOR_GOLD_BRIGHT);
    HLine(tPixTile.data(), T_SZ, T_SZ, 3, 12, 8, Color{ 255, 220, 100, 255 }); // Glowing inscription
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
            // Sea of Galilee reflections
            skyCol = Color{ 180, 135, 95, 255 };
        }
        for (int x = 0; x < BW; ++x) bSky[y * BW + x] = skyCol;
    }
    // Sun over Galilee
    FillCircle(bSky.data(), BW, BH, 160, 95, 22, Color{ 255, 245, 180, 255 });
    FillCircle(bSky.data(), BW, BH, 160, 95, 16, WHITE);
    bgBonusSunset = GraphicsManager::CreateTextureFromPixels(BW, BH, bSky.data());

    // Bonus Terraces (480x160)
    constexpr int BTW = 480, BTH = 160;
    std::vector<Color> bTerr(BTW * BTH, BLANK);
    for (int x = 0; x < BTW; ++x) {
        float h1 = sinf(x * 0.02f) * 18.0f;
        int hillY = static_cast<int>(105 + h1);
        for (int y = hillY; y < BTH; ++y) {
            bTerr[y * BTW + x] = Color{ 85, 120, 55, 255 };
        }
    }
    bgBonusTerraces = GraphicsManager::CreateTextureFromPixels(BTW, BTH, bTerr.data());

    // Kidron Sky (320x160)
    std::vector<Color> kSky(BW * BH);
    for (int y = 0; y < BH; ++y) {
        float ty = (float)y / (float)BH;
        Color skyCol = Color{ (unsigned char)(10 + ty * 20), (unsigned char)(8 + ty * 16), (unsigned char)(28 + ty * 32), 255 };
        for (int x = 0; x < BW; ++x) kSky[y * BW + x] = skyCol;
    }
    // Radiant Full Moon
    FillCircle(kSky.data(), BW, BH, 230, 42, 14, Color{ 250, 252, 255, 255 });
    FillCircle(kSky.data(), BW, BH, 230, 42, 18, Color{ 200, 220, 255, 60 });
    // Stars
    for (int s = 0; s < 45; ++s) {
        int sx = (s * 47) % BW;
        int sy = (s * 31) % 90;
        Set(kSky.data(), BW, BH, sx, sy, Color{ 230, 240, 255, 200 });
    }
    bgKidronSky = GraphicsManager::CreateTextureFromPixels(BW, BH, kSky.data());

    // Kidron Ravine (480x160)
    std::vector<Color> kRav(BTW * BTH, BLANK);
    for (int x = 0; x < BTW; ++x) {
        float rForm = sinf(x * 0.035f) * 25.0f + cosf(x * 0.07f) * 12.0f;
        int cliffY = static_cast<int>(95 + rForm);
        for (int y = cliffY; y < BTH; ++y) {
            kRav[y * BTW + x] = Color{ 32, 28, 38, 255 };
        }
    }
    bgKidronRavine = GraphicsManager::CreateTextureFromPixels(BTW, BTH, kRav.data());

    // Hedge Maze Sky (320x160)
    std::vector<Color> hSky(BW * BH);
    for (int y = 0; y < BH; ++y) {
        float ty = (float)y / (float)BH;
        Color skyCol = Color{ (unsigned char)(14 + ty * 18), (unsigned char)(12 + ty * 26), (unsigned char)(32 + ty * 38), 255 };
        for (int x = 0; x < BW; ++x) hSky[y * BW + x] = skyCol;
    }
    bgHedgeSky = GraphicsManager::CreateTextureFromPixels(BW, BH, hSky.data());

    // Hedge Labyrinth (480x160)
    std::vector<Color> hLab(BTW * BTH, BLANK);
    for (int x = 0; x < BTW; ++x) {
        int hedgeY = 110 + (x % 64 < 32 ? 0 : 8);
        for (int y = hedgeY; y < BTH; ++y) {
            hLab[y * BTW + x] = Color{ 18, 55, 28, 255 };
        }
    }
    bgHedgeLabyrinth = GraphicsManager::CreateTextureFromPixels(BTW, BTH, hLab.data());

    // Gethsemane Sky (320x160)
    std::vector<Color> gSky(BW * BH);
    for (int y = 0; y < BH; ++y) {
        float ty = (float)y / (float)BH;
        Color skyCol = Color{ (unsigned char)(18 + ty * 25), (unsigned char)(10 + ty * 18), (unsigned char)(36 + ty * 42), 255 };
        for (int x = 0; x < BW; ++x) gSky[y * BW + x] = skyCol;
    }
    // Gethsemane Crescent / Agony Moon
    FillCircle(gSky.data(), BW, BH, 90, 40, 16, Color{ 255, 240, 230, 255 });
    FillCircle(gSky.data(), BW, BH, 96, 38, 14, Color{ 18, 10, 36, 255 });
    bgGethsemaneSky = GraphicsManager::CreateTextureFromPixels(BW, BH, gSky.data());

    // Gethsemane Grove (480x160)
    std::vector<Color> gGrove(BTW * BTH, BLANK);
    for (int px = 40; px < BTW; px += 90) {
        // Massive Ancient Olive Tree silhouettes
        FillCircle(gGrove.data(), BTW, BTH, px, 80, 35, Color{ 28, 38, 30, 255 });
        FillRect(gGrove.data(), BTW, BTH, px - 8, 80, 16, 80, Color{ 36, 28, 22, 255 });
    }
    bgGethsemaneGrove = GraphicsManager::CreateTextureFromPixels(BTW, BTH, gGrove.data());
}

