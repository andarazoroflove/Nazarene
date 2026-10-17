#include "Mission3Graphics.h"
#include "Constants.h"
#include <vector>
#include <cmath>
#include <algorithm>
#include <cstdlib>

namespace M3Pixel {
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

void Mission3Graphics::GenerateAll(
    Texture2D& bgBazaarSky,
    Texture2D& bgBazaarRoofs,
    Texture2D& bgCaiaphasSky,
    Texture2D& bgCaiaphasPalace,
    Texture2D& bgCenacleSky,
    Texture2D& bgCenacleChamber
) {
    using namespace M3Pixel;

    const int SW = 24, SH = 32;   // Standard enemy sprite size
    const int BW = 32, BH = 36;   // Boss sprite size
    const int TW = 16, TH = 16;   // Tile size
    const int PW = 16, PH = 16;   // Pickup item size

    // Color Palettes
    Color skinLight   = Color{ 232, 185, 142, 255 };
    Color skinDark    = Color{ 175, 125,  85, 255 };
    Color hairDark    = Color{  38,  26,  20, 255 };
    Color goldBright  = COLOR_GOLD_BRIGHT;
    Color goldMid     = Color{ 205, 155,  35, 255 };
    Color goldDark    = Color{ 130,  95,  20, 255 };
    Color leatherDark = Color{  68,  42,  24, 255 };
    Color ironGrey    = Color{ 140, 145, 155, 255 };
    Color ironHi      = Color{ 205, 215, 225, 255 };
    Color crimsonRob  = Color{ 180,  30,  35, 255 };
    Color whiteLinen  = Color{ 238, 238, 242, 255 };
    Color linenShade  = Color{ 175, 178, 188, 255 };
    Color purplePriest= COLOR_CAIAPHAS_PURPLE;
    Color purpleBright= Color{ 170,  45, 150, 255 };
    Color greenTunic  = COLOR_JUDAS_TUNIC;
    Color ochreMantle = COLOR_JUDAS_OCHRE;

    // =========================================================================
    // 1. TEMPLE MONEY CHANGER (Bazaar Enemy) - 24x32
    // Greedy merchant with turban, green robe, coin bag, and balance scale flail
    // =========================================================================
    auto DrawMoneyChangerBase = [&](Color* p, int frame, bool attacking, bool hurt) {
        for (int i = 0; i < SW * SH; ++i) p[i] = BLANK;

        int legOff = (frame == 1) ? 2 : 0;
        int headX = 11, headY = 7;

        // Turban & Merchant Beard
        FillCircle(p, SW, SH, headX, headY, 5, hurt ? Color{ 255, 100, 100, 255 } : skinLight);
        FillRect(p, SW, SH, headX - 5, headY - 6, 11, 4, Color{ 215, 175, 55, 255 }); // Saffron turban
        HLine(p, SW, SH, headX - 4, headX + 4, headY - 4, goldBright);
        FillRect(p, SW, SH, headX - 4, headY + 3, 9, 4, hairDark); // Thick beard
        Set(p, SW, SH, headX + 1, headY - 1, hairDark); // Beady eye

        // Rich merchant robe (striped forest green & ochre)
        int bodyY = headY + 5;
        for (int y = bodyY; y <= bodyY + 12; ++y) {
            for (int x = headX - 4; x <= headX + 4; ++x) {
                Color c = ((x + y) % 3 == 0) ? Color{ 190, 140, 30, 255 } : Color{ 45, 95, 55, 255 };
                Set(p, SW, SH, x, y, c);
            }
        }
        OutlineRect(p, SW, SH, headX - 4, bodyY, 9, 13, Color{ 25, 55, 30, 255 });
        // Heavy golden coin sash
        HLine(p, SW, SH, headX - 4, headX + 4, bodyY + 5, goldBright);
        FillRect(p, SW, SH, headX - 5, bodyY + 6, 4, 4, leatherDark); // Money satchel

        // Legs / Sandaled feet
        FillRect(p, SW, SH, headX - 3 - legOff, bodyY + 13, 3, 6, leatherDark);
        FillRect(p, SW, SH, headX + 1 + legOff, bodyY + 13, 3, 6, leatherDark);

        if (attacking) {
            // Extended arm swinging the brass balance scale
            HLine(p, SW, SH, headX + 3, headX + 10, bodyY + 2, skinLight);
            // Brass chain & balance beam
            Line(p, SW, SH, headX + 10, bodyY + 2, headX + 12, bodyY + 10, goldDark);
            HLine(p, SW, SH, headX + 9, headX + 15, bodyY + 10, goldBright);
            // Hanging scale pans with heavy coins
            FillCircle(p, SW, SH, headX + 9, bodyY + 13, 2, goldMid);
            FillCircle(p, SW, SH, headX + 15, bodyY + 13, 2, goldMid);
        } else {
            // Holding balance scale close to chest
            FillRect(p, SW, SH, headX + 3, bodyY + 2, 3, 5, skinLight);
            HLine(p, SW, SH, headX + 4, headX + 8, bodyY + 3, goldBright);
            FillCircle(p, SW, SH, headX + 5, bodyY + 6, 2, goldMid);
            FillCircle(p, SW, SH, headX + 8, bodyY + 6, 2, goldMid);
        }
    };

    std::vector<Color> sPix(SW * SH);
    DrawMoneyChangerBase(sPix.data(), 0, false, false);
    GraphicsManager::SetSpriteTexture(SPR_MONEY_CHANGER_WALK_0, GraphicsManager::CreateTextureFromPixels(SW, SH, sPix.data()));

    DrawMoneyChangerBase(sPix.data(), 1, false, false);
    GraphicsManager::SetSpriteTexture(SPR_MONEY_CHANGER_WALK_1, GraphicsManager::CreateTextureFromPixels(SW, SH, sPix.data()));

    DrawMoneyChangerBase(sPix.data(), 0, true, false);
    GraphicsManager::SetSpriteTexture(SPR_MONEY_CHANGER_ATTACK, GraphicsManager::CreateTextureFromPixels(SW, SH, sPix.data()));

    DrawMoneyChangerBase(sPix.data(), 0, false, true);
    GraphicsManager::SetSpriteTexture(SPR_MONEY_CHANGER_HURT, GraphicsManager::CreateTextureFromPixels(SW, SH, sPix.data()));

    // =========================================================================
    // 2. LEVITE ENFORCER (Palace of Caiaphas Enemy) - 24x32
    // Temple guard in linen vestments, bronze helm, with ceremonial poleaxe
    // =========================================================================
    auto DrawLeviteBase = [&](Color* p, int frame, bool attacking, bool hurt) {
        for (int i = 0; i < SW * SH; ++i) p[i] = BLANK;

        int legOff = (frame == 1) ? 2 : 0;
        int headX = 10, headY = 7;

        // Bronze Casque Helmet with blue crest
        FillCircle(p, SW, SH, headX, headY, 5, hurt ? Color{ 255, 100, 100, 255 } : skinLight);
        FillRect(p, SW, SH, headX - 4, headY - 6, 9, 6, goldMid);
        HLine(p, SW, SH, headX - 5, headX + 4, headY - 1, goldBright);
        VLine(p, SW, SH, headX - 1, headY - 9, headY - 6, Color{ 40, 95, 215, 255 }); // Blue crest

        // Linen tunic with sacred blue borders
        int bodyY = headY + 5;
        FillRect(p, SW, SH, headX - 4, bodyY, 9, 13, whiteLinen);
        HLine(p, SW, SH, headX - 4, headX + 4, bodyY + 12, Color{ 45, 95, 210, 255 }); // Blue hem
        FillRect(p, SW, SH, headX - 3, bodyY + 2, 7, 6, leatherDark); // Reinforced breast cuirass

        // Legs
        FillRect(p, SW, SH, headX - 3 - legOff, bodyY + 13, 3, 6, skinDark);
        FillRect(p, SW, SH, headX + 1 + legOff, bodyY + 13, 3, 6, skinDark);

        if (attacking) {
            // Forward thrust with ceremonial poleaxe
            HLine(p, SW, SH, headX + 2, headX + 12, bodyY + 3, skinLight);
            // Long wooden haft
            HLine(p, SW, SH, headX - 2, headX + 15, bodyY + 4, leatherDark);
            // Bronze crescent axe blade & spear tip
            FillRect(p, SW, SH, headX + 13, bodyY + 1, 3, 7, goldBright);
            Set(p, SW, SH, headX + 16, bodyY + 4, ironHi); // Spear point
        } else {
            // Poleaxe held upright
            VLine(p, SW, SH, headX + 4, 3, bodyY + 17, leatherDark);
            FillRect(p, SW, SH, headX + 3, 1, 4, 4, goldBright); // Axe head
            Set(p, SW, SH, headX + 4, 0, ironHi); // Spear top
            FillRect(p, SW, SH, headX + 2, bodyY + 4, 3, 4, skinLight);
        }
    };

    DrawLeviteBase(sPix.data(), 0, false, false);
    GraphicsManager::SetSpriteTexture(SPR_LEVITE_ENFORCER_WALK_0, GraphicsManager::CreateTextureFromPixels(SW, SH, sPix.data()));

    DrawLeviteBase(sPix.data(), 1, false, false);
    GraphicsManager::SetSpriteTexture(SPR_LEVITE_ENFORCER_WALK_1, GraphicsManager::CreateTextureFromPixels(SW, SH, sPix.data()));

    DrawLeviteBase(sPix.data(), 0, true, false);
    GraphicsManager::SetSpriteTexture(SPR_LEVITE_ENFORCER_ATTACK, GraphicsManager::CreateTextureFromPixels(SW, SH, sPix.data()));

    DrawLeviteBase(sPix.data(), 0, false, true);
    GraphicsManager::SetSpriteTexture(SPR_LEVITE_ENFORCER_HURT, GraphicsManager::CreateTextureFromPixels(SW, SH, sPix.data()));

    // =========================================================================
    // 3. BARABBAS THE ZEALOT (Stage 7 Boss) - 32x36
    // Muscular bandit outlaw, dual scimitars, battle-scarred crimson garb
    // =========================================================================
    auto DrawBarabbas = [&](Color* p, int pose) {
        // pose: 0=idle, 1=walk0, 2=walk1, 3=slash, 4=slam, 5=hurt, 6=kneel
        for (int i = 0; i < BW * BH; ++i) p[i] = BLANK;

        int cx = 15, cy = 10;
        if (pose == 6) cy += 7; // Kneeling

        bool hurt = (pose == 5);
        Color bSkin = hurt ? Color{ 255, 90, 90, 255 } : Color{ 195, 140, 95, 255 };

        // Head, wild hair, scars, dark beard
        FillCircle(p, BW, BH, cx, cy, 6, bSkin);
        FillRect(p, BW, BH, cx - 6, cy - 7, 13, 5, hairDark); // Messy wild hair
        FillRect(p, BW, BH, cx - 5, cy + 2, 11, 6, hairDark); // Thick zealot beard
        HLine(p, BW, BH, cx - 5, cx + 5, cy - 3, crimsonRob); // Crimson bandana
        Set(p, BW, BH, cx + 2, cy - 1, Color{ 255, 40, 40, 255 }); // Fierce eye
        Line(p, BW, BH, cx + 1, cy - 3, cx + 3, cy + 1, Color{ 140, 40, 40, 255 }); // Facial scar

        // Tattered crimson zealot cloak & muscular bare chest/leather straps
        int by = cy + 6;
        if (pose != 6) {
            FillRect(p, BW, BH, cx - 6, by, 13, 14, bSkin);
            FillRect(p, BW, BH, cx - 7, by, 3, 15, crimsonRob); // Cape shoulder
            // Leather cross-belts & belt
            Line(p, BW, BH, cx - 5, by + 1, cx + 5, by + 9, leatherDark);
            Line(p, BW, BH, cx + 5, by + 1, cx - 5, by + 9, leatherDark);
            FillRect(p, BW, BH, cx - 6, by + 9, 13, 3, leatherDark);
            Set(p, BW, BH, cx, by + 10, goldBright); // Belt buckle
            // Tattered combat skirt
            FillRect(p, BW, BH, cx - 5, by + 12, 11, 4, crimsonRob);
        } else {
            // Kneeling pose
            FillRect(p, BW, BH, cx - 6, by, 13, 10, crimsonRob);
            FillRect(p, BW, BH, cx - 7, by + 8, 14, 4, leatherDark);
        }

        // Legs / Movement
        if (pose == 1) { // Walk 0
            FillRect(p, BW, BH, cx - 5, by + 16, 4, 8, leatherDark);
            FillRect(p, BW, BH, cx + 2, by + 14, 4, 8, leatherDark);
        } else if (pose == 2) { // Walk 1
            FillRect(p, BW, BH, cx - 3, by + 14, 4, 8, leatherDark);
            FillRect(p, BW, BH, cx + 3, by + 16, 4, 8, leatherDark);
        } else if (pose != 6) {
            FillRect(p, BW, BH, cx - 5, by + 15, 4, 9, leatherDark);
            FillRect(p, BW, BH, cx + 2, by + 15, 4, 9, leatherDark);
        }

        // Dual Scimitars & Arms
        if (pose == 3) {
            // Forward Vicious Double Slash
            FillRect(p, BW, BH, cx + 3, by + 2, 8, 4, bSkin);
            // Right Scimitar sweeping forward
            Line(p, BW, BH, cx + 10, by + 3, cx + 18, by - 4, ironHi);
            Line(p, BW, BH, cx + 18, by - 4, cx + 23, by + 2, ironGrey);
            // Left Scimitar slashing low
            Line(p, BW, BH, cx + 6, by + 5, cx + 16, by + 10, ironHi);
            Line(p, BW, BH, cx + 16, by + 10, cx + 20, by + 6, ironGrey);
        } else if (pose == 4) {
            // Ground Slam / Jump Impact
            FillRect(p, BW, BH, cx - 2, by - 3, 5, 5, bSkin);
            // Both blades raised overhead and driving downward
            Line(p, BW, BH, cx + 2, by - 4, cx + 8, by - 12, ironHi);
            Line(p, BW, BH, cx + 5, by - 12, cx + 9, by - 6, ironGrey);
            Line(p, BW, BH, cx - 2, by - 4, cx - 6, by - 11, ironHi);
        } else if (pose == 6) {
            // Defeated / Kneeling - dropped scimitars on ground
            HLine(p, BW, BH, cx + 2, cx + 12, BH - 2, ironGrey);
            HLine(p, BW, BH, cx - 12, cx - 2, BH - 2, ironGrey);
        } else {
            // Idle / Walk blades ready at sides
            Line(p, BW, BH, cx - 6, by + 4, cx - 12, by + 12, ironHi);
            Line(p, BW, BH, cx + 6, by + 4, cx + 13, by + 11, ironHi);
        }
    };

    std::vector<Color> bPix(BW * BH);
    DrawBarabbas(bPix.data(), 0);
    GraphicsManager::SetSpriteTexture(SPR_BARABBAS_IDLE, GraphicsManager::CreateTextureFromPixels(BW, BH, bPix.data()));

    DrawBarabbas(bPix.data(), 1);
    GraphicsManager::SetSpriteTexture(SPR_BARABBAS_WALK_0, GraphicsManager::CreateTextureFromPixels(BW, BH, bPix.data()));

    DrawBarabbas(bPix.data(), 2);
    GraphicsManager::SetSpriteTexture(SPR_BARABBAS_WALK_1, GraphicsManager::CreateTextureFromPixels(BW, BH, bPix.data()));

    DrawBarabbas(bPix.data(), 3);
    GraphicsManager::SetSpriteTexture(SPR_BARABBAS_SLASH, GraphicsManager::CreateTextureFromPixels(BW, BH, bPix.data()));

    DrawBarabbas(bPix.data(), 4);
    GraphicsManager::SetSpriteTexture(SPR_BARABBAS_SLAM, GraphicsManager::CreateTextureFromPixels(BW, BH, bPix.data()));

    DrawBarabbas(bPix.data(), 5);
    GraphicsManager::SetSpriteTexture(SPR_BARABBAS_HURT, GraphicsManager::CreateTextureFromPixels(BW, BH, bPix.data()));

    DrawBarabbas(bPix.data(), 6);
    GraphicsManager::SetSpriteTexture(SPR_BARABBAS_KNEEL, GraphicsManager::CreateTextureFromPixels(BW, BH, bPix.data()));

    // =========================================================================
    // 4. HIGH PRIEST CAIAPHAS (Stage 8 Boss) - 32x36
    // Opulent purple & gold vestments, Ephod 12-gem breastplate, mitre, gold staff
    // =========================================================================
    auto DrawCaiaphas = [&](Color* p, int pose) {
        // pose: 0=stand, 1=cast, 2=hurt, 3=kneel
        for (int i = 0; i < BW * BH; ++i) p[i] = BLANK;

        int cx = 15, cy = 9;
        if (pose == 3) cy += 7;

        bool hurt = (pose == 2);
        Color cSkin = hurt ? Color{ 255, 80, 80, 255 } : skinLight;

        // Head, silver beard, High Priest Mitre (tall white & gold turban)
        FillCircle(p, BW, BH, cx, cy, 5, cSkin);
        FillRect(p, BW, BH, cx - 4, cy + 2, 9, 6, Color{ 210, 215, 225, 255 }); // Silver beard
        // Tall Priest Mitre Turban
        FillRect(p, BW, BH, cx - 5, cy - 8, 11, 7, whiteLinen);
        OutlineRect(p, BW, BH, cx - 5, cy - 8, 11, 7, goldBright);
        HLine(p, BW, BH, cx - 4, cx + 4, cy - 2, goldBright); // Golden headplate inscription

        // Extravagant Robe of the High Priest (Purple with gold embroidery)
        int by = cy + 5;
        int robeW = (pose == 3) ? 14 : 12;
        FillRect(p, BW, BH, cx - robeW / 2, by, robeW, (pose == 3) ? 12 : 20, purplePriest);
        OutlineRect(p, BW, BH, cx - robeW / 2, by, robeW, (pose == 3) ? 12 : 20, goldMid);
        // Hem pomegranates & gold bells
        for (int x = cx - robeW / 2; x <= cx + robeW / 2; x += 2) {
            Set(p, BW, BH, x, by + ((pose == 3) ? 11 : 19), goldBright);
        }

        // The Hoshen: Breastplate of Judgment (Golden plate with 12 radiant gems)
        FillRect(p, BW, BH, cx - 4, by + 2, 9, 7, goldBright);
        OutlineRect(p, BW, BH, cx - 4, by + 2, 9, 7, goldDark);
        // 12 precious stones (4 rows of 3)
        Color gems[12] = {
            Color{ 255, 40, 40, 255 },  Color{ 255, 220, 40, 255 }, Color{ 50, 220, 50, 255 },
            Color{ 40, 180, 255, 255 }, Color{ 40, 60, 255, 255 },  Color{ 240, 240, 255, 255 },
            Color{ 255, 140, 20, 255 }, Color{ 220, 200, 160, 255 },Color{ 180, 40, 220, 255 },
            Color{ 40, 220, 190, 255 }, Color{ 230, 230, 240, 255 },Color{ 210, 50, 100, 255 }
        };
        for (int gy = 0; gy < 4; ++gy) {
            for (int gx = 0; gx < 3; ++gx) {
                Set(p, BW, BH, cx - 3 + gx * 3, by + 3 + gy, gems[gy * 3 + gx]);
            }
        }

        // Staff & Casting Arms
        if (pose == 1) { // Cast Wrathful Pillar
            // Arms raised high holding golden staff aloft
            FillRect(p, BW, BH, cx + 5, by - 4, 3, 7, cSkin);
            VLine(p, BW, BH, cx + 8, 2, by + 18, goldBright);
            FillCircle(p, BW, BH, cx + 8, 3, 3, Color{ 255, 245, 120, 255 }); // Glowing orb crown
            // Holy spark aura around hands
            Set(p, BW, BH, cx - 3, by - 2, goldBright);
            Set(p, BW, BH, cx + 11, by - 2, goldBright);
        } else if (pose != 3) {
            // Standing with staff planted
            VLine(p, BW, BH, cx + 7, 6, by + 20, goldMid);
            FillCircle(p, BW, BH, cx + 7, 7, 2, goldBright);
            FillRect(p, BW, BH, cx + 5, by + 4, 3, 4, cSkin);
        }
    };

    DrawCaiaphas(bPix.data(), 0);
    GraphicsManager::SetSpriteTexture(SPR_CAIAPHAS_STAND, GraphicsManager::CreateTextureFromPixels(BW, BH, bPix.data()));

    DrawCaiaphas(bPix.data(), 1);
    GraphicsManager::SetSpriteTexture(SPR_CAIAPHAS_CAST, GraphicsManager::CreateTextureFromPixels(BW, BH, bPix.data()));

    DrawCaiaphas(bPix.data(), 2);
    GraphicsManager::SetSpriteTexture(SPR_CAIAPHAS_HURT, GraphicsManager::CreateTextureFromPixels(BW, BH, bPix.data()));

    DrawCaiaphas(bPix.data(), 3);
    GraphicsManager::SetSpriteTexture(SPR_CAIAPHAS_KNEEL, GraphicsManager::CreateTextureFromPixels(BW, BH, bPix.data()));

    // Caiaphas Lightning Bolt Projectile (16x32)
    std::vector<Color> lightPix(16 * 32, BLANK);
    for (int y = 0; y < 32; ++y) {
        int lx = 8 + static_cast<int>(sinf(y * 0.4f) * 3.0f);
        M3Pixel::FillCircle(lightPix.data(), 16, 32, lx, y, 2, Color{ 255, 255, 220, 240 });
        M3Pixel::Set(lightPix.data(), 16, 32, lx, y, WHITE);
    }
    GraphicsManager::SetSpriteTexture(SPR_CAIAPHAS_LIGHTNING, GraphicsManager::CreateTextureFromPixels(16, 32, lightPix.data()));

    // =========================================================================
    // 5. JUDAS ISCARIOT (Stage 9 Final Boss) - 32x36
    // The Traitor: dark cowl, ochre mantle, coin purse, sica dagger, silver scatter
    // =========================================================================
    auto DrawJudas = [&](Color* p, int pose) {
        // pose: 0=idle, 1=walk0, 2=walk1, 3=toss silver, 4=dash, 5=stab, 6=hurt, 7=kneel
        for (int i = 0; i < BW * BH; ++i) p[i] = BLANK;

        int cx = 15, cy = 10;
        if (pose == 7) cy += 7; // Kneeling remorse

        bool hurt = (pose == 6);
        Color jSkin = hurt ? Color{ 255, 75, 75, 255 } : Color{ 215, 170, 130, 255 };

        // Dark Hood / Cowl casting deep shadow over eyes
        FillCircle(p, BW, BH, cx, cy, 6, greenTunic);
        FillRect(p, BW, BH, cx - 4, cy - 2, 9, 6, Color{ 25, 28, 22, 255 }); // Shadowed face
        FillCircle(p, BW, BH, cx, cy + 1, 3, jSkin); // Glimpse of face
        FillRect(p, BW, BH, cx - 3, cy + 3, 7, 4, hairDark); // Dark beard
        // Haunting yellow/amber eyes in shadow
        Set(p, BW, BH, cx + 1, cy, Color{ 255, 215, 60, 255 });
        Set(p, BW, BH, cx - 2, cy, Color{ 255, 215, 60, 255 });

        // Robes: Dark olive-drab tunic with worn ochre pilgrim mantle
        int by = cy + 6;
        if (pose == 4) {
            // Shadow Dash (stretched aerodynamic forward lean)
            FillRect(p, BW, BH, cx - 10, by, 20, 11, ochreMantle);
            OutlineRect(p, BW, BH, cx - 10, by, 20, 11, greenTunic);
            // Motion blur ghost trails
            HLine(p, BW, BH, cx - 14, cx - 9, by + 4, Color{ 140, 110, 40, 160 });
            HLine(p, BW, BH, cx - 16, cx - 11, by + 7, Color{ 60, 70, 50, 120 });
        } else if (pose == 7) {
            // Kneeling remorse
            FillRect(p, BW, BH, cx - 6, by, 13, 11, ochreMantle);
            // Dropped bag of silver spilled on ground
            FillCircle(p, BW, BH, cx + 6, BH - 3, 3, leatherDark);
            // Scattered 30 pieces of silver shining on the floor!
            for (int sx = cx - 10; sx <= cx + 14; sx += 3) {
                Set(p, BW, BH, sx, BH - 2, COLOR_SILVER_SHEEN);
                Set(p, BW, BH, sx + 1, BH - 1, COLOR_SILVER_MID);
            }
        } else {
            // Standing / Walking / Attacking
            FillRect(p, BW, BH, cx - 6, by, 13, 14, ochreMantle);
            FillRect(p, BW, BH, cx - 4, by + 1, 9, 13, greenTunic);
            OutlineRect(p, BW, BH, cx - 6, by, 13, 14, Color{ 40, 45, 30, 255 });
        }

        // Heavy leather purse filled with 30 silver shekels at waist
        if (pose != 7) {
            FillCircle(p, BW, BH, cx - 4, by + 8, 3, leatherDark);
            // Silver coins peeking through tied leather strings
            Set(p, BW, BH, cx - 4, by + 6, COLOR_SILVER_SHEEN);
            Set(p, BW, BH, cx - 3, by + 6, COLOR_SILVER_MID);
        }

        // Legs / Movement
        if (pose == 1) { // Walk 0
            FillRect(p, BW, BH, cx - 5, by + 15, 3, 8, leatherDark);
            FillRect(p, BW, BH, cx + 2, by + 13, 3, 8, leatherDark);
        } else if (pose == 2) { // Walk 1
            FillRect(p, BW, BH, cx - 3, by + 13, 3, 8, leatherDark);
            FillRect(p, BW, BH, cx + 3, by + 15, 3, 8, leatherDark);
        } else if (pose != 4 && pose != 7) {
            FillRect(p, BW, BH, cx - 4, by + 15, 3, 9, leatherDark);
            FillRect(p, BW, BH, cx + 2, by + 15, 3, 9, leatherDark);
        }

        // Attacks
        if (pose == 3) {
            // TOSS 30 PIECES OF SILVER: Arm flung forward scattering shining silver coins
            HLine(p, BW, BH, cx + 2, cx + 11, by + 2, jSkin);
            FillCircle(p, BW, BH, cx + 12, by + 2, 3, COLOR_SILVER_SHEEN);
            // Burst of glinting silver pieces around hand
            Set(p, BW, BH, cx + 14, by - 2, COLOR_SILVER_SHEEN);
            Set(p, BW, BH, cx + 15, by + 5, COLOR_SILVER_SHEEN);
            Set(p, BW, BH, cx + 11, by + 7, COLOR_SILVER_MID);
        } else if (pose == 5) {
            // STAB ATTACK: Concealed bronze/iron curved sica blade lunging forward
            FillRect(p, BW, BH, cx + 4, by + 3, 8, 3, jSkin);
            Line(p, BW, BH, cx + 11, by + 4, cx + 18, by + 4, ironHi);
            Set(p, BW, BH, cx + 19, by + 4, Color{ 255, 255, 255, 255 }); // Gleam on tip
        } else if (pose == 4) {
            // Dash blade forward
            Line(p, BW, BH, cx + 9, by + 4, cx + 16, by + 4, ironHi);
        }
    };

    DrawJudas(bPix.data(), 0);
    GraphicsManager::SetSpriteTexture(SPR_JUDAS_IDLE, GraphicsManager::CreateTextureFromPixels(BW, BH, bPix.data()));

    DrawJudas(bPix.data(), 1);
    GraphicsManager::SetSpriteTexture(SPR_JUDAS_WALK_0, GraphicsManager::CreateTextureFromPixels(BW, BH, bPix.data()));

    DrawJudas(bPix.data(), 2);
    GraphicsManager::SetSpriteTexture(SPR_JUDAS_WALK_1, GraphicsManager::CreateTextureFromPixels(BW, BH, bPix.data()));

    DrawJudas(bPix.data(), 3);
    GraphicsManager::SetSpriteTexture(SPR_JUDAS_TOSS_SILVER, GraphicsManager::CreateTextureFromPixels(BW, BH, bPix.data()));

    DrawJudas(bPix.data(), 4);
    GraphicsManager::SetSpriteTexture(SPR_JUDAS_DASH, GraphicsManager::CreateTextureFromPixels(BW, BH, bPix.data()));

    DrawJudas(bPix.data(), 5);
    GraphicsManager::SetSpriteTexture(SPR_JUDAS_STAB, GraphicsManager::CreateTextureFromPixels(BW, BH, bPix.data()));

    DrawJudas(bPix.data(), 6);
    GraphicsManager::SetSpriteTexture(SPR_JUDAS_HURT, GraphicsManager::CreateTextureFromPixels(BW, BH, bPix.data()));

    DrawJudas(bPix.data(), 7);
    GraphicsManager::SetSpriteTexture(SPR_JUDAS_KNEEL, GraphicsManager::CreateTextureFromPixels(BW, BH, bPix.data()));

    // =========================================================================
    // 6. MISSION 3 PROJECTILES & PICKUPS
    // =========================================================================
    // 6a. Glinting Silver Coin Projectile (8x8)
    std::vector<Color> coinPix(8 * 8, BLANK);
    FillCircle(coinPix.data(), 8, 8, 4, 4, 3, COLOR_SILVER_MID);
    OutlineCircle(coinPix.data(), 8, 8, 4, 4, 3, COLOR_SILVER_DARK);
    // Specular shine
    Set(coinPix.data(), 8, 8, 3, 2, COLOR_SILVER_SHEEN);
    Set(coinPix.data(), 8, 8, 4, 3, WHITE);
    GraphicsManager::SetSpriteTexture(SPR_SILVER_COIN_PROJECTILE, GraphicsManager::CreateTextureFromPixels(8, 8, coinPix.data()));

    // 6b. Blood Shockwave (16x12)
    std::vector<Color> shockPix(16 * 12, BLANK);
    for (int y = 0; y < 12; ++y) {
        float rad = 6.0f + y * 0.7f;
        int wSpan = static_cast<int>(rad);
        HLine(shockPix.data(), 16, 12, 8 - wSpan, 8 + wSpan, y, Color{ 190, 20, 30, static_cast<unsigned char>(180 - y * 12) });
    }
    GraphicsManager::SetSpriteTexture(SPR_BLOOD_SHOCKWAVE, GraphicsManager::CreateTextureFromPixels(16, 12, shockPix.data()));

    // 6c. Pickup: Bitter Herbs (Passover Maror) - 16x16
    std::vector<Color> pickPix(PW * PH, BLANK);
    FillCircle(pickPix.data(), PW, PH, 8, 9, 5, Color{ 45, 115, 45, 255 });
    // Bundle of leaves
    Line(pickPix.data(), PW, PH, 8, 9, 5, 3, Color{ 70, 165, 55, 255 });
    Line(pickPix.data(), PW, PH, 8, 9, 11, 3, Color{ 70, 165, 55, 255 });
    Line(pickPix.data(), PW, PH, 8, 9, 8, 2, Color{ 90, 195, 75, 255 });
    // Reed tie cord
    HLine(pickPix.data(), PW, PH, 5, 11, 10, Color{ 215, 175, 75, 255 });
    GraphicsManager::SetSpriteTexture(SPR_BITTER_HERBS, GraphicsManager::CreateTextureFromPixels(PW, PH, pickPix.data()));

    // 6d. Pickup: Passover Matzot Plate - 16x16
    for (int i = 0; i < PW * PH; ++i) pickPix[i] = BLANK;
    FillCircle(pickPix.data(), PW, PH, 8, 8, 6, Color{ 225, 200, 155, 255 });
    OutlineCircle(pickPix.data(), PW, PH, 8, 8, 6, Color{ 175, 140, 95, 255 });
    // Toasted perforations & bubbles
    for (int py = 5; py <= 11; py += 2) {
        for (int px = 5; px <= 11; px += 2) {
            Set(pickPix.data(), PW, PH, px, py, Color{ 135, 95, 55, 255 });
        }
    }
    GraphicsManager::SetSpriteTexture(SPR_PASSOVER_MATZOT, GraphicsManager::CreateTextureFromPixels(PW, PH, pickPix.data()));

    // 6e. Pickup: Holy Grail (Last Supper Communion Chalice) - 16x16
    for (int i = 0; i < PW * PH; ++i) pickPix[i] = BLANK;
    // Radiant Golden Chalice
    FillRect(pickPix.data(), PW, PH, 4, 3, 9, 6, goldBright);
    OutlineRect(pickPix.data(), PW, PH, 4, 3, 9, 6, goldDark);
    VLine(pickPix.data(), PW, PH, 8, 9, 12, goldBright); // Stem
    HLine(pickPix.data(), PW, PH, 5, 11, 13, goldDark);   // Base
    HLine(pickPix.data(), PW, PH, 4, 12, 14, goldBright); // Plinth
    // Ruby gems on cup
    Set(pickPix.data(), PW, PH, 6, 6, Color{ 255, 40, 60, 255 });
    Set(pickPix.data(), PW, PH, 8, 6, Color{ 255, 255, 255, 255 }); // Specular gleam
    Set(pickPix.data(), PW, PH, 10, 6, Color{ 255, 40, 60, 255 });
    GraphicsManager::SetSpriteTexture(SPR_HOLY_GRAIL, GraphicsManager::CreateTextureFromPixels(PW, PH, pickPix.data()));

    // =========================================================================
    // 7. TILES: STAGE 7 SILOAM BAZAAR TILES (16x16)
    // =========================================================================
    std::vector<Color> tPix(TW * TH);

    // TILE_BAZAAR_STONE_SURFACE: Warm Sandstone paving with mosaic borders
    for (int y = 0; y < TH; ++y) {
        for (int x = 0; x < TW; ++x) {
            Color c = ((x + y * 2) % 4 == 0) ? Color{ 215, 185, 140, 255 } : Color{ 190, 160, 115, 255 };
            if (y == 0) c = Color{ 240, 215, 175, 255 }; // Top highlight
            tPix[y * TW + x] = c;
        }
    }
    GraphicsManager::SetTileTexture(TILE_BAZAAR_STONE_SURFACE, GraphicsManager::CreateTextureFromPixels(TW, TH, tPix.data()));

    // TILE_BAZAAR_STONE_SUB: Rough chiseled Jerusalem limestone
    for (int y = 0; y < TH; ++y) {
        for (int x = 0; x < TW; ++x) {
            Color c = ((x * 3 + y) % 5 == 0) ? Color{ 145, 120, 85, 255 } : Color{ 165, 138, 100, 255 };
            tPix[y * TW + x] = c;
        }
    }
    GraphicsManager::SetTileTexture(TILE_BAZAAR_STONE_SUB, GraphicsManager::CreateTextureFromPixels(TW, TH, tPix.data()));

    // TILE_BAZAAR_AWNING_RED: Striped market awning canopy (crimson & cream)
    for (int y = 0; y < TH; ++y) {
        for (int x = 0; x < TW; ++x) {
            bool isStripe = ((x / 4) % 2 == 0);
            tPix[y * TW + x] = isStripe ? COLOR_BAZAAR_CANOPY : Color{ 245, 235, 210, 255 };
        }
    }
    HLine(tPix.data(), TW, TH, 0, TW - 1, TH - 1, Color{ 130, 30, 20, 255 });
    GraphicsManager::SetTileTexture(TILE_BAZAAR_AWNING_RED, GraphicsManager::CreateTextureFromPixels(TW, TH, tPix.data()));

    // TILE_BAZAAR_AWNING_GOLD: Striped market awning canopy (saffron gold & cream)
    for (int y = 0; y < TH; ++y) {
        for (int x = 0; x < TW; ++x) {
            bool isStripe = ((x / 4) % 2 == 0);
            tPix[y * TW + x] = isStripe ? Color{ 220, 160, 35, 255 } : Color{ 245, 235, 210, 255 };
        }
    }
    HLine(tPix.data(), TW, TH, 0, TW - 1, TH - 1, Color{ 160, 110, 20, 255 });
    GraphicsManager::SetTileTexture(TILE_BAZAAR_AWNING_GOLD, GraphicsManager::CreateTextureFromPixels(TW, TH, tPix.data()));

    // TILE_BAZAAR_SPICE_SACK: Burlap sack filled with bright spices
    for (int i = 0; i < TW * TH; ++i) tPix[i] = BLANK;
    FillRect(tPix.data(), TW, TH, 2, 5, 12, 11, Color{ 185, 145, 95, 255 });
    OutlineRect(tPix.data(), TW, TH, 2, 5, 12, 11, Color{ 130, 95, 55, 255 });
    FillRect(tPix.data(), TW, TH, 4, 3, 8, 4, COLOR_BAZAAR_SPICE); // Paprika / Turmeric mound
    GraphicsManager::SetTileTexture(TILE_BAZAAR_SPICE_SACK, GraphicsManager::CreateTextureFromPixels(TW, TH, tPix.data()));

    // TILE_BAZAAR_POTTERY: Stacked terracotta clay amphorae
    for (int i = 0; i < TW * TH; ++i) tPix[i] = BLANK;
    FillCircle(tPix.data(), TW, TH, 8, 10, 5, Color{ 195, 95, 55, 255 });
    FillRect(tPix.data(), TW, TH, 6, 3, 5, 4, Color{ 215, 115, 75, 255 });
    HLine(tPix.data(), TW, TH, 5, 11, 2, Color{ 150, 70, 35, 255 });
    GraphicsManager::SetTileTexture(TILE_BAZAAR_POTTERY, GraphicsManager::CreateTextureFromPixels(TW, TH, tPix.data()));

    // TILE_BAZAAR_LEDGE: Wooden merchant stall counter platform
    for (int i = 0; i < TW * TH; ++i) tPix[i] = BLANK;
    FillRect(tPix.data(), TW, TH, 0, 0, TW, 6, Color{ 155, 95, 45, 255 });
    HLine(tPix.data(), TW, TH, 0, TW - 1, 0, Color{ 195, 130, 75, 255 });
    HLine(tPix.data(), TW, TH, 0, TW - 1, 5, Color{ 105, 60, 25, 255 });
    GraphicsManager::SetTileTexture(TILE_BAZAAR_LEDGE, GraphicsManager::CreateTextureFromPixels(TW, TH, tPix.data()));

    // TILE_BAZAAR_LANTERN: Hanging brass bazaar lantern
    for (int i = 0; i < TW * TH; ++i) tPix[i] = BLANK;
    VLine(tPix.data(), TW, TH, 8, 0, 4, goldDark);
    FillRect(tPix.data(), TW, TH, 5, 5, 7, 8, goldBright);
    FillRect(tPix.data(), TW, TH, 6, 6, 5, 6, Color{ 255, 240, 140, 255 }); // Warm flame
    GraphicsManager::SetTileTexture(TILE_BAZAAR_LANTERN, GraphicsManager::CreateTextureFromPixels(TW, TH, tPix.data()));

    // =========================================================================
    // 8. TILES: STAGE 8 MOUNT ZION & PALACE OF CAIAPHAS (16x16)
    // =========================================================================
    // TILE_CAIAPHAS_MARBLE_SURFACE: Polished white & purple veined marble
    for (int y = 0; y < TH; ++y) {
        for (int x = 0; x < TW; ++x) {
            Color c = ((x + y) % 6 == 0) ? Color{ 205, 185, 215, 255 } : Color{ 235, 235, 242, 255 };
            if (y == 0) c = WHITE;
            tPix[y * TW + x] = c;
        }
    }
    GraphicsManager::SetTileTexture(TILE_CAIAPHAS_MARBLE_SURFACE, GraphicsManager::CreateTextureFromPixels(TW, TH, tPix.data()));

    // TILE_CAIAPHAS_MARBLE_SUB: Marble foundation block
    for (int y = 0; y < TH; ++y) {
        for (int x = 0; x < TW; ++x) {
            Color c = ((x * 2 + y) % 5 == 0) ? Color{ 185, 175, 195, 255 } : Color{ 210, 205, 220, 255 };
            tPix[y * TW + x] = c;
        }
    }
    GraphicsManager::SetTileTexture(TILE_CAIAPHAS_MARBLE_SUB, GraphicsManager::CreateTextureFromPixels(TW, TH, tPix.data()));

    // TILE_CAIAPHAS_BRAZIER: Ornate bronze tripod brazier with coals
    for (int i = 0; i < TW * TH; ++i) tPix[i] = BLANK;
    Line(tPix.data(), TW, TH, 4, 15, 7, 7, goldDark);
    Line(tPix.data(), TW, TH, 11, 15, 8, 7, goldDark);
    FillRect(tPix.data(), TW, TH, 4, 4, 8, 4, goldBright);
    FillCircle(tPix.data(), TW, TH, 8, 3, 2, Color{ 255, 120, 30, 255 }); // Burning fire
    GraphicsManager::SetTileTexture(TILE_CAIAPHAS_BRAZIER, GraphicsManager::CreateTextureFromPixels(TW, TH, tPix.data()));

    // TILE_CAIAPHAS_BALUSTRADE: Carved marble railing platform
    for (int i = 0; i < TW * TH; ++i) tPix[i] = BLANK;
    FillRect(tPix.data(), TW, TH, 0, 0, TW, 4, WHITE);
    HLine(tPix.data(), TW, TH, 0, TW - 1, 3, Color{ 185, 185, 200, 255 });
    // Balusters
    VLine(tPix.data(), TW, TH, 3, 4, 15, Color{ 220, 220, 230, 255 });
    VLine(tPix.data(), TW, TH, 7, 4, 15, Color{ 220, 220, 230, 255 });
    VLine(tPix.data(), TW, TH, 11, 4, 15, Color{ 220, 220, 230, 255 });
    VLine(tPix.data(), TW, TH, 15, 4, 15, Color{ 220, 220, 230, 255 });
    GraphicsManager::SetTileTexture(TILE_CAIAPHAS_BALUSTRADE, GraphicsManager::CreateTextureFromPixels(TW, TH, tPix.data()));

    // TILE_CAIAPHAS_COLUMN_HEAD: Corinthian acanthus capital
    for (int i = 0; i < TW * TH; ++i) tPix[i] = BLANK;
    FillRect(tPix.data(), TW, TH, 1, 0, 14, 5, goldBright);
    FillRect(tPix.data(), TW, TH, 3, 5, 10, 11, WHITE);
    OutlineRect(tPix.data(), TW, TH, 3, 5, 10, 11, Color{ 200, 200, 215, 255 });
    GraphicsManager::SetTileTexture(TILE_CAIAPHAS_COLUMN_HEAD, GraphicsManager::CreateTextureFromPixels(TW, TH, tPix.data()));

    // TILE_CAIAPHAS_COLUMN_MID: Fluted marble shaft
    for (int y = 0; y < TH; ++y) {
        for (int x = 0; x < TW; ++x) {
            Color c = (x >= 3 && x <= 12) ? (((x % 3) == 0) ? Color{ 200, 200, 215, 255 } : WHITE) : BLANK;
            tPix[y * TW + x] = c;
        }
    }
    GraphicsManager::SetTileTexture(TILE_CAIAPHAS_COLUMN_MID, GraphicsManager::CreateTextureFromPixels(TW, TH, tPix.data()));

    // TILE_CAIAPHAS_COLUMN_BASE: Molded plinth
    for (int i = 0; i < TW * TH; ++i) tPix[i] = BLANK;
    FillRect(tPix.data(), TW, TH, 3, 0, 10, 8, WHITE);
    FillRect(tPix.data(), TW, TH, 1, 8, 14, 8, Color{ 215, 215, 225, 255 });
    OutlineRect(tPix.data(), TW, TH, 1, 8, 14, 8, goldDark);
    GraphicsManager::SetTileTexture(TILE_CAIAPHAS_COLUMN_BASE, GraphicsManager::CreateTextureFromPixels(TW, TH, tPix.data()));

    // TILE_CAIAPHAS_GATE: Council heavy bronze gate
    for (int y = 0; y < TH; ++y) {
        for (int x = 0; x < TW; ++x) {
            bool bar = (x % 4 == 0) || (y % 8 == 0);
            tPix[y * TW + x] = bar ? goldBright : Color{ 55, 30, 20, 255 };
        }
    }
    GraphicsManager::SetTileTexture(TILE_CAIAPHAS_GATE, GraphicsManager::CreateTextureFromPixels(TW, TH, tPix.data()));

    // =========================================================================
    // 9. TILES: STAGE 9 THE UPPER ROOM (CENACLE / LAST SUPPER) - 16x16
    // =========================================================================
    // TILE_CENACLE_WOOD_FLOOR: Rich polished cedar planks
    for (int y = 0; y < TH; ++y) {
        for (int x = 0; x < TW; ++x) {
            Color c = ((x + y * 3) % 4 == 0) ? Color{ 135, 80, 45, 255 } : COLOR_CENACLE_BEAM;
            if (y == 0) c = Color{ 165, 105, 60, 255 }; // Top sheen
            if (y == 15) c = Color{ 55, 35, 20, 255 };   // Plank groove
            tPix[y * TW + x] = c;
        }
    }
    GraphicsManager::SetTileTexture(TILE_CENACLE_WOOD_FLOOR, GraphicsManager::CreateTextureFromPixels(TW, TH, tPix.data()));

    // TILE_CENACLE_WOOD_SUB: Heavy timber foundation
    for (int y = 0; y < TH; ++y) {
        for (int x = 0; x < TW; ++x) {
            Color c = ((x * 2 + y) % 3 == 0) ? Color{ 70, 42, 25, 255 } : Color{ 85, 52, 30, 255 };
            tPix[y * TW + x] = c;
        }
    }
    GraphicsManager::SetTileTexture(TILE_CENACLE_WOOD_SUB, GraphicsManager::CreateTextureFromPixels(TW, TH, tPix.data()));

    // TILE_CENACLE_TABLE_CLOTH: Communion table draped in fine white linen
    for (int y = 0; y < TH; ++y) {
        for (int x = 0; x < TW; ++x) {
            Color c = whiteLinen;
            if (y == 0) c = goldBright; // Gold woven trim
            if (y >= 10 && (x % 3 == 0)) c = linenShade; // Draped folds
            tPix[y * TW + x] = c;
        }
    }
    GraphicsManager::SetTileTexture(TILE_CENACLE_TABLE_CLOTH, GraphicsManager::CreateTextureFromPixels(TW, TH, tPix.data()));

    // TILE_CENACLE_CHALICE_ROW: Table top with chalices & bread loaves
    for (int i = 0; i < TW * TH; ++i) tPix[i] = BLANK;
    FillRect(tPix.data(), TW, TH, 0, 10, TW, 6, whiteLinen);
    HLine(tPix.data(), TW, TH, 0, TW - 1, 10, goldBright);
    // Silver & Gold chalices
    FillRect(tPix.data(), TW, TH, 3, 5, 3, 5, goldBright);
    FillRect(tPix.data(), TW, TH, 10, 5, 3, 5, COLOR_SILVER_SHEEN);
    GraphicsManager::SetTileTexture(TILE_CENACLE_CHALICE_ROW, GraphicsManager::CreateTextureFromPixels(TW, TH, tPix.data()));

    // TILE_CENACLE_BEAM_ARCH: Vaulted cedar timber beam arch
    for (int y = 0; y < TH; ++y) {
        for (int x = 0; x < TW; ++x) {
            Color c = ((x + y) % 4 == 0) ? Color{ 110, 68, 38, 255 } : COLOR_CENACLE_BEAM;
            tPix[y * TW + x] = c;
        }
    }
    GraphicsManager::SetTileTexture(TILE_CENACLE_BEAM_ARCH, GraphicsManager::CreateTextureFromPixels(TW, TH, tPix.data()));

    // TILE_CENACLE_CANDELABRA: Seven-branch menorah with glowing candles
    for (int i = 0; i < TW * TH; ++i) tPix[i] = BLANK;
    HLine(tPix.data(), TW, TH, 2, 13, 8, goldBright);
    VLine(tPix.data(), TW, TH, 8, 8, 15, goldBright);
    for (int cx = 2; cx <= 13; cx += 2) {
        VLine(tPix.data(), TW, TH, cx, 6, 8, goldMid);
        Set(tPix.data(), TW, TH, cx, 4, COLOR_CENACLE_CANDLE); // Candle flame
    }
    GraphicsManager::SetTileTexture(TILE_CENACLE_CANDELABRA, GraphicsManager::CreateTextureFromPixels(TW, TH, tPix.data()));

    // TILE_CENACLE_WINDOW_MOON: Stone arched window looking out at Passover moon
    for (int y = 0; y < TH; ++y) {
        for (int x = 0; x < TW; ++x) {
            bool isArch = (y < 4 && (x < 2 || x > 13));
            if (isArch) {
                tPix[y * TW + x] = Color{ 75, 65, 80, 255 }; // Stone frame
            } else {
                tPix[y * TW + x] = Color{ 22, 28, 55, 255 }; // Midnight sky
            }
        }
    }
    FillCircle(tPix.data(), TW, TH, 8, 8, 4, Color{ 255, 255, 220, 255 }); // Full Passover Moon
    GraphicsManager::SetTileTexture(TILE_CENACLE_WINDOW_MOON, GraphicsManager::CreateTextureFromPixels(TW, TH, tPix.data()));

    // TILE_CENACLE_STONE_WALL: Ancient quiet ashlar stone wall
    for (int y = 0; y < TH; ++y) {
        for (int x = 0; x < TW; ++x) {
            Color c = ((x + y * 2) % 5 == 0) ? Color{ 85, 75, 90, 255 } : Color{ 65, 55, 70, 255 };
            if (y % 8 == 0 || (x + (y / 8) * 8) % 16 == 0) c = Color{ 40, 35, 48, 255 }; // Mortar joints
            tPix[y * TW + x] = c;
        }
    }
    GraphicsManager::SetTileTexture(TILE_CENACLE_STONE_WALL, GraphicsManager::CreateTextureFromPixels(TW, TH, tPix.data()));

    // =========================================================================
    // 10. BACKGROUND PROCEDURAL TEXTURES (6 TEXTURES)
    // =========================================================================
    const int SKY_W = 320, SKY_H = 224;
    const int PAR_W = 480, PAR_H = 190;

    // 10a. STAGE 7 BAZAAR SKY (Golden Sunset transitioning into deep twilight)
    std::vector<Color> bzSky(SKY_W * SKY_H);
    for (int y = 0; y < SKY_H; ++y) {
        float f = y / (float)SKY_H;
        Color skyCol;
        if (f < 0.4f) {
            float t = f / 0.4f;
            skyCol = Color{ static_cast<unsigned char>(45 + t * 90), static_cast<unsigned char>(25 + t * 45), static_cast<unsigned char>(75 + t * 40), 255 };
        } else {
            float t = (f - 0.4f) / 0.6f;
            skyCol = Color{ static_cast<unsigned char>(135 + t * 110), static_cast<unsigned char>(70 + t * 75), static_cast<unsigned char>(115 - t * 65), 255 };
        }
        for (int x = 0; x < SKY_W; ++x) bzSky[y * SKY_W + x] = skyCol;
    }
    bgBazaarSky = GraphicsManager::CreateTextureFromPixels(SKY_W, SKY_H, bzSky.data());

    // 10b. STAGE 7 BAZAAR ROOFS PARALLAX (Domes, arches, minarets, cloth canopies)
    std::vector<Color> bzRoofs(PAR_W * PAR_H, BLANK);
    for (int x = 0; x < PAR_W; ++x) {
        int baseY = 110 + static_cast<int>(sinf(x * 0.03f) * 12.0f);
        // Roof structures & domes
        int domeX = x % 120;
        if (domeX >= 40 && domeX <= 80) {
            float dx = (domeX - 60) / 20.0f;
            int domeTop = baseY - 28 + static_cast<int>(dx * dx * 16.0f);
            for (int y = domeTop; y < PAR_H; ++y) {
                bzRoofs[y * PAR_W + x] = (y < baseY) ? Color{ 175, 125, 75, 255 } : Color{ 135, 95, 55, 255 };
            }
        } else {
            for (int y = baseY; y < PAR_H; ++y) {
                bzRoofs[y * PAR_W + x] = Color{ 145, 105, 65, 255 };
            }
        }
        // Palm trees silhouette in distance
        if (x % 160 == 25) {
            for (int ty = baseY - 45; ty <= baseY; ++ty) bzRoofs[ty * PAR_W + x] = Color{ 60, 45, 25, 255 };
            for (int lx = -12; lx <= 12; ++lx) {
                int px = x + lx;
                if (px >= 0 && px < PAR_W) {
                    bzRoofs[(baseY - 45 + abs(lx) / 2) * PAR_W + px] = Color{ 45, 80, 40, 255 };
                }
            }
        }
    }
    bgBazaarRoofs = GraphicsManager::CreateTextureFromPixels(PAR_W, PAR_H, bzRoofs.data());

    // 10c. STAGE 8 CAIAPHAS SKY (Brooding twilight purple over Mount Zion)
    std::vector<Color> caSky(SKY_W * SKY_H);
    for (int y = 0; y < SKY_H; ++y) {
        float f = y / (float)SKY_H;
        Color skyCol = {
            static_cast<unsigned char>(35 + f * 55),
            static_cast<unsigned char>(20 + f * 25),
            static_cast<unsigned char>(65 + f * 45),
            255
        };
        for (int x = 0; x < SKY_W; ++x) caSky[y * SKY_W + x] = skyCol;
    }
    bgCaiaphasSky = GraphicsManager::CreateTextureFromPixels(SKY_W, SKY_H, caSky.data());

    // 10d. STAGE 8 CAIAPHAS PALACE PARALLAX (High palace porticos & braziers)
    std::vector<Color> caPalace(PAR_W * PAR_H, BLANK);
    for (int x = 0; x < PAR_W; ++x) {
        int terraceY = 95 + static_cast<int>(sinf(x * 0.02f) * 8.0f);
        // Colonnade porticos
        bool isPillar = (x % 40 < 8);
        int topY = isPillar ? (terraceY - 35) : terraceY;
        for (int y = topY; y < PAR_H; ++y) {
            Color palCol = isPillar ? Color{ 210, 205, 225, 255 } : Color{ 145, 135, 160, 255 };
            caPalace[y * PAR_W + x] = palCol;
        }
        // Blazing braziers atop the watchtower terraces
        if (x % 80 == 38) {
            FillCircle(caPalace.data(), PAR_W, PAR_H, x, terraceY - 4, 3, Color{ 255, 140, 40, 255 });
        }
    }
    bgCaiaphasPalace = GraphicsManager::CreateTextureFromPixels(PAR_W, PAR_H, caPalace.data());

    // 10e. STAGE 9 CENACLE SKY (Deep midnight Passover blue with glowing moon)
    std::vector<Color> cenSky(SKY_W * SKY_H);
    for (int y = 0; y < SKY_H; ++y) {
        float f = y / (float)SKY_H;
        Color skyCol = {
            static_cast<unsigned char>(12 + f * 18),
            static_cast<unsigned char>(16 + f * 24),
            static_cast<unsigned char>(42 + f * 45),
            255
        };
        for (int x = 0; x < SKY_W; ++x) cenSky[y * SKY_W + x] = skyCol;
    }
    // Stars
    for (int s = 0; s < 45; ++s) {
        int sx = (s * 47) % SKY_W;
        int sy = (s * 31) % (SKY_H / 2);
        cenSky[sy * SKY_W + sx] = Color{ 255, 255, 230, 220 };
    }
    // Full Passover Moon
    FillCircle(cenSky.data(), SKY_W, SKY_H, 240, 48, 16, Color{ 255, 255, 235, 255 });
    OutlineCircle(cenSky.data(), SKY_W, SKY_H, 240, 48, 18, Color{ 255, 255, 200, 100 });
    bgCenacleSky = GraphicsManager::CreateTextureFromPixels(SKY_W, SKY_H, cenSky.data());

    // 10f. STAGE 9 CENACLE CHAMBER PARALLAX (Vaulted hall, tapestries, candlelight)
    std::vector<Color> cenChamber(PAR_W * PAR_H, BLANK);
    for (int x = 0; x < PAR_W; ++x) {
        // Vaulted arches overhead
        int archY = 60 + static_cast<int>(fabsf(sinf(x * 0.04f)) * 40.0f);
        for (int y = archY; y < PAR_H; ++y) {
            Color stoneCol = ((x % 30 == 0) || (y % 20 == 0)) ? Color{ 45, 38, 52, 255 } : Color{ 75, 62, 85, 255 };
            cenChamber[y * PAR_W + x] = stoneCol;
        }
        // Candle sconces on chamber pillars
        if (x % 75 == 37) {
            FillCircle(cenChamber.data(), PAR_W, PAR_H, x, archY + 18, 4, Color{ 255, 220, 100, 255 });
        }
    }
    bgCenacleChamber = GraphicsManager::CreateTextureFromPixels(PAR_W, PAR_H, cenChamber.data());
}

