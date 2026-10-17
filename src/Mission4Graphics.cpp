#include "Mission4Graphics.h"
#include "Constants.h"
#include <vector>
#include <cmath>
#include <algorithm>
#include <cstdlib>

namespace M4Pixel {
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
}

void Mission4Graphics::GenerateAll(
    Texture2D& bgDolorosaSky,
    Texture2D& bgDolorosaStreet,
    Texture2D& bgCourtSky,
    Texture2D& bgCourtBasilica,
    Texture2D& bgThroneSky,
    Texture2D& bgThronePalace
) {
    using namespace M4Pixel;

    // Common Palette
    Color skinBase     = Color{ 225, 175, 140, 255 };
    Color skinDark     = Color{ 175, 125,  95, 255 };
    Color hairDark     = Color{  50,  35,  25, 255 };
    Color romanBronze  = Color{ 205, 145,  35, 255 };
    Color romanGold    = Color{ 245, 205,  55, 255 };
    Color romanGoldHi  = Color{ 255, 235, 120, 255 };
    Color romanSteel   = Color{ 190, 205, 220, 255 };
    Color romanSteelDk = Color{ 110, 125, 140, 255 };
    Color romanCrimson = Color{ 180,  25,  38, 255 };
    Color romanRedDark = Color{ 115,  12,  22, 255 };
    Color imperialPurp = Color{  95,  15,  80, 255 };
    Color purpDark     = Color{  55,   8,  48, 255 };
    Color togaWhite    = Color{ 245, 242, 240, 255 };
    Color togaShade    = Color{ 190, 185, 185, 255 };
    Color woodBrown    = Color{ 125,  80,  45, 255 };
    Color woodDark     = Color{  75,  45,  25, 255 };
    Color wolfPelt     = Color{ 150, 145, 140, 255 };
    Color wolfDark     = Color{  95,  90,  85, 255 };

    // =========================================================================
    // 1. SPRITES GENERATION
    // =========================================================================

    // --- A. Roman Standard Bearer (Signifer) (32x32) ---
    auto GenSignifer = [&](int animState) {
        std::vector<Color> px(32 * 32, BLANK);
        Color* p = px.data();

        int legOffset = (animState == 1) ? 2 : 0;
        int thrustX = (animState == 2) ? 4 : 0;

        // Legs (Sandals & Greaves)
        FillRect(p, 32, 32, 10 - legOffset, 22, 3, 9, romanBronze);
        FillRect(p, 32, 32, 17 + legOffset, 22, 3, 9, romanBronze);
        FillRect(p, 32, 32, 9 - legOffset, 30, 5, 2, woodDark);
        FillRect(p, 32, 32, 17 + legOffset, 30, 5, 2, woodDark);

        // Chainmail Lorica Hamata & Tunic
        FillRect(p, 32, 32, 10, 12, 11, 11, romanCrimson);
        FillRect(p, 32, 32, 11, 13, 9, 8, romanSteelDk);
        for (int y = 14; y < 20; y += 2) {
            for (int x = 11; x < 20; x += 2) Set(p, 32, 32, x, y, romanSteel);
        }

        // Wolf-skin Pelt Drape over shoulders & back
        FillRect(p, 32, 32, 8, 8, 4, 13, wolfPelt);
        FillRect(p, 32, 32, 9, 9, 2, 11, wolfDark);

        // Head & Wolf Cowl Helmet
        FillRect(p, 32, 32, 12, 6, 7, 7, skinBase);
        FillRect(p, 32, 32, 10, 2, 11, 5, wolfPelt);
        Set(p, 32, 32, 11, 3, BLACK); // Wolf eye
        Set(p, 32, 32, 17, 3, BLACK);
        Set(p, 32, 32, 14, 5, romanBronze); // Helmet nosepiece
        Set(p, 32, 32, 15, 8, skinDark);    // Eye

        // Roman Standard (Signum) Spear Pole
        int poleX = 22 + thrustX;
        VLine(p, 32, 32, poleX, 1, 31, romanBronze);
        VLine(p, 32, 32, poleX + 1, 2, 30, romanGold);

        // Gilded Aquila / Eagle on top of standard
        FillRect(p, 32, 32, poleX - 3, 0, 8, 4, romanGold);
        Set(p, 32, 32, poleX - 4, 1, romanGoldHi);
        Set(p, 32, 32, poleX + 4, 1, romanGoldHi);

        // SPQR Red Vexillum Banner hanging from crossbar
        HLine(p, 32, 32, poleX - 4, poleX + 5, 4, romanGold);
        FillRect(p, 32, 32, poleX - 4, 5, 9, 6, romanCrimson);
        HLine(p, 32, 32, poleX - 3, poleX + 3, 6, romanGold); // Gold fringe
        Set(p, 32, 32, poleX - 1, 8, romanGoldHi);             // SPQR motif

        // Spear bottom ferrule
        if (animState == 2) {
            // Thrusting forward
            HLine(p, 32, 32, poleX - 2, poleX + 6, 16, romanSteel);
        }

        if (animState == 3) { // Hurt flash
            for (auto& c : px) if (c.a > 0) c = Color{ 255, 120, 120, c.a };
        }

        return GraphicsManager::CreateTextureFromPixels(32, 32, px.data());
    };

    GraphicsManager::SetSpriteTexture(SPR_SIGNIFER_WALK_0, GenSignifer(0));
    GraphicsManager::SetSpriteTexture(SPR_SIGNIFER_WALK_1, GenSignifer(1));
    GraphicsManager::SetSpriteTexture(SPR_SIGNIFER_ATTACK, GenSignifer(2));
    GraphicsManager::SetSpriteTexture(SPR_SIGNIFER_HURT,   GenSignifer(3));

    // --- B. Praetorian Guard (Scutum & Spatha) (32x32) ---
    auto GenPraetorian = [&](int animState) {
        std::vector<Color> px(32 * 32, BLANK);
        Color* p = px.data();

        int step = (animState == 1) ? 2 : 0;
        int bashX = (animState == 2) ? 6 : 0;

        // Legs
        FillRect(p, 32, 32, 10 - step, 22, 4, 9, romanSteel);
        FillRect(p, 32, 32, 17 + step, 22, 4, 9, romanSteel);
        FillRect(p, 32, 32, 9 - step, 30, 5, 2, woodDark);
        FillRect(p, 32, 32, 17 + step, 30, 5, 2, woodDark);

        // Lorica Segmentata (Steel & Gold banded plate)
        FillRect(p, 32, 32, 10, 11, 11, 12, imperialPurp);
        for (int y = 12; y <= 20; y += 2) {
            HLine(p, 32, 32, 10, 20, y, romanGold);
            HLine(p, 32, 32, 10, 20, y + 1, romanSteel);
        }

        // Head & Attic Helmet with Royal Purple Crest
        FillRect(p, 32, 32, 12, 6, 7, 6, skinBase);
        FillRect(p, 32, 32, 11, 4, 9, 3, romanGold);
        HLine(p, 32, 32, 8, 20, 2, imperialPurp); // Crest plume
        HLine(p, 32, 32, 9, 19, 1, Color{ 180, 45, 160, 255 });

        // Giant Curved Scutum Shield (Imperial Purple & Gold Lightning Wings)
        int shieldX = 18 + bashX;
        FillRect(p, 32, 32, shieldX, 8, 10, 22, imperialPurp);
        OutlineRect(p, 32, 32, shieldX, 8, 10, 22, romanGold);
        // Central Golden Umbo (Shield Boss)
        FillRect(p, 32, 32, shieldX + 3, 17, 4, 4, romanGoldHi);
        // Wing motifs
        HLine(p, 32, 32, shieldX + 1, shieldX + 8, 12, romanGold);
        HLine(p, 32, 32, shieldX + 1, shieldX + 8, 24, romanGold);

        // Spatha Sword in Right Hand
        if (animState == 2) {
            // Forward shield bash thrust
            HLine(p, 32, 32, shieldX + 6, shieldX + 12, 16, romanSteel);
        } else {
            VLine(p, 32, 32, 6, 12, 26, romanSteel);
            FillRect(p, 32, 32, 4, 11, 5, 2, romanGold); // Guard
        }

        if (animState == 3) {
            for (auto& c : px) if (c.a > 0) c = Color{ 255, 120, 120, c.a };
        }

        return GraphicsManager::CreateTextureFromPixels(32, 32, px.data());
    };

    GraphicsManager::SetSpriteTexture(SPR_PRAETORIAN_WALK_0,       GenPraetorian(0));
    GraphicsManager::SetSpriteTexture(SPR_PRAETORIAN_WALK_1,       GenPraetorian(1));
    GraphicsManager::SetSpriteTexture(SPR_PRAETORIAN_SHIELD_BASH, GenPraetorian(2));
    GraphicsManager::SetSpriteTexture(SPR_PRAETORIAN_HURT,        GenPraetorian(3));

    // --- C. Imperial Lictor (Fasces Rods & Axe) (32x32) ---
    auto GenLictor = [&](int animState) {
        std::vector<Color> px(32 * 32, BLANK);
        Color* p = px.data();

        int step = (animState == 1) ? 2 : 0;
        int swing = (animState == 2) ? 6 : 0;

        // Legs
        FillRect(p, 32, 32, 10 - step, 22, 3, 9, skinDark);
        FillRect(p, 32, 32, 17 + step, 22, 3, 9, skinDark);
        FillRect(p, 32, 32, 9 - step, 30, 5, 2, woodDark);
        FillRect(p, 32, 32, 17 + step, 30, 5, 2, woodDark);

        // Toga / Judicial Robe (Off-white with crimson sash)
        FillRect(p, 32, 32, 10, 11, 11, 12, togaWhite);
        for (int y = 11; y < 22; ++y) {
            Set(p, 32, 32, 10 + (y % 4), y, romanCrimson); // Magistrate sash
        }

        // Head & Laurel Wreath
        FillRect(p, 32, 32, 12, 5, 7, 7, skinBase);
        HLine(p, 32, 32, 11, 18, 5, romanGold); // Wreath
        FillRect(p, 32, 32, 11, 3, 8, 3, hairDark);

        // The Fasces (Bundle of elm rods tied with red leather + Bronze Axe Blade)
        int fascesX = (animState == 2) ? 22 : 6;
        int fascesY = (animState == 2) ? 14 : 4;
        FillRect(p, 32, 32, fascesX, fascesY, 5, 20, woodBrown);
        // Red leather binding cords
        HLine(p, 32, 32, fascesX, fascesX + 4, fascesY + 4, romanCrimson);
        HLine(p, 32, 32, fascesX, fascesX + 4, fascesY + 10, romanCrimson);
        HLine(p, 32, 32, fascesX, fascesX + 4, fascesY + 16, romanCrimson);

        // Bronze Axe Blade protruding from bundle
        FillRect(p, 32, 32, fascesX + 4, fascesY + 6, 5, 6, romanBronze);
        VLine(p, 32, 32, fascesX + 8, fascesY + 5, fascesY + 12, romanSteel); // Sharp edge

        if (animState == 3) {
            for (auto& c : px) if (c.a > 0) c = Color{ 255, 120, 120, c.a };
        }

        return GraphicsManager::CreateTextureFromPixels(32, 32, px.data());
    };

    GraphicsManager::SetSpriteTexture(SPR_LICTOR_WALK_0,        GenLictor(0));
    GraphicsManager::SetSpriteTexture(SPR_LICTOR_WALK_1,        GenLictor(1));
    GraphicsManager::SetSpriteTexture(SPR_LICTOR_FASCES_SWING, GenLictor(2));
    GraphicsManager::SetSpriteTexture(SPR_LICTOR_HURT,         GenLictor(3));

    // --- D. Stage 10 Boss: Praetor Prefect Quintus (48x48) ---
    auto GenPrefect = [&](int animState) {
        std::vector<Color> px(48 * 48, BLANK);
        Color* p = px.data();

        int step = (animState == 1 || animState == 2) ? 2 : 0;
        int rushX = (animState == 4) ? 8 : 0;

        // Legs (Ornate Bronze Greaves)
        FillRect(p, 48, 48, 16 - step, 32, 5, 14, romanBronze);
        FillRect(p, 48, 48, 25 + step, 32, 5, 14, romanBronze);
        FillRect(p, 48, 48, 14 - step, 44, 7, 3, woodDark);
        FillRect(p, 48, 48, 25 + step, 44, 7, 3, woodDark);

        // Scarlet General's Cape (Paludamentum) behind body
        FillRect(p, 48, 48, 10, 16, 8, 26, romanCrimson);
        FillRect(p, 48, 48, 8, 20, 4, 20, romanRedDark);

        // Gilded Lorica Musculata (Torso Armor)
        FillRect(p, 48, 48, 16, 16, 15, 17, romanBronze);
        FillRect(p, 48, 48, 18, 18, 11, 13, romanGold);
        // Pteruges (Armor skirt flaps)
        for (int x = 16; x <= 28; x += 3) {
            VLine(p, 48, 48, x, 31, 35, romanGoldHi);
            VLine(p, 48, 48, x + 1, 31, 35, romanRedDark);
        }

        // Head & Attic General Helmet with Transverse Red & Gold Crest
        FillRect(p, 48, 48, 19, 8, 9, 9, skinBase);
        FillRect(p, 48, 48, 18, 6, 11, 5, romanGold);
        HLine(p, 48, 48, 14, 30, 4, romanCrimson);
        HLine(p, 48, 48, 16, 28, 3, romanGoldHi);

        // Round Bronze Parma Shield on Left Hand
        int shX = 26 + rushX;
        FillRect(p, 48, 48, shX, 16, 14, 18, romanBronze);
        OutlineRect(p, 48, 48, shX, 16, 14, 18, romanGold);
        FillRect(p, 48, 48, shX + 4, 22, 6, 6, romanGoldHi); // Central Gorgon/Eagle boss

        // Silver Gladius Sword in Right Hand
        if (animState == 3) {
            // Forward slash
            HLine(p, 48, 48, 28, 45, 24, romanSteel);
            FillRect(p, 48, 48, 26, 22, 4, 6, romanGold);
        } else {
            VLine(p, 48, 48, 12, 18, 36, romanSteel);
            FillRect(p, 48, 48, 10, 18, 5, 3, romanGold);
        }

        // Shield Rush motion lines
        if (animState == 4) {
            HLine(p, 48, 48, 38, 46, 20, Color{ 255, 235, 120, 220 });
            HLine(p, 48, 48, 36, 47, 28, Color{ 255, 200, 50, 200 });
        }

        // Kneel defeat
        if (animState == 6) {
            // Lowered posture
            px.assign(48 * 48, BLANK);
            FillRect(p, 48, 48, 14, 26, 18, 14, romanBronze);
            FillRect(p, 48, 48, 16, 20, 12, 8, romanCrimson);
            FillRect(p, 48, 48, 17, 14, 10, 7, skinBase);
            FillRect(p, 48, 48, 16, 12, 12, 4, romanGold);
            HLine(p, 48, 48, 12, 32, 40, romanBronze); // Dropped sword
        }

        if (animState == 5) {
            for (auto& c : px) if (c.a > 0) c = Color{ 255, 120, 120, c.a };
        }

        return GraphicsManager::CreateTextureFromPixels(48, 48, px.data());
    };

    GraphicsManager::SetSpriteTexture(SPR_PREFECT_IDLE,        GenPrefect(0));
    GraphicsManager::SetSpriteTexture(SPR_PREFECT_WALK_0,      GenPrefect(1));
    GraphicsManager::SetSpriteTexture(SPR_PREFECT_WALK_1,      GenPrefect(2));
    GraphicsManager::SetSpriteTexture(SPR_PREFECT_SLASH,       GenPrefect(3));
    GraphicsManager::SetSpriteTexture(SPR_PREFECT_SHIELD_RUSH, GenPrefect(4));
    GraphicsManager::SetSpriteTexture(SPR_PREFECT_HURT,        GenPrefect(5));
    GraphicsManager::SetSpriteTexture(SPR_PREFECT_KNEEL,       GenPrefect(6));

    // --- E. Stage 11 Boss: Imperial Advocate Tertullus (48x48) ---
    auto GenAdvocate = [&](int animState) {
        std::vector<Color> px(48 * 48, BLANK);
        Color* p = px.data();

        // Billowing Court Mantle & Toga
        FillRect(p, 48, 48, 16, 16, 16, 26, imperialPurp);
        FillRect(p, 48, 48, 19, 18, 10, 22, togaWhite);
        OutlineRect(p, 48, 48, 16, 16, 16, 26, romanGold);

        // Head & Silver Hair
        FillRect(p, 48, 48, 20, 8, 9, 9, skinDark);
        FillRect(p, 48, 48, 18, 6, 13, 5, Color{ 200, 205, 215, 255 }); // Grey orator hair

        // Orator's Extended Hands & Legal Scroll
        if (animState == 1) { // Cast
            FillRect(p, 48, 48, 30, 14, 10, 6, skinBase);
            FillRect(p, 48, 48, 34, 12, 6, 14, Color{ 235, 220, 190, 255 }); // Open scroll
        } else if (animState == 2) { // Shout "CRUCIFIGE!"
            FillRect(p, 48, 48, 23, 12, 4, 4, BLACK); // Open screaming mouth
            // Energy rings
            for (int r = 16; r <= 28; r += 4) {
                Set(p, 48, 48, 32 + (r - 16), 14, Color{ 220, 80, 255, 220 });
            }
        } else { // Idle
            FillRect(p, 48, 48, 28, 18, 8, 5, skinBase);
            FillRect(p, 48, 48, 32, 16, 4, 12, Color{ 235, 220, 190, 255 }); // Rolled scroll
        }

        // Kneel defeat
        if (animState == 4) {
            px.assign(48 * 48, BLANK);
            FillRect(p, 48, 48, 16, 26, 16, 16, imperialPurp);
            FillRect(p, 48, 48, 18, 20, 10, 8, skinDark);
            FillRect(p, 48, 48, 17, 18, 12, 4, Color{ 200, 205, 215, 255 });
        }

        if (animState == 3) {
            for (auto& c : px) if (c.a > 0) c = Color{ 255, 120, 120, c.a };
        }

        return GraphicsManager::CreateTextureFromPixels(48, 48, px.data());
    };

    GraphicsManager::SetSpriteTexture(SPR_ADVOCATE_IDLE,  GenAdvocate(0));
    GraphicsManager::SetSpriteTexture(SPR_ADVOCATE_CAST,  GenAdvocate(1));
    GraphicsManager::SetSpriteTexture(SPR_ADVOCATE_SHOUT, GenAdvocate(2));
    GraphicsManager::SetSpriteTexture(SPR_ADVOCATE_HURT,  GenAdvocate(3));
    GraphicsManager::SetSpriteTexture(SPR_ADVOCATE_KNEEL, GenAdvocate(4));

    // Sonic Shout Wave Sprite (16x16)
    {
        std::vector<Color> px(16 * 16, BLANK);
        Color* p = px.data();
        for (int y = 0; y < 16; ++y) {
            float dy = y - 8.0f;
            int x = static_cast<int>(8.0f - (dy * dy) * 0.12f);
            if (x >= 0 && x < 16) {
                Set(p, 16, 16, x, y, Color{ 230, 120, 255, 240 });
                Set(p, 16, 16, x + 1, y, Color{ 255, 255, 255, 255 });
                Set(p, 16, 16, x + 2, y, Color{ 160, 40, 220, 180 });
            }
        }
        GraphicsManager::SetSpriteTexture(SPR_SHOUT_WAVE, GraphicsManager::CreateTextureFromPixels(16, 16, px.data()));
    }

    // --- F. Stage 12 Final Boss: Pontius Pilate (48x48) ---
    auto GenPilate = [&](int animState) {
        std::vector<Color> px(48 * 48, BLANK);
        Color* p = px.data();

        int step = (animState == 1 || animState == 2) ? 2 : 0;

        // Legs (Gilded Imperial Caligae Boots)
        FillRect(p, 48, 48, 17 - step, 34, 5, 12, romanGold);
        FillRect(p, 48, 48, 25 + step, 34, 5, 12, romanGold);
        FillRect(p, 48, 48, 15 - step, 44, 7, 3, romanBronze);
        FillRect(p, 48, 48, 25 + step, 44, 7, 3, romanBronze);

        // Senatorial Toga Praetexta (Pure White with Royal Tyrian Purple Border)
        FillRect(p, 48, 48, 15, 14, 18, 22, togaWhite);
        FillRect(p, 48, 48, 14, 15, 3, 21, imperialPurp); // Border stripe
        FillRect(p, 48, 48, 30, 15, 3, 21, imperialPurp);
        HLine(p, 48, 48, 14, 32, 35, imperialPurp);
        HLine(p, 48, 48, 14, 32, 36, romanGold); // Gold trim

        // Gilded Breastplate showing beneath toga drape
        FillRect(p, 48, 48, 19, 15, 10, 8, romanGoldHi);

        // Head, Clean-shaven Noble Roman Face & Golden Laurel Wreath Crown
        FillRect(p, 48, 48, 19, 6, 9, 9, skinBase);
        HLine(p, 48, 48, 17, 30, 6, romanGoldHi); // Laurel wreath
        Set(p, 48, 48, 16, 5, romanGold);
        Set(p, 48, 48, 31, 5, romanGold);
        FillRect(p, 48, 48, 19, 4, 9, 3, hairDark); // Roman crop hair
        Set(p, 48, 48, 25, 9, BLACK);               // Sharp eyes
        Set(p, 48, 48, 24, 11, skinDark);            // Roman aquiline nose

        // --- Animations ---
        if (animState == 3) {
            // Spatha Slash
            HLine(p, 48, 48, 26, 46, 20, romanSteel);
            HLine(p, 48, 48, 28, 44, 19, WHITE);
            FillRect(p, 48, 48, 24, 18, 4, 6, romanGold);
        } else if (animState == 4) {
            // Basin Handwash Splash
            FillRect(p, 48, 48, 28, 24, 14, 10, COLOR_BASIN_SILVER);
            FillRect(p, 48, 48, 30, 22, 10, 4, COLOR_BASIN_WATER);
            // Splashing water droplets
            Set(p, 48, 48, 32, 16, WHITE);
            Set(p, 48, 48, 36, 14, COLOR_BASIN_WATER);
            Set(p, 48, 48, 40, 17, WHITE);
            Set(p, 48, 48, 28, 18, COLOR_BASIN_WATER);
        } else if (animState == 5) {
            // Golden Stylus Beam Decree
            VLine(p, 48, 48, 32, 8, 24, romanGoldHi);
            FillRect(p, 48, 48, 30, 6, 5, 4, WHITE);
            // Stylus energy radiating
            Set(p, 48, 48, 28, 4, romanGoldHi);
            Set(p, 48, 48, 36, 4, romanGoldHi);
        } else if (animState == 6) {
            // Teleport Mirage
            for (auto& c : px) {
                if (c.a > 0) c = Color{ 220, 160, 255, 120 };
            }
        } else if (animState == 8) {
            // Kneel Handwash Defeat
            px.assign(48 * 48, BLANK);
            FillRect(p, 48, 48, 14, 26, 20, 16, togaWhite);
            FillRect(p, 48, 48, 16, 20, 10, 8, skinDark);
            HLine(p, 48, 48, 15, 27, 20, romanGold);
            // Hands frantically dipped in silver basin
            FillRect(p, 48, 48, 26, 24, 14, 12, COLOR_BASIN_SILVER);
            FillRect(p, 48, 48, 28, 22, 10, 5, COLOR_BASIN_WATER);
            FillRect(p, 48, 48, 24, 20, 6, 6, skinBase); // Hands
            // Overflowing water glow
            Set(p, 48, 48, 27, 37, COLOR_BASIN_WATER);
            Set(p, 48, 48, 38, 37, COLOR_BASIN_WATER);
        }

        if (animState == 7) {
            for (auto& c : px) if (c.a > 0) c = Color{ 255, 120, 120, c.a };
        }

        return GraphicsManager::CreateTextureFromPixels(48, 48, px.data());
    };

    GraphicsManager::SetSpriteTexture(SPR_PILATE_IDLE,         GenPilate(0));
    GraphicsManager::SetSpriteTexture(SPR_PILATE_WALK_0,       GenPilate(1));
    GraphicsManager::SetSpriteTexture(SPR_PILATE_WALK_1,       GenPilate(2));
    GraphicsManager::SetSpriteTexture(SPR_PILATE_SLASH,        GenPilate(3));
    GraphicsManager::SetSpriteTexture(SPR_PILATE_BASIN_SPLASH, GenPilate(4));
    GraphicsManager::SetSpriteTexture(SPR_PILATE_STYLUS_BEAM,  GenPilate(5));
    GraphicsManager::SetSpriteTexture(SPR_PILATE_TELEPORT,     GenPilate(6));
    GraphicsManager::SetSpriteTexture(SPR_PILATE_HURT,         GenPilate(7));
    GraphicsManager::SetSpriteTexture(SPR_PILATE_KNEEL_WASH,   GenPilate(8));

    // --- G. Mission 4 Projectiles & Pickups ---
    // 1. Water Droplet (8x8)
    {
        std::vector<Color> px(8 * 8, BLANK);
        Color* p = px.data();
        FillRect(p, 8, 8, 2, 2, 4, 4, COLOR_BASIN_WATER);
        Set(p, 8, 8, 3, 1, WHITE);
        Set(p, 8, 8, 4, 1, WHITE);
        Set(p, 8, 8, 3, 2, WHITE);
        GraphicsManager::SetSpriteTexture(SPR_WATER_DROPLET, GraphicsManager::CreateTextureFromPixels(8, 8, px.data()));
    }

    // 2. Golden Stylus Blade (12x6)
    {
        std::vector<Color> px(12 * 6, BLANK);
        Color* p = px.data();
        FillRect(p, 12, 6, 0, 1, 12, 4, romanGold);
        HLine(p, 12, 6, 2, 11, 2, romanGoldHi);
        HLine(p, 12, 6, 4, 10, 3, WHITE);
        GraphicsManager::SetSpriteTexture(SPR_GOLDEN_STYLUS_BLADE, GraphicsManager::CreateTextureFromPixels(12, 6, px.data()));
    }

    // 3. Stage 10 Pickup: Papyrus Decree Scroll (16x16)
    {
        std::vector<Color> px(16 * 16, BLANK);
        Color* p = px.data();
        FillRect(p, 16, 16, 3, 4, 10, 9, Color{ 235, 220, 180, 255 });
        OutlineRect(p, 16, 16, 3, 4, 10, 9, woodBrown);
        FillRect(p, 16, 16, 6, 7, 4, 4, romanCrimson); // Imperial wax seal
        Set(p, 16, 16, 7, 8, romanGoldHi);             // Eagle impression
        GraphicsManager::SetSpriteTexture(SPR_ROMAN_SCROLL, GraphicsManager::CreateTextureFromPixels(16, 16, px.data()));
    }

    // 4. Stage 11 Pickup: Imperial Caesar Eagle Seal (16x16)
    {
        std::vector<Color> px(16 * 16, BLANK);
        Color* p = px.data();
        // Purple Ribbon
        FillRect(p, 16, 16, 6, 1, 4, 5, imperialPurp);
        // Golden Medallion
        FillRect(p, 16, 16, 3, 5, 10, 10, romanGold);
        OutlineRect(p, 16, 16, 3, 5, 10, 10, romanGoldHi);
        // Roman Aquila Eagle emblem
        Set(p, 16, 16, 7, 7, romanRedDark);
        Set(p, 16, 16, 8, 7, romanRedDark);
        HLine(p, 16, 16, 5, 10, 9, romanRedDark); // Wings
        VLine(p, 16, 16, 7, 9, 12, romanRedDark); // Body & claws
        GraphicsManager::SetSpriteTexture(SPR_CAESAR_SEAL, GraphicsManager::CreateTextureFromPixels(16, 16, px.data()));
    }

    // 5. Stage 12 Pickup: Silver Pitcher / Ablution Ewer (16x16)
    {
        std::vector<Color> px(16 * 16, BLANK);
        Color* p = px.data();
        FillRect(p, 16, 16, 5, 5, 6, 9, COLOR_BASIN_SILVER);
        FillRect(p, 16, 16, 6, 7, 4, 6, COLOR_SILVER_SHEEN);
        FillRect(p, 16, 16, 3, 4, 3, 6, COLOR_SILVER_MID); // Handle
        FillRect(p, 16, 16, 10, 3, 3, 4, COLOR_SILVER_SHEEN); // Spout
        Set(p, 16, 16, 12, 2, COLOR_BASIN_WATER); // Droplet on lip
        GraphicsManager::SetSpriteTexture(SPR_PILATE_EWER, GraphicsManager::CreateTextureFromPixels(16, 16, px.data()));
    }

    // =========================================================================
    // 2. TILES GENERATION (16x16 each)
    // =========================================================================

    // Stage 10: Via Dolorosa
    {
        // TILE_DOLOROSA_STONE_SURFACE
        std::vector<Color> p0(16 * 16, COLOR_PRAETORIUM_STONE);
        HLine(p0.data(), 16, 16, 0, 15, 0, Color{ 215, 205, 195, 255 });
        HLine(p0.data(), 16, 16, 0, 15, 1, Color{ 195, 185, 175, 255 });
        VLine(p0.data(), 16, 16, 5, 2, 15, Color{ 145, 135, 120, 255 });
        VLine(p0.data(), 16, 16, 11, 2, 15, Color{ 145, 135, 120, 255 });
        GraphicsManager::SetTileTexture(TILE_DOLOROSA_STONE_SURFACE, GraphicsManager::CreateTextureFromPixels(16, 16, p0.data()));

        // TILE_DOLOROSA_STONE_SUB
        std::vector<Color> p1(16 * 16, Color{ 140, 130, 120, 255 });
        for (int y = 0; y < 16; y += 4) HLine(p1.data(), 16, 16, 0, 15, y, Color{ 110, 100, 90, 255 });
        GraphicsManager::SetTileTexture(TILE_DOLOROSA_STONE_SUB, GraphicsManager::CreateTextureFromPixels(16, 16, p1.data()));

        // TILE_DOLOROSA_BARRICADE
        std::vector<Color> p2(16 * 16, BLANK);
        FillRect(p2.data(), 16, 16, 1, 2, 14, 4, woodBrown);
        FillRect(p2.data(), 16, 16, 1, 10, 14, 4, woodBrown);
        VLine(p2.data(), 16, 16, 3, 0, 15, romanSteelDk);
        VLine(p2.data(), 16, 16, 12, 0, 15, romanSteelDk);
        GraphicsManager::SetTileTexture(TILE_DOLOROSA_BARRICADE, GraphicsManager::CreateTextureFromPixels(16, 16, p2.data()));

        // TILE_DOLOROSA_ARCH_ECCE
        std::vector<Color> p3(16 * 16, COLOR_PRAETORIUM_STONE);
        FillRect(p3.data(), 16, 16, 2, 0, 12, 4, Color{ 220, 210, 200, 255 });
        FillRect(p3.data(), 16, 16, 6, 1, 4, 3, romanGold); // Keystone medallion
        GraphicsManager::SetTileTexture(TILE_DOLOROSA_ARCH_ECCE, GraphicsManager::CreateTextureFromPixels(16, 16, p3.data()));

        // TILE_DOLOROSA_EAGLE_BANNER
        std::vector<Color> p4(16 * 16, BLANK);
        FillRect(p4.data(), 16, 16, 2, 0, 12, 15, romanCrimson);
        FillRect(p4.data(), 16, 16, 5, 3, 6, 6, romanGoldHi); // Golden Aquila
        HLine(p4.data(), 16, 16, 3, 12, 12, romanGold);
        GraphicsManager::SetTileTexture(TILE_DOLOROSA_EAGLE_BANNER, GraphicsManager::CreateTextureFromPixels(16, 16, p4.data()));

        // TILE_DOLOROSA_TORCH_BRACKET
        std::vector<Color> p5(16 * 16, BLANK);
        FillRect(p5.data(), 16, 16, 6, 8, 4, 8, romanSteelDk);
        FillRect(p5.data(), 16, 16, 5, 3, 6, 5, COLOR_TORCH_FIRE);
        FillRect(p5.data(), 16, 16, 6, 4, 4, 3, COLOR_TORCH_EMBER);
        GraphicsManager::SetTileTexture(TILE_DOLOROSA_TORCH_BRACKET, GraphicsManager::CreateTextureFromPixels(16, 16, p5.data()));

        // TILE_DOLOROSA_BALCONY
        std::vector<Color> p6(16 * 16, COLOR_PRAETORIUM_STONE);
        HLine(p6.data(), 16, 16, 0, 15, 0, romanGold);
        for (int x = 2; x < 15; x += 3) VLine(p6.data(), 16, 16, x, 2, 12, Color{ 140, 130, 120, 255 });
        GraphicsManager::SetTileTexture(TILE_DOLOROSA_BALCONY, GraphicsManager::CreateTextureFromPixels(16, 16, p6.data()));

        // TILE_DOLOROSA_GATE
        std::vector<Color> p7(16 * 16, Color{ 35, 30, 40, 255 });
        for (int x = 2; x < 16; x += 4) VLine(p7.data(), 16, 16, x, 0, 15, romanSteelDk);
        HLine(p7.data(), 16, 16, 0, 15, 4, romanSteelDk);
        HLine(p7.data(), 16, 16, 0, 15, 11, romanSteelDk);
        GraphicsManager::SetTileTexture(TILE_DOLOROSA_GATE, GraphicsManager::CreateTextureFromPixels(16, 16, p7.data()));
    }

    // Stage 11: The Gabbatha Courtroom (Lithostrotos Mosaic)
    {
        // TILE_COURT_MOSAIC_SURFACE
        std::vector<Color> p0(16 * 16, Color{ 225, 220, 210, 255 });
        // Classical Roman Checker Tesserae
        for (int y = 0; y < 16; ++y) {
            for (int x = 0; x < 16; ++x) {
                if ((x / 4 + y / 4) % 2 == 0) {
                    p0[y * 16 + x] = COLOR_COURT_MOSAIC_RED;
                } else {
                    p0[y * 16 + x] = COLOR_COURT_MOSAIC_GOLD;
                }
            }
        }
        HLine(p0.data(), 16, 16, 0, 15, 0, romanGoldHi);
        GraphicsManager::SetTileTexture(TILE_COURT_MOSAIC_SURFACE, GraphicsManager::CreateTextureFromPixels(16, 16, p0.data()));

        // TILE_COURT_MOSAIC_SUB
        std::vector<Color> p1(16 * 16, Color{ 65, 55, 65, 255 });
        for (int y = 0; y < 16; y += 4) HLine(p1.data(), 16, 16, 0, 15, y, Color{ 45, 38, 45, 255 });
        GraphicsManager::SetTileTexture(TILE_COURT_MOSAIC_SUB, GraphicsManager::CreateTextureFromPixels(16, 16, p1.data()));

        // TILE_COURT_BENCH_WOOD
        std::vector<Color> p2(16 * 16, BLANK);
        FillRect(p2.data(), 16, 16, 0, 4, 16, 5, woodBrown);
        HLine(p2.data(), 16, 16, 0, 15, 4, Color{ 175, 125, 75, 255 });
        FillRect(p2.data(), 16, 16, 2, 9, 3, 7, woodDark);
        FillRect(p2.data(), 16, 16, 11, 9, 3, 7, woodDark);
        GraphicsManager::SetTileTexture(TILE_COURT_BENCH_WOOD, GraphicsManager::CreateTextureFromPixels(16, 16, p2.data()));

        // TILE_COURT_BALUSTRADE
        std::vector<Color> p3(16 * 16, BLANK);
        FillRect(p3.data(), 16, 16, 0, 2, 16, 4, Color{ 230, 225, 220, 255 });
        FillRect(p3.data(), 16, 16, 0, 12, 16, 4, Color{ 200, 195, 190, 255 });
        for (int x = 2; x < 15; x += 4) FillRect(p3.data(), 16, 16, x, 6, 2, 6, Color{ 215, 210, 205, 255 });
        GraphicsManager::SetTileTexture(TILE_COURT_BALUSTRADE, GraphicsManager::CreateTextureFromPixels(16, 16, p3.data()));

        // TILE_COURT_COLUMN_HEAD
        std::vector<Color> p4(16 * 16, BLANK);
        FillRect(p4.data(), 16, 16, 1, 0, 14, 5, Color{ 240, 235, 230, 255 });
        FillRect(p4.data(), 16, 16, 2, 5, 12, 7, romanGold); // Acanthus leaf capital
        FillRect(p4.data(), 16, 16, 3, 12, 10, 4, Color{ 220, 215, 210, 255 });
        GraphicsManager::SetTileTexture(TILE_COURT_COLUMN_HEAD, GraphicsManager::CreateTextureFromPixels(16, 16, p4.data()));

        // TILE_COURT_COLUMN_MID
        std::vector<Color> p5(16 * 16, BLANK);
        FillRect(p5.data(), 16, 16, 3, 0, 10, 16, Color{ 225, 220, 215, 255 });
        for (int x = 5; x <= 11; x += 2) VLine(p5.data(), 16, 16, x, 0, 15, Color{ 180, 175, 170, 255 });
        GraphicsManager::SetTileTexture(TILE_COURT_COLUMN_MID, GraphicsManager::CreateTextureFromPixels(16, 16, p5.data()));

        // TILE_COURT_COLUMN_BASE
        std::vector<Color> p6(16 * 16, BLANK);
        FillRect(p6.data(), 16, 16, 3, 0, 10, 6, Color{ 225, 220, 215, 255 });
        FillRect(p6.data(), 16, 16, 2, 6, 12, 5, Color{ 200, 195, 190, 255 });
        FillRect(p6.data(), 16, 16, 1, 11, 14, 5, Color{ 175, 170, 165, 255 });
        GraphicsManager::SetTileTexture(TILE_COURT_COLUMN_BASE, GraphicsManager::CreateTextureFromPixels(16, 16, p6.data()));

        // TILE_COURT_EAGLE_PEDESTAL
        std::vector<Color> p7(16 * 16, BLANK);
        FillRect(p7.data(), 16, 16, 2, 8, 12, 8, Color{ 190, 185, 180, 255 });
        FillRect(p7.data(), 16, 16, 4, 1, 8, 7, romanBronze); // Bronze Aquila
        HLine(p7.data(), 16, 16, 3, 12, 3, romanGold);        // Wing spread
        GraphicsManager::SetTileTexture(TILE_COURT_EAGLE_PEDESTAL, GraphicsManager::CreateTextureFromPixels(16, 16, p7.data()));
    }

    // Stage 12: Praetorium Throne Room
    {
        // TILE_THRONE_MARBLE_SURFACE
        std::vector<Color> p0(16 * 16, Color{ 245, 240, 245, 255 });
        HLine(p0.data(), 16, 16, 0, 15, 0, romanGoldHi);
        HLine(p0.data(), 16, 16, 0, 15, 1, Color{ 230, 220, 235, 255 });
        // Purple marble veins
        Set(p0.data(), 16, 16, 3, 4, Color{ 180, 120, 180, 140 });
        Set(p0.data(), 16, 16, 7, 8, Color{ 180, 120, 180, 140 });
        Set(p0.data(), 16, 16, 12, 12, Color{ 180, 120, 180, 140 });
        GraphicsManager::SetTileTexture(TILE_THRONE_MARBLE_SURFACE, GraphicsManager::CreateTextureFromPixels(16, 16, p0.data()));

        // TILE_THRONE_MARBLE_SUB
        std::vector<Color> p1(16 * 16, Color{ 85, 45, 80, 255 });
        HLine(p1.data(), 16, 16, 0, 15, 0, romanGold);
        GraphicsManager::SetTileTexture(TILE_THRONE_MARBLE_SUB, GraphicsManager::CreateTextureFromPixels(16, 16, p1.data()));

        // TILE_THRONE_VELVET_CANOPY
        std::vector<Color> p2(16 * 16, imperialPurp);
        HLine(p2.data(), 16, 16, 0, 15, 0, romanGoldHi);
        HLine(p2.data(), 16, 16, 0, 15, 15, romanGold);
        // Golden tassels
        for (int x = 1; x < 15; x += 3) {
            Set(p2.data(), 16, 16, x, 14, romanGoldHi);
        }
        GraphicsManager::SetTileTexture(TILE_THRONE_VELVET_CANOPY, GraphicsManager::CreateTextureFromPixels(16, 16, p2.data()));

        // TILE_THRONE_BASIN_PEDESTAL
        std::vector<Color> p3(16 * 16, BLANK);
        FillRect(p3.data(), 16, 16, 4, 8, 8, 8, romanBronze);
        FillRect(p3.data(), 16, 16, 1, 2, 14, 6, COLOR_BASIN_SILVER);
        FillRect(p3.data(), 16, 16, 3, 3, 10, 3, COLOR_BASIN_WATER);
        GraphicsManager::SetTileTexture(TILE_THRONE_BASIN_PEDESTAL, GraphicsManager::CreateTextureFromPixels(16, 16, p3.data()));

        // TILE_THRONE_GOLD_EAGLE
        std::vector<Color> p4(16 * 16, BLANK);
        FillRect(p4.data(), 16, 16, 3, 2, 10, 12, romanGold);
        HLine(p4.data(), 16, 16, 1, 14, 4, romanGoldHi); // Outstretched wings
        HLine(p4.data(), 16, 16, 2, 13, 5, romanGoldHi);
        Set(p4.data(), 16, 16, 7, 1, romanGoldHi);       // Crown/head
        FillRect(p4.data(), 16, 16, 4, 12, 8, 3, romanBronze); // Wreath perch
        GraphicsManager::SetTileTexture(TILE_THRONE_GOLD_EAGLE, GraphicsManager::CreateTextureFromPixels(16, 16, p4.data()));

        // TILE_THRONE_PILLAR_ARCH
        std::vector<Color> p5(16 * 16, Color{ 235, 230, 235, 255 });
        FillRect(p5.data(), 16, 16, 0, 0, 16, 4, romanGold);
        FillRect(p5.data(), 16, 16, 2, 4, 12, 12, imperialPurp);
        GraphicsManager::SetTileTexture(TILE_THRONE_PILLAR_ARCH, GraphicsManager::CreateTextureFromPixels(16, 16, p5.data()));

        // TILE_THRONE_CANDELABRA
        std::vector<Color> p6(16 * 16, BLANK);
        VLine(p6.data(), 16, 16, 7, 4, 15, romanGold);
        HLine(p6.data(), 16, 16, 2, 13, 6, romanGold);
        for (int x = 2; x <= 13; x += 3) {
            Set(p6.data(), 16, 16, x, 4, COLOR_TORCH_FIRE);
            Set(p6.data(), 16, 16, x, 3, COLOR_TORCH_EMBER);
        }
        GraphicsManager::SetTileTexture(TILE_THRONE_CANDELABRA, GraphicsManager::CreateTextureFromPixels(16, 16, p6.data()));

        // TILE_THRONE_DAIS_STEP
        std::vector<Color> p7(16 * 16, Color{ 240, 235, 240, 255 });
        HLine(p7.data(), 16, 16, 0, 15, 0, romanGoldHi);
        HLine(p7.data(), 16, 16, 0, 15, 7, romanGold);
        HLine(p7.data(), 16, 16, 0, 15, 8, Color{ 220, 215, 220, 255 });
        HLine(p7.data(), 16, 16, 0, 15, 15, Color{ 170, 165, 170, 255 });
        GraphicsManager::SetTileTexture(TILE_THRONE_DAIS_STEP, GraphicsManager::CreateTextureFromPixels(16, 16, p7.data()));
    }

    // =========================================================================
    // 3. PARALLAX BACKGROUND TEXTURES (320x112 each)
    // =========================================================================

    // Stage 10: Via Dolorosa
    {
        // bgDolorosaSky: High dramatic noon sky with bronze/amber sun rays over Jerusalem
        std::vector<Color> pSky(320 * 112);
        for (int y = 0; y < 112; ++y) {
            float t = (float)y / 112.0f;
            Color skyCol = {
                static_cast<unsigned char>(180 + t * 45),
                static_cast<unsigned char>(140 + t * 50),
                static_cast<unsigned char>(100 + t * 35),
                255
            };
            for (int x = 0; x < 320; ++x) {
                pSky[y * 320 + x] = skyCol;
            }
        }
        // Distant fortress battlements on horizon
        for (int x = 0; x < 320; ++x) {
            int h = 18 + static_cast<int>(sinf(x * 0.05f) * 6.0f) + ((x % 30 < 10) ? 8 : 0);
            for (int y = 112 - h; y < 112; ++y) {
                pSky[y * 320 + x] = Color{ 135, 105, 80, 255 };
            }
        }
        bgDolorosaSky = GraphicsManager::CreateTextureFromPixels(320, 112, pSky.data());

        // bgDolorosaStreet: Roman fortress gates, guard balconies, standards
        std::vector<Color> pStreet(320 * 112, BLANK);
        for (int x = 0; x < 320; ++x) {
            int wallH = 48 + static_cast<int>(sinf(x * 0.04f) * 12.0f);
            for (int y = 112 - wallH; y < 112; ++y) {
                pStreet[y * 320 + x] = Color{ 165, 145, 130, 255 };
            }
            // Roman standards every 60 px
            if (x % 60 == 25) {
                for (int y = 112 - wallH - 18; y < 112; ++y) {
                    pStreet[y * 320 + x] = romanBronze;
                }
                for (int dx = -3; dx <= 3; ++dx) {
                    pStreet[(112 - wallH - 18) * 320 + (x + dx)] = romanGold;
                }
            }
        }
        bgDolorosaStreet = GraphicsManager::CreateTextureFromPixels(320, 112, pStreet.data());
    }

    // Stage 11: The Gabbatha Courtroom
    {
        // bgCourtSky: Arched Clerestory Windows into Roman Sky
        std::vector<Color> pSky(320 * 112);
        for (int y = 0; y < 112; ++y) {
            float t = (float)y / 112.0f;
            Color c = {
                static_cast<unsigned char>(70 + t * 40),
                static_cast<unsigned char>(60 + t * 45),
                static_cast<unsigned char>(95 + t * 65),
                255
            };
            for (int x = 0; x < 320; ++x) pSky[y * 320 + x] = c;
        }
        // Clerestory window arches
        for (int x = 20; x < 310; x += 45) {
            for (int dx = 0; dx < 22; ++dx) {
                int wx = x + dx;
                for (int wy = 12; wy < 65; ++wy) {
                    pSky[wy * 320 + wx] = Color{ 230, 235, 245, 255 }; // Sunlight streaming in
                }
            }
        }
        bgCourtSky = GraphicsManager::CreateTextureFromPixels(320, 112, pSky.data());

        // bgCourtBasilica: Colonnade Galleries & Purple Magistrate Banners
        std::vector<Color> pBasilica(320 * 112, BLANK);
        for (int x = 0; x < 320; ++x) {
            // Basilica upper gallery ledge
            for (int y = 60; y < 66; ++y) pBasilica[y * 320 + x] = Color{ 210, 205, 200, 255 };
            // Columns
            if (x % 36 < 6) {
                for (int y = 30; y < 112; ++y) pBasilica[y * 320 + x] = Color{ 225, 220, 215, 255 };
            }
            // Hanging imperial banners
            if (x % 72 >= 20 && x % 72 <= 32) {
                for (int y = 66; y < 98; ++y) pBasilica[y * 320 + x] = imperialPurp;
                pBasilica[98 * 320 + x] = romanGold;
            }
        }
        bgCourtBasilica = GraphicsManager::CreateTextureFromPixels(320, 112, pBasilica.data());
    }

    // Stage 12: Praetorium Throne Room
    {
        // bgThroneSky: Grand Palace Dome Skylight
        std::vector<Color> pSky(320 * 112);
        for (int y = 0; y < 112; ++y) {
            float t = (float)y / 112.0f;
            Color c = {
                static_cast<unsigned char>(45 + t * 45),
                static_cast<unsigned char>(25 + t * 30),
                static_cast<unsigned char>(65 + t * 50),
                255
            };
            for (int x = 0; x < 320; ++x) pSky[y * 320 + x] = c;
        }
        // Ornate coffered palace ceiling rib arches
        for (int x = 0; x < 320; x += 40) {
            for (int y = 0; y < 45; ++y) {
                pSky[y * 320 + x] = romanGold;
                pSky[y * 320 + x + 1] = Color{ 85, 25, 75, 255 };
            }
        }
        bgThroneSky = GraphicsManager::CreateTextureFromPixels(320, 112, pSky.data());

        // bgThronePalace: Imperial Throne Dais, Colossal Statues & Velvet Draperies
        std::vector<Color> pPalace(320 * 112, BLANK);
        for (int x = 0; x < 320; ++x) {
            // Massive Corinthian columns in background
            if (x % 50 < 10) {
                for (int y = 15; y < 112; ++y) pPalace[y * 320 + x] = Color{ 235, 230, 235, 255 };
            }
            // Rich imperial purple draperies
            if (x % 100 >= 30 && x % 100 <= 60) {
                for (int y = 25; y < 100; ++y) pPalace[y * 320 + x] = imperialPurp;
                pPalace[100 * 320 + x] = romanGoldHi;
            }
            // Caesar statues in alcoves
            if (x % 100 >= 40 && x % 100 <= 50) {
                for (int y = 45; y < 85; ++y) pPalace[y * 320 + x] = Color{ 245, 240, 240, 255 };
            }
        }
        bgThronePalace = GraphicsManager::CreateTextureFromPixels(320, 112, pPalace.data());
    }
}

