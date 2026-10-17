#include "Graphics.h"
#include "Mission2Graphics.h"
#include "Mission3Graphics.h"
#include "Mission4Graphics.h"
#include "TitleImageBytes.h"
#include <cmath>
#include <cstdlib>
#include <vector>
#include <algorithm>

Texture2D GraphicsManager::s_sprites[SPRITE_TOTAL_COUNT] = {};
Texture2D GraphicsManager::s_tiles[TILE_TOTAL_COUNT] = {};
Texture2D GraphicsManager::s_bgSkyDistant = {};
Texture2D GraphicsManager::s_bgTempleOfDavid = {};
Texture2D GraphicsManager::s_bgMidJerusalemVillas = {};
Texture2D GraphicsManager::s_bgNearStreetscape = {};
Texture2D GraphicsManager::s_bgTempleSanctuary = {};
Texture2D GraphicsManager::s_bgTempleColonnade = {};
Texture2D GraphicsManager::s_bgGolgothaSky = {};
Texture2D GraphicsManager::s_bgGolgothaCrags = {};
Texture2D GraphicsManager::s_bgKidronSky = {};
Texture2D GraphicsManager::s_bgKidronRavine = {};
Texture2D GraphicsManager::s_bgHedgeSky = {};
Texture2D GraphicsManager::s_bgHedgeLabyrinth = {};
Texture2D GraphicsManager::s_bgGethsemaneSky = {};
Texture2D GraphicsManager::s_bgGethsemaneGrove = {};
Texture2D GraphicsManager::s_bgBonusSunset = {};
Texture2D GraphicsManager::s_bgBonusTerraces = {};
Texture2D GraphicsManager::s_bgBazaarSky = {};
Texture2D GraphicsManager::s_bgBazaarRoofs = {};
Texture2D GraphicsManager::s_bgCaiaphasSky = {};
Texture2D GraphicsManager::s_bgCaiaphasPalace = {};
Texture2D GraphicsManager::s_bgCenacleSky = {};
Texture2D GraphicsManager::s_bgCenacleChamber = {};
Texture2D GraphicsManager::s_bgDolorosaSky = {};
Texture2D GraphicsManager::s_bgDolorosaStreet = {};
Texture2D GraphicsManager::s_bgCourtSky = {};
Texture2D GraphicsManager::s_bgCourtBasilica = {};
Texture2D GraphicsManager::s_bgThroneSky = {};
Texture2D GraphicsManager::s_bgThronePalace = {};
Texture2D GraphicsManager::s_texMissionMap = {};
Texture2D GraphicsManager::s_texTitleSplash = {};
bool GraphicsManager::s_initialized = false;
bool GraphicsManager::s_hasExternalTextures = false;
int GraphicsManager::s_baseWidth[SPRITE_TOTAL_COUNT] = { 0 };
int GraphicsManager::s_baseHeight[SPRITE_TOTAL_COUNT] = { 0 };

// Helper to create texture from raw pixel array
Texture2D GraphicsManager::CreateTextureFromPixels(int width, int height, const Color* pixels) {
    Image img = GenImageColor(width, height, BLANK);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Color c = pixels[y * width + x];
            ImageDrawPixel(&img, x, y, c);
        }
    }
    Texture2D tex = LoadTextureFromImage(img);
    SetTextureFilter(tex, TEXTURE_FILTER_POINT);
    UnloadImage(img);
    return tex;
}

// Pixel drawing primitives for procedural generation
namespace PixelArt {
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
                Color existing = p[y * w + x];
                float a = c.a / 255.0f;
                float invA = 1.0f - a;
                Color blended = {
                    static_cast<unsigned char>(c.r * a + existing.r * invA),
                    static_cast<unsigned char>(c.g * a + existing.g * invA),
                    static_cast<unsigned char>(c.b * a + existing.b * invA),
                    static_cast<unsigned char>(std::max(c.a, existing.a))
                };
                p[y * w + x] = blended;
            }
        }
    }

    inline void HLine(Color* p, int w, int h, int x0, int x1, int y, Color c) {
        if (y < 0 || y >= h) return;
        int sx = std::max(0, std::min(x0, x1));
        int ex = std::min(w - 1, std::max(x0, x1));
        for (int x = sx; x <= ex; ++x) Set(p, w, h, x, y, c);
    }

    inline void VLine(Color* p, int w, int h, int x, int y0, int y1, Color c) {
        if (x < 0 || x >= w) return;
        int sy = std::max(0, std::min(y0, y1));
        int ey = std::min(h - 1, std::max(y0, y1));
        for (int y = sy; y <= ey; ++y) Set(p, w, h, x, y, c);
    }

    inline void Line(Color* p, int w, int h, int x0, int y0, int x1, int y1, Color c) {
        int dx = std::abs(x1 - x0);
        int sx = x0 < x1 ? 1 : -1;
        int dy = -std::abs(y1 - y0);
        int sy = y0 < y1 ? 1 : -1;
        int err = dx + dy;
        int curX = x0;
        int curY = y0;
        while (true) {
            Set(p, w, h, curX, curY, c);
            if (curX == x1 && curY == y1) break;
            int e2 = 2 * err;
            if (e2 >= dy) { err += dy; curX += sx; }
            if (e2 <= dx) { err += dx; curY += sy; }
        }
    }

    inline void FillRect(Color* p, int w, int h, int x, int y, int rw, int rh, Color c) {
        for (int j = y; j < y + rh; ++j) {
            HLine(p, w, h, x, x + rw - 1, j, c);
        }
    }

    inline void OutlineRect(Color* p, int w, int h, int x, int y, int rw, int rh, Color c) {
        HLine(p, w, h, x, x + rw - 1, y, c);
        HLine(p, w, h, x, x + rw - 1, y + rh - 1, c);
        VLine(p, w, h, x, y, y + rh - 1, c);
        VLine(p, w, h, x + rw - 1, y, y + rh - 1, c);
    }

    inline void FillCircle(Color* p, int w, int h, int cx, int cy, int r, Color c) {
        for (int dy = -r; dy <= r; ++dy) {
            int y = cy + dy;
            if (y < 0 || y >= h) continue;
            int dxMax = static_cast<int>(sqrtf(static_cast<float>(r * r - dy * dy)));
            HLine(p, w, h, cx - dxMax, cx + dxMax, y, c);
        }
    }

    inline void OutlineCircle(Color* p, int w, int h, int cx, int cy, int r, Color c) {
        int x = 0, y = r;
        int d = 3 - 2 * r;
        auto drawPoints = [&](int px, int py) {
            Set(p, w, h, cx + px, cy + py, c);
            Set(p, w, h, cx - px, cy + py, c);
            Set(p, w, h, cx + px, cy - py, c);
            Set(p, w, h, cx - px, cy - py, c);
            Set(p, w, h, cx + py, cy + px, c);
            Set(p, w, h, cx - py, cy + px, c);
            Set(p, w, h, cx + py, cy - px, c);
            Set(p, w, h, cx - py, cy - px, c);
        };
        drawPoints(x, y);
        while (y >= x) {
            x++;
            if (d > 0) { y--; d = d + 4 * (x - y) + 10; }
            else { d = d + 4 * x + 6; }
            drawPoints(x, y);
        }
    }

    inline void FillEllipse(Color* p, int w, int h, int cx, int cy, int rx, int ry, Color c) {
        if (rx <= 0 || ry <= 0) return;
        for (int dy = -ry; dy <= ry; ++dy) {
            int y = cy + dy;
            if (y < 0 || y >= h) continue;
            float fy = static_cast<float>(dy) / static_cast<float>(ry);
            float term = 1.0f - fy * fy;
            if (term >= 0.0f) {
                int dxMax = static_cast<int>(rx * sqrtf(term));
                HLine(p, w, h, cx - dxMax, cx + dxMax, y, c);
            }
        }
    }
}

// Generate all procedural sprites
void GraphicsManager::GenerateAllProceduralSprites() {
    using namespace PixelArt;

    // 1. Nazarene (The Player) - 28x32 Authentic Sega Arcade Masterpiece
    constexpr int PW = 28;
    constexpr int PH = 32;

    auto PaintNazarene = [](Color* p, int frame, bool crouch, bool hurt, bool attackThrow, bool attackMelee) {
        for (int i = 0; i < PW * PH; ++i) p[i] = BLANK;

        // Palette
        Color outline   = Color{ 28, 18, 14, 255 };
        Color hairLight = Color{ 160, 105, 55, 255 };
        Color hairMid   = Color{ 95, 52, 26, 255 };
        Color hairDark  = Color{ 45, 22, 12, 255 };
        Color hairGlint = COLOR_GOLD_BRIGHT;

        Color skinHi    = hurt ? Color{ 255, 195, 195, 255 } : Color{ 255, 225, 190, 255 };
        Color skinMid   = hurt ? Color{ 245, 145, 145, 255 } : Color{ 230, 168, 120, 255 };
        Color skinFlush = hurt ? Color{ 245, 120, 120, 255 } : Color{ 210, 138, 95, 255 };
        Color skinDark  = hurt ? Color{ 200, 100, 100, 255 } : Color{ 165, 105, 68, 255 };

        Color robePure  = Color{ 255, 255, 255, 255 };
        Color robeLight = hurt ? Color{ 255, 215, 215, 255 } : Color{ 246, 246, 240, 255 };
        Color robeMid   = hurt ? Color{ 230, 180, 180, 255 } : Color{ 195, 202, 214, 255 };
        Color robeDark  = hurt ? Color{ 190, 140, 140, 255 } : Color{ 135, 142, 158, 255 };
        Color robeDeep  = hurt ? Color{ 150, 110, 110, 255 } : Color{  95, 102, 118, 255 };

        Color sashHi    = hurt ? Color{ 255, 90, 100, 255 } : Color{ 245, 65, 75, 255 };
        Color sashMid   = hurt ? Color{ 220, 40, 50, 255 }  : Color{ 185, 22, 32, 255 };
        Color sashDark  = hurt ? Color{ 140, 20, 30, 255 }  : Color{ 110, 12, 20, 255 };
        Color sashGold  = COLOR_ROMAN_GOLD;

        Color sandalHi  = Color{ 165, 115, 70, 255 };
        Color sandalMid = Color{ 110, 72, 40, 255 };
        Color sandalSole= Color{ 60, 35, 20, 255 };
        Color staffWood = Color{ 140, 90, 42, 255 };
        Color staffGold = COLOR_GOLD_BRIGHT;

        Color tzitzitWhite = Color{ 255, 255, 255, 255 };
        Color tzitzitBlue  = Color{  45,  95, 185, 255 }; // Tekhelet royal biblical blue

        int headY = crouch ? 12 : 2;
        int torsoY = headY + 9;
        int feetY = 31;

        // --- DIVINE AURA / HALO (Subtle warm glow around the head) ---
        for (int y = headY - 1; y <= headY + 3; ++y) {
            for (int x = 8; x <= 16; ++x) {
                float dx = x - 12.0f;
                float dy = y - (headY + 1.0f);
                if (dx * dx + dy * dy <= 22.0f) {
                    Blend(p, PW, PH, x, y, Color{ 255, 235, 140, 75 });
                }
            }
        }

        // --- HEAD & HAIR (Rounded organic cranium & expressive facial profile) ---
        // Organic rounded cranium base
        FillCircle(p, PW, PH, 12, headY + 3, 4, hairMid);
        // Hair top volume & highlights
        // Hair top volume & golden celestial rim light
        HLine(p, PW, PH, 10, 14, headY, hairLight);
        HLine(p, PW, PH, 9, 13, headY - 1, hairMid);
        Set(p, PW, PH, 11, headY - 1, hairLight);
        Set(p, PW, PH, 11, headY - 1, hairGlint); // Divine specular rim light on crown
        Set(p, PW, PH, 12, headY - 1, hairGlint);
        Set(p, PW, PH, 10, headY, hairGlint);
        
        // Cascading back locks
        for (int y = headY + 1; y <= headY + 9; ++y) {
            int wave = (y % 3 == 0) ? 1 : 0;
            Set(p, PW, PH, 8 - wave, y, hairDark);
            Set(p, PW, PH, 9 - wave, y, hairDark);
            Set(p, PW, PH, 10 - wave, y, (y % 2 == 0) ? hairLight : hairMid);
            if (!crouch && y >= headY + 5) Set(p, PW, PH, 7 - wave, y, hairDark);
        }

        // Face profile
        // Forehead
        FillRect(p, PW, PH, 12, headY + 1, 4, 2, skinMid);
        HLine(p, PW, PH, 13, 15, headY + 1, skinHi);
        // Brow & Eye
        Set(p, PW, PH, 12, headY + 3, skinFlush);
        Set(p, PW, PH, 13, headY + 3, skinMid);
        Set(p, PW, PH, 14, headY + 3, Color{ 255, 255, 255, 255 }); // Sclera
        Set(p, PW, PH, 15, headY + 3, outline);                      // Pupil / Iris
        Set(p, PW, PH, 14, headY + 2, Color{ 255, 255, 255, 220 }); // Pupil specular catchlight
        Set(p, PW, PH, 15, headY + 2, skinDark);                     // Brow shadow
        // Nose (Prominent noble bridge)
        Set(p, PW, PH, 15, headY + 4, skinHi); // Bridge highlight
        Set(p, PW, PH, 16, headY + 4, skinHi); // Nose tip
        Set(p, PW, PH, 15, headY + 5, skinDark); // Nostril base
        // Cheek & Jaw
        Set(p, PW, PH, 13, headY + 4, skinFlush);
        Set(p, PW, PH, 14, headY + 4, skinMid);
        // Mustache
        HLine(p, PW, PH, 14, 16, headY + 5, hairMid);
        Set(p, PW, PH, 15, headY + 5, hairLight);
        // Lips
        Set(p, PW, PH, 15, headY + 6, Color{ 215, 125, 105, 255 });
        // Full wavy beard curving around chin
        FillRect(p, PW, PH, 11, headY + 6, 4, 3, hairDark);
        HLine(p, PW, PH, 12, 15, headY + 7, hairMid);
        Set(p, PW, PH, 13, headY + 7, hairLight);
        HLine(p, PW, PH, 12, 14, headY + 8, hairDark); // Chin taper
        Set(p, PW, PH, 13, headY + 8, hairMid);

        if (!crouch) {
            // --- STANDING / WALKING TORSO & FLOWING ROBES ---
            int walkOffset = 0;
            if (frame == 1) walkOffset = -2;
            else if (frame == 3) walkOffset = 2;

            // Neck with collar contour
            FillRect(p, PW, PH, 11, headY + 9, 3, 2, skinDark);
            Set(p, PW, PH, 12, headY + 9, skinMid);

            // Robe Body with realistic cloth pleats & drapery
            // Robe Body with realistic 5-tone cloth pleats & drapery
            for (int y = torsoY; y <= feetY - 4; ++y) {
                int progress = y - torsoY;
                int flare = progress / 3;
                int leftX = 8 - flare + (walkOffset > 0 ? 1 : 0);
                int rightX = 17 + flare + (walkOffset < 0 ? -1 : 0);

                for (int x = leftX; x <= rightX; ++x) {
                    Color c = robeLight;
                    // Natural curved vertical drapery shadows
                    if (x == rightX) c = robePure;
                    else if (x == rightX - 1) c = robeLight;
                    else if (x == leftX || x == leftX + 1) c = robeMid;
                    else if (x == 11 && y > torsoY + 4) c = robeDark; // Deep pleat shadow
                    else if (x == 12 && y > torsoY + 4) c = robeMid;
                    else if (x == 11 && y > torsoY + 4) c = robeDeep; // Ambient occlusion pleat
                    else if (x == 12 && y > torsoY + 4) c = robeDark;
                    else if (x == 13 && y > torsoY + 4) c = robeMid;
                    else if (x == 14) c = robePure; // Front pleat highlight
                    Set(p, PW, PH, x, y, c);
                }
                // Outlines on garment edges
                Set(p, PW, PH, leftX - 1, y, outline);
                Set(p, PW, PH, rightX + 1, y, outline);
            }

            // Scalloped Hem with realistic cloth ripples
            // Scalloped Hem with realistic cloth ripples & Tzitzit fringes
            for (int x = 7; x <= 19; ++x) {
                Set(p, PW, PH, x, feetY - 4, (x % 3 == 0) ? robeDark : robeLight);
                Set(p, PW, PH, x, feetY - 3, (x % 2 == 0) ? outline : robeMid);
            }
            // Tzitzit (Biblical ritual corner tassels with white & tekhelet threads)
            Set(p, PW, PH, 7, feetY - 2, tzitzitWhite);
            Set(p, PW, PH, 7, feetY - 1, tzitzitBlue);
            Set(p, PW, PH, 7, feetY,     tzitzitWhite);
            Set(p, PW, PH, 19, feetY - 2, tzitzitWhite);
            Set(p, PW, PH, 19, feetY - 1, tzitzitBlue);
            Set(p, PW, PH, 19, feetY,     tzitzitWhite);

            // --- ROYAL CRIMSON HIMATION / SASH ---
            // Diagonally drapes across chest with gold embroidered trim
            for (int y = torsoY; y <= feetY - 6; ++y) {
                int sx = 14 - (y - torsoY) + (walkOffset / 2);
                if (sx >= 8 && sx <= 18) {
                    Set(p, PW, PH, sx, y, sashHi);
                    Set(p, PW, PH, sx + 1, y, sashMid);
                    Set(p, PW, PH, sx + 2, y, sashDark);
                    Set(p, PW, PH, sx + 3, y, sashGold); // Gold embroidered border
                }
            }
            // Shoulder knot
            FillRect(p, PW, PH, 9, torsoY, 3, 3, sashMid);
            Set(p, PW, PH, 10, torsoY, sashHi);
            Set(p, PW, PH, 9, torsoY + 1, sashGold);

            // --- ARMS & ATTACKS ---
            if (attackThrow) {
                // Dynamic muscular forward throw!
                // Deltoid & Bicep (3px wide)
                FillRect(p, PW, PH, 15, torsoY + 1, 4, 3, skinMid);
                HLine(p, PW, PH, 16, 18, torsoY, skinHi);
                // Forearm extending forward (3px wide)
                FillRect(p, PW, PH, 19, torsoY + 1, 5, 2, skinMid);
                HLine(p, PW, PH, 19, 23, torsoY + 1, skinHi);
                // Hand releasing spike with fingers & golden spark
                Set(p, PW, PH, 24, torsoY + 1, skinHi);
                Set(p, PW, PH, 25, torsoY + 1, skinHi); // Extended fingers
                Set(p, PW, PH, 24, torsoY + 2, skinMid); // Thumb
                Set(p, PW, PH, 24, torsoY + 1, Color{ 255, 60, 70, 255 }); // Stigmata spark
                Set(p, PW, PH, 26, torsoY + 1, staffGold); // Holy launch spark
                Set(p, PW, PH, 27, torsoY, staffGold);
            } else if (attackMelee) {
                // Majestic Palm Strike & Heavy Olive Staff!
                // Muscular arm thrust
                FillRect(p, PW, PH, 16, torsoY + 2, 7, 3, skinMid);
                HLine(p, PW, PH, 17, 22, torsoY + 1, skinHi);
                // Open Palm emitting divine radiant aura
                Set(p, PW, PH, 23, torsoY + 1, COLOR_GOLD_BRIGHT);
                Set(p, PW, PH, 23, torsoY + 2, Color{ 255, 60, 70, 255 }); // Holy stigmata
                Set(p, PW, PH, 24, torsoY + 2, robePure);
                Set(p, PW, PH, 24, torsoY + 3, robePure);
                Set(p, PW, PH, 25, torsoY + 2, COLOR_GOLD_BRIGHT);
                Set(p, PW, PH, 25, torsoY + 3, COLOR_GOLD_BRIGHT);
                // Thick 3px Carved Olive-Wood Quarterstaff!
                for (int sy = torsoY - 8; sy <= feetY - 1; ++sy) {
                    Set(p, PW, PH, 22, sy, staffWood);
                    Set(p, PW, PH, 23, sy, Color{ 175, 120, 60, 255 }); // Wood grain highlight
                }
                // Brass ferrule caps on top & bottom
                HLine(p, PW, PH, 22, 23, torsoY - 9, staffGold);
                HLine(p, PW, PH, 22, 23, torsoY - 8, staffGold);
                HLine(p, PW, PH, 22, 23, feetY - 2, staffGold);
                HLine(p, PW, PH, 22, 23, feetY - 1, staffGold);
            } else {
                // Relaxed arm draped at side
                FillRect(p, PW, PH, 10, torsoY + 2, 3, 5, robeMid);
                Set(p, PW, PH, 11, torsoY + 2, robePure);
                FillRect(p, PW, PH, 11, torsoY + 7, 2, 2, skinMid); // Hand with fingers
                Set(p, PW, PH, 12, torsoY + 7, skinHi);
                Set(p, PW, PH, 11, torsoY + 7, Color{ 255, 60, 70, 255 }); // Holy stigmata
                Set(p, PW, PH, 12, torsoY + 8, Color{ 255, 235, 120, 255 }); // Subtle golden glow
            }

            // --- LEGS & JUDEAN SANDALS (Muscular anatomical calves!) ---
            int leg1X = 10 + walkOffset;
            int leg2X = 15 - walkOffset;
            for (int y = feetY - 3; y <= feetY - 1; ++y) {
                // 3-pixel wide muscular calves (not 1px sticks!)
                FillRect(p, PW, PH, leg1X - 1, y, 3, 1, skinMid);
                Set(p, PW, PH, leg1X, y, skinHi);
                FillRect(p, PW, PH, leg2X - 1, y, 3, 1, skinMid);
                Set(p, PW, PH, leg2X, y, skinHi);
                // Criss-cross leather sandal straps
                if (y % 2 == 0) {
                    Set(p, PW, PH, leg1X - 1, y, sandalHi);
                    Set(p, PW, PH, leg2X - 1, y, sandalHi);
                } else {
                    Set(p, PW, PH, leg1X + 1, y, sandalMid);
                    Set(p, PW, PH, leg2X + 1, y, sandalMid);
                }
            }
            // Leather soles & toes
            HLine(p, PW, PH, leg1X - 1, leg1X + 2, feetY, sandalSole);
            HLine(p, PW, PH, leg2X - 1, leg2X + 2, feetY, sandalSole);
            Set(p, PW, PH, leg1X, feetY - 1, sandalHi);
            Set(p, PW, PH, leg2X, feetY - 1, sandalHi);
        } else {
            // --- CROUCHED POSE (Natural anatomical crouch) ---
            // Bunched Robes over bent thighs
            for (int y = torsoY; y <= feetY - 3; ++y) {
                int leftX = 7;
                int rightX = 19;
                for (int x = leftX; x <= rightX; ++x) {
                    Color c = (x > 13) ? robeLight : robeMid;
                    if (y > torsoY + 5) c = robeDark;
                    Set(p, PW, PH, x, y, c);
                }
                Set(p, PW, PH, leftX - 1, y, outline);
                Set(p, PW, PH, rightX + 1, y, outline);
            }
            // Folded Sash over knees
            FillRect(p, PW, PH, 10, torsoY + 2, 7, 3, sashMid);
            HLine(p, PW, PH, 10, 16, torsoY + 1, sashHi);
            HLine(p, PW, PH, 10, 16, torsoY + 5, sashGold);

            if (attackThrow || attackMelee) {
                FillRect(p, PW, PH, 16, torsoY + 3, 7, 2, skinMid);
                HLine(p, PW, PH, 17, 22, torsoY + 2, skinHi);
                Set(p, PW, PH, 24, torsoY + 3, staffGold);
            }

            // Tucked sandals along ground
            HLine(p, PW, PH, 7, 19, feetY - 1, sandalHi);
            HLine(p, PW, PH, 6, 20, feetY, sandalSole);
        }
    };

    std::vector<Color> p(PW * PH);

    // Idle
    PaintNazarene(p.data(), 0, false, false, false, false);
    s_sprites[SPR_PLAYER_IDLE] = CreateTextureFromPixels(PW, PH, p.data());

    // Walk 0, 1, 2, 3
    for (int f = 0; f < 4; ++f) {
        PaintNazarene(p.data(), f, false, false, false, false);
        s_sprites[SPR_PLAYER_WALK_0 + f] = CreateTextureFromPixels(PW, PH, p.data());
    }

    // Crouch
    PaintNazarene(p.data(), 0, true, false, false, false);
    s_sprites[SPR_PLAYER_CROUCH] = CreateTextureFromPixels(PW, PH, p.data());

    // Jump
    PaintNazarene(p.data(), 1, false, false, false, false);
    s_sprites[SPR_PLAYER_JUMP] = CreateTextureFromPixels(PW, PH, p.data());

    // Somersault Rotation (High Jump) - 28x28
    constexpr int SW = 28, SH = 28;
    std::vector<Color> spin(SW * SH);
    for (int f = 0; f < 4; ++f) {
        for (int i = 0; i < SW * SH; ++i) spin[i] = BLANK;
        float angle = f * (3.14159f / 2.0f);
        float cx = 13.5f, cy = 13.5f;

        for (int y = 0; y < SH; ++y) {
            for (int x = 0; x < SW; ++x) {
                float dx = x - cx;
                float dy = y - cy;
                float dist = sqrtf(dx * dx + dy * dy);
                if (dist <= 11.5f && dist >= 3.0f) {
                    float a = atan2f(dy, dx) + angle;
                    float s = sinf(a * 2.0f);
                    if (s > 0.4f) spin[y * SW + x] = COLOR_CRIMSON_BRIGHT;
                    else if (s > 0.0f) spin[y * SW + x] = COLOR_ROMAN_CRIMSON;
                    else if (s > -0.4f) spin[y * SW + x] = COLOR_ROBE_WHITE;
                    else spin[y * SW + x] = COLOR_ROMAN_GOLD;
                } else if (dist < 3.0f) {
                    spin[y * SW + x] = COLOR_SKIN;
                }
            }
        }
        s_sprites[SPR_PLAYER_SOMERSAULT_0 + f] = CreateTextureFromPixels(SW, SH, spin.data());
    }

    // Attacks & Hurt
    PaintNazarene(p.data(), 0, false, false, true, false);
    s_sprites[SPR_PLAYER_ATTACK_STAND] = CreateTextureFromPixels(PW, PH, p.data());

    PaintNazarene(p.data(), 0, true, false, true, false);
    s_sprites[SPR_PLAYER_ATTACK_CROUCH] = CreateTextureFromPixels(PW, PH, p.data());

    PaintNazarene(p.data(), 0, false, false, false, true);
    s_sprites[SPR_PLAYER_MELEE_STRIKE] = CreateTextureFromPixels(PW, PH, p.data());

    PaintNazarene(p.data(), 0, false, true, false, false);
    s_sprites[SPR_PLAYER_HURT] = CreateTextureFromPixels(PW, PH, p.data());

    // Majestic Ascension to Heaven (arms spread wide in blessing, robes billowing)
    auto PaintAscension = [](Color* p) {
        for (int i = 0; i < PW * PH; ++i) p[i] = BLANK;

        Color robePure  = COLOR_ROBE_WHITE;
        Color robeLight = Color{ 235, 238, 245, 255 };
        Color robeMid   = COLOR_ROBE_SHADOW;
        Color robeDark  = COLOR_ROBE_DEEP;
        Color sashHi    = COLOR_CRIMSON_BRIGHT;
        Color sashMid   = COLOR_ROMAN_CRIMSON;
        Color sashDark  = COLOR_CRIMSON_DARK;
        Color sashGold  = COLOR_GOLD_BRIGHT;
        Color skinHi    = COLOR_SKIN_HIGHLIGHT;
        Color skinMid   = COLOR_SKIN;
        Color skinDark  = COLOR_SKIN_DARK;
        (void)skinDark;
        Color hairLight = Color{ 145, 95, 55, 255 };
        Color hairMid   = Color{ 105, 62, 32, 255 };
        Color hairDark  = Color{  65, 38, 18, 255 };
        Color haloOuter = Color{ 255, 235, 120, 110 };
        Color haloCore  = Color{ 255, 255, 220, 180 };
        Color stigmata  = Color{ 255, 245, 140, 255 };

        // 1. Radiant Nimbus Halo around head
        FillCircle(p, PW, PH, 13, 5, 6, haloOuter);
        FillCircle(p, PW, PH, 13, 5, 4, haloCore);

        // 2. Head & flowing hair (gazing upward toward heaven)
        FillCircle(p, PW, PH, 13, 6, 3, hairMid);
        HLine(p, PW, PH, 11, 15, 3, hairLight);
        for (int y = 5; y <= 12; ++y) {
            Set(p, PW, PH, 9, y, hairDark);
            Set(p, PW, PH, 10, y, (y % 2 == 0) ? hairLight : hairMid);
            Set(p, PW, PH, 16, y, (y % 2 == 0) ? hairLight : hairMid);
            Set(p, PW, PH, 17, y, hairDark);
        }
        FillRect(p, PW, PH, 11, 5, 5, 4, skinMid);
        HLine(p, PW, PH, 12, 14, 5, skinHi);
        // Peaceful eyes raised toward the heavens
        Set(p, PW, PH, 12, 5, Color{ 255, 255, 255, 255 });
        Set(p, PW, PH, 14, 5, Color{ 255, 255, 255, 255 });
        Set(p, PW, PH, 12, 4, Color{ 40, 25, 20, 255 });
        Set(p, PW, PH, 14, 4, Color{ 40, 25, 20, 255 });
        Set(p, PW, PH, 13, 6, skinHi);
        HLine(p, PW, PH, 12, 14, 8, hairDark);
        Set(p, PW, PH, 13, 8, hairMid);

        // 3. Torso & Flowing Linen Robes (Billowing gently downwards)
        for (int y = 10; y <= 27; ++y) {
            int progress = y - 10;
            int flare = progress / 3;
            int lx = 8 - flare;
            int rx = 18 + flare;
            for (int x = lx; x <= rx; ++x) {
                Color c = robeLight;
                if (x == lx || x == rx) c = robeDark;
                else if (x == lx + 1 || x == rx - 1) c = robeMid;
                else if (x == 13 || x == 14) c = robePure;
                Set(p, PW, PH, x, y, c);
            }
        }
        for (int x = 4; x <= 22; ++x) {
            Set(p, PW, PH, x, 27, (x % 2 == 0) ? robePure : robeLight);
            Set(p, PW, PH, x, 28, (x % 3 == 0) ? robeMid : robePure);
        }

        // 4. Royal Crimson Himation & Golden Border
        for (int y = 10; y <= 26; ++y) {
            int sx = 14 - (y - 10) / 2;
            if (sx >= 6 && sx <= 20) {
                Set(p, PW, PH, sx, y, sashHi);
                Set(p, PW, PH, sx + 1, y, sashMid);
                Set(p, PW, PH, sx + 2, y, sashDark);
                Set(p, PW, PH, sx + 3, y, sashGold);
            }
        }

        // 5. Arms spread wide in divine blessing & majesty
        FillRect(p, PW, PH, 6, 10, 3, 3, robeMid);
        FillRect(p, PW, PH, 4, 8, 3, 3, skinMid);
        FillRect(p, PW, PH, 2, 6, 3, 3, skinHi);
        Set(p, PW, PH, 1, 5, skinHi);
        Set(p, PW, PH, 2, 5, skinHi);
        Set(p, PW, PH, 2, 6, stigmata);

        FillRect(p, PW, PH, 18, 10, 3, 3, robeMid);
        FillRect(p, PW, PH, 20, 8, 3, 3, skinMid);
        FillRect(p, PW, PH, 22, 6, 3, 3, skinHi);
        Set(p, PW, PH, 24, 5, skinHi);
        Set(p, PW, PH, 25, 5, skinHi);
        Set(p, PW, PH, 24, 6, stigmata);

        // 6. Bare feet / sandals dangling down gently
        HLine(p, PW, PH, 10, 12, 29, skinMid);
        HLine(p, PW, PH, 14, 16, 29, skinMid);
        HLine(p, PW, PH, 10, 12, 30, Color{ 120, 75, 40, 255 });
        HLine(p, PW, PH, 14, 16, 30, Color{ 120, 75, 40, 255 });
    };

    PaintAscension(p.data());
    s_sprites[SPR_PLAYER_ASCEND] = CreateTextureFromPixels(PW, PH, p.data());

    // 2. Lion of Judah (Divine Wrath Form) - 40x40 Majestic Celestial Lion
    constexpr int LW = 40, LH = 40;
    std::vector<Color> lion(LW * LH, BLANK);
    Color goldPure   = Color{ 255, 255, 220, 255 };
    Color goldHi     = Color{ 255, 235,  95, 255 };
    Color goldMid    = Color{ 235, 185,  35, 255 };
    Color goldDark   = Color{ 175, 115,  15, 255 };
    Color lionEye    = Color{  80, 210, 255, 255 };
    Color nimbusHalo = Color{ 255, 245, 160, 200 };

    float lcx = 19.5f, lcy = 19.5f;
    for (int y = 0; y < LH; ++y) {
        for (int x = 0; x < LW; ++x) {
            float dx = x - lcx;
            float dy = y - lcy;
            float dist = sqrtf(dx * dx + dy * dy);

            // Cruciform Nimbus & Sunburst Rays
            if (dist <= 18.0f && dist > 13.0f) {
                float a = atan2f(dy, dx);
                float rays = sinf(a * 16.0f);
                if (rays > 0.3f || (fabsf(dx) < 2.0f) || (fabsf(dy) < 2.0f)) {
                    lion[y * LW + x] = nimbusHalo;
                }
            }
            // Dense Golden Mane
            else if (dist <= 13.0f && dist > 7.5f) {
                float a = atan2f(dy, dx);
                float tufts = sinf(a * 10.0f);
                if (tufts > 0.2f) lion[y * LW + x] = goldHi;
                else if (tufts > -0.3f) lion[y * LW + x] = goldMid;
                else lion[y * LW + x] = goldDark;
            }
            // Inner Noble Face
            else if (dist <= 7.5f) {
                lion[y * LW + x] = goldHi;
            }
        }
    }
    // Muzzle, Eyes, Fangs
    FillRect(lion.data(), LW, LH, 16, 21, 8, 5, goldDark);
    HLine(lion.data(), LW, LH, 17, 22, 23, Color{ 60, 20, 10, 255 });
    Set(lion.data(), LW, LH, 17, 24, goldPure);
    Set(lion.data(), LW, LH, 22, 24, goldPure);
    FillRect(lion.data(), LW, LH, 14, 16, 3, 2, lionEye);
    FillRect(lion.data(), LW, LH, 23, 16, 3, 2, lionEye);
    Set(lion.data(), LW, LH, 15, 16, Color{ 255, 255, 255, 255 });
    Set(lion.data(), LW, LH, 24, 16, Color{ 255, 255, 255, 255 });
    Set(lion.data(), LW, LH, 19, 14, goldPure);
    Set(lion.data(), LW, LH, 20, 14, goldPure);
    s_sprites[SPR_LION_OF_JUDAH] = CreateTextureFromPixels(LW, LH, lion.data());

    // 3. Patrol Guard (Roman Legionary) - 28x32
    std::vector<Color> guard(PW * PH);
    auto PaintGuard = [](Color* p, int frame, bool attack, bool hurt) {
        for (int i = 0; i < PW * PH; ++i) p[i] = BLANK;

        Color steelHi    = hurt ? Color{ 255, 210, 210, 255 } : Color{ 240, 245, 255, 255 };
        Color steelMid   = hurt ? Color{ 255, 140, 140, 255 } : Color{ 175, 185, 200, 255 };
        Color steelDark  = hurt ? Color{ 180,  80,  80, 255 } : Color{  95, 105, 120, 255 };
        Color bronzeHi   = Color{ 255, 225, 115, 255 };
        Color bronzeMid  = Color{ 205, 150,  60, 255 };
        Color bronzeDark = Color{ 125,  80,  25, 255 };
        Color crestHi    = Color{ 255,  85,  95, 255 };
        Color crestMid   = Color{ 235,  32,  42, 255 };
        Color crestDark  = Color{ 145,  15,  22, 255 };
        Color tunicRed   = Color{ 185,  25,  32, 255 };
        Color tunicDark  = Color{ 110,  12,  18, 255 };
        (void)tunicDark;
        Color skinHi     = Color{ 252, 215, 180, 255 };
        Color skinMid    = Color{ 228, 175, 130, 255 };
        Color skinDark   = Color{ 168, 115,  78, 255 };
        Color skinFlush  = Color{ 215, 130, 110, 255 };
        Color leatherHi  = Color{ 150, 100,  55, 255 };
        Color leatherMid = Color{ 105,  65,  32, 255 };
        Color leatherDark= Color{  62,  36,  16, 255 };
        Color woodShaft  = Color{ 115,  72,  35, 255 };
        Color woodGrain  = Color{ 160, 105,  50, 255 };
        Color outline    = Color{  28,  20,  16, 255 };

        int walkOff = (frame == 1) ? 2 : -1;
        if (attack) walkOff = 3;

        // --- GALEA HELMET (Imperial Gallic with Cheek Guards & Horsehair Crest) ---
        // Flowing Scarlet Horsehair Crest (Curved plume with tufted tips)
        for (int x = 8; x <= 18; ++x) {
            Set(p, PW, PH, x, 0, crestHi);
            Set(p, PW, PH, x, 1, crestMid);
            Set(p, PW, PH, x, 2, (x < 11 || x > 15) ? crestDark : bronzeHi);
        }
        Set(p, PW, PH, 7, 1, crestDark);
        Set(p, PW, PH, 19, 1, crestHi);

        // Bronze Helmet Skullcap & Brow Peak
        FillCircle(p, PW, PH, 14, 4, 3, bronzeMid);
        HLine(p, PW, PH, 11, 17, 3, bronzeHi);
        HLine(p, PW, PH, 12, 16, 2, bronzeHi);
        Set(p, PW, PH, 14, 2, Color{ 255, 255, 255, 240 }); // Specular helmet gleam
        Set(p, PW, PH, 15, 3, Color{ 255, 255, 255, 220 });
        // Brass Brow Reinforcement Cross
        HLine(p, PW, PH, 13, 17, 4, bronzeHi);
        // Neck Guard (Flanged rear flare)
        Set(p, PW, PH, 9, 5, bronzeDark);
        Set(p, PW, PH, 9, 6, bronzeMid);
        Set(p, PW, PH, 10, 6, bronzeDark);
        // Hinged Cheek Guard (Buccula)
        VLine(p, PW, PH, 12, 5, 7, bronzeMid);
        Set(p, PW, PH, 12, 6, bronzeHi); // Rivet

        // --- REALISTIC HUMAN SOLDIER FACE ---
        // Forehead
        Set(p, PW, PH, 14, 4, skinMid);
        Set(p, PW, PH, 15, 4, skinHi);
        // Brow & Focused Eye
        Set(p, PW, PH, 14, 5, Color{ 255, 255, 255, 255 }); // Sclera
        Set(p, PW, PH, 15, 5, outline);                      // Pupil
        Set(p, PW, PH, 14, 4, skinDark);                     // Brow shadow
        // Aquiline Roman Nose
        Set(p, PW, PH, 16, 5, skinHi);  // Bridge
        Set(p, PW, PH, 17, 5, skinHi);  // Nose tip
        Set(p, PW, PH, 16, 6, skinDark);// Nostril shadow
        // Cheek & Jaw
        Set(p, PW, PH, 13, 6, skinFlush);
        Set(p, PW, PH, 14, 6, skinMid);
        Set(p, PW, PH, 15, 6, Color{ 195, 105, 88, 255 });  // Lips
        Set(p, PW, PH, 13, 7, skinMid);
        Set(p, PW, PH, 14, 7, skinMid);                      // Chin
        Set(p, PW, PH, 15, 7, skinDark);

        // Neck & Tunic Collar
        FillRect(p, PW, PH, 12, 8, 3, 2, skinDark);
        HLine(p, PW, PH, 11, 16, 9, tunicRed);

        // --- LORICA SEGMENTATA (Overlapping Steel Girth Plates & Hinges) ---
        // Curved shoulder lames (Humeralia)
        FillRect(p, PW, PH, 9, 10, 4, 3, steelMid);
        HLine(p, PW, PH, 9, 12, 10, steelHi);
        FillRect(p, PW, PH, 16, 10, 4, 3, steelMid);
        HLine(p, PW, PH, 16, 19, 10, steelHi);

        // Torso steel plates (Horizontal curved plates with 3D bevels)
        for (int y = 11; y <= 18; ++y) {
            Color band = (y % 2 == 0) ? steelHi : steelMid;
            HLine(p, PW, PH, 10, 18, y, band);
            Set(p, PW, PH, 10, y, steelDark);
            Set(p, PW, PH, 18, y, steelDark);
            Set(p, PW, PH, 14, y, steelHi); // Central highlight
        }
        // Bronze Hinges & Tie-Rings down sternum
        Set(p, PW, PH, 13, 12, bronzeHi);
        Set(p, PW, PH, 15, 12, bronzeHi);
        Set(p, PW, PH, 13, 15, bronzeHi);
        Set(p, PW, PH, 15, 15, bronzeHi);
        Set(p, PW, PH, 13, 17, bronzeHi);
        Set(p, PW, PH, 15, 17, bronzeHi);

        // Cingulum Militare (Studded War Belt & Hanging Pteruges Apron)
        HLine(p, PW, PH, 9, 19, 19, bronzeMid);
        HLine(p, PW, PH, 11, 17, 19, bronzeHi);
        // Hanging leather apron straps with bronze pendant tips
        for (int x = 11; x <= 17; x += 2) {
            VLine(p, PW, PH, x, 20, 22, leatherMid);
            Set(p, PW, PH, x, 21, bronzeHi);
            Set(p, PW, PH, x, 22, bronzeHi); // Leaf terminal
        }
        // Red woolen tunic skirt
        FillRect(p, PW, PH, 9, 21, 10, 2, tunicRed);
        HLine(p, PW, PH, 9, 18, 23, Color{ 240, 195, 60, 255 }); // Gold hem border

        // --- ARMS & PILUM SPEAR ---
        if (attack) {
            // Dynamic forward spear lunge!
            // Arms thrust forward gripping pilum with both hands
            FillRect(p, PW, PH, 14, 11, 6, 3, skinMid);
            HLine(p, PW, PH, 15, 19, 11, skinHi);
            FillRect(p, PW, PH, 20, 11, 4, 3, skinMid);
            HLine(p, PW, PH, 20, 23, 11, skinHi);
            // Bronze armlets
            VLine(p, PW, PH, 16, 11, 13, bronzeHi);
            VLine(p, PW, PH, 21, 11, 13, bronzeHi);

            // Horizontal Pilum Spear (Long ash shaft, iron shank, leaf head)
            for (int y = 11; y <= 13; ++y) {
                HLine(p, PW, PH, 8, 23, y, (y == 12) ? woodGrain : woodShaft);
            }
            // Balance weight
            FillCircle(p, PW, PH, 23, 12, 2, bronzeMid);
            Set(p, PW, PH, 23, 11, bronzeHi);
            // Long iron shank & forged leaf-head
            HLine(p, PW, PH, 24, 27, 12, steelDark);
            FillRect(p, PW, PH, 25, 11, 3, 3, steelMid);
            HLine(p, PW, PH, 25, 27, 12, steelHi);
            Set(p, PW, PH, 27, 12, Color{ 255, 255, 255, 255 }); // Specular gleam!

            // Double wind-slash trail arcs
            HLine(p, PW, PH, 19, 27, 9, Color{ 255, 255, 255, 180 });
            HLine(p, PW, PH, 21, 27, 15, Color{ 255, 255, 255, 180 });
        } else {
            // Normal Walk: Holds Pilum upright
            FillRect(p, PW, PH, 15, 11, 3, 6, skinMid);
            Set(p, PW, PH, 16, 11, skinHi);
            Set(p, PW, PH, 16, 16, skinHi); // Hand gripping spear

            // Upright Pilum Spear
            for (int x = 18; x <= 19; ++x) {
                VLine(p, PW, PH, x, 5, 27, (x == 18) ? woodGrain : woodShaft);
            }
            // Bronze pyramid weight
            FillRect(p, PW, PH, 17, 5, 3, 2, bronzeMid);
            Set(p, PW, PH, 18, 5, bronzeHi);
            // Blackened iron shank & Leaf head
            VLine(p, PW, PH, 18, 1, 4, steelDark);
            VLine(p, PW, PH, 18, 0, 2, steelHi);
            Set(p, PW, PH, 17, 1, steelMid);
            Set(p, PW, PH, 19, 1, steelMid);
            Set(p, PW, PH, 18, 0, Color{ 255, 255, 255, 255 });
        }

        // --- MUSCULAR ANATOMICAL LEGS & ROMAN CALIGAE ---
        int leg1X = 10 - walkOff;
        int leg2X = 15 + walkOff;
        for (int y = 24; y <= 29; ++y) {
            // Muscular 3px thighs and calves
            FillRect(p, PW, PH, leg1X, y, 3, 1, skinMid);
            Set(p, PW, PH, leg1X + 1, y, skinHi);
            FillRect(p, PW, PH, leg2X, y, 3, 1, skinMid);
            Set(p, PW, PH, leg2X + 1, y, skinHi);

            // Caligae leather strap lattice
            if (y >= 27) {
                Set(p, PW, PH, leg1X, y, (y % 2 == 0) ? leatherMid : leatherHi);
                Set(p, PW, PH, leg2X, y, (y % 2 == 0) ? leatherMid : leatherHi);
            }
        }
        // Hobnailed Leather Soles
        FillRect(p, PW, PH, leg1X - 1, 30, 4, 2, leatherDark);
        FillRect(p, PW, PH, leg2X - 1, 30, 4, 2, leatherDark);
        HLine(p, PW, PH, leg1X - 1, leg1X + 2, 30, leatherMid);
        HLine(p, PW, PH, leg2X - 1, leg2X + 2, 30, leatherMid);
        // Iron hobnails along the bottom of the soles
        Set(p, PW, PH, leg1X - 1, 31, steelHi);
        Set(p, PW, PH, leg1X + 1, 31, steelHi);
        Set(p, PW, PH, leg2X - 1, 31, steelHi);
        Set(p, PW, PH, leg2X + 1, 31, steelHi);
    };

    PaintGuard(guard.data(), 0, false, false);
    s_sprites[SPR_GUARD_WALK_0] = CreateTextureFromPixels(PW, PH, guard.data());
    PaintGuard(guard.data(), 1, false, false);
    s_sprites[SPR_GUARD_WALK_1] = CreateTextureFromPixels(PW, PH, guard.data());
    PaintGuard(guard.data(), 0, true, false);
    s_sprites[SPR_GUARD_ATTACK] = CreateTextureFromPixels(PW, PH, guard.data());
    PaintGuard(guard.data(), 0, false, true);
    s_sprites[SPR_GUARD_HURT] = CreateTextureFromPixels(PW, PH, guard.data());

    // 4. Auxiliary Archer - 28x32
    std::vector<Color> archer(PW * PH);
    auto PaintArcher = [](Color* p, bool duck, bool fire, bool hurt) {
        for (int i = 0; i < PW * PH; ++i) p[i] = BLANK;

        Color leatherHi  = hurt ? Color{ 255, 180, 180, 255 } : Color{ 170, 115,  65, 255 };
        Color leatherMid = hurt ? Color{ 230, 110, 110, 255 } : Color{ 120,  75,  38, 255 };
        Color leatherDark= hurt ? Color{ 160,  60,  60, 255 } : Color{  72,  42,  18, 255 };
        Color tunicGreen = Color{  52,  88,  52, 255 };
        Color tunicDark  = Color{  32,  56,  32, 255 };
        Color bowWood    = Color{ 145,  85,  38, 255 };
        Color bowHorn    = Color{  85,  52,  24, 255 };
        Color bowGrip    = Color{ 210, 190, 150, 255 };
        Color stringCol  = Color{ 245, 245, 240, 255 };
        Color sandBag    = Color{ 195, 175, 135, 255 };
        Color sandDark   = Color{ 125, 105,  70, 255 };
        Color skinHi     = Color{ 250, 205, 165, 255 };
        Color skinMid    = Color{ 220, 160, 110, 255 };
        Color skinDark   = Color{ 160, 105,  68, 255 };
        Color arrowFeath = Color{ 235,  35,  45, 255 };
        Color arrowFeathW= Color{ 255, 255, 255, 255 };
        Color bronzeHi   = Color{ 250, 210,  95, 255 };
        Color bronzeMid  = Color{ 195, 145,  50, 255 };
        Color outline    = Color{  28,  20,  16, 255 };

        if (!duck) {
            // --- STANDING / FIRING ARCHER (Eastern Auxiliary Sagittarius) ---
            // Conical Bronze Helmet with Mail Aventail
            FillCircle(p, PW, PH, 14, 4, 3, bronzeMid);
            HLine(p, PW, PH, 12, 16, 2, bronzeHi);
            Set(p, PW, PH, 14, 1, bronzeHi); // Spire tip
            HLine(p, PW, PH, 11, 17, 4, bronzeHi); // Brow rim
            // Mail aventail protecting neck
            for (int y = 5; y <= 8; ++y) {
                Set(p, PW, PH, 10, y, (y % 2 == 0) ? leatherDark : bronzeMid);
                Set(p, PW, PH, 11, y, (y % 2 == 0) ? bronzeMid : leatherDark);
            }

            // Realistic Human Face Profile
            Set(p, PW, PH, 14, 4, skinMid);
            Set(p, PW, PH, 15, 4, skinHi);
            // Aiming eye looking forward
            Set(p, PW, PH, 14, 5, Color{ 255, 255, 255, 255 }); // Sclera
            Set(p, PW, PH, 15, 5, outline);                      // Pupil
            Set(p, PW, PH, 14, 4, skinDark);                     // Furrowed brow
            // Nose & Mustache
            Set(p, PW, PH, 16, 5, skinHi);  // Nose
            Set(p, PW, PH, 17, 5, skinMid);
            Set(p, PW, PH, 15, 6, leatherDark); // Dark mustache
            Set(p, PW, PH, 16, 6, leatherDark);
            Set(p, PW, PH, 14, 7, skinMid);     // Chin & trimmed beard
            Set(p, PW, PH, 15, 7, leatherDark);

            // Neck & Green Tunic Collar
            FillRect(p, PW, PH, 12, 8, 3, 2, skinDark);
            HLine(p, PW, PH, 11, 16, 9, tunicGreen);

            // Lorica Squamata (Scale Armor Brigandine)
            FillRect(p, PW, PH, 10, 10, 8, 9, leatherMid);
            OutlineRect(p, PW, PH, 10, 10, 8, 9, leatherDark);
            // Overlapping bronze scale highlights
            for (int y = 11; y <= 17; y += 2) {
                for (int x = 11; x <= 16; x += 2) {
                    Set(p, PW, PH, x, y, bronzeHi);
                    Set(p, PW, PH, x + 1, y, bronzeMid);
                }
            }
            // Cross-chest quiver baldric
            for (int y = 10; y <= 18; ++y) {
                int bx = 16 - (y - 10) * 6 / 8;
                Set(p, PW, PH, bx, y, leatherDark);
                Set(p, PW, PH, bx + 1, y, leatherHi);
            }

            // Quiver on Back with Eagle Feathers
            FillRect(p, PW, PH, 7, 7, 3, 11, leatherDark);
            OutlineRect(p, PW, PH, 7, 7, 3, 11, leatherMid);
            // Distinct red & white fletched arrows protruding
            Set(p, PW, PH, 6, 5, arrowFeath);
            Set(p, PW, PH, 7, 4, arrowFeathW);
            Set(p, PW, PH, 8, 5, arrowFeath);
            Set(p, PW, PH, 7, 3, arrowFeath);

            // Composite Recurve Bow (Recurved horn limbs, wood core, wrapped grip)
            if (fire) {
                // Dynamic archery draw!
                // Left arm fully extended forward holding bow
                // Left arm fully extended forward holding bow with protective leather bracer
                FillRect(p, PW, PH, 16, 12, 5, 2, skinMid);
                HLine(p, PW, PH, 16, 20, 12, skinHi);
                FillRect(p, PW, PH, 17, 12, 3, 2, leatherDark); // Archer bracer
                HLine(p, PW, PH, 17, 19, 12, leatherHi);
                // Right arm pulled back to the cheek
                FillRect(p, PW, PH, 12, 11, 3, 3, skinMid);
                Set(p, PW, PH, 13, 11, skinHi);

                // Recurve Bow under full tension
                for (int y = 5; y <= 21; ++y) {
                    int curve = (y >= 10 && y <= 16) ? 3 : (y >= 7 && y <= 19 ? 1 : 0);
                    Set(p, PW, PH, 21 - curve, y, bowWood);
                    Set(p, PW, PH, 22 - curve, y, bowHorn);
                }
                // Wrapped leather grip at center
                FillRect(p, PW, PH, 18, 12, 2, 3, bowGrip);

                // Taut bowstring drawn back to cheek
                DrawLine(13, 12, 22, 5, stringCol);
                DrawLine(13, 12, 22, 21, stringCol);
                // Taut bowstring drawn back to cheek (pixel-drawn directly into texture)
                Line(p, PW, PH, 13, 12, 22, 5, stringCol);
                Line(p, PW, PH, 13, 12, 22, 21, stringCol);

                // Nocked Arrow with steel bodkin tip!
                HLine(p, PW, PH, 12, 26, 12, Color{ 240, 240, 230, 255 });
                HLine(p, PW, PH, 26, 28, 12, COLOR_STEEL_BRIGHT);
                Set(p, PW, PH, 27, 11, COLOR_STEEL_BRIGHT);
                Set(p, PW, PH, 27, 13, COLOR_STEEL_BRIGHT);
                Set(p, PW, PH, 28, 12, Color{ 255, 255, 255, 255 });
                // Fletchings at nock
                HLine(p, PW, PH, 11, 13, 11, arrowFeath);
                HLine(p, PW, PH, 11, 13, 13, arrowFeath);
            } else {
                // Standing at Rest
                FillRect(p, PW, PH, 13, 11, 3, 5, skinMid);
                Set(p, PW, PH, 14, 11, skinHi);
                Set(p, PW, PH, 15, 15, skinHi); // Hand

                // Relaxed recurve bow held vertically
                for (int y = 6; y <= 22; ++y) {
                    int tip = (y <= 7 || y >= 21) ? 1 : 0;
                    Set(p, PW, PH, 18 + tip, y, bowWood);
                    Set(p, PW, PH, 19 + tip, y, bowHorn);
                }
                VLine(p, PW, PH, 17, 7, 21, stringCol);
            }

            // Green Tunic Skirt & Muscular Legs
            FillRect(p, PW, PH, 10, 19, 8, 4, tunicGreen);
            HLine(p, PW, PH, 10, 17, 22, tunicDark);
            // Legs in leather gaiters
            FillRect(p, PW, PH, 10, 23, 3, 7, skinMid);
            Set(p, PW, PH, 11, 23, skinHi);
            FillRect(p, PW, PH, 15, 23, 3, 7, skinMid);
            Set(p, PW, PH, 16, 23, skinHi);
            // Archer boots
            FillRect(p, PW, PH, 9, 30, 4, 2, leatherDark);
            FillRect(p, PW, PH, 15, 30, 4, 2, leatherDark);
            HLine(p, PW, PH, 9, 12, 30, leatherMid);
            HLine(p, PW, PH, 15, 18, 30, leatherMid);
        } else {
            // --- DUCKED BEHIND FORTIFIED SANDBAG REVETMENT ---
            // Peeking soldier head
            FillCircle(p, PW, PH, 13, 13, 3, bronzeMid);
            HLine(p, PW, PH, 11, 15, 11, bronzeHi);
            Set(p, PW, PH, 13, 10, bronzeHi);
            Set(p, PW, PH, 14, 13, skinHi);
            Set(p, PW, PH, 15, 13, outline); // Focused aiming eye
            Set(p, PW, PH, 16, 13, skinHi);  // Nose
            Set(p, PW, PH, 14, 14, leatherDark); // Mustache

            // Wicker Gabion & Burlap Sandbag Barrier
            FillRect(p, PW, PH, 13, 15, 14, 17, sandBag);
            OutlineRect(p, PW, PH, 13, 15, 14, 17, sandDark);
            // Burlap seams & stitching
            HLine(p, PW, PH, 14, 26, 20, sandDark);
            HLine(p, PW, PH, 14, 26, 25, sandDark);
            VLine(p, PW, PH, 20, 16, 31, sandDark);
            // Woven wicker cross-hatch texture
            for (int y = 17; y <= 30; y += 3) {
                for (int x = 15; x <= 25; x += 3) {
                    Set(p, PW, PH, x, y, sandDark);
                    Set(p, PW, PH, x + 1, y, Color{ 225, 205, 165, 255 });
                }
            }
            // Base earth mound
            HLine(p, PW, PH, 12, 27, 31, Color{ 80, 55, 30, 255 });
        }
    };

    PaintArcher(archer.data(), true, false, false);
    s_sprites[SPR_ARCHER_DUCK] = CreateTextureFromPixels(PW, PH, archer.data());
    PaintArcher(archer.data(), false, false, false);
    s_sprites[SPR_ARCHER_STAND] = CreateTextureFromPixels(PW, PH, archer.data());
    PaintArcher(archer.data(), false, true, false);
    s_sprites[SPR_ARCHER_FIRE] = CreateTextureFromPixels(PW, PH, archer.data());
    PaintArcher(archer.data(), false, false, true);
    s_sprites[SPR_ARCHER_HURT] = CreateTextureFromPixels(PW, PH, archer.data());

    // 5. Heavy Legionary (Tower Scutum Shield) - 32x34
    constexpr int HW = 32, HH = 34;
    std::vector<Color> heavy(HW * HH);
    auto PaintHeavy = [](Color* p, int frame, bool attack, bool hurt) {
        for (int i = 0; i < HW * HH; ++i) p[i] = BLANK;

        Color scutumRed   = Color{ 190,  24,  30, 255 };
        Color scutumHi    = Color{ 235,  45,  52, 255 };
        Color scutumDark  = Color{ 120,  14,  20, 255 };
        Color goldEagle   = COLOR_GOLD_BRIGHT;
        Color brassRim    = COLOR_ROMAN_GOLD;
        Color ironSteel   = hurt ? Color{ 255, 180, 180, 255 } : COLOR_STEEL_BRIGHT;
        Color ironMid     = hurt ? Color{ 240, 120, 120, 255 } : COLOR_IRON;
        Color ironDark    = Color{  75,  85, 100, 255 };
        Color crestRed    = Color{ 235,  30,  40, 255 };
        Color crestHi     = Color{ 255,  85,  95, 255 };
        Color skinHi      = Color{ 250, 205, 165, 255 };
        Color skinMid     = Color{ 220, 160, 110, 255 };
        Color skinDark    = Color{ 160, 105,  68, 255 };
        Color outline     = Color{  28,  20,  16, 255 };

        // Transverse Galea Crest visible over shield
        for (int x = 8; x <= 18; ++x) {
            Set(p, HW, HH, x, 1, crestHi);
            Set(p, HW, HH, x, 2, crestRed);
            Set(p, HW, HH, x, 3, (x < 11 || x > 15) ? Color{ 130, 15, 22, 255 } : brassRim);
        }
        // Helmet Bowl & Eyebrows
        FillRect(p, HW, HH, 8, 4, 10, 4, ironSteel);
        HLine(p, HW, HH, 9, 17, 4, Color{ 255, 255, 255, 255 });
        // Fierce human eyes peering fiercely over the shield rim!
        Set(p, HW, HH, 13, 6, Color{ 255, 255, 255, 255 });
        Set(p, HW, HH, 14, 6, outline);
        Set(p, HW, HH, 17, 6, Color{ 255, 255, 255, 255 });
        Set(p, HW, HH, 18, 6, outline);
        Set(p, HW, HH, 13, 5, skinDark);
        Set(p, HW, HH, 17, 5, skinDark);

        if (!attack) {
            // --- TOWER SCUTUM SHIELD (16x26 Curved Roman Masterpiece!) ---
            // 2.5D curved surface shading
            for (int y = 6; y <= 29; ++y) {
                for (int x = 14; x <= 29; ++x) {
                    Color c = scutumRed;
                    if (x >= 20 && x <= 23) c = scutumHi;        // Central curvature highlight
                    else if (x == 14 || x == 29) c = scutumDark; // Shadowed curved flanks
                    p[y * HW + x] = c;
                }
            }
            // Polished Brass Rim & Corner Plates
            OutlineRect(p, HW, HH, 14, 6, 16, 24, brassRim);
            FillRect(p, HW, HH, 14, 6, 3, 3, brassRim);
            FillRect(p, HW, HH, 27, 6, 3, 3, brassRim);
            FillRect(p, HW, HH, 14, 27, 3, 3, brassRim);
            FillRect(p, HW, HH, 27, 27, 3, 3, brassRim);

            // Imperial Golden Eagle (Aquila) spreading magnificent wings!
            HLine(p, HW, HH, 16, 27, 11, goldEagle);
            HLine(p, HW, HH, 17, 26, 12, goldEagle);
            HLine(p, HW, HH, 19, 24, 13, goldEagle);
            Set(p, HW, HH, 21, 10, goldEagle); // Eagle head
            Set(p, HW, HH, 22, 10, Color{ 255, 255, 255, 255 });
            Set(p, HW, HH, 16, 10, goldEagle); // Wing tips
            Set(p, HW, HH, 27, 10, goldEagle);

            // Central Heavy Bronze Umbo (Shield Boss) with 3D spherical gleam!
            FillCircle(p, HW, HH, 21, 18, 3, COLOR_BRONZE);
            FillCircle(p, HW, HH, 21, 18, 2, brassRim);
            Set(p, HW, HH, 21, 17, Color{ 255, 255, 220, 255 });
            Set(p, HW, HH, 20, 18, Color{ 255, 255, 220, 255 });

            // Laurel Wreath (SPQR Corona Triumphalis) embracing the boss
            Color laurelGreen = Color{ 48, 120, 52, 255 };
            Color laurelHi    = Color{ 110, 190, 95, 255 };
            Set(p, HW, HH, 17, 16, laurelHi);
            Set(p, HW, HH, 16, 17, laurelGreen);
            Set(p, HW, HH, 16, 19, laurelGreen);
            Set(p, HW, HH, 17, 20, laurelHi);
            Set(p, HW, HH, 25, 16, laurelHi);
            Set(p, HW, HH, 26, 17, laurelGreen);
            Set(p, HW, HH, 26, 19, laurelGreen);
            Set(p, HW, HH, 25, 20, laurelHi);

            // Lower Winged Thunderbolts (Fulmina)
            HLine(p, HW, HH, 17, 26, 23, goldEagle);
            HLine(p, HW, HH, 18, 25, 24, goldEagle);
            Set(p, HW, HH, 16, 23, goldEagle);
            Set(p, HW, HH, 27, 23, goldEagle);
        } else {
            // --- GLADIUS SWORD THRUST (Lowers Shield, Lunges Forward!) ---
            // Shield pulled to flank
            FillRect(p, HW, HH, 9, 8, 9, 21, scutumRed);
            OutlineRect(p, HW, HH, 9, 8, 9, 21, brassRim);
            FillCircle(p, HW, HH, 13, 18, 2, brassRim);

            // Exposed Armored Torso & Muscular Arm with Segmented Manica
            FillRect(p, HW, HH, 14, 10, 6, 8, ironSteel);
            // Arm lunging forward in segmented lames
            FillRect(p, HW, HH, 18, 13, 6, 4, ironSteel);
            HLine(p, HW, HH, 18, 23, 13, Color{ 255, 255, 255, 255 });
            // Hand & Brass Gladius Hilt
            FillRect(p, HW, HH, 23, 13, 2, 4, skinMid);
            Set(p, HW, HH, 24, 13, skinHi);
            VLine(p, HW, HH, 24, 12, 17, brassRim); // Guard
            Set(p, HW, HH, 22, 14, brassRim);      // Round pommel

            // Double-Edged Steel Gladius Blade
            FillRect(p, HW, HH, 25, 14, 6, 3, ironSteel);
            HLine(p, HW, HH, 25, 30, 14, Color{ 255, 255, 255, 255 }); // Fuller highlight
            HLine(p, HW, HH, 25, 30, 16, ironDark);
            Set(p, HW, HH, 31, 15, Color{ 255, 255, 255, 255 });      // Sharp point!

            // Dynamic silver-white wind slash curve
            HLine(p, HW, HH, 22, 31, 11, Color{ 255, 255, 255, 200 });
            HLine(p, HW, HH, 24, 31, 18, Color{ 255, 255, 255, 200 });
        }

        // Armored Legs with Greaves & Caligae
        int step = (frame == 1) ? 2 : 0;
        // Front leg with heavy bronze greave (Ocrea)
        FillRect(p, HW, HH, 14 + step, 24, 5, 8, brassRim);
        HLine(p, HW, HH, 15 + step, 17 + step, 25, Color{ 255, 245, 150, 255 });
        FillRect(p, HW, HH, 14 + step, 32, 5, 2, ironDark);
        // Rear leg with steel lames
        FillRect(p, HW, HH, 9 - step, 24, 4, 8, ironMid);
        FillRect(p, HW, HH, 8 - step, 32, 5, 2, ironDark);
    };

    PaintHeavy(heavy.data(), 0, false, false);
    s_sprites[SPR_HEAVY_WALK_0] = CreateTextureFromPixels(HW, HH, heavy.data());
    PaintHeavy(heavy.data(), 1, false, false);
    s_sprites[SPR_HEAVY_WALK_1] = CreateTextureFromPixels(HW, HH, heavy.data());
    PaintHeavy(heavy.data(), 0, true, false);
    s_sprites[SPR_HEAVY_ATTACK] = CreateTextureFromPixels(HW, HH, heavy.data());
    PaintHeavy(heavy.data(), 0, false, true);
    s_sprites[SPR_HEAVY_HURT] = CreateTextureFromPixels(HW, HH, heavy.data());

    // 6. Stage 1 Boss: The Centurion Gladiator (Ken-Oh equivalent) - 46x54 Colossus
    constexpr int BW = 46, BH = 54;
    std::vector<Color> boss(BW * BH);
    auto PaintBoss = [](Color* p, int state, bool hurt) {
        for (int i = 0; i < BW * BH; ++i) p[i] = BLANK;

        Color bronzeHi   = hurt ? Color{ 255, 210, 210, 255 } : Color{ 255, 225, 110, 255 };
        Color bronzeMid  = hurt ? Color{ 255, 140, 140, 255 } : Color{ 225, 175,  55, 255 };
        Color bronzeDark = hurt ? Color{ 180,  80,  80, 255 } : Color{ 140,  95,  25, 255 };
        Color goldPure   = hurt ? Color{ 255, 230, 230, 255 } : COLOR_GOLD_BRIGHT;
        Color skinMuscle = hurt ? Color{ 255, 190, 190, 255 } : Color{ 220, 160, 115, 255 };
        Color skinMid    = skinMuscle;
        Color skinHi     = hurt ? Color{ 255, 225, 225, 255 } : Color{ 250, 205, 165, 255 };
        Color skinDark   = hurt ? Color{ 180,  90,  90, 255 } : Color{ 160, 105,  68, 255 };
        Color crestHi    = Color{ 255,  85,  95, 255 };
        Color crestMid   = Color{ 235,  30,  40, 255 };
        Color crestDark  = Color{ 140,  12,  18, 255 };
        Color eyeGlow    = Color{ 255,  40,  40, 255 };
        Color leather    = Color{ 110,  65,  35, 255 };
        Color leatherDark= Color{  65,  38,  18, 255 };
        Color outline    = Color{  30,  18,  14, 255 };
        (void)outline;

        if (state == 4) {
            // --- STATE 4: KNEELING IN DEFEAT (Forced to one knee, clutching wounded cuirass) ---
            int ky = 10; // Entire figure lowered by 10px

            // Crest tilted forward
            for (int x = 11; x <= 33; ++x) {
                Set(p, BW, BH, x, ky - 3, crestHi);
                Set(p, BW, BH, x, ky - 2, crestMid);
                Set(p, BW, BH, x, ky - 1, crestDark);
            }
            FillRect(p, BW, BH, 17, ky - 1, 10, 2, bronzeMid);

            // Skullcap & face hung low
            FillCircle(p, BW, BH, 22, ky + 4, 6, bronzeMid);
            HLine(p, BW, BH, 17, 27, ky + 2, bronzeHi);
            // Cheek guards
            VLine(p, BW, BH, 16, ky + 4, ky + 9, bronzeMid);
            VLine(p, BW, BH, 28, ky + 4, ky + 9, bronzeMid);
            // Face looking down in pain
            FillRect(p, BW, BH, 18, ky + 4, 8, 4, skinMid);
            HLine(p, BW, BH, 19, 25, ky + 5, skinDark);
            Set(p, BW, BH, 20, ky + 6, Color{ 220, 80, 80, 255 }); // Eyes grimacing shut
            Set(p, BW, BH, 24, ky + 6, Color{ 220, 80, 80, 255 });
            HLine(p, BW, BH, 20, 24, ky + 8, Color{ 150, 40, 40, 255 }); // Grimacing mouth

            // Cuirass tilted
            FillRect(p, BW, BH, 14, ky + 11, 18, 14, bronzeMid);
            FillCircle(p, BW, BH, 18, ky + 14, 3, bronzeHi);
            FillCircle(p, BW, BH, 28, ky + 14, 3, bronzeHi);
            // Shattered lion emblem with fatal wound crack
            FillCircle(p, BW, BH, 23, ky + 15, 2, goldPure);
            Set(p, BW, BH, 23, ky + 15, Color{ 255, 60, 40, 255 }); // Glowing wound penetration!
            Set(p, BW, BH, 24, ky + 16, Color{ 255, 60, 40, 255 });

            // Left hand clutching the wound
            FillRect(p, BW, BH, 20, ky + 14, 5, 4, skinMuscle);
            HLine(p, BW, BH, 21, 24, ky + 14, skinHi);
            // Left arm coming from shoulder
            FillRect(p, BW, BH, 12, ky + 13, 8, 4, skinMuscle);

            // Right arm limp, fallen to side, whip dropped on floor
            FillRect(p, BW, BH, 32, ky + 14, 4, 12, skinMuscle);
            FillRect(p, BW, BH, 33, ky + 26, 3, 3, leatherDark);
            // Dropped whip on cobblestones
            HLine(p, BW, BH, 34, 44, 52, leather);
            Set(p, BW, BH, 45, 52, goldPure);

            // Pteruges & Tunic draped
            FillRect(p, BW, BH, 14, ky + 25, 18, 3, leatherDark);
            FillRect(p, BW, BH, 14, ky + 28, 18, 2, Color{ 185, 25, 32, 255 });

            // Bent legs: Left knee firmly on ground, right knee raised
            FillCircle(p, BW, BH, 16, 44, 3, bronzeHi);
            FillRect(p, BW, BH, 14, 47, 5, 6, bronzeMid);
            HLine(p, BW, BH, 13, 19, 53, leatherDark);

            // Left knee touching ground
            FillRect(p, BW, BH, 24, 46, 8, 6, bronzeMid);
            HLine(p, BW, BH, 24, 31, 52, bronzeHi); // Greave on ground
            HLine(p, BW, BH, 23, 32, 53, leatherDark);
            return;
        } else if (state == 5) {
            // --- STATE 5: WITHERING INTO ASH (Cracking magma fissures & disintegrating armor) ---
            Color ashCharcoal = Color{ 48, 42, 50, 255 };
            Color ashMid      = Color{ 72, 64, 75, 255 };
            Color ashLight    = Color{ 105, 95, 110, 255 };
            Color emberCore   = Color{ 255, 240, 110, 255 };
            Color emberFire   = Color{ 255, 110,  30, 255 };
            Color emberDark   = Color{ 180,  40,  15, 255 };

            int wy = 16; // Crumbling downward

            // Crumbling crest dissolving into drifting sparks
            for (int x = 14; x <= 31; ++x) {
                if ((x + wy) % 2 == 0) {
                    Set(p, BW, BH, x, wy - 2, ashMid);
                    Set(p, BW, BH, x, wy - 1, emberFire);
                }
            }
            // Ascending ember motes
            Set(p, BW, BH, 16, wy - 6, emberCore);
            Set(p, BW, BH, 23, wy - 8, emberFire);
            Set(p, BW, BH, 29, wy - 5, emberCore);

            // Cracked, crumbling skullcap
            FillCircle(p, BW, BH, 22, wy + 4, 5, ashCharcoal);
            HLine(p, BW, BH, 18, 26, wy + 3, ashMid);
            Set(p, BW, BH, 21, wy + 2, emberCore);
            Set(p, BW, BH, 22, wy + 3, emberFire);
            Set(p, BW, BH, 23, wy + 4, emberFire);

            // Body disintegrating into stone & ash with deep fissures
            FillRect(p, BW, BH, 14, wy + 9, 18, 16, ashCharcoal);
            for (int y = wy + 9; y <= wy + 25; ++y) {
                for (int x = 14; x <= 31; ++x) {
                    if ((x ^ y) % 3 == 0) Set(p, BW, BH, x, y, ashMid);
                    if ((x * y) % 7 == 0) Set(p, BW, BH, x, y, ashLight);
                }
            }

            // Jagged glowing fissures of judgment ripping through the torso
            VLine(p, BW, BH, 22, wy + 10, wy + 24, emberFire);
            VLine(p, BW, BH, 23, wy + 11, wy + 23, emberCore);
            HLine(p, BW, BH, 16, 29, wy + 16, emberFire);
            HLine(p, BW, BH, 18, 27, wy + 17, emberCore);
            HLine(p, BW, BH, 15, 30, wy + 22, emberDark);

            // Lower collapsed mass settling onto ground
            FillRect(p, BW, BH, 10, 44, 26, 9, ashCharcoal);
            for (int x = 10; x <= 35; ++x) {
                Set(p, BW, BH, x, 52, ((x % 3 == 0) ? emberFire : ashMid));
                Set(p, BW, BH, x, 53, ashCharcoal);
            }
            return;
        } else if (state == 6) {
            // --- STATE 6: ASH HEAP & EMBERS (Dissolved completely into the stone floor) ---
            Color ashCharcoal = Color{ 42, 36, 44, 255 };
            Color ashDark     = Color{ 60, 52, 62, 255 };
            Color ashLight    = Color{ 90, 80, 95, 255 };
            Color emberFire   = Color{ 255, 95, 25, 255 };
            Color emberGold   = Color{ 255, 215, 70, 255 };

            // Low collapsed mound of ash along ground (y = 44 to 53)
            FillEllipse(p, BW, BH, 23, 49, 16, 4, ashCharcoal);
            FillEllipse(p, BW, BH, 23, 48, 12, 3, ashDark);
            HLine(p, BW, BH, 14, 32, 47, ashLight);

            // Shattered bronze fragments of lion buckle and crest
            Set(p, BW, BH, 21, 48, COLOR_GOLD_BRIGHT);
            Set(p, BW, BH, 22, 48, COLOR_ROMAN_GOLD);
            Set(p, BW, BH, 26, 49, COLOR_BRONZE);

            // Glowing dying embers in the ash mound
            Set(p, BW, BH, 15, 50, emberFire);
            Set(p, BW, BH, 19, 49, emberGold);
            Set(p, BW, BH, 24, 50, emberFire);
            Set(p, BW, BH, 28, 49, emberGold);
            Set(p, BW, BH, 31, 51, emberFire);

            // Fading smoke/ember sparks rising into the air
            Set(p, BW, BH, 18, 41, emberGold);
            Set(p, BW, BH, 23, 38, emberFire);
            Set(p, BW, BH, 27, 43, emberGold);
            return;
        }

        // --- TRANSVERSE SCARLET HORSEHAIR CREST (Massive arching sideways plume!) ---
        for (int x = 11; x <= 35; ++x) {
            Set(p, BW, BH, x, 0, crestHi);
            Set(p, BW, BH, x, 1, crestHi);
            Set(p, BW, BH, x, 2, crestMid);
            Set(p, BW, BH, x, 3, crestMid);
            Set(p, BW, BH, x, 4, (x >= 17 && x <= 29) ? bronzeHi : crestDark);
        }
        // Crest box mount with lion engraving
        FillRect(p, BW, BH, 17, 4, 12, 2, bronzeMid);
        HLine(p, BW, BH, 17, 28, 4, bronzeHi);

        // --- CENTURION HELMET & WARRIOR FACE ---
        // Polished Bronze Helmet Skullcap
        FillCircle(p, BW, BH, 23, 9, 6, bronzeMid);
        HLine(p, BW, BH, 17, 29, 6, bronzeHi);
        HLine(p, BW, BH, 18, 28, 5, bronzeHi);
        // Brow reinforcement visor band
        HLine(p, BW, BH, 17, 29, 9, bronzeHi);
        // Cheek Guards & Neck Flange
        VLine(p, BW, BH, 16, 9, 14, bronzeMid);
        VLine(p, BW, BH, 30, 9, 14, bronzeMid);
        Set(p, BW, BH, 16, 11, bronzeHi); // Rivet
        Set(p, BW, BH, 30, 11, bronzeHi);

        // Menacing Human Warrior Face
        // Forehead
        FillRect(p, BW, BH, 18, 8, 10, 2, skinMid);
        // Fierce, Burning Warrior Eyes & Furrowed Brow
        HLine(p, BW, BH, 18, 28, 9, skinDark); // Furrowed brow shadow
        Set(p, BW, BH, 20, 10, eyeGlow);
        Set(p, BW, BH, 21, 10, Color{ 255, 255, 255, 255 }); // Intense pupil
        Set(p, BW, BH, 25, 10, eyeGlow);
        Set(p, BW, BH, 26, 10, Color{ 255, 255, 255, 255 });
        // Chiselled Roman Nose & Cheekbones
        VLine(p, BW, BH, 23, 10, 13, skinHi); // Nose bridge
        Set(p, BW, BH, 24, 12, skinHi);      // Nose tip
        Set(p, BW, BH, 19, 12, skinMuscle);  // Cheekbone
        Set(p, BW, BH, 27, 12, skinMuscle);
        // War Scar across left cheek
        Set(p, BW, BH, 20, 12, Color{ 220, 80, 80, 255 });
        Set(p, BW, BH, 21, 13, Color{ 220, 80, 80, 255 });
        // Grim Mouth & Strong Chiselled Jaw
        HLine(p, BW, BH, 21, 25, 13, Color{ 175, 90, 75, 255 });
        FillRect(p, BW, BH, 20, 14, 7, 2, skinDark);
        HLine(p, BW, BH, 21, 25, 15, skinMuscle); // Heavy chin

        // --- SCULPTED BRONZE MUSCLE CUIRASS (Lorica Musculata) ---
        // Anatomical Pectoral Plates
        FillRect(p, BW, BH, 14, 16, 18, 15, bronzeMid);
        // Left Pectoral
        FillCircle(p, BW, BH, 18, 20, 3, bronzeHi);
        Set(p, BW, BH, 18, 19, goldPure);
        // Right Pectoral
        FillCircle(p, BW, BH, 28, 20, 3, bronzeHi);
        Set(p, BW, BH, 28, 19, goldPure);
        // Sternum & Epigastric Fossa
        VLine(p, BW, BH, 23, 17, 30, bronzeDark);
        HLine(p, BW, BH, 16, 30, 23, bronzeDark);

        // Embossed Golden Roaring Lion Gorgoneion Emblem!
        FillCircle(p, BW, BH, 23, 21, 2, goldPure);
        Set(p, BW, BH, 22, 20, Color{ 255, 255, 255, 255 }); // Mane highlight
        Set(p, BW, BH, 24, 20, Color{ 255, 255, 255, 255 });
        Set(p, BW, BH, 22, 21, Color{ 255, 40, 40, 255 }); // Ruby eyes on lion
        Set(p, BW, BH, 24, 21, Color{ 255, 40, 40, 255 });
        Set(p, BW, BH, 23, 22, bronzeDark); // Roaring mouth

        // Anatomical Rectus Abdominis ("Six-Pack") & Rib Arches
        FillRect(p, BW, BH, 17, 24, 4, 3, bronzeHi);
        FillRect(p, BW, BH, 25, 24, 4, 3, bronzeHi);
        FillRect(p, BW, BH, 17, 28, 4, 3, bronzeHi);
        FillRect(p, BW, BH, 25, 28, 4, 3, bronzeHi);
        HLine(p, BW, BH, 16, 30, 27, bronzeDark);

        // Massive Spiked Pauldrons with Lion Bosses
        FillCircle(p, BW, BH, 13, 18, 4, bronzeMid);
        FillCircle(p, BW, BH, 13, 18, 2, bronzeHi);
        Set(p, BW, BH, 13, 17, Color{ 255, 255, 255, 230 }); // Specular reflection
        Set(p, BW, BH, 10, 15, goldPure); // Spike
        Set(p, BW, BH, 12, 14, goldPure);
        Set(p, BW, BH, 14, 13, goldPure);
        FillCircle(p, BW, BH, 33, 18, 4, bronzeMid);
        FillCircle(p, BW, BH, 33, 18, 2, bronzeHi);
        Set(p, BW, BH, 33, 17, Color{ 255, 255, 255, 230 }); // Specular reflection
        Set(p, BW, BH, 36, 15, goldPure); // Spike
        Set(p, BW, BH, 34, 14, goldPure);
        Set(p, BW, BH, 32, 13, goldPure);

        // Studded War Belt (Balteus) with Triple-Layered Pteruges
        FillRect(p, BW, BH, 13, 31, 20, 4, leatherDark);
        HLine(p, BW, BH, 13, 32, 32, bronzeHi);
        FillCircle(p, BW, BH, 23, 33, 2, goldPure); // Lion buckle
        // Hanging Leather Pteruges with Brass Leaf Terminals
        for (int x = 14; x <= 32; x += 3) {
            VLine(p, BW, BH, x, 35, 41, leather);
            Set(p, BW, BH, x, 40, bronzeMid);
            Set(p, BW, BH, x, 41, bronzeHi); // Brass terminal leaf
        }
        // Red officer tunic skirt showing underneath
        FillRect(p, BW, BH, 14, 38, 18, 3, Color{ 185, 25, 32, 255 });

        // --- MUSCULAR GREAVED LEGS & COMBAT BOOTS ---
        // Muscular Thighs
        FillRect(p, BW, BH, 16, 41, 5, 2, skinMuscle);
        FillRect(p, BW, BH, 25, 41, 5, 2, skinMuscle);
        // Heavy Bronze Greaves (Ocreae with anatomical knee plates)
        FillRect(p, BW, BH, 15, 43, 6, 9, bronzeMid);
        FillCircle(p, BW, BH, 18, 43, 2, bronzeHi); // Knee plate
        HLine(p, BW, BH, 16, 19, 45, bronzeHi);     // Shin highlight
        FillRect(p, BW, BH, 25, 43, 6, 9, bronzeMid);
        FillCircle(p, BW, BH, 28, 43, 2, bronzeHi);
        HLine(p, BW, BH, 26, 29, 45, bronzeHi);
        // Heavy Strapped Combat Boots
        FillRect(p, BW, BH, 14, 52, 8, 2, leatherDark);
        FillRect(p, BW, BH, 24, 52, 8, 2, leatherDark);
        HLine(p, BW, BH, 14, 21, 52, bronzeMid);
        HLine(p, BW, BH, 24, 31, 52, bronzeMid);

        // --- MUSCULAR ARMS & COMBAT WHIP STATES ---
        if (state == 0) {
            // Idle / Walk: Massive arm draped at side, whip coiled
            FillRect(p, BW, BH, 30, 20, 6, 6, skinMuscle);
            HLine(p, BW, BH, 31, 35, 20, skinHi);
            // Spiked leather bracer
            FillRect(p, BW, BH, 32, 24, 5, 5, leatherDark);
            Set(p, BW, BH, 34, 25, bronzeHi);
            // Coiled braided leather whip
            VLine(p, BW, BH, 35, 26, 36, leather);
            VLine(p, BW, BH, 36, 28, 38, leatherDark);
            Set(p, BW, BH, 35, 38, goldPure); // Spiked weight
        } else if (state == 1) {
            // Whip Prepare / Windup: Arm raised overhead, flexing bicep!
            FillRect(p, BW, BH, 28, 8, 7, 7, skinMuscle);
            HLine(p, BW, BH, 29, 34, 8, skinHi);
            // Forearm raised high
            FillRect(p, BW, BH, 32, 2, 5, 8, skinMuscle);
            FillRect(p, BW, BH, 33, 4, 4, 4, leatherDark);
            Set(p, BW, BH, 35, 5, bronzeHi); // Spiked knuckle
            // Whip swirling overhead
            HLine(p, BW, BH, 24, 42, 2, leather);
            Set(p, BW, BH, 43, 2, goldPure);
        } else if (state == 2) {
            // Whip Horizontal Crack: Arm lunged straight forward at shoulder height!
            FillRect(p, BW, BH, 30, 18, 14, 5, skinMuscle);
            HLine(p, BW, BH, 31, 43, 18, skinHi);
            // Spiked leather gauntlet
            FillRect(p, BW, BH, 38, 18, 5, 5, leatherDark);
            Set(p, BW, BH, 40, 19, bronzeHi);
            Set(p, BW, BH, 44, 20, goldPure); // Whip handle socket
        } else if (state == 3) {
            // Whip Overhead Slam: Powerful high leap / downward slam!
            FillRect(p, BW, BH, 28, 6, 6, 12, skinMuscle);
            HLine(p, BW, BH, 29, 33, 6, skinHi);
            VLine(p, BW, BH, 31, 4, 16, leather);
            Set(p, BW, BH, 31, 3, goldPure);
        }
    };

    PaintBoss(boss.data(), 0, false);
    s_sprites[SPR_BOSS_IDLE] = CreateTextureFromPixels(BW, BH, boss.data());
    PaintBoss(boss.data(), 0, false);
    s_sprites[SPR_BOSS_WALK_0] = CreateTextureFromPixels(BW, BH, boss.data());
    PaintBoss(boss.data(), 0, false);
    s_sprites[SPR_BOSS_WALK_1] = CreateTextureFromPixels(BW, BH, boss.data());
    PaintBoss(boss.data(), 1, false);
    s_sprites[SPR_BOSS_WHIP_PREPARE] = CreateTextureFromPixels(BW, BH, boss.data());
    PaintBoss(boss.data(), 2, false);
    s_sprites[SPR_BOSS_WHIP_CRACK] = CreateTextureFromPixels(BW, BH, boss.data());
    PaintBoss(boss.data(), 3, false);
    s_sprites[SPR_BOSS_WHIP_SLAM] = CreateTextureFromPixels(BW, BH, boss.data());
    PaintBoss(boss.data(), 0, true);
    s_sprites[SPR_BOSS_HURT] = CreateTextureFromPixels(BW, BH, boss.data());
    PaintBoss(boss.data(), 4, false);
    s_sprites[SPR_BOSS_KNEEL] = CreateTextureFromPixels(BW, BH, boss.data());
    PaintBoss(boss.data(), 5, false);
    s_sprites[SPR_BOSS_WITHER_0] = CreateTextureFromPixels(BW, BH, boss.data());
    PaintBoss(boss.data(), 6, false);
    s_sprites[SPR_BOSS_WITHER_1] = CreateTextureFromPixels(BW, BH, boss.data());

    // 7. Projectiles & Collectibles
    // Iron Roman Spike (18x12 pixels - Thick forged iron with radiant holy aura!)
    constexpr int NW = 18, NH = 12;
    std::vector<Color> nail(NW * NH, BLANK);
    for (int y = 0; y < NH; ++y) {
        for (int x = 0; x < NW; ++x) {
            float dx = x - 9.0f;
            float dy = y - 5.5f;
            if (dx * dx * 0.4f + dy * dy <= 14.0f) {
                nail[y * NW + x] = Color{ 255, 230, 90, 140 };
            }
        }
    }
    FillRect(nail.data(), NW, NH, 3, 4, 9, 4, COLOR_IRON);
    HLine(nail.data(), NW, NH, 3, 11, 4, Color{ 255, 255, 255, 255 });
    HLine(nail.data(), NW, NH, 4, 11, 5, COLOR_STEEL_BRIGHT);
    HLine(nail.data(), NW, NH, 3, 11, 7, Color{ 60, 65, 75, 255 });
    FillRect(nail.data(), NW, NH, 12, 3, 5, 6, COLOR_STEEL_BRIGHT);
    OutlineRect(nail.data(), NW, NH, 12, 3, 5, 6, Color{ 60, 65, 75, 255 });
    Set(nail.data(), NW, NH, 13, 4, Color{ 255, 255, 255, 255 });
    Set(nail.data(), NW, NH, 2, 5, Color{ 255, 255, 255, 255 });
    Set(nail.data(), NW, NH, 1, 5, Color{ 255, 255, 255, 255 });
    Set(nail.data(), NW, NH, 0, 5, Color{ 255, 255, 255, 255 });
    Set(nail.data(), NW, NH, 2, 6, COLOR_STEEL_BRIGHT);
    s_sprites[SPR_NAIL] = CreateTextureFromPixels(NW, NH, nail.data());

    // Arrow (20x10 pixels - Highly visible broadhead & scarlet eagle fletchings!)
    constexpr int AW = 20, AH = 10;
    std::vector<Color> arrow(AW * AH, BLANK);
    FillRect(arrow.data(), AW, AH, 4, 4, 10, 2, Color{ 145, 95, 50, 255 });
    HLine(arrow.data(), AW, AH, 4, 13, 4, Color{ 190, 135, 75, 255 });
    FillRect(arrow.data(), AW, AH, 14, 3, 4, 4, COLOR_STEEL_BRIGHT);
    HLine(arrow.data(), AW, AH, 14, 18, 4, Color{ 255, 255, 255, 255 });
    Set(arrow.data(), AW, AH, 19, 4, Color{ 255, 255, 255, 255 });
    Set(arrow.data(), AW, AH, 14, 2, COLOR_STEEL_BRIGHT);
    Set(arrow.data(), AW, AH, 14, 6, COLOR_STEEL_BRIGHT);
    for (int fx = 0; fx <= 4; ++fx) {
        Set(arrow.data(), AW, AH, fx, 2, COLOR_ROMAN_CRIMSON);
        Set(arrow.data(), AW, AH, fx, 3, Color{ 255, 255, 255, 255 });
        Set(arrow.data(), AW, AH, fx, 6, Color{ 255, 255, 255, 255 });
        Set(arrow.data(), AW, AH, fx, 7, COLOR_ROMAN_CRIMSON);
    }
    s_sprites[SPR_ARROW] = CreateTextureFromPixels(AW, AH, arrow.data());

    // Spiked Whip Head (14x14 pixels - Massive 4-spiked steel morningstar orb!)
    constexpr int WHW = 14, WHH = 14;
    std::vector<Color> whipHead(WHW * WHH, BLANK);
    FillCircle(whipHead.data(), WHW, WHH, 7, 7, 4, COLOR_BRONZE);
    Set(whipHead.data(), WHW, WHH, 6, 6, Color{ 255, 235, 120, 255 });
    VLine(whipHead.data(), WHW, WHH, 7, 0, 3, COLOR_STEEL_BRIGHT);
    VLine(whipHead.data(), WHW, WHH, 7, 10, 13, COLOR_STEEL_BRIGHT);
    HLine(whipHead.data(), WHW, WHH, 0, 3, 7, COLOR_STEEL_BRIGHT);
    HLine(whipHead.data(), WHW, WHH, 10, 13, 7, COLOR_STEEL_BRIGHT);
    Set(whipHead.data(), WHW, WHH, 7, 0, Color{ 255, 255, 255, 255 });
    Set(whipHead.data(), WHW, WHH, 0, 7, Color{ 255, 255, 255, 255 });
    Set(whipHead.data(), WHW, WHH, 13, 7, Color{ 255, 255, 255, 255 });
    Set(whipHead.data(), WHW, WHH, 7, 13, Color{ 255, 255, 255, 255 });
    s_sprites[SPR_WHIP_HEAD] = CreateTextureFromPixels(WHW, WHH, whipHead.data());

    // Loaves and Fishes Wicker Basket (22x18)
    constexpr int BSK_W = 22, BSK_H = 18;
    std::vector<Color> bsk(BSK_W * BSK_H, BLANK);
    Color wickerHi   = Color{ 215, 165,  90, 255 };
    Color wickerMid  = Color{ 165, 115,  55, 255 };
    Color wickerDark = Color{ 110,  70,  30, 255 };
    Color breadHi    = Color{ 245, 195, 100, 255 };
    Color breadCrust = Color{ 175, 105,  40, 255 };
    Color fishSilver = Color{ 190, 220, 235, 255 };
    Color fishBlue   = Color{ 105, 155, 185, 255 };
    Color fishEye    = Color{  30,  50,  70, 255 };

    // Woven wicker body with textured cross-weave
    for (int y = 7; y <= 16; ++y) {
        int inset = (y >= 14) ? 3 : 1;
        for (int x = 2 + inset; x <= 19 - inset; ++x) {
            Color c = ((x + y) % 2 == 0) ? wickerHi : wickerMid;
            if (y == 16) c = wickerDark;
            bsk[y * BSK_W + x] = c;
        }
    }
    // Braided rim
    HLine(bsk.data(), BSK_W, BSK_H, 2, 19, 6, wickerHi);
    HLine(bsk.data(), BSK_W, BSK_H, 2, 19, 7, wickerDark);

    // Two Silver Fishes with blue sheen
    HLine(bsk.data(), BSK_W, BSK_H, 4, 12, 5, fishSilver);
    HLine(bsk.data(), BSK_W, BSK_H, 5, 11, 4, fishBlue);
    Set(bsk.data(), BSK_W, BSK_H, 11, 5, fishEye); // Eye
    HLine(bsk.data(), BSK_W, BSK_H, 8, 16, 4, fishSilver);
    HLine(bsk.data(), BSK_W, BSK_H, 9, 15, 3, fishBlue);
    Set(bsk.data(), BSK_W, BSK_H, 15, 4, fishEye);

    // Golden Baked Loaves with scored crust
    FillRect(bsk.data(), BSK_W, BSK_H, 5, 2, 4, 2, breadHi);
    Set(bsk.data(), BSK_W, BSK_H, 6, 2, breadCrust); // Score mark
    FillRect(bsk.data(), BSK_W, BSK_H, 12, 2, 4, 2, breadHi);
    Set(bsk.data(), BSK_W, BSK_H, 13, 2, breadCrust);
    s_sprites[SPR_BASKET_LOAVES_FISHES] = CreateTextureFromPixels(BSK_W, BSK_H, bsk.data());

    // UI Heart Pip (10x10)
    constexpr int IW = 10, IH = 10;
    std::vector<Color> heart(IW * IH, BLANK);
    Color redRuby = Color{ 245, 35, 45, 255 };
    FillRect(heart.data(), IW, IH, 2, 2, 3, 2, redRuby);
    FillRect(heart.data(), IW, IH, 6, 2, 3, 2, redRuby);
    FillRect(heart.data(), IW, IH, 1, 3, 9, 3, redRuby);
    FillRect(heart.data(), IW, IH, 2, 6, 7, 2, redRuby);
    FillRect(heart.data(), IW, IH, 3, 8, 5, 1, redRuby);
    Set(heart.data(), IW, IH, 5, 9, redRuby);
    Set(heart.data(), IW, IH, 3, 3, Color{ 255, 180, 180, 255 }); // Highlight
    s_sprites[SPR_ICON_HEART] = CreateTextureFromPixels(IW, IH, heart.data());

    // UI Wrath Icon (12x12)
    constexpr int WRI_W = 12, WRI_H = 12;
    std::vector<Color> wrathIco(WRI_W * WRI_H, BLANK);
    FillRect(wrathIco.data(), WRI_W, WRI_H, 2, 2, 8, 8, COLOR_GOLD_BRIGHT);
    OutlineRect(wrathIco.data(), WRI_W, WRI_H, 1, 1, 10, 10, COLOR_ROMAN_CRIMSON);
    Set(wrathIco.data(), WRI_W, WRI_H, 4, 5, Color{ 60, 20, 10, 255 });
    Set(wrathIco.data(), WRI_W, WRI_H, 7, 5, Color{ 60, 20, 10, 255 });
    HLine(wrathIco.data(), WRI_W, WRI_H, 5, 6, 8, Color{ 60, 20, 10, 255 });
    s_sprites[SPR_ICON_WRATH] = CreateTextureFromPixels(WRI_W, WRI_H, wrathIco.data());
    s_sprites[SPR_ICON_BASKET] = s_sprites[SPR_BASKET_LOAVES_FISHES];

    // =========================================================================
    // SICARII SHADOW ASSASSIN (Judean Zealot Dagger-Man / Shinobi Wall-Crawler)
    // =========================================================================
    constexpr int SIC_W = 24, SIC_H = 32;
    std::vector<Color> sic(SIC_W * SIC_H);

    auto PaintSicarii = [&](Color* p, int action) {
        // action: 0=Cling, 1=Crawl0, 2=Crawl1, 3=Hop, 4=Stand, 5=Slash, 6=Throw, 7=Hurt
        for (int i = 0; i < SIC_W * SIC_H; ++i) p[i] = BLANK;

        Color cowlHi     = Color{ 48,  56,  80, 255 };
        Color cowlMid    = COLOR_SICARII_DARK;          // 28, 32, 48
        Color cowlDark   = Color{ 16,  18,  28, 255 };
        Color wrapHi     = Color{ 70,  92, 138, 255 };
        Color wrapMid    = COLOR_SICARII_BLUE;          // 48, 65, 98
        Color wrapDark   = Color{ 30,  40,  62, 255 };
        Color skinHi     = Color{ 220, 168, 122, 255 };
        Color skinMid    = Color{ 180, 128,  88, 255 };
        Color skinDark   = Color{ 130,  85,  52, 255 };
        (void)skinDark;
        Color eyeAmber   = Color{ 255, 215,  55, 255 }; // Fierce piercing gaze in hood shadow
        Color leatherMid = Color{ 110,  68,  34, 255 };
        Color leatherHi  = Color{ 165, 110,  55, 255 };
        Color bladeEdge  = COLOR_SICARII_BLADE;         // 230, 240, 255
        Color bronzeGleam= Color{ 245, 195,  70, 255 };

        if (action == 0) {
            // --- 0: WALL CLING (Clinging flat vertically to column/wall) ---
            FillRect(p, SIC_W, SIC_H, 4, 8, 9, 14, cowlMid);
            HLine(p, SIC_W, SIC_H, 4, 12, 8, cowlDark);
            VLine(p, SIC_W, SIC_H, 4, 8, 21, cowlDark);
            FillCircle(p, SIC_W, SIC_H, 8, 5, 4, cowlMid);
            FillCircle(p, SIC_W, SIC_H, 8, 5, 3, cowlDark);
            FillRect(p, SIC_W, SIC_H, 9, 4, 4, 4, Color{ 10, 12, 18, 255 });
            Set(p, SIC_W, SIC_H, 10, 5, eyeAmber);
            Set(p, SIC_W, SIC_H, 11, 5, Color{ 255, 255, 255, 240 });
            FillRect(p, SIC_W, SIC_H, 2, 2, 4, 3, skinMid);
            HLine(p, SIC_W, SIC_H, 2, 5, 2, skinHi);
            VLine(p, SIC_W, SIC_H, 2, 1, 4, leatherMid);
            FillRect(p, SIC_W, SIC_H, 11, 10, 3, 4, skinMid);
            HLine(p, SIC_W, SIC_H, 13, 19, 14, bladeEdge);
            Set(p, SIC_W, SIC_H, 19, 13, bladeEdge);
            Set(p, SIC_W, SIC_H, 12, 14, bronzeGleam);
            FillRect(p, SIC_W, SIC_H, 3, 22, 6, 4, wrapMid);
            FillRect(p, SIC_W, SIC_H, 2, 25, 4, 3, skinMid);
            HLine(p, SIC_W, SIC_H, 1, 4, 28, leatherMid);
            VLine(p, SIC_W, SIC_H, 13, 15, 25, cowlMid);
            VLine(p, SIC_W, SIC_H, 14, 18, 27, cowlDark);
        }
        else if (action == 1 || action == 2) {
            // --- 1 & 2: WALL CRAWL (Sliding down vertical column) ---
            int phase = (action == 1) ? 0 : 1;
            FillRect(p, SIC_W, SIC_H, 5, 9, 8, 12, cowlMid);
            OutlineRect(p, SIC_W, SIC_H, 5, 9, 8, 12, cowlDark);
            FillCircle(p, SIC_W, SIC_H, 9, 5, 4, cowlMid);
            FillRect(p, SIC_W, SIC_H, 8, 5, 4, 3, Color{ 10, 12, 18, 255 });
            Set(p, SIC_W, SIC_H, 9, 6, eyeAmber);
            if (phase == 0) {
                FillRect(p, SIC_W, SIC_H, 3, 1, 3, 6, skinMid);
                Set(p, SIC_W, SIC_H, 2, 1, leatherMid);
                FillRect(p, SIC_W, SIC_H, 12, 16, 4, 7, skinMid);
                Set(p, SIC_W, SIC_H, 15, 23, leatherMid);
                FillRect(p, SIC_W, SIC_H, 4, 21, 6, 5, wrapMid);
                FillRect(p, SIC_W, SIC_H, 2, 26, 4, 3, skinMid);
                Line(p, SIC_W, SIC_H, 13, 10, 18, 16, bladeEdge);
            } else {
                FillRect(p, SIC_W, SIC_H, 12, 2, 3, 6, skinMid);
                Set(p, SIC_W, SIC_H, 14, 2, leatherMid);
                FillRect(p, SIC_W, SIC_H, 3, 16, 4, 7, skinMid);
                Set(p, SIC_W, SIC_H, 2, 23, leatherMid);
                FillRect(p, SIC_W, SIC_H, 6, 21, 6, 5, wrapMid);
                FillRect(p, SIC_W, SIC_H, 5, 26, 4, 3, skinMid);
                Line(p, SIC_W, SIC_H, 12, 11, 17, 17, bladeEdge);
            }
            Line(p, SIC_W, SIC_H, 11, 10, 16, 4, cowlMid);
            Line(p, SIC_W, SIC_H, 12, 11, 18, 5, cowlDark);
        }
        else if (action == 3) {
            // --- 3: HOP (Shinobi acrobatic mid-air flip) ---
            FillCircle(p, SIC_W, SIC_H, 12, 14, 9, cowlMid);
            OutlineCircle(p, SIC_W, SIC_H, 12, 14, 9, cowlDark);
            FillCircle(p, SIC_W, SIC_H, 11, 11, 4, cowlDark);
            Set(p, SIC_W, SIC_H, 13, 10, eyeAmber);
            Set(p, SIC_W, SIC_H, 14, 10, Color{ 255, 255, 255, 240 });
            Line(p, SIC_W, SIC_H, 7, 14, 17, 14, wrapHi);
            Line(p, SIC_W, SIC_H, 6, 16, 18, 16, wrapMid);
            FillRect(p, SIC_W, SIC_H, 7, 21, 4, 4, leatherMid);
            FillRect(p, SIC_W, SIC_H, 13, 21, 4, 4, leatherMid);
            Line(p, SIC_W, SIC_H, 18, 6, 22, 12, bladeEdge);
            Set(p, SIC_W, SIC_H, 23, 11, Color{ 255, 255, 255, 255 });
            Line(p, SIC_W, SIC_H, 3, 18, 7, 24, bladeEdge);
        }
        else if (action == 4) {
            // --- 4: STAND (Agile combat stance) ---
            FillCircle(p, SIC_W, SIC_H, 11, 6, 4, cowlMid);
            HLine(p, SIC_W, SIC_H, 8, 14, 3, cowlHi);
            FillRect(p, SIC_W, SIC_H, 9, 6, 6, 4, Color{ 12, 14, 22, 255 });
            Set(p, SIC_W, SIC_H, 12, 7, eyeAmber);
            Set(p, SIC_W, SIC_H, 13, 7, Color{ 255, 255, 255, 240 });
            HLine(p, SIC_W, SIC_H, 10, 14, 9, wrapMid);
            HLine(p, SIC_W, SIC_H, 11, 13, 10, wrapDark);
            FillRect(p, SIC_W, SIC_H, 7, 10, 10, 3, cowlMid);
            HLine(p, SIC_W, SIC_H, 7, 16, 10, cowlHi);
            FillRect(p, SIC_W, SIC_H, 8, 13, 8, 8, wrapMid);
            Line(p, SIC_W, SIC_H, 8, 13, 15, 20, leatherMid);
            Line(p, SIC_W, SIC_H, 15, 13, 8, 20, leatherMid);
            Set(p, SIC_W, SIC_H, 11, 16, bronzeGleam);
            HLine(p, SIC_W, SIC_H, 7, 16, 21, leatherMid);
            Set(p, SIC_W, SIC_H, 11, 21, bronzeGleam);
            VLine(p, SIC_W, SIC_H, 6, 13, 24, cowlDark);
            VLine(p, SIC_W, SIC_H, 5, 15, 26, cowlMid);
            FillRect(p, SIC_W, SIC_H, 15, 14, 3, 4, skinMid);
            Line(p, SIC_W, SIC_H, 17, 17, 22, 19, bladeEdge);
            Set(p, SIC_W, SIC_H, 23, 18, bladeEdge);
            Set(p, SIC_W, SIC_H, 17, 17, bronzeGleam);
            FillRect(p, SIC_W, SIC_H, 5, 12, 3, 3, skinMid);
            Line(p, SIC_W, SIC_H, 4, 11, 1, 7, bladeEdge);
            Set(p, SIC_W, SIC_H, 1, 6, bladeEdge);
            FillRect(p, SIC_W, SIC_H, 7, 22, 4, 6, wrapDark);
            FillRect(p, SIC_W, SIC_H, 13, 22, 4, 6, wrapMid);
            FillRect(p, SIC_W, SIC_H, 6, 28, 5, 3, leatherMid);
            FillRect(p, SIC_W, SIC_H, 13, 28, 5, 3, leatherMid);
            HLine(p, SIC_W, SIC_H, 6, 11, 31, leatherHi);
            HLine(p, SIC_W, SIC_H, 13, 18, 31, leatherHi);
        }
        else if (action == 5) {
            // --- 5: SLASH (Double-bladed cross slash) ---
            FillCircle(p, SIC_W, SIC_H, 14, 6, 4, cowlMid);
            FillRect(p, SIC_W, SIC_H, 14, 6, 5, 3, Color{ 12, 14, 22, 255 });
            Set(p, SIC_W, SIC_H, 16, 7, eyeAmber);
            FillRect(p, SIC_W, SIC_H, 10, 11, 9, 8, wrapMid);
            Line(p, SIC_W, SIC_H, 9, 12, 1, 8, cowlMid);
            Line(p, SIC_W, SIC_H, 8, 13, 0, 10, cowlDark);
            Line(p, SIC_W, SIC_H, 8, 15, 2, 14, cowlMid);
            FillRect(p, SIC_W, SIC_H, 17, 11, 4, 3, skinMid);
            FillRect(p, SIC_W, SIC_H, 16, 15, 4, 3, skinMid);
            Line(p, SIC_W, SIC_H, 16, 6, 23, 17, bladeEdge);
            Line(p, SIC_W, SIC_H, 16, 22, 23, 11, bladeEdge);
            Set(p, SIC_W, SIC_H, 22, 14, Color{ 255, 255, 255, 255 });
            FillRect(p, SIC_W, SIC_H, 5, 22, 5, 6, wrapDark);
            FillRect(p, SIC_W, SIC_H, 15, 20, 5, 7, wrapMid);
            FillRect(p, SIC_W, SIC_H, 4, 28, 5, 3, leatherMid);
            FillRect(p, SIC_W, SIC_H, 17, 27, 6, 4, leatherMid);
        }
        else if (action == 6) {
            // --- 6: THROW (Spinning dagger toss) ---
            FillCircle(p, SIC_W, SIC_H, 10, 6, 4, cowlMid);
            FillRect(p, SIC_W, SIC_H, 10, 6, 5, 3, Color{ 12, 14, 22, 255 });
            Set(p, SIC_W, SIC_H, 12, 7, eyeAmber);
            FillRect(p, SIC_W, SIC_H, 7, 11, 8, 9, wrapMid);
            FillRect(p, SIC_W, SIC_H, 14, 10, 6, 3, skinMid);
            HLine(p, SIC_W, SIC_H, 14, 19, 10, skinHi);
            Set(p, SIC_W, SIC_H, 20, 11, skinHi);
            HLine(p, SIC_W, SIC_H, 20, 23, 11, Color{ 255, 255, 255, 220 });
            FillRect(p, SIC_W, SIC_H, 3, 13, 4, 3, skinMid);
            Line(p, SIC_W, SIC_H, 3, 14, 0, 18, bladeEdge);
            FillRect(p, SIC_W, SIC_H, 6, 21, 4, 7, wrapMid);
            FillRect(p, SIC_W, SIC_H, 12, 21, 5, 6, wrapDark);
            FillRect(p, SIC_W, SIC_H, 5, 28, 5, 3, leatherMid);
            FillRect(p, SIC_W, SIC_H, 14, 27, 5, 4, leatherMid);
        }
        else if (action == 7) {
            // --- 7: HURT ---
            Color hurtCowl = Color{ 110, 50, 70, 255 };
            Color hurtSkin = Color{ 255, 140, 140, 255 };
            FillCircle(p, SIC_W, SIC_H, 8, 6, 4, hurtCowl);
            FillRect(p, SIC_W, SIC_H, 6, 11, 8, 9, hurtCowl);
            FillRect(p, SIC_W, SIC_H, 13, 12, 4, 3, hurtSkin);
            FillRect(p, SIC_W, SIC_H, 2, 9, 4, 3, hurtSkin);
            FillRect(p, SIC_W, SIC_H, 4, 21, 5, 7, cowlDark);
            FillRect(p, SIC_W, SIC_H, 11, 21, 5, 6, cowlDark);
        }
    };

    PaintSicarii(sic.data(), 0);
    s_sprites[SPR_SICARII_WALL_CLING] = CreateTextureFromPixels(SIC_W, SIC_H, sic.data());
    PaintSicarii(sic.data(), 1);
    s_sprites[SPR_SICARII_WALL_CRAWL_0] = CreateTextureFromPixels(SIC_W, SIC_H, sic.data());
    PaintSicarii(sic.data(), 2);
    s_sprites[SPR_SICARII_WALL_CRAWL_1] = CreateTextureFromPixels(SIC_W, SIC_H, sic.data());
    PaintSicarii(sic.data(), 3);
    s_sprites[SPR_SICARII_HOP] = CreateTextureFromPixels(SIC_W, SIC_H, sic.data());
    PaintSicarii(sic.data(), 4);
    s_sprites[SPR_SICARII_STAND] = CreateTextureFromPixels(SIC_W, SIC_H, sic.data());
    PaintSicarii(sic.data(), 5);
    s_sprites[SPR_SICARII_SLASH] = CreateTextureFromPixels(SIC_W, SIC_H, sic.data());
    PaintSicarii(sic.data(), 6);
    s_sprites[SPR_SICARII_THROW] = CreateTextureFromPixels(SIC_W, SIC_H, sic.data());
    PaintSicarii(sic.data(), 7);
    s_sprites[SPR_SICARII_HURT] = CreateTextureFromPixels(SIC_W, SIC_H, sic.data());

    // Sicarii Curved Sica Dagger Projectile (8x8)
    constexpr int DAG_W = 8, DAG_H = 8;
    std::vector<Color> dagger(DAG_W * DAG_H, BLANK);
    Line(dagger.data(), DAG_W, DAG_H, 1, 6, 6, 1, COLOR_SICARII_BLADE);
    Set(dagger.data(), DAG_W, DAG_H, 7, 1, Color{ 255, 255, 255, 255 });
    Set(dagger.data(), DAG_W, DAG_H, 2, 5, Color{ 200, 220, 245, 255 });
    Set(dagger.data(), DAG_W, DAG_H, 5, 2, Color{ 255, 255, 255, 240 });
    Set(dagger.data(), DAG_W, DAG_H, 0, 7, Color{ 215, 160, 45, 255 });
    Set(dagger.data(), DAG_W, DAG_H, 1, 7, Color{ 160, 105, 30, 255 });
    s_sprites[SPR_SICARII_DAGGER] = CreateTextureFromPixels(DAG_W, DAG_H, dagger.data());

    // =========================================================================
    // TEMPLE BOSS: COMMANDER MALCHUS (Temple Guard Master) - 32x48
    // =========================================================================
    constexpr int TBW = 32, TBH = 48;
    std::vector<Color> tboss(TBW * TBH);

    auto PaintTempleBoss = [&](Color* p, int action) {
        // action: 0=Idle, 1=Walk0, 2=Walk1, 3=Slash, 4=Slam, 5=Hurt, 6=Kneel
        for (int i = 0; i < TBW * TBH; ++i) p[i] = BLANK;

        bool hurt = (action == 5);
        Color bronzeHi   = hurt ? Color{ 255, 160, 160, 255 } : Color{ 255, 230, 110, 255 };
        Color bronzeMid  = hurt ? Color{ 210,  90,  90, 255 } : Color{ 215, 140,  45, 255 };
        Color bronzeDark = hurt ? Color{ 140,  40,  40, 255 } : Color{ 145,  80,  20, 255 };
        Color purpleHi   = Color{ 155,  50, 185, 255 };
        Color purpleMid  = COLOR_TEMPLE_PURPLE; // 95, 25, 115
        Color purpleDark = Color{  55,  12,  68, 255 };
        Color goldTrim   = COLOR_TEMPLE_GOLD;
        Color skinHi     = Color{ 240, 195, 155, 255 };
        Color skinMid    = Color{ 200, 145, 105, 255 };
        Color skinDark   = Color{ 145,  95,  60, 255 };
        (void)skinDark;
        Color bladeSteel = Color{ 235, 245, 255, 255 };
        Color bladeDark  = Color{ 150, 170, 195, 255 };

        if (action == 6) {
            FillCircle(p, TBW, TBH, 14, 18, 5, skinMid);
            HLine(p, TBW, TBH, 10, 18, 14, Color{ 60, 40, 25, 255 });
            Set(p, TBW, TBH, 16, 18, Color{ 160, 60, 60, 255 });
            FillRect(p, TBW, TBH, 9, 23, 13, 10, bronzeMid);
            OutlineRect(p, TBW, TBH, 9, 23, 13, 10, bronzeDark);
            FillRect(p, TBW, TBH, 4, 33, 18, 6, purpleMid);
            HLine(p, TBW, TBH, 3, 23, 38, goldTrim);
            FillRect(p, TBW, TBH, 7, 34, 7, 8, bronzeMid);
            FillRect(p, TBW, TBH, 16, 30, 8, 12, bronzeMid);
            Line(p, TBW, TBH, 20, 42, 30, 43, bladeSteel);
            Set(p, TBW, TBH, 19, 42, bronzeHi);
            return;
        }

        int walkOff = (action == 2) ? 2 : ((action == 1) ? -2 : 0);

        for (int x = 11; x <= 19; ++x) {
            Set(p, TBW, TBH, x, 1, goldTrim);
            Set(p, TBW, TBH, x, 2, (x % 2 == 1) ? Color{ 255, 255, 255, 240 } : goldTrim);
        }
        FillCircle(p, TBW, TBH, 15, 6, 5, bronzeMid);
        HLine(p, TBW, TBH, 11, 19, 5, bronzeHi);
        HLine(p, TBW, TBH, 11, 20, 7, goldTrim);
        VLine(p, TBW, TBH, 17, 7, 11, bronzeHi);
        Set(p, TBW, TBH, 18, 8, Color{ 255, 255, 255, 255 });
        Set(p, TBW, TBH, 19, 8, Color{ 30, 20, 15, 255 });
        FillRect(p, TBW, TBH, 15, 11, 5, 4, Color{ 55, 40, 30, 255 });
        Set(p, TBW, TBH, 19, 10, skinMid);

        FillRect(p, TBW, TBH, 6, 13, 5, 22, purpleMid);
        VLine(p, TBW, TBH, 6, 13, 34, purpleDark);
        VLine(p, TBW, TBH, 7, 14, 34, purpleHi);
        HLine(p, TBW, TBH, 6, 11, 35, goldTrim);

        FillRect(p, TBW, TBH, 10, 13, 12, 14, bronzeMid);
        for (int y = 14; y <= 25; ++y) {
            for (int x = 11; x <= 20; ++x) {
                if ((x + y) % 2 == 0) Set(p, TBW, TBH, x, y, bronzeHi);
                else Set(p, TBW, TBH, x, y, bronzeDark);
            }
        }
        FillRect(p, TBW, TBH, 14, 16, 4, 4, goldTrim);
        Set(p, TBW, TBH, 15, 17, Color{ 255, 255, 255, 240 });
        HLine(p, TBW, TBH, 9, 22, 27, goldTrim);
        for (int x = 11; x <= 20; x += 2) {
            VLine(p, TBW, TBH, x, 28, 32, purpleDark);
            Set(p, TBW, TBH, x, 32, goldTrim);
        }

        if (action == 3) {
            FillRect(p, TBW, TBH, 18, 14, 8, 4, skinMid);
            HLine(p, TBW, TBH, 18, 25, 14, skinHi);
            Line(p, TBW, TBH, 25, 16, 31, 24, bladeSteel);
            Line(p, TBW, TBH, 24, 17, 30, 25, bladeDark);
            Set(p, TBW, TBH, 31, 25, Color{ 255, 255, 255, 255 });
            FillRect(p, TBW, TBH, 23, 13, 3, 4, goldTrim);
        }
        else if (action == 4) {
            FillRect(p, TBW, TBH, 13, 5, 4, 8, skinMid);
            FillRect(p, TBW, TBH, 15, 2, 4, 8, skinHi);
            VLine(p, TBW, TBH, 16, 0, 7, bladeSteel);
            VLine(p, TBW, TBH, 17, 0, 7, bladeSteel);
            Set(p, TBW, TBH, 16, 8, goldTrim);
        }
        else {
            FillRect(p, TBW, TBH, 20, 15, 4, 8, skinMid);
            HLine(p, TBW, TBH, 20, 23, 15, bronzeHi);
            Line(p, TBW, TBH, 22, 21, 25, 36, bladeSteel);
            Line(p, TBW, TBH, 23, 22, 26, 37, bladeDark);
            FillRect(p, TBW, TBH, 21, 19, 3, 3, goldTrim);
        }

        int leftLegX = 11 + walkOff;
        int rightLegX = 17 - walkOff;
        FillRect(p, TBW, TBH, leftLegX, 32, 5, 12, bronzeMid);
        VLine(p, TBW, TBH, leftLegX + 1, 32, 43, bronzeHi);
        FillRect(p, TBW, TBH, rightLegX, 32, 5, 12, bronzeMid);
        VLine(p, TBW, TBH, rightLegX + 1, 32, 43, bronzeHi);
        HLine(p, TBW, TBH, leftLegX - 1, leftLegX + 4, 44, bronzeDark);
        HLine(p, TBW, TBH, rightLegX - 1, rightLegX + 4, 44, bronzeDark);
        HLine(p, TBW, TBH, leftLegX - 1, leftLegX + 4, 45, bronzeMid);
        HLine(p, TBW, TBH, rightLegX - 1, rightLegX + 4, 45, bronzeMid);
    };

    PaintTempleBoss(tboss.data(), 0);
    s_sprites[SPR_TEMPLE_BOSS_IDLE] = CreateTextureFromPixels(TBW, TBH, tboss.data());
    PaintTempleBoss(tboss.data(), 1);
    s_sprites[SPR_TEMPLE_BOSS_WALK_0] = CreateTextureFromPixels(TBW, TBH, tboss.data());
    PaintTempleBoss(tboss.data(), 2);
    s_sprites[SPR_TEMPLE_BOSS_WALK_1] = CreateTextureFromPixels(TBW, TBH, tboss.data());
    PaintTempleBoss(tboss.data(), 3);
    s_sprites[SPR_TEMPLE_BOSS_SLASH] = CreateTextureFromPixels(TBW, TBH, tboss.data());
    PaintTempleBoss(tboss.data(), 4);
    s_sprites[SPR_TEMPLE_BOSS_SLAM] = CreateTextureFromPixels(TBW, TBH, tboss.data());
    PaintTempleBoss(tboss.data(), 5);
    s_sprites[SPR_TEMPLE_BOSS_HURT] = CreateTextureFromPixels(TBW, TBH, tboss.data());
    PaintTempleBoss(tboss.data(), 6);
    s_sprites[SPR_TEMPLE_BOSS_KNEEL] = CreateTextureFromPixels(TBW, TBH, tboss.data());

    // =========================================================================
    // STAGE 3 FINAL BOSS: SUPREME LEGATUS / TRIBUNAL PRAETOR LONGINUS (44x48)
    // Dark Burnished Obsidian Plate with Gold Filigree & Flowing Scarlet Cloak
    // =========================================================================
    constexpr int TPW = 44, TPH = 48;
    std::vector<Color> tpraetor(TPW * TPH, BLANK);

    auto PaintTribunalPraetor = [&](Color* p, int pose) {
        for (int i = 0; i < TPW * TPH; ++i) p[i] = BLANK;

        Color skinMid     = Color{ 195, 145, 105, 255 };
        Color plateBlack  = Color{  32,  28,  38, 255 };
        Color plateHi     = Color{  68,  62,  78, 255 };
        Color goldImp     = COLOR_GOLD_BRIGHT;
        Color goldMid     = COLOR_ROMAN_GOLD;
        Color goldDark    = Color{ 160, 115,  20, 255 };
        Color capeRed     = COLOR_ROMAN_CRIMSON;
        Color capeDark    = Color{ 110,  15,  22, 255 };
        Color capeHi      = Color{ 235,  45,  55, 255 };
        Color steelBlade  = Color{ 245, 248, 255, 255 };
        Color steelEdge   = Color{ 160, 175, 195, 255 };
        Color eyeGlow     = Color{ 255,  50,  40, 255 };

        if (pose == 5) { // HURT FLASH
            plateBlack = Color{ 140, 30, 40, 255 };
            plateHi = Color{ 220, 70, 80, 255 };
            goldImp = Color{ 255, 200, 200, 255 };
        }

        int walkOff = (pose == 1) ? 2 : ((pose == 2) ? -2 : 0);

        // --- FLOWING IMPERIAL SCARLET CAPE (Back Layer) ---
        if (pose != 6) { // Not kneeling
            for (int cy = 16; cy <= 44; ++cy) {
                int cx1 = 4 - (cy - 16) / 5;
                int cx2 = 14 + (cy - 16) / 3;
                FillRect(p, TPW, TPH, cx1, cy, cx2 - cx1 + 1, 1, (cy % 3 == 0) ? capeDark : capeRed);
                Set(p, TPW, TPH, cx1, cy, capeDark);
                Set(p, TPW, TPH, cx2, cy, capeHi);
            }
        }

        // --- IMPERIAL GOLDEN HORNED CREST & HELMET ---
        if (pose == 6) { // KNEELING
            FillRect(p, TPW, TPH, 14, 16, 12, 10, plateBlack);
            HLine(p, TPW, TPH, 14, 25, 16, goldImp);
            FillCircle(p, TPW, TPH, 20, 18, 5, goldMid);
            FillRect(p, TPW, TPH, 12, 24, 16, 14, plateBlack);
            HLine(p, TPW, TPH, 13, 27, 26, goldImp);
            FillCircle(p, TPW, TPH, 20, 30, 3, goldImp); // Eagle emblem
            FillRect(p, TPW, TPH, 10, 36, 18, 8, plateBlack);
            // Sword planted downward in ground
            VLine(p, TPW, TPH, 28, 22, 45, steelBlade);
            VLine(p, TPW, TPH, 29, 22, 45, steelEdge);
            HLine(p, TPW, TPH, 25, 32, 22, goldImp);
            FillCircle(p, TPW, TPH, 28, 20, 2, goldDark);
            return;
        }

        // Standing / Attacking Helm
        // Majestic Roman Imperial Transverse Crest (Scarlet & Gold)
        FillRect(p, TPW, TPH, 12, 2, 16, 4, capeRed);
        HLine(p, TPW, TPH, 13, 26, 2, goldImp);
        HLine(p, TPW, TPH, 14, 25, 3, capeHi);

        // Golden Helmet Dome
        FillCircle(p, TPW, TPH, 20, 8, 6, goldMid);
        FillCircle(p, TPW, TPH, 19, 7, 4, goldImp);
        // Golden Laurel Wreath around helmet
        for (int lx = 14; lx <= 25; lx += 2) {
            Set(p, TPW, TPH, lx, 8, Color{ 255, 245, 140, 255 });
            Set(p, TPW, TPH, lx + 1, 9, goldDark);
        }

        // Visor & Piercing Glowing Red Commander Eyes
        FillRect(p, TPW, TPH, 16, 10, 8, 3, plateBlack);
        Set(p, TPW, TPH, 18, 11, eyeGlow);
        Set(p, TPW, TPH, 22, 11, eyeGlow);
        Set(p, TPW, TPH, 19, 11, Color{ 255, 255, 255, 255 });
        Set(p, TPW, TPH, 23, 11, Color{ 255, 255, 255, 255 });

        // Chiselled Bronze Cheekplates & Jaw
        FillRect(p, TPW, TPH, 15, 12, 3, 4, goldDark);
        FillRect(p, TPW, TPH, 23, 12, 3, 4, goldDark);
        FillRect(p, TPW, TPH, 18, 13, 4, 3, skinMid);

        // --- OBSIDIAN MUSCLE CUIRASS (Lorica Musculata with Golden Eagle) ---
        FillRect(p, TPW, TPH, 12, 16, 16, 14, plateBlack);
        OutlineRect(p, TPW, TPH, 12, 16, 16, 14, plateHi);
        // Sculpted anatomical pectoral plates
        FillCircle(p, TPW, TPH, 16, 20, 3, plateHi);
        FillCircle(p, TPW, TPH, 24, 20, 3, plateHi);
        // Golden Aquila (Imperial Eagle) embossed across the chest
        FillCircle(p, TPW, TPH, 20, 21, 2, goldImp);
        Line(p, TPW, TPH, 17, 19, 23, 19, goldImp);
        Line(p, TPW, TPH, 16, 20, 24, 20, goldMid);
        // Heavy Spiked Golden Pauldrons
        FillCircle(p, TPW, TPH, 10, 18, 4, goldMid);
        FillCircle(p, TPW, TPH, 10, 18, 2, goldImp);
        FillCircle(p, TPW, TPH, 30, 18, 4, goldMid);
        FillCircle(p, TPW, TPH, 30, 18, 2, goldImp);

        // Studded War Belt & Triple-Layered Pteruges
        FillRect(p, TPW, TPH, 12, 29, 16, 3, Color{ 90, 45, 20, 255 });
        HLine(p, TPW, TPH, 12, 27, 30, goldImp);
        FillCircle(p, TPW, TPH, 20, 30, 2, goldImp); // Imperial medusa buckle
        for (int px = 13; px <= 27; px += 3) {
            VLine(p, TPW, TPH, px, 32, 37, Color{ 75, 35, 15, 255 });
            Set(p, TPW, TPH, px, 37, goldImp); // Brass leaf terminal
        }

        // --- WEAPONS: IMPERIAL GOLDEN SPATHA & SHIELD ---
        if (pose == 3) { // SLASH POSE: Sweeping Golden Spatha
            Line(p, TPW, TPH, 28, 20, 42, 14, steelBlade);
            Line(p, TPW, TPH, 28, 21, 42, 15, steelEdge);
            Line(p, TPW, TPH, 27, 19, 41, 13, Color{ 255, 255, 255, 255 }); // Gleam
            FillRect(p, TPW, TPH, 26, 20, 4, 3, goldImp); // Crossguard
            FillCircle(p, TPW, TPH, 25, 22, 2, goldDark); // Pommel
            // Radiant slash arc trail
            Line(p, TPW, TPH, 38, 8, 43, 24, goldImp);
            Line(p, TPW, TPH, 36, 6, 43, 20, Color{ 255, 255, 255, 220 });
        } else if (pose == 4) { // SLAM POSE: Overhead Imperial Strike
            Line(p, TPW, TPH, 26, 18, 38, 4, steelBlade);
            Line(p, TPW, TPH, 27, 19, 39, 5, steelEdge);
            Line(p, TPW, TPH, 25, 17, 37, 3, Color{ 255, 255, 255, 255 });
            FillRect(p, TPW, TPH, 27, 16, 4, 4, goldImp);
            // Heavy Roman Scutum Shield
            FillRect(p, TPW, TPH, 30, 16, 10, 18, capeRed);
            OutlineRect(p, TPW, TPH, 30, 16, 10, 18, goldImp);
            FillCircle(p, TPW, TPH, 35, 25, 3, goldImp); // Golden shield boss
        } else { // IDLE / WALK
            Line(p, TPW, TPH, 28, 22, 38, 38, steelBlade);
            Line(p, TPW, TPH, 29, 23, 39, 39, steelEdge);
            FillRect(p, TPW, TPH, 26, 20, 3, 3, goldImp);
            // Armored Scutum Shield on left arm
            FillRect(p, TPW, TPH, 6, 20, 6, 15, capeRed);
            VLine(p, TPW, TPH, 6, 20, 34, goldImp);
            VLine(p, TPW, TPH, 11, 20, 34, goldImp);
            Set(p, TPW, TPH, 8, 27, goldImp);
        }

        // --- GREAVED LEGS & BOOTS ---
        int leftLegX = 13 + walkOff;
        int rightLegX = 21 - walkOff;
        FillRect(p, TPW, TPH, leftLegX, 36, 5, 9, plateBlack);
        VLine(p, TPW, TPH, leftLegX + 1, 36, 44, plateHi);
        FillCircle(p, TPW, TPH, leftLegX + 2, 37, 2, goldImp); // Knee boss
        FillRect(p, TPW, TPH, rightLegX, 36, 5, 9, plateBlack);
        VLine(p, TPW, TPH, rightLegX + 1, 36, 44, plateHi);
        FillCircle(p, TPW, TPH, rightLegX + 2, 37, 2, goldImp);
        // Armored Caligae Boots
        FillRect(p, TPW, TPH, leftLegX - 1, 45, 7, 3, Color{ 70, 35, 15, 255 });
        HLine(p, TPW, TPH, leftLegX - 1, leftLegX + 5, 45, goldMid);
        FillRect(p, TPW, TPH, rightLegX - 1, 45, 7, 3, Color{ 70, 35, 15, 255 });
        HLine(p, TPW, TPH, rightLegX - 1, rightLegX + 5, 45, goldMid);
    };

    PaintTribunalPraetor(tpraetor.data(), 0);
    s_sprites[SPR_TRIBUNAL_IDLE] = CreateTextureFromPixels(TPW, TPH, tpraetor.data());
    PaintTribunalPraetor(tpraetor.data(), 1);
    s_sprites[SPR_TRIBUNAL_WALK_0] = CreateTextureFromPixels(TPW, TPH, tpraetor.data());
    PaintTribunalPraetor(tpraetor.data(), 2);
    s_sprites[SPR_TRIBUNAL_WALK_1] = CreateTextureFromPixels(TPW, TPH, tpraetor.data());
    PaintTribunalPraetor(tpraetor.data(), 3);
    s_sprites[SPR_TRIBUNAL_SLASH] = CreateTextureFromPixels(TPW, TPH, tpraetor.data());
    PaintTribunalPraetor(tpraetor.data(), 4);
    s_sprites[SPR_TRIBUNAL_SLAM] = CreateTextureFromPixels(TPW, TPH, tpraetor.data());
    PaintTribunalPraetor(tpraetor.data(), 5);
    s_sprites[SPR_TRIBUNAL_HURT] = CreateTextureFromPixels(TPW, TPH, tpraetor.data());
    PaintTribunalPraetor(tpraetor.data(), 6);
    s_sprites[SPR_TRIBUNAL_KNEEL] = CreateTextureFromPixels(TPW, TPH, tpraetor.data());

    // Sacred Golden Censer Pickup (18x18)
    constexpr int CEN_W = 18, CEN_H = 18;
    std::vector<Color> censer(CEN_W * CEN_H, BLANK);
    Color censerGoldHi   = COLOR_GOLD_BRIGHT;
    Color censerGoldMid  = COLOR_GOLD_DARK;
    Color censerGoldDark = Color{ 175, 120,  25, 255 };
    Color coalRed        = Color{ 255,  60,  30, 255 };
    Color coalHot        = Color{ 255, 220, 100, 255 };
    Color smokeCol       = Color{ 230, 225, 245, 170 };

    Set(censer.data(), CEN_W, CEN_H, 8, 1, censerGoldHi);
    Set(censer.data(), CEN_W, CEN_H, 9, 1, censerGoldHi);
    Set(censer.data(), CEN_W, CEN_H, 8, 2, censerGoldDark);
    Set(censer.data(), CEN_W, CEN_H, 9, 2, censerGoldDark);
    for (int y = 3; y <= 7; ++y) {
        if (y % 2 == 1) {
            Set(censer.data(), CEN_W, CEN_H, 6, y, censerGoldMid);
            Set(censer.data(), CEN_W, CEN_H, 8, y, censerGoldHi);
            Set(censer.data(), CEN_W, CEN_H, 11, y, censerGoldMid);
        }
    }
    FillCircle(censer.data(), CEN_W, CEN_H, 8, 10, 4, censerGoldMid);
    HLine(censer.data(), CEN_W, CEN_H, 5, 12, 10, censerGoldHi);
    Set(censer.data(), CEN_W, CEN_H, 7, 10, coalRed);
    Set(censer.data(), CEN_W, CEN_H, 8, 10, coalHot);
    Set(censer.data(), CEN_W, CEN_H, 10, 10, coalRed);
    FillCircle(censer.data(), CEN_W, CEN_H, 8, 13, 4, censerGoldMid);
    HLine(censer.data(), CEN_W, CEN_H, 6, 11, 14, censerGoldDark);
    HLine(censer.data(), CEN_W, CEN_H, 7, 10, 15, censerGoldHi);
    Set(censer.data(), CEN_W, CEN_H, 11, 3, smokeCol);
    Set(censer.data(), CEN_W, CEN_H, 12, 2, smokeCol);
    Set(censer.data(), CEN_W, CEN_H, 13, 1, smokeCol);
    Set(censer.data(), CEN_W, CEN_H, 5, 2, smokeCol);
    s_sprites[SPR_CENSER_GOLD] = CreateTextureFromPixels(CEN_W, CEN_H, censer.data());
}

// Generate all procedural tiles (16x16 pixels each)
void GraphicsManager::GenerateAllProceduralTiles() {
    using namespace PixelArt;
    constexpr int TW = TILE_SIZE;
    constexpr int TH = TILE_SIZE;

    std::vector<Color> p(TW * TH);

    // 1. TILE_GROUND_SURFACE (Ancient Ashlar Flagstones & Cedar Floor Planks)
    Color floorHi   = Color{ 235, 215, 185, 255 };
    Color floorMid  = Color{ 190, 168, 138, 255 };
    Color floorShade= Color{ 150, 128, 100, 255 };
    Color floorSeam = Color{  85,  70,  55, 255 };

    for (int y = 0; y < TH; ++y) {
        for (int x = 0; x < TW; ++x) {
            Color c = floorMid;
            // Horizontal wood plank / stone course seams
            if (y == 0 || y == 8 || y == 15) {
                c = floorSeam;
            } else if (y == 1 || y == 9) {
                c = floorHi; // Specular light on plank edges
            } else if ((y < 8 && x == 8) || (y >= 8 && (x == 4 || x == 12))) {
                c = floorSeam; // Vertical board joints
            } else if (y == 7 || y == 14) {
                c = floorShade; // Under-edge shadow
            } else if ((x ^ (y * 2)) % 5 == 0) {
                c = floorHi; // Subtle wood grain / stone crystalline fleck
            }
            p[y * TW + x] = c;
        }
    }
    s_tiles[TILE_GROUND_SURFACE] = CreateTextureFromPixels(TW, TH, p.data());

    // 2. TILE_GROUND_SUBSURFACE (Ancient Bedrock & Stratified Earth)
    Color subHi  = Color{ 105, 88, 72, 255 };
    Color subMid = Color{  82, 68, 55, 255 };
    Color subLow = Color{  58, 48, 38, 255 };

    for (int y = 0; y < TH; ++y) {
        for (int x = 0; x < TW; ++x) {
            Color c = subMid;
            if (y % 4 == 0 || (x % 8 == 0 && (y % 8 < 4))) c = subLow;
            else if ((x + y) % 5 == 0) c = subHi;
            p[y * TW + x] = c;
        }
    }
    s_tiles[TILE_GROUND_SUBSURFACE] = CreateTextureFromPixels(TW, TH, p.data());

    // 3. TILE_BALCONY_PLATFORM (Ornate Carved Mezzanine Beam with Classical Relief Scrollwork & Cedar Floor)
    for (int i = 0; i < TW * TH; ++i) p[i] = BLANK;
    // Top Walkway Coping (y = 0..2)
    HLine(p.data(), TW, TH, 0, 15, 0, Color{ 255, 245, 220, 255 }); // Polished wood/marble top edge highlight
    HLine(p.data(), TW, TH, 0, 15, 1, Color{ 225, 175, 110, 255 }); // Warm cedar plank surface
    HLine(p.data(), TW, TH, 0, 15, 2, Color{ 160, 105,  55, 255 }); // Architrave shadow groove
    // Carved Relief Beam Face (y = 3..11) - Intricate acanthus / swirl frieze (Matches Shinobi carved beam!)
    FillRect(p.data(), TW, TH, 0, 3, TW, 9, Color{ 145, 80, 42, 255 });
    for (int x = 0; x < TW; ++x) {
        // Relief scroll wave / rosette carving
        bool isRidge = ((x % 8 == 2) || (x % 8 == 5) || ((x + 2) % 4 == 0));
        bool isShadow = ((x % 8 == 3) || (x % 8 == 6));
        if (isRidge) {
            Set(p.data(), TW, TH, x, 4, Color{ 235, 185, 120, 255 });
            Set(p.data(), TW, TH, x, 5, Color{ 215, 155,  95, 255 });
            Set(p.data(), TW, TH, x, 7, Color{ 235, 185, 120, 255 });
            Set(p.data(), TW, TH, x, 8, Color{ 215, 155,  95, 255 });
        } else if (isShadow) {
            Set(p.data(), TW, TH, x, 4, Color{ 85, 45, 22, 255 });
            Set(p.data(), TW, TH, x, 6, Color{ 85, 45, 22, 255 });
            Set(p.data(), TW, TH, x, 8, Color{ 85, 45, 22, 255 });
        }
        // Gilded / Brass Rosette Boss at center of each 8-pixel module
        if (x % 8 == 4) {
            Set(p.data(), TW, TH, x, 6, COLOR_GOLD_BRIGHT);
            Set(p.data(), TW, TH, x - 1, 6, COLOR_ROMAN_GOLD);
            Set(p.data(), TW, TH, x + 1, 6, COLOR_ROMAN_GOLD);
            Set(p.data(), TW, TH, x, 5, COLOR_GOLD_BRIGHT);
            Set(p.data(), TW, TH, x, 7, Color{ 130, 80, 15, 255 });
        }
    }
    // Classical Dentil Molding along Bottom (y = 12..15)
    HLine(p.data(), TW, TH, 0, 15, 12, Color{ 110, 60, 30, 255 });
    for (int x = 0; x < TW; x += 4) {
        FillRect(p.data(), TW, TH, x, 13, 2, 2, Color{ 210, 160, 100, 255 });
        FillRect(p.data(), TW, TH, x + 2, 13, 2, 2, Color{ 65, 32, 16, 255 });
    }
    HLine(p.data(), TW, TH, 0, 15, 15, Color{ 60, 30, 15, 255 }); // Lower soffit shadow
    s_tiles[TILE_BALCONY_PLATFORM] = CreateTextureFromPixels(TW, TH, p.data());

    // 4. TILE_AQUEDUCT_TOP (Water Channel Cornice)
    Color aquaHi  = Color{ 230, 215, 190, 255 };
    Color aquaMid = Color{ 190, 175, 150, 255 };
    Color aquaLow = Color{ 130, 115,  95, 255 };

    for (int y = 0; y < TH; ++y) {
        for (int x = 0; x < TW; ++x) {
            Color c = aquaMid;
            if (y <= 2) c = aquaHi;
            else if (y == 3) c = aquaLow;
            else if (y >= 4 && y <= 6) c = Color{ 150, 135, 115, 255 };
            else if (y == 7) c = Color{ 85, 75, 65, 255 }; // Bottom shadow
            else c = aquaMid;
            p[y * TW + x] = c;
        }
    }
    s_tiles[TILE_AQUEDUCT_TOP] = CreateTextureFromPixels(TW, TH, p.data());

    // 5. TILE_AQUEDUCT_ARCH_L
    for (int y = 0; y < TH; ++y) {
        for (int x = 0; x < TW; ++x) {
            if (x < 6 || y < 4) {
                p[y * TW + x] = (x == 5 || y == 3) ? Color{ 120, 105, 85, 255 } : Color{ 185, 170, 145, 255 };
            } else {
                p[y * TW + x] = BLANK;
            }
        }
    }
    s_tiles[TILE_AQUEDUCT_ARCH_L] = CreateTextureFromPixels(TW, TH, p.data());

    // 6. TILE_AQUEDUCT_ARCH_R
    for (int y = 0; y < TH; ++y) {
        for (int x = 0; x < TW; ++x) {
            if (x >= 10 || y < 4) {
                p[y * TW + x] = (x == 10 || y == 3) ? Color{ 120, 105, 85, 255 } : Color{ 185, 170, 145, 255 };
            } else {
                p[y * TW + x] = BLANK;
            }
        }
    }
    s_tiles[TILE_AQUEDUCT_ARCH_R] = CreateTextureFromPixels(TW, TH, p.data());

    // 7. TILE_COLUMN_HEAD (Corinthian Capital with Acanthus Volutes)
    for (int i = 0; i < TW * TH; ++i) p[i] = BLANK;
    FillRect(p.data(), TW, TH, 1, 0, 14, 3, Color{ 245, 235, 215, 255 }); // Abacus
    // Acanthus volute scrolls
    Set(p.data(), TW, TH, 2, 3, Color{ 255, 245, 225, 255 });
    Set(p.data(), TW, TH, 13, 3, Color{ 255, 245, 225, 255 });
    FillRect(p.data(), TW, TH, 3, 3, 10, 6, Color{ 215, 200, 175, 255 });
    Set(p.data(), TW, TH, 5, 5, Color{ 150, 135, 115, 255 });
    Set(p.data(), TW, TH, 10, 5, Color{ 150, 135, 115, 255 });
    FillRect(p.data(), TW, TH, 4, 9, 8, 7, Color{ 230, 215, 190, 255 }); // Necking
    s_tiles[TILE_COLUMN_HEAD] = CreateTextureFromPixels(TW, TH, p.data());

    // 8. TILE_COLUMN_MID (Fluted Cylindrical Shaft)
    for (int y = 0; y < TH; ++y) {
        for (int x = 0; x < TW; ++x) {
            if (x >= 4 && x <= 11) {
                Color c = Color{ 225, 212, 185, 255 };
                if (x == 4 || x == 7 || x == 10) c = Color{ 165, 150, 130, 255 }; // Flutes
                else if (x == 5 || x == 8) c = Color{ 250, 240, 220, 255 };      // Specular highlight
                p[y * TW + x] = c;
            } else {
                p[y * TW + x] = BLANK;
            }
        }
    }
    s_tiles[TILE_COLUMN_MID] = CreateTextureFromPixels(TW, TH, p.data());

    // 9. TILE_COLUMN_BASE (Attic Base with Torus Molding)
    for (int i = 0; i < TW * TH; ++i) p[i] = BLANK;
    FillRect(p.data(), TW, TH, 4, 0, 8, 5, Color{ 225, 210, 185, 255 });
    FillRect(p.data(), TW, TH, 2, 5, 12, 4, Color{ 240, 225, 200, 255 }); // Torus
    FillRect(p.data(), TW, TH, 1, 9, 14, 7, Color{ 190, 175, 150, 255 }); // Plinth
    s_tiles[TILE_COLUMN_BASE] = CreateTextureFromPixels(TW, TH, p.data());

    // 10. TILE_WOOD_CRATE (Rich Oak Planks with Iron Brackets)
    Color oakLight  = Color{ 170, 115,  60, 255 };
    Color oakMid    = Color{ 135,  85,  40, 255 };
    Color oakDark   = Color{  95,  55,  25, 255 };
    Color ironPlate = Color{  65,  70,  80, 255 };
    Color ironRivet = Color{ 200, 210, 220, 255 };

    for (int y = 0; y < TH; ++y) {
        for (int x = 0; x < TW; ++x) {
            Color c = ((y == 0 || y == 15 || x == 0 || x == 15 || x == y || x == (15 - y))) ? oakDark : oakMid;
            if (y == 1 || x == 1) c = oakLight;
            // Corner Iron Angle-Irons
            if ((x <= 3 || x >= 12) && (y <= 3 || y >= 12)) c = ironPlate;
            p[y * TW + x] = c;
        }
    }
    // Rivets
    Set(p.data(), TW, TH, 2, 2, ironRivet);
    Set(p.data(), TW, TH, 13, 2, ironRivet);
    Set(p.data(), TW, TH, 2, 13, ironRivet);
    Set(p.data(), TW, TH, 13, 13, ironRivet);
    s_tiles[TILE_WOOD_CRATE] = CreateTextureFromPixels(TW, TH, p.data());

    // 11. TILE_CLAY_JAR (Terracotta Wine Amphora)
    for (int i = 0; i < TW * TH; ++i) p[i] = BLANK;
    Color terraHi   = Color{ 235, 125,  65, 255 };
    Color terraMid  = Color{ 195,  95,  45, 255 };
    Color terraDark = Color{ 135,  55,  25, 255 };
    FillRect(p.data(), TW, TH, 5, 4, 6, 10, terraMid);
    HLine(p.data(), TW, TH, 6, 9, 3, terraHi); // Lip
    HLine(p.data(), TW, TH, 5, 8, 5, terraHi); // Highlight
    // Handles
    Set(p.data(), TW, TH, 3, 5, terraDark);
    Set(p.data(), TW, TH, 3, 6, terraDark);
    Set(p.data(), TW, TH, 12, 5, terraDark);
    Set(p.data(), TW, TH, 12, 6, terraDark);
    s_tiles[TILE_CLAY_JAR] = CreateTextureFromPixels(TW, TH, p.data());

    // 12. TILE_GATE_FRAME (Monumental Stone Archway)
    for (int y = 0; y < TH; ++y) {
        for (int x = 0; x < TW; ++x) {
            Color c = (x < 3 || x > 12) ? Color{ 105, 95, 85, 255 } : Color{ 165, 150, 135, 255 };
            if (y == 0 || y == 15) c = Color{ 80, 70, 60, 255 };
            p[y * TW + x] = c;
        }
    }
    s_tiles[TILE_GATE_FRAME] = CreateTextureFromPixels(TW, TH, p.data());

    // 13. TILE_GATE_BARS (Heavy Spiked Cast-Iron Portcullis)
    for (int i = 0; i < TW * TH; ++i) p[i] = BLANK;
    for (int x = 2; x < TW; x += 4) {
        VLine(p.data(), TW, TH, x, 0, 15, COLOR_STEEL_BRIGHT);
        VLine(p.data(), TW, TH, x + 1, 0, 15, Color{ 70, 75, 85, 255 });
    }
    HLine(p.data(), TW, TH, 0, 15, 7, Color{ 90, 95, 105, 255 }); // Crossbar
    // Bottom Spike points
    for (int x = 2; x < TW; x += 4) {
        Set(p.data(), TW, TH, x, 15, COLOR_STEEL_BRIGHT);
    }
    s_tiles[TILE_GATE_BARS] = CreateTextureFromPixels(TW, TH, p.data());

    // 14. TILE_SPQR_BANNER (Roman Imperial Crimson & Gold Vexillum Banner)
    for (int i = 0; i < TW * TH; ++i) p[i] = BLANK;
    // Brass Hanging Crossbar with Ball Finials
    HLine(p.data(), TW, TH, 1, 14, 0, COLOR_ROMAN_GOLD);
    Set(p.data(), TW, TH, 0, 0, COLOR_GOLD_BRIGHT); // Left finial
    Set(p.data(), TW, TH, 15, 0, COLOR_GOLD_BRIGHT); // Right finial
    // Crimson Silk Fabric Body
    FillRect(p.data(), TW, TH, 2, 1, 12, 13, COLOR_ROMAN_CRIMSON);
    OutlineRect(p.data(), TW, TH, 2, 1, 12, 13, COLOR_ROMAN_GOLD); // Gold embroidered border
    HLine(p.data(), TW, TH, 3, 12, 2, Color{ 235, 45, 55, 255 }); // Top fold highlight
    // Imperial Golden Eagle (Aquila) & SPQR
    HLine(p.data(), TW, TH, 5, 10, 4, COLOR_GOLD_BRIGHT); // Outspread wings
    Set(p.data(), TW, TH, 4, 3, COLOR_GOLD_BRIGHT);
    Set(p.data(), TW, TH, 11, 3, COLOR_GOLD_BRIGHT);
    Set(p.data(), TW, TH, 7, 3, COLOR_GOLD_BRIGHT); // Eagle head
    Set(p.data(), TW, TH, 8, 3, COLOR_GOLD_BRIGHT);
    FillRect(p.data(), TW, TH, 6, 5, 4, 2, COLOR_GOLD_BRIGHT); // Body & wreath
    // Gold Fringe / Tassels on bottom
    for (int x = 3; x <= 12; x += 2) {
        Set(p.data(), TW, TH, x, 14, COLOR_ROMAN_GOLD);
        Set(p.data(), TW, TH, x, 15, COLOR_GOLD_BRIGHT);
    }
    s_tiles[TILE_SPQR_BANNER] = CreateTextureFromPixels(TW, TH, p.data());

    // 15. TILE_STONE_WINDOW (Diamond Criss-Cross Woodwork Lattice Screen / Kumiko Screen)
    for (int i = 0; i < TW * TH; ++i) p[i] = BLANK;
    Color latticeHi   = Color{ 225, 175, 110, 255 };
    Color latticeMid  = Color{ 170, 110,  60, 255 };
    Color latticeDark = Color{ 105,  58,  28, 255 };
    Color latticeDeep = Color{  55,  28,  14, 255 };
    Color backPaper   = Color{ 240, 235, 218, 240 }; // Shoji parchment background
    Color paperShadow = Color{ 205, 195, 175, 240 };

    // Shoji screen background with subtle paper grain
    for (int y = 0; y < TH; ++y) {
        for (int x = 0; x < TW; ++x) {
            p[y * TW + x] = ((x + y) % 6 == 0) ? paperShadow : backPaper;
        }
    }
    // Diamond criss-cross diagonal lattice slats (45-degree angled woodwork)
    for (int y = 0; y < TH; ++y) {
        for (int x = 0; x < TW; ++x) {
            bool diag1 = ((x + y) % 8 == 0 || (x + y) % 8 == 1);
            bool diag2 = ((x - y + 16) % 8 == 0 || (x - y + 16) % 8 == 1);
            if (diag1 || diag2) {
                // Slat wood highlight / shadow
                bool isTopEdge = ((x + y) % 8 == 0 || (x - y + 16) % 8 == 0);
                p[y * TW + x] = isTopEdge ? latticeHi : latticeMid;
            } else if (((x + y) % 8 == 2) || ((x - y + 16) % 8 == 2)) {
                // Drop shadow behind slats on paper
                p[y * TW + x] = Color{ 150, 140, 125, 255 };
            }
        }
    }
    // Outer wooden frame border
    HLine(p.data(), TW, TH, 0, 15, 0, latticeHi);
    HLine(p.data(), TW, TH, 0, 15, 15, latticeDeep);
    VLine(p.data(), TW, TH, 0, 0, 15, latticeMid);
    VLine(p.data(), TW, TH, 15, 0, 15, latticeDark);
    s_tiles[TILE_STONE_WINDOW] = CreateTextureFromPixels(TW, TH, p.data());

    // 16. TILE_OIL_LAMP (Ancient Terracotta Oil Lamp / Lucerna on Stone Corbel)
    for (int i = 0; i < TW * TH; ++i) p[i] = BLANK;
    // Stone Corbel bracket
    FillRect(p.data(), TW, TH, 4, 11, 8, 4, Color{ 185, 172, 148, 255 });
    HLine(p.data(), TW, TH, 5, 10, 11, Color{ 220, 208, 185, 255 });
    // Terracotta Lamp Dish
    FillRect(p.data(), TW, TH, 5, 8, 7, 3, Color{ 195, 95, 45, 255 });
    Set(p.data(), TW, TH, 4, 7, Color{ 135, 55, 25, 255 }); // Handle loop
    Set(p.data(), TW, TH, 4, 8, Color{ 135, 55, 25, 255 });
    Set(p.data(), TW, TH, 12, 7, Color{ 220, 115, 55, 255 }); // Spout
    // Golden Flickering Flame & Halo
    FillCircle(p.data(), TW, TH, 12, 5, 3, Color{ 255, 215, 90, 160 }); // Light aura
    Set(p.data(), TW, TH, 12, 5, Color{ 255, 160, 40, 255 });
    Set(p.data(), TW, TH, 12, 4, Color{ 255, 250, 210, 255 }); // White hot flame tip!
    s_tiles[TILE_OIL_LAMP] = CreateTextureFromPixels(TW, TH, p.data());

    // 17. TILE_TEMPLE_FLOOR_CHECKER (White & Black Polished Marble Checkerboard)
    for (int y = 0; y < TH; ++y) {
        for (int x = 0; x < TW; ++x) {
            bool whiteCheck = ((x < 8 && y < 8) || (x >= 8 && y >= 8));
            Color c = whiteCheck ? COLOR_TEMPLE_MARBLE : Color{ 32, 30, 42, 255 };
            if (x == 0 || x == 8 || y == 0 || y == 8) {
                c = whiteCheck ? COLOR_TEMPLE_SHADOW : Color{ 18, 16, 24, 255 };
            } else if (whiteCheck && ((x + y == 5) || (x + y == 19))) {
                c = Color{ 220, 195, 140, 255 };
            }
            p[y * TW + x] = c;
        }
    }
    s_tiles[TILE_TEMPLE_FLOOR_CHECKER] = CreateTextureFromPixels(TW, TH, p.data());

    // 18. TILE_TEMPLE_FLOOR_SUB (Hewn White Limestone Foundation)
    for (int y = 0; y < TH; ++y) {
        for (int x = 0; x < TW; ++x) {
            Color c = ((x + y) % 7 == 0) ? COLOR_TEMPLE_SHADOW : COLOR_TEMPLE_MARBLE;
            p[y * TW + x] = c;
        }
    }
    OutlineRect(p.data(), TW, TH, 0, 0, TW, TH, Color{ 150, 145, 135, 255 });
    Set(p.data(), TW, TH, 7, 7, COLOR_TEMPLE_GOLD);
    Set(p.data(), TW, TH, 8, 7, COLOR_TEMPLE_GOLD);
    Set(p.data(), TW, TH, 7, 8, COLOR_TEMPLE_GOLD);
    Set(p.data(), TW, TH, 8, 8, COLOR_TEMPLE_GOLD);
    s_tiles[TILE_TEMPLE_FLOOR_SUB] = CreateTextureFromPixels(TW, TH, p.data());

    // 19. TILE_TEMPLE_BALCONY_1 (Mezzanine Ledge - Tier 1 Platform)
    for (int i = 0; i < TW * TH; ++i) p[i] = BLANK;
    FillRect(p.data(), TW, TH, 0, 0, TW, 3, COLOR_TEMPLE_MARBLE);
    HLine(p.data(), TW, TH, 0, TW - 1, 0, Color{ 255, 255, 255, 255 });
    HLine(p.data(), TW, TH, 0, TW - 1, 2, COLOR_TEMPLE_SHADOW);
    FillRect(p.data(), TW, TH, 0, 3, TW, 7, COLOR_TEMPLE_CEDAR);
    HLine(p.data(), TW, TH, 0, TW - 1, 3, COLOR_TEMPLE_GOLD);
    Set(p.data(), TW, TH, 3, 6, COLOR_TEMPLE_GOLD);
    Set(p.data(), TW, TH, 11, 6, COLOR_TEMPLE_GOLD);
    s_tiles[TILE_TEMPLE_BALCONY_1] = CreateTextureFromPixels(TW, TH, p.data());

    // 20. TILE_TEMPLE_BALCONY_2 (High Gallery Parapet - Tier 2 Platform)
    for (int i = 0; i < TW * TH; ++i) p[i] = BLANK;
    FillRect(p.data(), TW, TH, 0, 0, TW, 3, COLOR_TEMPLE_CEDAR);
    HLine(p.data(), TW, TH, 0, TW - 1, 0, Color{ 175, 115, 60, 255 });
    HLine(p.data(), TW, TH, 0, TW - 1, 2, COLOR_TEMPLE_GOLD);
    for (int y = 3; y <= 12; ++y) {
        for (int x = 0; x < TW; ++x) {
            if ((x + y) % 4 == 0 || (x - y + 16) % 4 == 0) {
                p[y * TW + x] = COLOR_TEMPLE_GOLD;
            }
        }
    }
    s_tiles[TILE_TEMPLE_BALCONY_2] = CreateTextureFromPixels(TW, TH, p.data());

    // 21. TILE_TEMPLE_COLUMN_HEAD (Golden Corinthian Capital)
    for (int i = 0; i < TW * TH; ++i) p[i] = BLANK;
    FillRect(p.data(), TW, TH, 1, 2, 14, 14, COLOR_TEMPLE_GOLD);
    HLine(p.data(), TW, TH, 0, 15, 2, Color{ 255, 245, 160, 255 });
    FillCircle(p.data(), TW, TH, 2, 5, 2, Color{ 190, 130, 20, 255 });
    FillCircle(p.data(), TW, TH, 13, 5, 2, Color{ 190, 130, 20, 255 });
    VLine(p.data(), TW, TH, 7, 6, 14, Color{ 255, 245, 160, 255 });
    VLine(p.data(), TW, TH, 8, 6, 14, Color{ 255, 245, 160, 255 });
    s_tiles[TILE_TEMPLE_COLUMN_HEAD] = CreateTextureFromPixels(TW, TH, p.data());

    // 22. TILE_TEMPLE_COLUMN_MID (Fluted White Marble Shaft)
    for (int i = 0; i < TW * TH; ++i) p[i] = BLANK;
    FillRect(p.data(), TW, TH, 3, 0, 10, TH, COLOR_TEMPLE_MARBLE);
    VLine(p.data(), TW, TH, 3, 0, TH - 1, COLOR_TEMPLE_SHADOW);
    VLine(p.data(), TW, TH, 12, 0, TH - 1, COLOR_TEMPLE_SHADOW);
    VLine(p.data(), TW, TH, 5, 0, TH - 1, Color{ 220, 215, 205, 255 });
    VLine(p.data(), TW, TH, 7, 0, TH - 1, Color{ 255, 255, 255, 255 });
    VLine(p.data(), TW, TH, 9, 0, TH - 1, Color{ 220, 215, 205, 255 });
    s_tiles[TILE_TEMPLE_COLUMN_MID] = CreateTextureFromPixels(TW, TH, p.data());

    // 23. TILE_TEMPLE_COLUMN_BASE (Molded White Marble Plinth)
    for (int i = 0; i < TW * TH; ++i) p[i] = BLANK;
    FillRect(p.data(), TW, TH, 3, 0, 10, 6, COLOR_TEMPLE_MARBLE);
    FillRect(p.data(), TW, TH, 1, 6, 14, 5, COLOR_TEMPLE_MARBLE);
    FillRect(p.data(), TW, TH, 0, 11, 16, 5, COLOR_TEMPLE_MARBLE);
    HLine(p.data(), TW, TH, 1, 14, 6, Color{ 255, 255, 255, 255 });
    HLine(p.data(), TW, TH, 0, 15, 11, Color{ 255, 255, 255, 255 });
    HLine(p.data(), TW, TH, 0, 15, 15, COLOR_TEMPLE_SHADOW);
    s_tiles[TILE_TEMPLE_COLUMN_BASE] = CreateTextureFromPixels(TW, TH, p.data());

    // 24. TILE_TEMPLE_MENORAH (Golden 7-Branched Candelabrum)
    for (int i = 0; i < TW * TH; ++i) p[i] = BLANK;
    VLine(p.data(), TW, TH, 7, 4, 15, COLOR_TEMPLE_GOLD);
    VLine(p.data(), TW, TH, 8, 4, 15, COLOR_TEMPLE_GOLD);
    HLine(p.data(), TW, TH, 4, 11, 14, COLOR_TEMPLE_GOLD);
    HLine(p.data(), TW, TH, 3, 12, 15, Color{ 200, 140, 25, 255 });
    HLine(p.data(), TW, TH, 5, 9, 10, COLOR_TEMPLE_GOLD);
    VLine(p.data(), TW, TH, 5, 5, 9, COLOR_TEMPLE_GOLD);
    VLine(p.data(), TW, TH, 9, 5, 9, COLOR_TEMPLE_GOLD);
    HLine(p.data(), TW, TH, 3, 11, 12, COLOR_TEMPLE_GOLD);
    VLine(p.data(), TW, TH, 3, 5, 11, COLOR_TEMPLE_GOLD);
    VLine(p.data(), TW, TH, 11, 5, 11, COLOR_TEMPLE_GOLD);
    HLine(p.data(), TW, TH, 1, 13, 13, COLOR_TEMPLE_GOLD);
    VLine(p.data(), TW, TH, 1, 5, 12, COLOR_TEMPLE_GOLD);
    VLine(p.data(), TW, TH, 13, 5, 12, COLOR_TEMPLE_GOLD);
    int menorahFlames[7] = { 1, 3, 5, 7, 9, 11, 13 };
    for (int f = 0; f < 7; ++f) {
        int x = menorahFlames[f];
        Set(p.data(), TW, TH, x, 4, COLOR_TEMPLE_GOLD);
        Set(p.data(), TW, TH, x, 3, Color{ 255, 170, 30, 255 });
        Set(p.data(), TW, TH, x, 2, Color{ 255, 250, 180, 255 });
    }
    s_tiles[TILE_TEMPLE_MENORAH] = CreateTextureFromPixels(TW, TH, p.data());

    // 25. TILE_TEMPLE_DRAPERY_PURPLE (Tyrian Purple Velvet with Gold Fringe)
    for (int y = 0; y < TH; ++y) {
        for (int x = 0; x < TW; ++x) {
            int fold = (x % 5);
            Color c = (fold == 0) ? Color{ 55, 12, 68, 255 } :
                      ((fold == 1 || fold == 2) ? Color{ 135, 40, 160, 255 } : COLOR_TEMPLE_PURPLE);
            p[y * TW + x] = c;
        }
    }
    HLine(p.data(), TW, TH, 0, TW - 1, 13, COLOR_TEMPLE_GOLD);
    for (int x = 0; x < TW; ++x) {
        p[14 * TW + x] = Color{ 200, 150, 30, 255 };
        if (x % 2 == 0) p[15 * TW + x] = COLOR_TEMPLE_GOLD;
    }
    s_tiles[TILE_TEMPLE_DRAPERY_PURPLE] = CreateTextureFromPixels(TW, TH, p.data());

    // 26. TILE_TEMPLE_ALTAR (Horned Golden Altar of Incense)
    for (int i = 0; i < TW * TH; ++i) p[i] = BLANK;
    FillRect(p.data(), TW, TH, 1, 5, 14, 11, COLOR_TEMPLE_GOLD);
    OutlineRect(p.data(), TW, TH, 1, 5, 14, 11, Color{ 180, 120, 25, 255 });
    Set(p.data(), TW, TH, 1, 3, COLOR_TEMPLE_GOLD);
    Set(p.data(), TW, TH, 1, 4, COLOR_TEMPLE_GOLD);
    Set(p.data(), TW, TH, 14, 3, COLOR_TEMPLE_GOLD);
    Set(p.data(), TW, TH, 14, 4, COLOR_TEMPLE_GOLD);
    HLine(p.data(), TW, TH, 3, 12, 5, Color{ 255, 140, 40, 255 });
    Set(p.data(), TW, TH, 6, 2, Color{ 240, 230, 255, 180 });
    Set(p.data(), TW, TH, 9, 1, Color{ 240, 230, 255, 180 });
    s_tiles[TILE_TEMPLE_ALTAR] = CreateTextureFromPixels(TW, TH, p.data());

    // 27. TILE_TEMPLE_INCENSE_BURNER (Standing Bronze Brazier)
    for (int i = 0; i < TW * TH; ++i) p[i] = BLANK;
    Line(p.data(), TW, TH, 7, 7, 3, 15, Color{ 140, 85, 30, 255 });
    Line(p.data(), TW, TH, 8, 7, 12, 15, Color{ 140, 85, 30, 255 });
    VLine(p.data(), TW, TH, 7, 7, 14, Color{ 190, 125, 45, 255 });
    FillCircle(p.data(), TW, TH, 7, 6, 4, Color{ 190, 125, 45, 255 });
    HLine(p.data(), TW, TH, 4, 11, 4, Color{ 240, 180, 60, 255 });
    Set(p.data(), TW, TH, 6, 4, Color{ 255, 60, 30, 255 });
    Set(p.data(), TW, TH, 8, 4, Color{ 255, 120, 40, 255 });
    FillCircle(p.data(), TW, TH, 7, 2, 2, Color{ 255, 215, 80, 120 });
    s_tiles[TILE_TEMPLE_INCENSE_BURNER] = CreateTextureFromPixels(TW, TH, p.data());

    // =========================================================================
    // STAGE 3: GOLGOTHA CITADEL & CRAGS TILES
    // =========================================================================
    Color cragDark   = Color{  52,  46,  48, 255 };
    Color cragMid    = Color{  78,  72,  76, 255 };
    Color cragHi     = Color{ 115, 108, 114, 255 };
    Color ironDark   = Color{  28,  26,  32, 255 };
    Color ironHi     = Color{  65,  62,  75, 255 };
    Color flameHot   = Color{ 255, 230,  80, 255 };
    Color flameOrange= Color{ 255, 110,  30, 255 };
    Color flameRed   = Color{ 200,  30,  20, 255 };

    // 28. TILE_GOLGOTHA_ROCK_SURFACE
    for (int y = 0; y < TH; ++y) {
        for (int x = 0; x < TW; ++x) {
            Color c = ((x + y * 2) % 4 == 0) ? cragHi : cragMid;
            if (y == 0) c = Color{ 135, 128, 134, 255 };
            p[y * TW + x] = c;
        }
    }
    s_tiles[TILE_GOLGOTHA_ROCK_SURFACE] = CreateTextureFromPixels(TW, TH, p.data());

    // 29. TILE_GOLGOTHA_ROCK_SUB
    for (int y = 0; y < TH; ++y) {
        for (int x = 0; x < TW; ++x) {
            Color c = ((x * 3 + y) % 5 == 0) ? cragDark : cragMid;
            p[y * TW + x] = c;
        }
    }
    s_tiles[TILE_GOLGOTHA_ROCK_SUB] = CreateTextureFromPixels(TW, TH, p.data());

    // 30. TILE_GOLGOTHA_LEDGE_1 (Crag Platform)
    for (int i = 0; i < TW * TH; ++i) p[i] = BLANK;
    FillRect(p.data(), TW, TH, 0, 0, TW, 6, cragMid);
    HLine(p.data(), TW, TH, 0, TW - 1, 0, cragHi);
    HLine(p.data(), TW, TH, 0, TW - 1, 5, cragDark);
    for (int x = 2; x < TW; x += 4) Set(p.data(), TW, TH, x, 6, cragDark);
    s_tiles[TILE_GOLGOTHA_LEDGE_1] = CreateTextureFromPixels(TW, TH, p.data());

    // 31. TILE_GOLGOTHA_LEDGE_2 (Fortress Battlement)
    for (int i = 0; i < TW * TH; ++i) p[i] = BLANK;
    FillRect(p.data(), TW, TH, 0, 0, TW, 6, ironDark);
    HLine(p.data(), TW, TH, 0, TW - 1, 0, ironHi);
    HLine(p.data(), TW, TH, 0, TW - 1, 5, Color{ 15, 12, 18, 255 });
    Set(p.data(), TW, TH, 4, 3, COLOR_ROMAN_GOLD);
    Set(p.data(), TW, TH, 11, 3, COLOR_ROMAN_GOLD);
    s_tiles[TILE_GOLGOTHA_LEDGE_2] = CreateTextureFromPixels(TW, TH, p.data());

    // 32. TILE_GOLGOTHA_COLUMN_HEAD
    for (int i = 0; i < TW * TH; ++i) p[i] = BLANK;
    FillRect(p.data(), TW, TH, 2, 2, 12, 14, cragMid);
    FillRect(p.data(), TW, TH, 1, 0, 14, 4, ironHi);
    OutlineRect(p.data(), TW, TH, 1, 0, 14, 4, ironDark);
    HLine(p.data(), TW, TH, 2, 13, 1, Color{ 255, 235, 140, 255 });
    s_tiles[TILE_GOLGOTHA_COLUMN_HEAD] = CreateTextureFromPixels(TW, TH, p.data());

    // 33. TILE_GOLGOTHA_COLUMN_MID
    for (int i = 0; i < TW * TH; ++i) p[i] = BLANK;
    FillRect(p.data(), TW, TH, 3, 0, 10, TH, cragMid);
    VLine(p.data(), TW, TH, 3, 0, TH - 1, cragHi);
    VLine(p.data(), TW, TH, 12, 0, TH - 1, cragDark);
    s_tiles[TILE_GOLGOTHA_COLUMN_MID] = CreateTextureFromPixels(TW, TH, p.data());

    // 34. TILE_GOLGOTHA_COLUMN_BASE
    for (int i = 0; i < TW * TH; ++i) p[i] = BLANK;
    FillRect(p.data(), TW, TH, 3, 0, 10, 10, cragMid);
    FillRect(p.data(), TW, TH, 1, 10, 14, 6, ironDark);
    HLine(p.data(), TW, TH, 2, 13, 10, ironHi);
    s_tiles[TILE_GOLGOTHA_COLUMN_BASE] = CreateTextureFromPixels(TW, TH, p.data());

    // 35. TILE_GOLGOTHA_BRAZIER (Roaring Fortress Fire)
    for (int i = 0; i < TW * TH; ++i) p[i] = BLANK;
    FillRect(p.data(), TW, TH, 3, 8, 10, 5, ironDark);
    Line(p.data(), TW, TH, 3, 13, 1, 15, ironDark);
    Line(p.data(), TW, TH, 12, 13, 14, 15, ironDark);
    FillCircle(p.data(), TW, TH, 8, 5, 4, flameOrange);
    FillCircle(p.data(), TW, TH, 8, 4, 2, flameHot);
    Set(p.data(), TW, TH, 6, 2, flameRed);
    Set(p.data(), TW, TH, 9, 3, flameRed);
    s_tiles[TILE_GOLGOTHA_BRAZIER] = CreateTextureFromPixels(TW, TH, p.data());

    // 36. TILE_GOLGOTHA_STANDARD (Roman Imperial Eagle Standard)
    for (int i = 0; i < TW * TH; ++i) p[i] = BLANK;
    VLine(p.data(), TW, TH, 8, 0, TH - 1, Color{ 175, 120, 25, 255 });
    // Golden Eagle Aquila
    FillCircle(p.data(), TW, TH, 8, 3, 3, COLOR_GOLD_BRIGHT);
    Line(p.data(), TW, TH, 4, 2, 12, 2, COLOR_GOLD_BRIGHT);
    // Scarlet SPQR Banner
    FillRect(p.data(), TW, TH, 4, 6, 8, 9, COLOR_ROMAN_CRIMSON);
    OutlineRect(p.data(), TW, TH, 4, 6, 8, 9, COLOR_ROMAN_GOLD);
    Set(p.data(), TW, TH, 6, 9, COLOR_ROMAN_GOLD);
    Set(p.data(), TW, TH, 9, 9, COLOR_ROMAN_GOLD);
    s_tiles[TILE_GOLGOTHA_STANDARD] = CreateTextureFromPixels(TW, TH, p.data());

    // 37. TILE_GOLGOTHA_WALL_STONE
    for (int y = 0; y < TH; ++y) {
        for (int x = 0; x < TW; ++x) {
            Color c = cragDark;
            if (x == 0 || y == 0 || y == 8 || (y < 8 && x == 8)) c = Color{ 25, 22, 28, 255 };
            else if (x == 1 || y == 1 || y == 9 || (y < 8 && x == 9)) c = cragMid;
            p[y * TW + x] = c;
        }
    }
    s_tiles[TILE_GOLGOTHA_WALL_STONE] = CreateTextureFromPixels(TW, TH, p.data());
}

// Generate the 3-layer parallax backgrounds
void GraphicsManager::GenerateParallaxBackgrounds() {
    using namespace PixelArt;
    // =========================================================================
    // 4-LAYER MULTI-DEPTH CINEMATIC PARALLAX BACKGROUND SYSTEM
    // Featuring The Monumental Temple & Citadel of David (Migdal David)
    // =========================================================================

    // 1. Far Celestial Sky & Distant Mountains (320x160, 0.05x scroll)
    constexpr int SKY_W = 320;
    constexpr int SKY_H = 160;
    std::vector<Color> bgSky(SKY_W * SKY_H);

    for (int y = 0; y < SKY_H; ++y) {
        float skyT = static_cast<float>(y) / SKY_H;
        // Judean twilight sunset gradient
        Color skyCol;
        if (skyT < 0.25f) {
            float t = skyT / 0.25f;
            skyCol = {
                static_cast<unsigned char>(32 + t * 46),
                static_cast<unsigned char>(22 + t * 14),
                static_cast<unsigned char>(54 + t * 14),
                255
            };
        } else if (skyT < 0.58f) {
            float t = (skyT - 0.25f) / 0.33f;
            skyCol = {
                static_cast<unsigned char>(78 + t * 107),
                static_cast<unsigned char>(36 + t * 26),
                static_cast<unsigned char>(68 - t * 20),
                255
            };
        } else {
            float t = (skyT - 0.58f) / 0.42f;
            skyCol = {
                static_cast<unsigned char>(185 + t * 70),
                static_cast<unsigned char>(62 + t * 133),
                static_cast<unsigned char>(48 + t * 27),
                255
            };
        }

        for (int x = 0; x < SKY_W; ++x) {
            // Radiant Golden Setting Sun (Partially dipping behind mountains)
            float sunDist = sqrtf((x - 110) * (x - 110) + (y - 75) * (y - 75));
            if (sunDist < 36.0f) {
                float sunA = 1.0f - (sunDist / 36.0f);
                skyCol.r = static_cast<unsigned char>(std::min(255.0f, skyCol.r + sunA * 160.0f));
                skyCol.g = static_cast<unsigned char>(std::min(255.0f, skyCol.g + sunA * 140.0f));
                skyCol.b = static_cast<unsigned char>(std::min(255.0f, skyCol.b + sunA * 85.0f));
            }

            // Distant Mountains of Moab silhouette
            float mtn1 = 104.0f + 16.0f * sinf(x * 0.018f) + 8.0f * cosf(x * 0.045f);
            float mtn2 = 122.0f + 12.0f * sinf(x * 0.035f + 1.2f);

            // Silhouetted flock of peaceful doves soaring in V-formation
            bool isDove = ((x == 160 && y == 42) || (x == 168 && y == 46) || (x == 152 && y == 46) ||
                           (x == 176 && y == 51) || (x == 144 && y == 51));

            if (isDove) {
                bgSky[y * SKY_W + x] = Color{ 255, 248, 230, 230 };
            } else if (y >= mtn2) {
                // Secondary foothills in warm dusty umber
                bgSky[y * SKY_W + x] = ((x + y) % 4 == 0) ? Color{ 82, 54, 62, 255 } : Color{ 96, 64, 72, 255 };
            } else if (y >= mtn1) {
                // Deep twilight purple mountain ridge
                bgSky[y * SKY_W + x] = ((x + y) % 5 == 0) ? Color{ 62, 42, 68, 255 } : Color{ 74, 50, 78, 255 };
            } else {
                bgSky[y * SKY_W + x] = skyCol;
            }
        }
    }
    s_bgSkyDistant = CreateTextureFromPixels(SKY_W, SKY_H, bgSky.data());

    // 2. The Temple & Citadel of David Panorama (960x160, 0.20x scroll)
    // Towering at the 3rd and 4th screens (x = 280 to 680 of the 960px texture)
    constexpr int DAVID_W = 960;
    constexpr int DAVID_H = 160;
    std::vector<Color> bgDavid(DAVID_W * DAVID_H, BLANK);

    for (int y = 0; y < DAVID_H; ++y) {
        for (int x = 0; x < DAVID_W; ++x) {
            // Palette for Herodian and Davidic stone architecture
            Color ashlarLight = Color{ 235, 222, 198, 255 };
            Color ashlarMid   = Color{ 198, 182, 156, 255 };
            Color ashlarDark  = Color{ 148, 132, 108, 255 };
            Color ashlarDeep  = Color{ 102,  88,  70, 255 };
            Color bronzeGold  = Color{ 238, 195,  45, 255 };
            Color bronzeBright= Color{ 255, 235, 110, 255 };
            Color royalBlue   = Color{  28,  55, 130, 255 };
            Color cedarWood   = Color{ 115,  72,  38, 255 };

            // ==============================================================
            // SECTION B: THE TEMPLE & CITADEL OF DAVID (x = 280 to 680)
            // 4 Architectural Tiers / Levels:
            // Tier 1: Stepped Stone Structure Bedrock Foundation (y = 118 to 158)
            // Tier 2: Fortified Royal Terrace & Gate of Zion (y = 88 to 117)
            // Tier 3: Palace of King David & Proto-Aeolic Colonnade (y = 56 to 87)
            // Tier 4: Citadel Keep - Tower of David & Bronze Cupola (y = 18 to 55)
            // ==============================================================
            if (x >= 280 && x <= 680) {
                // TIER 4: Citadel Keep - Tower of David (Migdal David) (x = 430 to 520, y = 18 to 55)
                bool isCitadelMast   = (x == 475 && y >= 8 && y <= 18);
                bool isCitadelPennant= (x >= 476 && x <= 494 && y >= 10 && y <= 15);
                bool isCitadelDome   = (x >= 460 && x <= 490 && y >= 18 && y <= 32);
                bool isCitadelCornice= (x >= 430 && x <= 520 && y >= 33 && y <= 36);
                bool isCitadelKeep   = (x >= 435 && x <= 515 && y >= 37 && y <= 55);
                bool isCitadelCrenel = (isCitadelKeep && y <= 40 && (x % 10 < 6));

                // TIER 3: Palace of King David (Beit David) (x = 340 to 610, y = 56 to 87)
                bool isPalaceRoof    = (x >= 335 && x <= 615 && y >= 56 && y <= 59);
                bool isPalaceHall    = (x >= 340 && x <= 610 && y >= 60 && y <= 87);
                bool isProtoPillar   = (isPalaceHall && (x % 38 >= 8 && x % 38 <= 13));
                bool isPalaceWindow  = (isPalaceHall && (x % 38 >= 20 && x % 38 <= 26) && y >= 65 && y <= 76);
                bool isDavidBanner   = (isPalaceHall && (x % 76 >= 32 && x % 76 <= 38) && y >= 62 && y <= 82);

                // TIER 2: Fortified Royal Terrace & Gate of Zion (x = 300 to 650, y = 88 to 117)
                bool isGateWall      = (x >= 300 && x <= 650 && y >= 88 && y <= 117);
                bool isTerraceCrenel = (isGateWall && y >= 88 && y <= 92 && (x % 12 < 7));
                bool isZionGateArch  = (x >= 450 && x <= 500 && y >= 94 && y <= 117);
                bool isGateInnerDoors= (x >= 458 && x <= 492 && y >= 100 && y <= 117);

                // TIER 1: The Stepped Stone Structure Foundation (x = 280 to 670, y = 118 to 158)
                bool isSteppedStone  = (x >= 280 && x <= 670 && y >= 118);

                if (isCitadelMast) {
                    bgDavid[y * DAVID_W + x] = cedarWood;
                } else if (isCitadelPennant) {
                    bgDavid[y * DAVID_W + x] = (y == 12 || y == 13) ? bronzeGold : royalBlue;
                } else if (isCitadelDome) {
                    // Polished Bronze Cupola catching sunset rays
                    float dx = x - 475.0f;
                    float dy = y - 32.0f;
                    if (dx * dx * 0.55f + dy * dy <= 196.0f) {
                        bool specular = (fabsf(dx - 5.0f) < 3.0f && y <= 24);
                        bgDavid[y * DAVID_W + x] = specular ? Color{ 255, 255, 240, 255 } : ((dx < 0) ? bronzeBright : bronzeGold);
                    }
                } else if (isCitadelCornice) {
                    bgDavid[y * DAVID_W + x] = (y == 33) ? ashlarLight : ashlarDark;
                } else if (isCitadelKeep) {
                    if (y <= 40 && !isCitadelCrenel) {
                        // Air between battlements
                    } else {
                        // Drafted Herodian ashlar blocks with margin bevels
                        bool hBevel = (y % 7 == 0);
                        bool vBevel = ((x + (y / 7) * 9) % 18 == 0);
                        bgDavid[y * DAVID_W + x] = (hBevel || vBevel) ? ashlarDeep : (((x + y) % 3 == 0) ? ashlarLight : ashlarMid);
                    }
                } else if (isDavidBanner) {
                    // Royal Blue Judean standard with golden Star of David
                    bgDavid[y * DAVID_W + x] = ((y >= 70 && y <= 74) && (x % 76 >= 34 && x % 76 <= 36)) ? bronzeBright : royalBlue;
                } else if (isPalaceWindow) {
                    // Arched window glowing with royal golden candlelight
                    bgDavid[y * DAVID_W + x] = (y == 65) ? Color{ 255, 240, 150, 255 } : Color{ 245, 185, 60, 255 };
                } else if (isProtoPillar) {
                    // Proto-Aeolic fluted stone column
                    bool volute = (y >= 60 && y <= 63);
                    bgDavid[y * DAVID_W + x] = volute ? bronzeGold : ((x % 38 == 10) ? ashlarLight : ashlarMid);
                } else if (isPalaceRoof) {
                    bgDavid[y * DAVID_W + x] = (y == 56) ? bronzeGold : cedarWood;
                } else if (isPalaceHall) {
                    bool hLine = (y % 8 == 0);
                    bgDavid[y * DAVID_W + x] = hLine ? ashlarDark : ashlarMid;
                } else if (isGateInnerDoors) {
                    // Bronze-studded heavy cedar gate
                    bgDavid[y * DAVID_W + x] = (x == 475) ? Color{ 50, 30, 15, 255 } : (((x + y) % 4 == 0) ? bronzeGold : cedarWood);
                } else if (isZionGateArch) {
                    bgDavid[y * DAVID_W + x] = (y <= 97) ? bronzeGold : ashlarDark;
                } else if (isGateWall) {
                    if (y <= 92 && !isTerraceCrenel) {
                        // Battlement opening
                    } else {
                        bool hCourse = (y % 6 == 0);
                        bool vCourse = ((x + (y / 6) * 11) % 20 == 0);
                        bgDavid[y * DAVID_W + x] = (hCourse || vCourse) ? ashlarDeep : ashlarMid;
                    }
                } else if (isSteppedStone) {
                    // Megalithic stepped stone structure courses
                    int step = (y - 118) / 5;
                    bool stepEdge = ((y - 118) % 5 == 0);
                    bgDavid[y * DAVID_W + x] = stepEdge ? ashlarLight : ((step % 2 == 0) ? ashlarMid : ashlarDark);
                }
            }
            // ==============================================================
            // SECTION A: Ancient Jerusalem City Walls & Watchtowers (x = 0 to 280)
            // ==============================================================
            else if (x < 280) {
                bool isWall = (y >= 92 && y <= 150);
                bool isCrenel = (y >= 92 && y <= 96 && (x % 14 < 8));
                bool isTower = ((x >= 50 && x <= 78) || (x >= 180 && x <= 208)) && (y >= 75);
                bool isTowerCrenel = (isTower && y >= 75 && y <= 79 && (x % 8 < 5));

                if (isTower) {
                    if (y <= 79 && !isTowerCrenel) {}
                    else {
                        bool hCourse = (y % 7 == 0);
                        bgDavid[y * DAVID_W + x] = hCourse ? ashlarDeep : ashlarMid;
                    }
                } else if (isWall) {
                    if (y <= 96 && !isCrenel) {}
                    else {
                        bool hCourse = (y % 6 == 0);
                        bgDavid[y * DAVID_W + x] = hCourse ? ashlarDeep : Color{ 175, 160, 138, 255 };
                    }
                }
            }
            // ==============================================================
            // SECTION C: Mount Moriah, Second Temple & Antonia Fortress (x = 680 to 960)
            // ==============================================================
            else {
                // Second Temple Colonnade (x = 700 to 820, y = 62 to 140)
                bool isTemplePortico = (x >= 710 && x <= 810 && y >= 65 && y <= 140);
                bool isTemplePediment= (x >= 730 && x <= 790 && y >= 52 && y <= 64);
                bool isTemplePillar  = (isTemplePortico && (x % 14 < 4));

                // Antonia Fortress Garrison (x = 840 to 940, y = 40 to 155)
                bool isAntonia = (x >= 840 && x <= 940 && y >= 45 && y <= 155);
                bool isAntoniaTower = (x >= 890 && x <= 930 && y >= 32);
                bool isAntoniaFlag  = (x == 910 && y >= 16 && y <= 32);
                bool isSPQRBanner   = (x >= 911 && x <= 928 && y >= 18 && y <= 27);

                if (isAntoniaFlag) {
                    bgDavid[y * DAVID_W + x] = cedarWood;
                } else if (isSPQRBanner) {
                    bgDavid[y * DAVID_W + x] = (y == 22 || y == 23) ? bronzeGold : COLOR_ROMAN_CRIMSON;
                } else if (isAntoniaTower && y <= 38 && (x % 8 >= 5)) {
                    // Crenel gap
                } else if (isAntoniaTower || isAntonia) {
                    bool hC = (y % 7 == 0);
                    bgDavid[y * DAVID_W + x] = hC ? Color{ 110, 95, 85, 255 } : Color{ 160, 145, 130, 255 };
                } else if (isTemplePediment) {
                    bgDavid[y * DAVID_W + x] = bronzeBright;
                } else if (isTemplePortico) {
                    bgDavid[y * DAVID_W + x] = isTemplePillar ? Color{ 245, 240, 230, 255 } : Color{ 145, 115, 95, 255 };
                }
            }
        }
    }
    s_bgTempleOfDavid = CreateTextureFromPixels(DAVID_W, DAVID_H, bgDavid.data());

    // 3. Midground Ancient Jerusalem Villas & Aqueducts (640x160, 0.45x scroll)
    constexpr int MID_W = 640;
    constexpr int MID_H = 160;
    std::vector<Color> bgMid(MID_W * MID_H, BLANK);

    for (int y = 0; y < MID_H; ++y) {
        for (int x = 0; x < MID_W; ++x) {
            // Residential Villas with flat rooftops, pergolas & windows
            int houseX = x % 160;
            bool isHouse1 = (houseX >= 10 && houseX <= 64 && y >= 92 && y <= 155);
            bool isHouse2 = (houseX >= 94 && houseX <= 150 && y >= 82 && y <= 155);

            if (isHouse1 || isHouse2) {
                int roofY = isHouse1 ? 92 : 82;
                if (y == roofY || y == roofY + 1) {
                    bgMid[y * MID_W + x] = Color{ 185, 170, 148, 240 };
                } else {
                    bgMid[y * MID_W + x] = ((x ^ (y * 2)) % 8 == 0) ? Color{ 142, 126, 110, 235 } : Color{ 165, 150, 132, 235 };
                }

                // Arched stone windows glowing with warm interior candlelight
                int winY1 = roofY + 14;
                int winY2 = roofY + 34;
                int winX1 = isHouse1 ? 26 : 110;
                int winX2 = isHouse1 ? 48 : 134;
                if ((abs(houseX - winX1) <= 3 && y >= winY1 && y <= winY1 + 7) ||
                    (abs(houseX - winX2) <= 3 && y >= winY1 && y <= winY1 + 7) ||
                    (abs(houseX - winX1) <= 3 && y >= winY2 && y <= winY2 + 7)) {
                    bgMid[y * MID_W + x] = (y == winY1 || y == winY2) ? Color{ 255, 230, 120, 255 } : Color{ 245, 175, 55, 255 };
                }
            }

            // Roman Aqueduct Arches spanning valley between buildings
            int archSpan = x % 80;
            if (y >= 74 && y <= 84 && !isHouse1 && !isHouse2) {
                bgMid[y * MID_W + x] = (y == 74) ? Color{ 195, 180, 160, 230 } : Color{ 155, 140, 120, 230 };
            } else if (y > 84 && y < 144 && !isHouse1 && !isHouse2) {
                if (archSpan < 14 || archSpan > 66) {
                    bgMid[y * MID_W + x] = Color{ 142, 126, 108, 230 }; // Massive arch pillar
                } else {
                    float archY = 84.0f + 28.0f * sinf((archSpan - 14) / 52.0f * 3.14159f);
                    if (y <= archY) {
                        bgMid[y * MID_W + x] = Color{ 148, 132, 112, 230 };
                    }
                }
            }

            // Mediterranean Cypress Trees (Deep forest green spires)
            int treeX = x % 100;
            if (treeX >= 76 && treeX <= 84 && y >= 85 && y <= 152) {
                int dist = abs(treeX - 80);
                int maxW = (152 - y) / 10 + 1;
                if (dist <= maxW) {
                    bgMid[y * MID_W + x] = ((x + y) % 3 == 0) ? Color{ 48, 82, 48, 245 } : Color{ 32, 58, 32, 245 };
                }
            }
        }
    }
    s_bgMidJerusalemVillas = CreateTextureFromPixels(MID_W, MID_H, bgMid.data());

    // 4. Near Streetscape Columns, Tapestries & Braziers (480x160, 0.70x scroll)
    constexpr int NEAR_W = 480;
    constexpr int NEAR_H = 160;
    std::vector<Color> bgNear(NEAR_W * NEAR_H, BLANK);

    for (int y = 0; y < NEAR_H; ++y) {
        for (int x = 0; x < NEAR_W; ++x) {
            // Classical Corinthian Portico Columns every 120px
            int colSpan = x % 120;
            if (colSpan >= 10 && colSpan <= 22 && y >= 35 && y <= 155) {
                if (y <= 42) {
                    // Carved Acanthus Capital
                    bgNear[y * NEAR_W + x] = Color{ 235, 225, 205, 230 };
                } else {
                    // Fluted shaft
                    bool flute = (colSpan % 3 == 0);
                    bgNear[y * NEAR_W + x] = flute ? Color{ 165, 150, 132, 220 } : Color{ 205, 192, 172, 220 };
                }
            }

            // Hanging Roman SPQR Banner
            if (colSpan >= 55 && colSpan <= 67 && y >= 45 && y <= 85) {
                bgNear[y * NEAR_W + x] = (y <= 47 || y >= 83) ? COLOR_ROMAN_GOLD : COLOR_ROMAN_CRIMSON;
                if (y >= 58 && y <= 64 && colSpan >= 58 && colSpan <= 64) {
                    bgNear[y * NEAR_W + x] = COLOR_GOLD_BRIGHT; // SPQR Eagle
                }
            }

            // Bronze Street Braziers with crackling fire flames
            if (colSpan >= 88 && colSpan <= 96 && y >= 115 && y <= 145) {
                if (y >= 132) {
                    // Tripod stand
                    bgNear[y * NEAR_W + x] = Color{ 80, 50, 25, 240 };
                } else if (y >= 126) {
                    // Bronze bowl
                    bgNear[y * NEAR_W + x] = Color{ 190, 135, 50, 255 };
                } else {
                    // Crackling fire
                    bgNear[y * NEAR_W + x] = (y <= 120) ? Color{ 255, 235, 110, 255 } : Color{ 245, 95, 25, 255 };
                }
            }
        }
    }
    s_bgNearStreetscape = CreateTextureFromPixels(NEAR_W, NEAR_H, bgNear.data());

    // =========================================================================
    // STAGE 2: TEMPLE SANCTUARY & COLONNADE PARALLAX LAYERS
    // =========================================================================
    // 5. Far Temple Sanctuary Interior (320x160, 0.15x scroll)
    constexpr int TS_W = 320;
    constexpr int TS_H = 160;
    std::vector<Color> bgSanctuary(TS_W * TS_H);

    for (int y = 0; y < TS_H; ++y) {
        for (int x = 0; x < TS_W; ++x) {
            float ty = static_cast<float>(y) / TS_H;
            Color wallBase = {
                static_cast<unsigned char>(35 + ty * 30),
                static_cast<unsigned char>(25 + ty * 25),
                static_cast<unsigned char>(40 + ty * 20),
                255
            };
            bgSanctuary[y * TS_W + x] = wallBase;

            // Vaulted Cedar Coffered Ceiling (y = 0 to 42)
            if (y <= 42) {
                if (y % 14 == 0 || y % 14 == 1) {
                    bgSanctuary[y * TS_W + x] = COLOR_TEMPLE_CEDAR;
                } else if (x % 32 == 0 || x % 32 == 1) {
                    bgSanctuary[y * TS_W + x] = COLOR_TEMPLE_CEDAR;
                } else if ((x % 32 == 16) && (y % 14 == 7)) {
                    bgSanctuary[y * TS_W + x] = COLOR_TEMPLE_GOLD;
                }
            }

            // High Clerestory Arched Windows (y = 44 to 76, spaced every 48 px)
            int winX = x % 48;
            if (winX >= 16 && winX <= 32 && y >= 44 && y <= 76) {
                int dx = winX - 24;
                int dy = y - 56;
                if (y <= 56 || (dx * dx + dy * dy <= 64)) {
                    bgSanctuary[y * TS_W + x] = Color{ 215, 235, 255, 255 };
                }
            }
        }
    }

    // Crepuscular Morning God Rays streaming down from clerestory windows
    for (int wx = 24; wx < TS_W; wx += 48) {
        for (int step = 0; step < 75; ++step) {
            int rx = wx + step * 2;
            int ry = 58 + step;
            if (rx >= 0 && rx < TS_W && ry >= 0 && ry < TS_H) {
                Blend(bgSanctuary.data(), TS_W, TS_H, rx, ry, Color{ 255, 245, 180, 45 });
                Blend(bgSanctuary.data(), TS_W, TS_H, rx + 1, ry, Color{ 255, 255, 220, 35 });
            }
        }
    }
    s_bgTempleSanctuary = CreateTextureFromPixels(TS_W, TS_H, bgSanctuary.data());

    // 6. Near Temple Colonnade (480x160, 0.50x scroll)
    constexpr int TC_W = 480;
    constexpr int TC_H = 160;
    std::vector<Color> bgColonnade(TC_W * TC_H, BLANK);

    for (int px = 20; px < TC_W; px += 60) {
        FillRect(bgColonnade.data(), TC_W, TC_H, px - 6, 12, 14, 10, COLOR_TEMPLE_GOLD);
        HLine(bgColonnade.data(), TC_W, TC_H, px - 7, px + 8, 12, Color{ 255, 245, 160, 255 });
        FillRect(bgColonnade.data(), TC_W, TC_H, px - 4, 22, 10, 130, COLOR_TEMPLE_MARBLE);
        VLine(bgColonnade.data(), TC_W, TC_H, px - 4, 22, 151, COLOR_TEMPLE_SHADOW);
        VLine(bgColonnade.data(), TC_W, TC_H, px + 5, 22, 151, COLOR_TEMPLE_SHADOW);
        VLine(bgColonnade.data(), TC_W, TC_H, px, 22, 151, Color{ 255, 255, 255, 240 });
        FillRect(bgColonnade.data(), TC_W, TC_H, px - 6, 148, 14, 8, COLOR_TEMPLE_MARBLE);

        int lampX = px + 30;
        if (lampX < TC_W - 10) {
            VLine(bgColonnade.data(), TC_W, TC_H, lampX, 10, 32, Color{ 140, 100, 40, 255 });
            FillCircle(bgColonnade.data(), TC_W, TC_H, lampX, 35, 3, Color{ 215, 150, 45, 255 });
            FillCircle(bgColonnade.data(), TC_W, TC_H, lampX, 35, 7, Color{ 255, 210, 80, 50 });
            Set(bgColonnade.data(), TC_W, TC_H, lampX, 34, Color{ 255, 255, 220, 255 });
        }

        if (px + 60 < TC_W) {
            for (int dx = 6; dx < 54; ++dx) {
                float swagT = sinf((dx - 6) / 48.0f * 3.14159f);
                int swagY = static_cast<int>(18 + swagT * 14.0f);
                FillRect(bgColonnade.data(), TC_W, TC_H, px + dx, swagY, 1, 6, COLOR_TEMPLE_PURPLE);
                Set(bgColonnade.data(), TC_W, TC_H, px + dx, swagY + 6, COLOR_TEMPLE_GOLD);
            }
        }
    }
    s_bgTempleColonnade = CreateTextureFromPixels(TC_W, TC_H, bgColonnade.data());

    // =========================================================================
    // STAGE 3: GOLGOTHA CITADEL PARALLAX LAYERS
    // =========================================================================
    // 7. Golgotha Sky & Mount Calvary Distant Horizon (320x160, 0.10x scroll)
    constexpr int GS_W = 320, GS_H = 160;
    std::vector<Color> bgGolgothaSky(GS_W * GS_H);

    for (int y = 0; y < GS_H; ++y) {
        float ty = (float)y / (float)GS_H;
        Color skyCol;
        if (ty < 0.35f) {
            float t = ty / 0.35f;
            skyCol = Color{ (unsigned char)(28 + t * 30), (unsigned char)(22 + t * 24), (unsigned char)(42 + t * 32), 255 };
        } else if (ty < 0.70f) {
            float t = (ty - 0.35f) / 0.35f;
            skyCol = Color{ (unsigned char)(58 + t * 75), (unsigned char)(46 + t * 45), (unsigned char)(74 - t * 20), 255 };
        } else {
            float t = (ty - 0.70f) / 0.30f;
            skyCol = Color{ (unsigned char)(133 + t * 50), (unsigned char)(91 + t * 35), (unsigned char)(54 - t * 15), 255 };
        }

        for (int x = 0; x < GS_W; ++x) {
            bgGolgothaSky[y * GS_W + x] = skyCol;
        }
    }

    // Distant dark jagged silhouette of Mount Golgotha & Calvary hills
    for (int x = 0; x < GS_W; ++x) {
        float peak1 = sinf(x * 0.024f) * 22.0f;
        float peak2 = sinf(x * 0.065f + 1.2f) * 12.0f;
        int hillY = static_cast<int>(105 + peak1 + peak2);
        for (int y = hillY; y < GS_H; ++y) {
            bgGolgothaSky[y * GS_W + x] = Color{ 36, 30, 42, 255 };
        }
    }
    s_bgGolgothaSky = CreateTextureFromPixels(GS_W, GS_H, bgGolgothaSky.data());

    // 8. Golgotha Near Crags & Fortress Antonia Ramparts (480x160, 0.45x scroll)
    constexpr int GC_W = 480, GC_H = 160;
    std::vector<Color> bgGolgothaCrags(GC_W * GC_H, BLANK);

    for (int x = 0; x < GC_W; ++x) {
        float cragForm = sinf(x * 0.038f) * 28.0f + cosf(x * 0.082f) * 14.0f;
        int cragTop = static_cast<int>(88 + cragForm);
        for (int y = cragTop; y < GC_H; ++y) {
            Color rock = ((x + y * 2) % 6 == 0) ? Color{ 58, 52, 60, 240 } : Color{ 74, 66, 76, 240 };
            if (y == cragTop) rock = Color{ 105, 96, 110, 255 };
            bgGolgothaCrags[y * GC_W + x] = rock;
        }

        // Antonia fortress watchtowers and battlements every 120 pixels
        if (x % 120 >= 10 && x % 120 <= 46) {
            for (int y = 52; y < GC_H; ++y) {
                if (y == 52 && (x % 6 < 3)) continue; // Crenellations
                bgGolgothaCrags[y * GC_W + x] = ((x ^ y) % 7 == 0) ? Color{ 48, 42, 52, 250 } : Color{ 62, 55, 66, 250 };
            }
            // Burning torch / signal brazier atop watchtowers
            if (x % 120 == 28) {
                VLine(bgGolgothaCrags.data(), GC_W, GC_H, x, 40, 52, Color{ 30, 26, 34, 255 });
                FillCircle(bgGolgothaCrags.data(), GC_W, GC_H, x, 39, 3, Color{ 255, 130, 40, 255 });
                Set(bgGolgothaCrags.data(), GC_W, GC_H, x, 38, Color{ 255, 235, 120, 255 });
            }
        }
    }
    s_bgGolgothaCrags = CreateTextureFromPixels(GC_W, GC_H, bgGolgothaCrags.data());
}

void GraphicsManager::GenerateMissionMap() {
    using namespace PixelArt;
    constexpr int MW = VIRTUAL_WIDTH;
    constexpr int MH = VIRTUAL_HEIGHT;
    std::vector<Color> mapPix(MW * MH);

    Color parchmentBase = Color{ 236, 222, 192, 255 };
    Color parchmentDark = Color{ 208, 188, 152, 255 };
    Color parchmentEdge = Color{ 175, 150, 112, 255 };
    Color inkDark       = Color{  52,  38,  28, 255 };
    Color inkRed        = Color{ 175,  28,  32, 255 };
    Color inkGold       = Color{ 195, 145,  35, 255 };
    Color waterBlue     = Color{ 180, 205, 218, 255 };

    // 1. Aged Antique Vellum / Parchment Mottling & Vignette
    for (int y = 0; y < MH; ++y) {
        for (int x = 0; x < MW; ++x) {
            float edgeDist = std::min({ (float)x, (float)(MW - 1 - x), (float)y, (float)(MH - 1 - y) });
            float vignette = std::clamp(edgeDist / 32.0f, 0.0f, 1.0f);
            float grain = ((x * 13 + y * 29) % 17) / 17.0f * 8.0f;
            Color c = {
                static_cast<unsigned char>(parchmentEdge.r + vignette * (parchmentBase.r - parchmentEdge.r) + grain),
                static_cast<unsigned char>(parchmentEdge.g + vignette * (parchmentBase.g - parchmentEdge.g) + grain),
                static_cast<unsigned char>(parchmentEdge.b + vignette * (parchmentBase.b - parchmentEdge.b) + grain),
                255
            };
            mapPix[y * MW + x] = c;
        }
    }

    // 2. Classical Roman / Judean Cartouche Border
    OutlineRect(mapPix.data(), MW, MH, 4, 4, MW - 8, MH - 8, inkDark);
    OutlineRect(mapPix.data(), MW, MH, 7, 7, MW - 14, MH - 14, inkGold);
    OutlineRect(mapPix.data(), MW, MH, 9, 9, MW - 18, MH - 18, inkDark);
    FillRect(mapPix.data(), MW, MH, 5, 5, 4, 4, inkGold);
    FillRect(mapPix.data(), MW, MH, MW - 9, 5, 4, 4, inkGold);
    FillRect(mapPix.data(), MW, MH, 5, MH - 9, 4, 4, inkGold);
    FillRect(mapPix.data(), MW, MH, MW - 9, MH - 9, 4, 4, inkGold);

    // 3. Topographic Contours (Hills of Jerusalem & Valleys)
    for (int y = 20; y < 165; ++y) {
        int kx = static_cast<int>(205 + sinf(y * 0.06f) * 16.0f);
        Set(mapPix.data(), MW, MH, kx, y, waterBlue);
        Set(mapPix.data(), MW, MH, kx + 1, y, waterBlue);
        Set(mapPix.data(), MW, MH, kx - 1, y, parchmentDark);
    }
    for (int x = 40; x < 210; ++x) {
        int hy = static_cast<int>(148 + sinf(x * 0.04f) * 8.0f);
        Set(mapPix.data(), MW, MH, x, hy, parchmentDark);
        Set(mapPix.data(), MW, MH, x, hy + 1, inkDark);
    }

    // 4. Fortified Stone City Walls
    HLine(mapPix.data(), MW, MH, 30, 240, 48, inkDark);
    HLine(mapPix.data(), MW, MH, 30, 240, 49, parchmentDark);
    for (int x = 32; x < 238; x += 6) {
        FillRect(mapPix.data(), MW, MH, x, 46, 3, 2, inkDark);
    }
    VLine(mapPix.data(), MW, MH, 30, 48, 145, inkDark);
    HLine(mapPix.data(), MW, MH, 30, 210, 145, inkDark);
    VLine(mapPix.data(), MW, MH, 210, 48, 145, inkDark);

    // 5. Campaign March Dotted Red Route
    for (int t = 0; t <= 30; ++t) {
        if (t % 2 == 0) {
            float frac = t / 30.0f;
            int rx = static_cast<int>(60 + frac * (160 - 60));
            int ry = static_cast<int>(90 + frac * (78 - 90) + sinf(frac * 3.14159f) * -10.0f);
            FillCircle(mapPix.data(), MW, MH, rx, ry, 1, inkRed);
        }
    }
    for (int t = 0; t <= 30; ++t) {
        if (t % 2 == 0) {
            float frac = t / 30.0f;
            int rx = static_cast<int>(160 + frac * (255 - 160));
            int ry = static_cast<int>(78 + frac * (95 - 78) + sinf(frac * 3.14159f) * -12.0f);
            FillCircle(mapPix.data(), MW, MH, rx, ry, 1, inkRed);
        }
    }

    // 6. Area 1 Vignette: Fortress Antonia & Roman Garrison (X ~ 48..76, Y ~ 70..100)
    FillRect(mapPix.data(), MW, MH, 48, 76, 28, 24, Color{ 220, 205, 175, 255 });
    OutlineRect(mapPix.data(), MW, MH, 48, 76, 28, 24, inkDark);
    FillRect(mapPix.data(), MW, MH, 46, 70, 10, 8, Color{ 220, 205, 175, 255 });
    OutlineRect(mapPix.data(), MW, MH, 46, 70, 10, 8, inkDark);
    FillRect(mapPix.data(), MW, MH, 68, 70, 10, 8, Color{ 220, 205, 175, 255 });
    OutlineRect(mapPix.data(), MW, MH, 68, 70, 10, 8, inkDark);
    FillRect(mapPix.data(), MW, MH, 54, 62, 10, 6, inkRed);
    VLine(mapPix.data(), MW, MH, 53, 60, 70, inkDark);
    for (int ax = 78; ax < 105; ax += 8) {
        HLine(mapPix.data(), MW, MH, ax, ax + 7, 88, inkDark);
        VLine(mapPix.data(), MW, MH, ax, 88, 98, inkDark);
    }

    // 7. Area 2 Vignette: The Great Temple Mount & Porch (X ~ 142..180, Y ~ 54..92)
    FillRect(mapPix.data(), MW, MH, 142, 66, 38, 26, Color{ 245, 240, 225, 255 });
    OutlineRect(mapPix.data(), MW, MH, 142, 66, 38, 26, inkDark);
    for (int cx = 145; cx <= 177; cx += 4) {
        VLine(mapPix.data(), MW, MH, cx, 68, 86, inkDark);
    }
    FillRect(mapPix.data(), MW, MH, 152, 54, 18, 14, Color{ 255, 248, 230, 255 });
    OutlineRect(mapPix.data(), MW, MH, 152, 54, 18, 14, inkDark);
    Line(mapPix.data(), MW, MH, 151, 54, 161, 46, inkGold);
    Line(mapPix.data(), MW, MH, 171, 54, 161, 46, inkGold);
    VLine(mapPix.data(), MW, MH, 161, 42, 46, inkGold);

    // 8. Area 3 Vignette: Mount Golgotha & Upper Citadel (X ~ 244..278, Y ~ 78..112)
    for (int dy = 0; dy < 22; ++dy) {
        int gw = 34 - dy;
        HLine(mapPix.data(), MW, MH, 244 + dy / 2, 244 + dy / 2 + gw, 94 + dy, Color{ 210, 195, 165, 255 });
    }
    FillCircle(mapPix.data(), MW, MH, 246, 88, 5, Color{ 95, 120, 75, 255 });
    FillCircle(mapPix.data(), MW, MH, 268, 85, 6, Color{ 95, 120, 75, 255 });
    FillCircle(mapPix.data(), MW, MH, 258, 80, 5, Color{ 80, 105, 65, 255 });
    FillRect(mapPix.data(), MW, MH, 253, 90, 14, 16, Color{ 225, 215, 190, 255 });
    OutlineRect(mapPix.data(), MW, MH, 253, 90, 14, 16, inkDark);

    // 9. Classical Antique Compass Rose
    int crX = 286, crY = 145;
    FillCircle(mapPix.data(), MW, MH, crX, crY, 14, parchmentBase);
    OutlineCircle(mapPix.data(), MW, MH, crX, crY, 14, inkDark);
    OutlineCircle(mapPix.data(), MW, MH, crX, crY, 12, inkGold);
    VLine(mapPix.data(), MW, MH, crX, crY - 14, crY + 14, inkDark);
    HLine(mapPix.data(), MW, MH, crX - 14, crX + 14, crY, inkDark);
    Line(mapPix.data(), MW, MH, crX, crY - 12, crX + 4, crY, inkRed);
    Line(mapPix.data(), MW, MH, crX, crY - 12, crX - 4, crY, inkDark);

    s_texMissionMap = CreateTextureFromPixels(MW, MH, mapPix.data());
}

void GraphicsManager::InitSpriteBaseSizes() {
    // Player
    for (int i = SPR_PLAYER_IDLE; i <= SPR_PLAYER_HURT; ++i) {
        s_baseWidth[i] = 28; s_baseHeight[i] = 36;
    }
    s_baseWidth[SPR_PLAYER_ASCEND] = 28; s_baseHeight[SPR_PLAYER_ASCEND] = 36;
    for (int i = SPR_PLAYER_SOMERSAULT_0; i <= SPR_PLAYER_SOMERSAULT_3; ++i) {
        s_baseWidth[i] = 28; s_baseHeight[i] = 28;
    }
    s_baseWidth[SPR_LION_OF_JUDAH] = 48; s_baseHeight[SPR_LION_OF_JUDAH] = 32;

    // Enemies
    for (int i = SPR_GUARD_WALK_0; i <= SPR_GUARD_HURT; ++i) {
        s_baseWidth[i] = 28; s_baseHeight[i] = 36;
    }
    for (int i = SPR_ARCHER_DUCK; i <= SPR_ARCHER_HURT; ++i) {
        s_baseWidth[i] = 28; s_baseHeight[i] = 36;
    }
    for (int i = SPR_HEAVY_WALK_0; i <= SPR_HEAVY_HURT; ++i) {
        s_baseWidth[i] = 32; s_baseHeight[i] = 38;
    }
    for (int i = SPR_BOSS_IDLE; i <= SPR_BOSS_WITHER_1; ++i) {
        s_baseWidth[i] = 36; s_baseHeight[i] = 44;
    }
    for (int i = SPR_SICARII_WALL_CLING; i <= SPR_SICARII_HURT; ++i) {
        s_baseWidth[i] = 28; s_baseHeight[i] = 32;
    }
    s_baseWidth[SPR_SICARII_DAGGER] = 8; s_baseHeight[SPR_SICARII_DAGGER] = 8;
    for (int i = SPR_TEMPLE_BOSS_IDLE; i <= SPR_TEMPLE_BOSS_KNEEL; ++i) {
        s_baseWidth[i] = 40; s_baseHeight[i] = 48;
    }
    for (int i = SPR_TRIBUNAL_IDLE; i <= SPR_TRIBUNAL_KNEEL; ++i) {
        s_baseWidth[i] = 40; s_baseHeight[i] = 48;
    }
    for (int i = SPR_VILLAGER_ELDER_RUN_0; i <= SPR_VILLAGER_SATISFIED; ++i) {
        s_baseWidth[i] = 24; s_baseHeight[i] = 32;
    }
    s_baseWidth[SPR_BREAD_LOAF] = 10; s_baseHeight[SPR_BREAD_LOAF] = 8;
    s_baseWidth[SPR_FISH_PROJECTILE] = 12; s_baseHeight[SPR_FISH_PROJECTILE] = 8;
    s_baseWidth[SPR_JESUS_BASKET_AIM] = 30; s_baseHeight[SPR_JESUS_BASKET_AIM] = 36;
    s_baseWidth[SPR_JESUS_BASKET_THROW] = 30; s_baseHeight[SPR_JESUS_BASKET_THROW] = 36;
    for (int i = SPR_TORMENTOR_WALK_0; i <= SPR_TORMENTOR_HURT; ++i) {
        s_baseWidth[i] = 28; s_baseHeight[i] = 36;
    }
    for (int i = SPR_THORN_SICARII_CROUCH; i <= SPR_THORN_SICARII_HURT; ++i) {
        s_baseWidth[i] = 28; s_baseHeight[i] = 32;
    }
    for (int i = SPR_CORRUPTED_PRIEST_STAND; i <= SPR_CORRUPTED_PRIEST_HURT; ++i) {
        s_baseWidth[i] = 28; s_baseHeight[i] = 36;
    }
    s_baseWidth[SPR_DARK_ORB] = 12; s_baseHeight[SPR_DARK_ORB] = 12;
    s_baseWidth[SPR_DARK_SPIKE] = 16; s_baseHeight[SPR_DARK_SPIKE] = 24;
    for (int i = SPR_CLONE_IDLE; i <= SPR_CLONE_KNEEL; ++i) {
        s_baseWidth[i] = 28; s_baseHeight[i] = 36;
    }
    s_baseWidth[SPR_SILVER_PITCHER] = 16; s_baseHeight[SPR_SILVER_PITCHER] = 16;
    s_baseWidth[SPR_OLIVE_WREATH] = 16; s_baseHeight[SPR_OLIVE_WREATH] = 16;
    s_baseWidth[SPR_SACRED_CHALICE] = 16; s_baseHeight[SPR_SACRED_CHALICE] = 16;
    for (int i = SPR_MONEY_CHANGER_WALK_0; i <= SPR_MONEY_CHANGER_HURT; ++i) {
        s_baseWidth[i] = 28; s_baseHeight[i] = 36;
    }
    for (int i = SPR_LEVITE_ENFORCER_WALK_0; i <= SPR_LEVITE_ENFORCER_HURT; ++i) {
        s_baseWidth[i] = 32; s_baseHeight[i] = 38;
    }
    for (int i = SPR_BARABBAS_IDLE; i <= SPR_BARABBAS_KNEEL; ++i) {
        s_baseWidth[i] = 38; s_baseHeight[i] = 44;
    }
    for (int i = SPR_CAIAPHAS_STAND; i <= SPR_CAIAPHAS_KNEEL; ++i) {
        s_baseWidth[i] = 32; s_baseHeight[i] = 40;
    }
    s_baseWidth[SPR_CAIAPHAS_LIGHTNING] = 16; s_baseHeight[SPR_CAIAPHAS_LIGHTNING] = 96;
    for (int i = SPR_JUDAS_IDLE; i <= SPR_JUDAS_KNEEL; ++i) {
        s_baseWidth[i] = 28; s_baseHeight[i] = 36;
    }
    s_baseWidth[SPR_SILVER_COIN_PROJECTILE] = 8; s_baseHeight[SPR_SILVER_COIN_PROJECTILE] = 8;
    s_baseWidth[SPR_BLOOD_SHOCKWAVE] = 16; s_baseHeight[SPR_BLOOD_SHOCKWAVE] = 16;
    s_baseWidth[SPR_BITTER_HERBS] = 16; s_baseHeight[SPR_BITTER_HERBS] = 16;
    s_baseWidth[SPR_PASSOVER_MATZOT] = 16; s_baseHeight[SPR_PASSOVER_MATZOT] = 16;
    s_baseWidth[SPR_HOLY_GRAIL] = 16; s_baseHeight[SPR_HOLY_GRAIL] = 16;
    for (int i = SPR_SIGNIFER_WALK_0; i <= SPR_SIGNIFER_HURT; ++i) {
        s_baseWidth[i] = 30; s_baseHeight[i] = 38;
    }
    for (int i = SPR_PRAETORIAN_WALK_0; i <= SPR_PRAETORIAN_HURT; ++i) {
        s_baseWidth[i] = 32; s_baseHeight[i] = 40;
    }
    for (int i = SPR_LICTOR_WALK_0; i <= SPR_LICTOR_HURT; ++i) {
        s_baseWidth[i] = 28; s_baseHeight[i] = 36;
    }
    for (int i = SPR_PILATE_IDLE; i <= SPR_PILATE_KNEEL_WASH; ++i) {
        s_baseWidth[i] = 36; s_baseHeight[i] = 44;
    }
    s_baseWidth[SPR_WATER_DROPLET] = 8; s_baseHeight[SPR_WATER_DROPLET] = 8;
    s_baseWidth[SPR_GOLDEN_STYLUS_BLADE] = 14; s_baseHeight[SPR_GOLDEN_STYLUS_BLADE] = 8;
    s_baseWidth[SPR_SHOUT_WAVE] = 16; s_baseHeight[SPR_SHOUT_WAVE] = 16;
    s_baseWidth[SPR_ROMAN_SCROLL] = 16; s_baseHeight[SPR_ROMAN_SCROLL] = 16;
    s_baseWidth[SPR_CAESAR_SEAL] = 16; s_baseHeight[SPR_CAESAR_SEAL] = 16;
    s_baseWidth[SPR_PILATE_EWER] = 16; s_baseHeight[SPR_PILATE_EWER] = 16;
    s_baseWidth[SPR_NAIL] = 8; s_baseHeight[SPR_NAIL] = 8;
    s_baseWidth[SPR_ARROW] = 16; s_baseHeight[SPR_ARROW] = 8;
    s_baseWidth[SPR_WHIP_HEAD] = 14; s_baseHeight[SPR_WHIP_HEAD] = 14;
    s_baseWidth[SPR_BASKET_LOAVES_FISHES] = 22; s_baseHeight[SPR_BASKET_LOAVES_FISHES] = 18;
    s_baseWidth[SPR_ICON_HEART] = 10; s_baseHeight[SPR_ICON_HEART] = 10;
    s_baseWidth[SPR_ICON_WRATH] = 12; s_baseHeight[SPR_ICON_WRATH] = 12;
    s_baseWidth[SPR_ICON_BASKET] = 22; s_baseHeight[SPR_ICON_BASKET] = 18;
    s_baseWidth[SPR_CENSER_GOLD] = 18; s_baseHeight[SPR_CENSER_GOLD] = 18;
}

void GraphicsManager::LoadExternal16BitSprites() {
    struct MapEntry {
        SpriteId id;
        const char* filename;
    };

    static const MapEntry entries[] = {
        { SPR_PLAYER_IDLE, "spr_player_idle.png" },
        { SPR_PLAYER_WALK_0, "spr_player_walk_0.png" },
        { SPR_PLAYER_WALK_1, "spr_player_walk_1.png" },
        { SPR_PLAYER_WALK_2, "spr_player_walk_2.png" },
        { SPR_PLAYER_WALK_3, "spr_player_walk_3.png" },
        { SPR_PLAYER_CROUCH, "spr_player_crouch.png" },
        { SPR_PLAYER_JUMP, "spr_player_jump.png" },
        { SPR_PLAYER_ATTACK_STAND, "spr_player_attack_stand.png" },
        { SPR_PLAYER_ATTACK_CROUCH, "spr_player_attack_crouch.png" },
        { SPR_PLAYER_MELEE_STRIKE, "spr_player_melee_strike.png" },
        { SPR_PLAYER_HURT, "spr_player_hurt.png" },

        { SPR_GUARD_WALK_0, "spr_guard_walk_0.png" },
        { SPR_GUARD_WALK_1, "spr_guard_walk_1.png" },
        { SPR_GUARD_ATTACK, "spr_guard_attack.png" },
        { SPR_GUARD_HURT, "spr_guard_hurt.png" },

        { SPR_ARCHER_DUCK, "spr_archer_duck.png" },
        { SPR_ARCHER_STAND, "spr_archer_stand.png" },
        { SPR_ARCHER_FIRE, "spr_archer_fire.png" },
        { SPR_ARCHER_HURT, "spr_archer_hurt.png" },

        { SPR_HEAVY_WALK_0, "spr_heavy_walk_0.png" },
        { SPR_HEAVY_WALK_1, "spr_heavy_walk_1.png" },
        { SPR_HEAVY_ATTACK, "spr_heavy_attack.png" },
        { SPR_HEAVY_HURT, "spr_heavy_hurt.png" },

        { SPR_BOSS_IDLE, "spr_boss_idle.png" },
        { SPR_BOSS_WALK_0, "spr_boss_idle.png" },
        { SPR_BOSS_WALK_1, "spr_boss_idle.png" },
        { SPR_BOSS_WHIP_PREPARE, "spr_boss_whip_prepare.png" },
        { SPR_BOSS_WHIP_CRACK, "spr_boss_whip_crack.png" },
        { SPR_BOSS_WHIP_SLAM, "spr_boss_whip_slam.png" },
        { SPR_BOSS_HURT, "spr_boss_hurt.png" },

        { SPR_SICARII_WALL_CLING, "spr_sicarii_wall_cling.png" },
        { SPR_SICARII_WALL_CRAWL_0, "spr_sicarii_wall_cling.png" },
        { SPR_SICARII_WALL_CRAWL_1, "spr_sicarii_wall_cling.png" },
        { SPR_SICARII_HOP, "spr_sicarii_wall_cling.png" },
        { SPR_SICARII_STAND, "spr_sicarii_stand.png" },
        { SPR_SICARII_SLASH, "spr_sicarii_slash.png" },
        { SPR_SICARII_THROW, "spr_sicarii_throw.png" },
        { SPR_SICARII_HURT, "spr_sicarii_stand.png" },

        { SPR_JUDAS_IDLE, "spr_judas_idle.png" },
        { SPR_JUDAS_WALK_0, "spr_judas_idle.png" },
        { SPR_JUDAS_WALK_1, "spr_judas_idle.png" },
        { SPR_JUDAS_TOSS_SILVER, "spr_judas_toss_silver.png" },
        { SPR_JUDAS_DASH, "spr_judas_dash.png" },
        { SPR_JUDAS_STAB, "spr_judas_stab.png" },
        { SPR_JUDAS_HURT, "spr_judas_idle.png" },

        { SPR_PILATE_IDLE, "spr_pilate_idle.png" },
        { SPR_PILATE_WALK_0, "spr_pilate_idle.png" },
        { SPR_PILATE_WALK_1, "spr_pilate_idle.png" },
        { SPR_PILATE_BASIN_SPLASH, "spr_pilate_basin.png" },
        { SPR_PILATE_SLASH, "spr_pilate_gavel_strike.png" },

        { SPR_ARROW, "spr_arrow.png" },
        { SPR_BREAD_LOAF, "spr_bread_loaf.png" },
        { SPR_FISH_PROJECTILE, "spr_fish_projectile.png" },
        { SPR_SILVER_COIN_PROJECTILE, "spr_silver_coin_projectile.png" },
        { SPR_ICON_HEART, "spr_icon_heart.png" },
        { SPR_ICON_WRATH, "spr_icon_wrath.png" },
        { SPR_BASKET_LOAVES_FISHES, "spr_basket_loaves_fishes.png" },
    };

    for (const auto& e : entries) {
        char path[128];
        snprintf(path, sizeof(path), "assets/sprites/%s", e.filename);
        if (!FileExists(path)) {
            snprintf(path, sizeof(path), "assets/%s", e.filename);
        }
        if (FileExists(path)) {
            Texture2D tex = LoadTexture(path);
            if (tex.id > 0) {
                if (s_sprites[e.id].id > 0) {
                    UnloadTexture(s_sprites[e.id]);
                }
                SetTextureFilter(tex, TEXTURE_FILTER_BILINEAR);
                s_sprites[e.id] = tex;
                s_hasExternalTextures = true;
            }
        }
    }
}

void GraphicsManager::Init() {
    if (s_initialized) return;

    InitSpriteBaseSizes();
    GenerateAllProceduralSprites();
    LoadExternal16BitSprites();
    GenerateAllProceduralTiles();
    GenerateParallaxBackgrounds();
    GenerateMissionMap();

    Mission2Graphics::GenerateAll(
        s_bgKidronSky,
        s_bgKidronRavine,
        s_bgHedgeSky,
        s_bgHedgeLabyrinth,
        s_bgGethsemaneSky,
        s_bgGethsemaneGrove,
        s_bgBonusSunset,
        s_bgBonusTerraces
    );

    Mission3Graphics::GenerateAll(
        s_bgBazaarSky,
        s_bgBazaarRoofs,
        s_bgCaiaphasSky,
        s_bgCaiaphasPalace,
        s_bgCenacleSky,
        s_bgCenacleChamber
    );

    Mission4Graphics::GenerateAll(
        s_bgDolorosaSky,
        s_bgDolorosaStreet,
        s_bgCourtSky,
        s_bgCourtBasilica,
        s_bgThroneSky,
        s_bgThronePalace
    );

    // Load Title Splash Screen (from file or embedded bytes)
    Image splashImg{};
    if (FileExists("assets/title.jpg")) {
        splashImg = LoadImage("assets/title.jpg");
    } else if (FileExists("title.jpg")) {
        splashImg = LoadImage("title.jpg");
    } else {
        splashImg = LoadImageFromMemory(".jpg", TITLE_IMAGE_DATA, TITLE_IMAGE_SIZE);
    }

    if (splashImg.data != nullptr) {
        s_texTitleSplash = LoadTextureFromImage(splashImg);
        SetTextureFilter(s_texTitleSplash, TEXTURE_FILTER_BILINEAR);
        UnloadImage(splashImg);
    }

    s_initialized = true;
}

void GraphicsManager::Close() {
    if (!s_initialized) return;

    for (int i = 0; i < SPRITE_TOTAL_COUNT; ++i) {
        if (s_sprites[i].id > 0) UnloadTexture(s_sprites[i]);
    }
    for (int i = 0; i < TILE_TOTAL_COUNT; ++i) {
        if (s_tiles[i].id > 0) UnloadTexture(s_tiles[i]);
    }

    if (s_bgSkyDistant.id > 0) UnloadTexture(s_bgSkyDistant);
    if (s_bgTempleOfDavid.id > 0) UnloadTexture(s_bgTempleOfDavid);
    if (s_bgMidJerusalemVillas.id > 0) UnloadTexture(s_bgMidJerusalemVillas);
    if (s_bgNearStreetscape.id > 0) UnloadTexture(s_bgNearStreetscape);
    if (s_bgTempleSanctuary.id > 0) UnloadTexture(s_bgTempleSanctuary);
    if (s_bgTempleColonnade.id > 0) UnloadTexture(s_bgTempleColonnade);
    if (s_bgGolgothaSky.id > 0) UnloadTexture(s_bgGolgothaSky);
    if (s_bgGolgothaCrags.id > 0) UnloadTexture(s_bgGolgothaCrags);
    if (s_bgKidronSky.id > 0) UnloadTexture(s_bgKidronSky);
    if (s_bgKidronRavine.id > 0) UnloadTexture(s_bgKidronRavine);
    if (s_bgHedgeSky.id > 0) UnloadTexture(s_bgHedgeSky);
    if (s_bgHedgeLabyrinth.id > 0) UnloadTexture(s_bgHedgeLabyrinth);
    if (s_bgGethsemaneSky.id > 0) UnloadTexture(s_bgGethsemaneSky);
    if (s_bgGethsemaneGrove.id > 0) UnloadTexture(s_bgGethsemaneGrove);
    if (s_bgBonusSunset.id > 0) UnloadTexture(s_bgBonusSunset);
    if (s_bgBonusTerraces.id > 0) UnloadTexture(s_bgBonusTerraces);
    if (s_bgBazaarSky.id > 0) UnloadTexture(s_bgBazaarSky);
    if (s_bgBazaarRoofs.id > 0) UnloadTexture(s_bgBazaarRoofs);
    if (s_bgCaiaphasSky.id > 0) UnloadTexture(s_bgCaiaphasSky);
    if (s_bgCaiaphasPalace.id > 0) UnloadTexture(s_bgCaiaphasPalace);
    if (s_bgCenacleSky.id > 0) UnloadTexture(s_bgCenacleSky);
    if (s_bgCenacleChamber.id > 0) UnloadTexture(s_bgCenacleChamber);
    if (s_bgDolorosaSky.id > 0) UnloadTexture(s_bgDolorosaSky);
    if (s_bgDolorosaStreet.id > 0) UnloadTexture(s_bgDolorosaStreet);
    if (s_bgCourtSky.id > 0) UnloadTexture(s_bgCourtSky);
    if (s_bgCourtBasilica.id > 0) UnloadTexture(s_bgCourtBasilica);
    if (s_bgThroneSky.id > 0) UnloadTexture(s_bgThroneSky);
    if (s_bgThronePalace.id > 0) UnloadTexture(s_bgThronePalace);
    if (s_texMissionMap.id > 0) UnloadTexture(s_texMissionMap);
    if (s_texTitleSplash.id > 0) UnloadTexture(s_texTitleSplash);

    s_initialized = false;
}

void GraphicsManager::DrawTitleSplash(int destWidth, int destHeight) {
    if (s_texTitleSplash.id == 0) return;
    Rectangle src = { 0, 0, (float)s_texTitleSplash.width, (float)s_texTitleSplash.height };
    Rectangle dst = { 0, 0, (float)destWidth, (float)destHeight };
    DrawTexturePro(s_texTitleSplash, src, dst, { 0, 0 }, 0.0f, WHITE);
}

void GraphicsManager::DrawMissionMap(int missionNumber, int currentArea, bool a1Cleared, bool a2Cleared, bool a3Cleared, float time) {
    if (s_texMissionMap.id == 0) return;

    DrawTexture(s_texMissionMap, 0, 0, WHITE);

    struct AreaNode {
        int x, y, w, h;
        const char* name;
        const char* sub;
        const char* boss;
        bool cleared;
    };

    AreaNode areas[3];
    if (missionNumber == 4) {
        areas[0] = {  42,  64, 46, 44, "AREA 1: VIA DOLOROSA", "THE STREETS OF JUDGMENT", "PREFECT QUINTUS", a1Cleared };
        areas[1] = { 138,  48, 52, 48, "AREA 2: THE GABBATHA", "LITHOSTROTOS TRIBUNAL", "ADVOCATE TERTULLUS", a2Cleared };
        areas[2] = { 236,  72, 48, 44, "AREA 3: THE PRAETORIUM", "THE THRONE ROOM & BEMA", "PONTIUS PILATE", a3Cleared };
    } else if (missionNumber == 3) {
        areas[0] = {  42,  64, 46, 44, "AREA 1: SILOAM BAZAAR", "THE OLD CITY & AQUEDUCTS", "BARABBAS THE ZEALOT", a1Cleared };
        areas[1] = { 138,  48, 52, 48, "AREA 2: PALACE OF CAIAPHAS", "MOUNT ZION AUDIENCE HALL", "HIGH PRIEST CAIAPHAS", a2Cleared };
        areas[2] = { 236,  72, 48, 44, "AREA 3: THE UPPER ROOM", "THE CENACLE & LAST SUPPER", "JUDAS ISCARIOT", a3Cleared };
    } else if (missionNumber == 2) {
        areas[0] = {  42,  64, 46, 44, "AREA 1: VALLEY OF KIDRON", "THE CHASM & MOONLIT BRIDGES", "TORMENTOR DECURION", a1Cleared };
        areas[1] = { 138,  48, 52, 48, "AREA 2: CYPRESS HEDGE MAZE", "ROYAL PALACE LABYRINTH", "SHADOW INQUISITOR", a2Cleared };
        areas[2] = { 236,  72, 48, 44, "AREA 3: GETHSEMANE GROVE", "THE ANCIENT OLIVE TREE", "CHRISTUS TENEBRIS", a3Cleared };
    } else {
        areas[0] = {  42,  64, 46, 44, "AREA 1: VIA DOLOROSA", "ROMAN GARRISON & AQUEDUCT", "CENTURION BRUTUS", a1Cleared };
        areas[1] = { 138,  48, 52, 48, "AREA 2: THE INNER TEMPLE", "SOLOMON'S PORCH & SANCTUARY", "SICARII & MALCHUS", a2Cleared };
        areas[2] = { 236,  72, 48, 44, "AREA 3: GOLGOTHA CITADEL", "THE CRAGS & UPPER CITADEL", "TRIBUNAL PRAETOR", a3Cleared };
    }

    for (int i = 0; i < 3; ++i) {
        const auto& a = areas[i];
        bool isCurrent = (currentArea == (i + 1));

        if (a.cleared) {
            DrawRectangle(a.x - 2, a.y - 2, a.w + 4, a.h + 4, Color{ 40, 35, 45, 130 });
            DrawRectangleLines(a.x - 2, a.y - 2, a.w + 4, a.h + 4, Color{ 90, 75, 70, 200 });

            const char* stamp = "DEFEATED";
            int sw = MeasureText(stamp, 10);
            int sx = a.x + (a.w - sw) / 2;
            int sy = a.y + (a.h - 12) / 2;

            DrawRectangle(sx - 4, sy - 2, sw + 8, 14, Color{ 140, 15, 20, 210 });
            DrawRectangleLines(sx - 5, sy - 3, sw + 10, 16, Color{ 230, 45, 55, 255 });
            DrawRectangleLines(sx - 3, sy - 1, sw + 6, 12, Color{ 255, 120, 120, 220 });
            DrawText(stamp, sx, sy, 10, Color{ 255, 240, 240, 255 });
        }
        else if (isCurrent) {
            bool blink = (static_cast<int>(time * 6.0f) % 2 == 0);
            Color frameCol = blink ? COLOR_GOLD_BRIGHT : Color{ 255, 255, 255, 240 };

            DrawRectangleLines(a.x - 3, a.y - 3, a.w + 6, a.h + 6, frameCol);
            DrawRectangleLines(a.x - 4, a.y - 4, a.w + 8, a.h + 8, COLOR_ROMAN_CRIMSON);

            int len = 7;
            DrawLine(a.x - 6, a.y - 6, a.x - 6 + len, a.y - 6, frameCol);
            DrawLine(a.x - 6, a.y - 6, a.x - 6, a.y - 6 + len, frameCol);
            DrawLine(a.x + a.w + 5, a.y - 6, a.x + a.w + 5 - len, a.y - 6, frameCol);
            DrawLine(a.x + a.w + 5, a.y - 6, a.x + a.w + 5, a.y - 6 + len, frameCol);
            DrawLine(a.x - 6, a.y + a.h + 5, a.x - 6 + len, a.y + a.h + 5, frameCol);
            DrawLine(a.x - 6, a.y + a.h + 5, a.x - 6, a.y + a.h + 5 - len, frameCol);
            DrawLine(a.x + a.w + 5, a.y + a.h + 5, a.x + a.w + 5 - len, a.y + a.h + 5, frameCol);
            DrawLine(a.x + a.w + 5, a.y + a.h + 5, a.x + a.w + 5, a.y + a.h + 5 - len, frameCol);

            if (blink) {
                const char* targetLbl = ">> TARGET <<";
                int tlw = MeasureText(targetLbl, 10);
                DrawRectangle(a.x + (a.w - tlw) / 2 - 2, a.y - 15, tlw + 4, 11, Color{ 15, 10, 18, 220 });
                DrawText(targetLbl, a.x + (a.w - tlw) / 2, a.y - 14, 10, COLOR_GOLD_BRIGHT);
            }
        }
        else {
            DrawRectangle(a.x - 2, a.y - 2, a.w + 4, a.h + 4, Color{ 20, 15, 18, 90 });
            DrawRectangleLines(a.x - 2, a.y - 2, a.w + 4, a.h + 4, Color{ 140, 120, 95, 160 });
        }
    }

    const char* topTitle = (missionNumber == 4) ? "MISSION 4: THE TRIAL BEFORE PILATE" :
                          ((missionNumber == 3) ? "MISSION 3: THE PASSION & CENACLE" :
                          ((missionNumber == 2) ? "MISSION 2: GETHSEMANE OUTSKIRTS" : "MISSION 1: JERUSALEM CAMPAIGN"));
    int ttw = MeasureText(topTitle, 10);
    DrawRectangle((VIRTUAL_WIDTH - ttw) / 2 - 10, 12, ttw + 20, 15, Color{ 24, 18, 22, 230 });
    DrawRectangleLines((VIRTUAL_WIDTH - ttw) / 2 - 10, 12, ttw + 20, 15, COLOR_GOLD_BRIGHT);
    DrawText(topTitle, (VIRTUAL_WIDTH - ttw) / 2, 15, 10, COLOR_GOLD_BRIGHT);

    int bx = 16, by = 162, bw = 288, bh = 52;
    DrawRectangle(bx, by, bw, bh, Color{ 18, 14, 22, 240 });
    DrawRectangleLines(bx - 1, by - 1, bw + 2, bh + 2, COLOR_ROMAN_CRIMSON);
    DrawRectangleLines(bx - 2, by - 2, bw + 4, bh + 4, COLOR_GOLD_BRIGHT);

    int curIdx = std::clamp(currentArea - 1, 0, 2);
    const auto& curArea = areas[curIdx];

    DrawText(curArea.name, bx + 10, by + 6, 10, COLOR_GOLD_BRIGHT);
    DrawText(curArea.sub, bx + 10, by + 18, 10, Color{ 210, 215, 225, 255 });
    char targetBuf[64];
    snprintf(targetBuf, sizeof(targetBuf), "TARGET: %s", curArea.boss);
    DrawText(targetBuf, bx + 10, by + 29, 10, Color{ 255, 95, 95, 255 });

    bool promptBlink = (static_cast<int>(time * 4.0f) % 2 == 0);
    if (promptBlink) {
        const char* prompt = "[PRESS START OR ATTACK TO EMBARK]";
        int pw = MeasureText(prompt, 10);
        DrawText(prompt, bx + bw - pw - 10, by + 37, 10, COLOR_GOLD_BRIGHT);
    }
}

void GraphicsManager::DrawSecretSelectScreen(int step, int missionIndex, int stageIndex, int credits, float time) {
    // 1. Screen Backdrop & Roman Ornate Border
    DrawRectangle(0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT, Color{ 12, 10, 18, 255 });
    DrawRectangleGradientV(0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT, Color{ 22, 15, 30, 255 }, Color{ 8, 6, 12, 255 });

    DrawRectangleLines(3, 3, VIRTUAL_WIDTH - 6, VIRTUAL_HEIGHT - 6, COLOR_ROMAN_GOLD);
    DrawRectangleLines(5, 5, VIRTUAL_WIDTH - 10, VIRTUAL_HEIGHT - 10, Color{ 60, 45, 20, 255 });

    if (step == 0) {
        // =========================================================================
        // STEP 1: MISSION SELECT (SHOWCASING ONLY THE 4 MISSIONS WITH ARTWORK)
        // =========================================================================
        DrawRectangle(7, 6, VIRTUAL_WIDTH - 14, 21, Color{ 28, 18, 38, 240 });
        DrawLine(7, 27, VIRTUAL_WIDTH - 7, 27, COLOR_ROMAN_GOLD);

        const char* secTitle = "SECRET MISSION ARCHIVE";
        int tw = MeasureText(secTitle, 10);
        DrawText(secTitle, (VIRTUAL_WIDTH - tw) / 2, 8, 10, COLOR_GOLD_BRIGHT);

        const char* secSub = "- SELECT MISSION (I - IV) -";
        int sw = MeasureText(secSub, 10);
        DrawText(secSub, (VIRTUAL_WIDTH - sw) / 2, 17, 10, COLOR_ROMAN_STONE);

        struct MissionCardData {
            const char* badge;
            Color badgeCol;
            const char* title;
            const char* sub;
            Texture2D bgTex;
            SpriteId bossSpr;
            int x, y;
        };

        MissionCardData mCards[4] = {
            { "MISSION I",   COLOR_ROMAN_CRIMSON,     "JERUSALEM",    "Temple & Citadel",     s_bgTempleOfDavid,   SPR_BOSS_IDLE,       12,  31 },
            { "MISSION II",  Color{ 35, 125, 50, 255 }, "GETHSEMANE",   "The Olive Garden",     s_bgGethsemaneGrove, SPR_CLONE_IDLE,     164,  31 },
            { "MISSION III", Color{ 120, 40, 150, 255 }, "LAST SUPPER", "The Upper Room",      s_bgCenacleChamber,  SPR_JUDAS_IDLE,      12, 107 },
            { "MISSION IV",  COLOR_PRAETORIUM_PURPLE, "PILATE TRIAL", "The Praetorium",       s_bgThronePalace,    SPR_PILATE_IDLE,    164, 107 }
        };

        int cardW = 144;
        int cardH = 72;

        for (int m = 0; m < 4; ++m) {
            const auto& mc = mCards[m];
            bool isSel = (m == missionIndex);

            // Card background artwork from parallax texture
            if (mc.bgTex.id > 0) {
                Rectangle src = { 0, 0, (float)mc.bgTex.width, (float)mc.bgTex.height };
                Rectangle dst = { (float)mc.x, (float)mc.y, (float)cardW, (float)cardH };
                Color tint = isSel ? WHITE : Color{ 125, 125, 140, 220 };
                DrawTexturePro(mc.bgTex, src, dst, { 0, 0 }, 0.0f, tint);
            } else {
                DrawRectangle(mc.x, mc.y, cardW, cardH, Color{ 30, 24, 38, 255 });
            }

            // Dark gradient overlay on bottom half so text is crisp
            DrawRectangleGradientV(mc.x, mc.y + 24, cardW, cardH - 24, Color{ 0, 0, 0, 25 }, Color{ 10, 8, 14, 235 });

            // Character Sprites in Card: Hero Jesus on left, Boss on right
            DrawDropShadow(mc.x + 18.0f, mc.y + 54.0f, 18.0f, 5.0f);
            DrawSprite(SPR_PLAYER_IDLE, mc.x + 6, mc.y + 24, false, isSel ? WHITE : Color{ 180, 180, 190, 220 });

            DrawDropShadow(mc.x + cardW - 20.0f, mc.y + 54.0f, 22.0f, 5.0f);
            DrawSprite(mc.bossSpr, mc.x + cardW - 36, mc.y + 16, true, isSel ? WHITE : Color{ 180, 180, 190, 220 });

            // Roman badge at top-left
            DrawRectangle(mc.x + 4, mc.y + 4, 62, 11, mc.badgeCol);
            DrawRectangleLines(mc.x + 4, mc.y + 4, 62, 11, COLOR_ROMAN_GOLD);
            DrawText(mc.badge, mc.x + 7, mc.y + 5, 10, COLOR_GOLD_BRIGHT);

            // Title & Subtitle
            Color titleCol = isSel ? COLOR_GOLD_BRIGHT : Color{ 220, 215, 230, 255 };
            DrawText(mc.title, mc.x + 8, mc.y + 43, 10, titleCol);
            DrawText(mc.sub, mc.x + 8, mc.y + 55, 10, isSel ? COLOR_ROBE_WHITE : Color{ 160, 155, 170, 255 });

            // Selection Highlight
            if (isSel) {
                float pulse = 0.5f + 0.5f * sinf(time * 8.0f);
                Color borderCol = ColorAlpha(COLOR_GOLD_BRIGHT, 0.75f + 0.25f * pulse);
                DrawRectangleLines(mc.x - 2, mc.y - 2, cardW + 4, cardH + 4, borderCol);
                DrawRectangleLines(mc.x - 1, mc.y - 1, cardW + 2, cardH + 2, COLOR_ROMAN_CRIMSON);

                // Corner tabs
                int tab = 5;
                DrawLine(mc.x - 4, mc.y - 4, mc.x - 4 + tab, mc.y - 4, COLOR_GOLD_BRIGHT);
                DrawLine(mc.x - 4, mc.y - 4, mc.x - 4, mc.y - 4 + tab, COLOR_GOLD_BRIGHT);
                DrawLine(mc.x + cardW + 3, mc.y - 4, mc.x + cardW + 3 - tab, mc.y - 4, COLOR_GOLD_BRIGHT);
                DrawLine(mc.x + cardW + 3, mc.y - 4, mc.x + cardW + 3, mc.y - 4 + tab, COLOR_GOLD_BRIGHT);
                DrawLine(mc.x - 4, mc.y + cardH + 3, mc.x - 4 + tab, mc.y + cardH + 3, COLOR_GOLD_BRIGHT);
                DrawLine(mc.x - 4, mc.y + cardH + 3, mc.x - 4, mc.y + cardH + 3 - tab, COLOR_GOLD_BRIGHT);
                DrawLine(mc.x + cardW + 3, mc.y + cardH + 3, mc.x + cardW + 3 - tab, mc.y + cardH + 3, COLOR_GOLD_BRIGHT);
                DrawLine(mc.x + cardW + 3, mc.y + cardH + 3, mc.x + cardW + 3, mc.y + cardH + 3 - tab, COLOR_GOLD_BRIGHT);

                // Prompt tag
                DrawRectangle(mc.x + 28, mc.y + cardH - 12, cardW - 56, 11, Color{ 15, 10, 20, 235 });
                DrawRectangleLines(mc.x + 28, mc.y + cardH - 12, cardW - 56, 11, COLOR_GOLD_BRIGHT);
                DrawText(">> SELECT <<", mc.x + 36, mc.y + cardH - 11, 10, COLOR_GOLD_BRIGHT);
            } else {
                DrawRectangleLines(mc.x, mc.y, cardW, cardH, Color{ 60, 45, 25, 180 });
            }
        }

        // Bottom Navigation Footer
        DrawLine(7, 182, VIRTUAL_WIDTH - 7, 182, COLOR_ROMAN_GOLD);
        DrawRectangle(7, 183, VIRTUAL_WIDTH - 14, 36, Color{ 14, 10, 20, 240 });

        const char* hintNav = "[ARROWS / D-PAD] SELECT MISSION    [START / JUMP] CHOOSE STAGES";
        int nhw = MeasureText(hintNav, 10);
        DrawText(hintNav, (VIRTUAL_WIDTH - nhw) / 2, 187, 10, COLOR_ROMAN_GOLD);

        const char* hintBack = "[B / ESC] RETURN TO TITLE";
        int bhw = MeasureText(hintBack, 10);
        DrawText(hintBack, (VIRTUAL_WIDTH - bhw) / 2, 200, 10, COLOR_ROMAN_STONE);

        char credStr[32];
        snprintf(credStr, sizeof(credStr), "CREDIT %02d", credits);
        DrawText(credStr, VIRTUAL_WIDTH - MeasureText(credStr, 10) - 14, 204, 10, COLOR_ROMAN_GOLD);
    } else {
        // =========================================================================
        // STEP 2: STAGE SELECT (CHOOSE SUB-BOARD 1, 2, OR 3 FOR CHOSEN MISSION)
        // =========================================================================
        DrawRectangle(7, 6, VIRTUAL_WIDTH - 14, 23, Color{ 28, 18, 38, 240 });
        DrawLine(7, 29, VIRTUAL_WIDTH - 7, 29, COLOR_ROMAN_GOLD);

        const char* mNames[4] = {
            "MISSION I: JERUSALEM OUTSKIRTS & CITADEL",
            "MISSION II: THE ROAD TO GETHSEMANE",
            "MISSION III: THE CENACLE & LAST SUPPER",
            "MISSION IV: THE TRIAL OF PONTIUS PILATE"
        };
        const char* chosenMName = (missionIndex >= 0 && missionIndex < 4) ? mNames[missionIndex] : mNames[0];
        int mtw = MeasureText(chosenMName, 10);
        DrawText(chosenMName, (VIRTUAL_WIDTH - mtw) / 2, 8, 10, COLOR_GOLD_BRIGHT);

        const char* subPrompt = (missionIndex == 0) ? "- CHOOSE SUB-BOARD (1, 2, 3, OR BONUS) -" : "- CHOOSE SUB-BOARD (1, 2, OR 3) -";
        int stw = MeasureText(subPrompt, 10);
        DrawText(subPrompt, (VIRTUAL_WIDTH - stw) / 2, 18, 10, COLOR_ROMAN_STONE);

        struct StageCardData {
            const char* numBadge;
            const char* title;
            const char* sub;
            const char* boss;
            Texture2D bgTex;
            SpriteId bossSpr;
            SpriteId pickupSpr;
            const char* pickupText;
        };

        std::vector<StageCardData> stages;
        if (missionIndex == 3) {
            // Mission 4: The Trial of Pilate
            stages = {
                { "1", "VIA DOLOROSA",   "Streets of Judgment",  "PREFECT QUINTUS",    s_bgDolorosaStreet,  SPR_PREFECT_IDLE,  SPR_ROMAN_SCROLL, "4 Scrolls" },
                { "2", "THE GABBATHA",   "Lithostrotos Court",   "ADVOCATE TERTULLUS", s_bgCourtBasilica,   SPR_ADVOCATE_IDLE, SPR_CAESAR_SEAL,  "4 Seals" },
                { "3", "THE PRAETORIUM", "Throne Room & Bema",   "PONTIUS PILATE",     s_bgThronePalace,    SPR_PILATE_IDLE,   SPR_PILATE_EWER,  "4 Ewers" }
            };
        } else if (missionIndex == 2) {
            // Mission 3: The Last Supper
            stages = {
                { "1", "SILOAM BAZAAR",  "Old City Aqueduct",    "BARABBAS THE ZEALOT", s_bgBazaarRoofs,     SPR_BARABBAS_IDLE, SPR_BITTER_HERBS, "4 Herbs" },
                { "2", "CAIAPHAS",       "Mount Zion Audience",  "HIGH PRIEST CAIAPHAS",s_bgCaiaphasPalace,  SPR_CAIAPHAS_STAND,SPR_PASSOVER_MATZOT,"4 Matzot" },
                { "3", "THE CENACLE",    "The Last Supper",      "JUDAS ISCARIOT",      s_bgCenacleChamber,  SPR_JUDAS_IDLE,    SPR_HOLY_GRAIL,   "4 Grails" }
            };
        } else if (missionIndex == 1) {
            // Mission 2: Road to Gethsemane
            stages = {
                { "1", "KIDRON VALLEY",  "Moonlit Chasm",        "TORMENTOR DECURION",  s_bgKidronRavine,    SPR_TORMENTOR_WALK_0, SPR_SILVER_PITCHER, "4 Pitchers" },
                { "2", "CYPRESS MAZE",   "Royal Labyrinth",      "SHADOW INQUISITOR",   s_bgHedgeLabyrinth,  SPR_THORN_SICARII_SLASH, SPR_OLIVE_WREATH, "4 Wreaths" },
                { "3", "GETHSEMANE",     "The Olive Tree",       "CHRISTUS TENEBRIS",   s_bgGethsemaneGrove, SPR_CLONE_IDLE,   SPR_SACRED_CHALICE, "4 Chalices" }
            };
        } else {
            // Mission 1: Jerusalem Campaign + Bonus
            stages = {
                { "1", "OUTSKIRTS",      "Roman Garrison",       "CENTURION BRUTUS",    s_bgNearStreetscape, SPR_BOSS_IDLE,        SPR_ICON_BASKET, "4 Baskets" },
                { "2", "INNER TEMPLE",   "Solomon's Porch",      "COMMANDER MALCHUS",   s_bgTempleSanctuary, SPR_TEMPLE_BOSS_IDLE, SPR_CENSER_GOLD, "4 Censers" },
                { "3", "GOLGOTHA",       "Upper Citadel",        "PRAETOR LONGINUS",    s_bgGolgothaCrags,   SPR_TRIBUNAL_IDLE,    SPR_CENSER_GOLD, "4 Relics" },
                { "★", "BEATITUDES",     "Mount of Beatitudes",  "FEEDING THE 5,000",   s_bgBonusSunset,     SPR_PLAYER_IDLE,      SPR_BREAD_LOAF,  "Bread & Fish" }
            };
        }

        int count = static_cast<int>(stages.size());
        int cardW = (count == 4) ? 68 : 92;
        int cardH = 138;
        int spacing = (count == 4) ? 6 : 8;
        int totalW = count * cardW + (count - 1) * spacing;
        int startX = (VIRTUAL_WIDTH - totalW) / 2;
        int cardY = 34;

        for (int s = 0; s < count; ++s) {
            const auto& sc = stages[s];
            int cx = startX + s * (cardW + spacing);
            bool isSel = (s == stageIndex);

            // Background preview
            if (sc.bgTex.id > 0) {
                Rectangle src = { 0, 0, (float)sc.bgTex.width, (float)sc.bgTex.height };
                Rectangle dst = { (float)cx, (float)cardY, (float)cardW, (float)cardH };
                Color tint = isSel ? WHITE : Color{ 120, 120, 130, 220 };
                DrawTexturePro(sc.bgTex, src, dst, { 0, 0 }, 0.0f, tint);
            } else {
                DrawRectangle(cx, cardY, cardW, cardH, Color{ 25, 20, 32, 255 });
            }

            // Dark gradient overlay
            DrawRectangleGradientV(cx, cardY, cardW, cardH, Color{ 10, 8, 14, 120 }, Color{ 10, 8, 14, 245 });

            // Top Stage Number Badge
            int medalCenterX = cx + cardW / 2;
            int medalCenterY = cardY + 14;
            DrawCircle(medalCenterX, medalCenterY, 10.0f, isSel ? COLOR_ROMAN_CRIMSON : Color{ 24, 18, 30, 255 });
            DrawCircleLines(medalCenterX, medalCenterY, 10.0f, isSel ? COLOR_GOLD_BRIGHT : COLOR_ROMAN_GOLD);
            int nw = MeasureText(sc.numBadge, 10);
            DrawText(sc.numBadge, medalCenterX - nw / 2, medalCenterY - 4, 10, COLOR_GOLD_BRIGHT);

            // Sprite centered in card
            DrawDropShadow(cx + cardW / 2.0f, cardY + 62.0f, 24.0f, 6.0f);
            DrawSprite(sc.bossSpr, cx + (cardW - 32) / 2, cardY + 32, true, isSel ? WHITE : Color{ 190, 190, 200, 220 });

            // Sub-board Title & Sub
            Color titleCol = isSel ? COLOR_GOLD_BRIGHT : Color{ 220, 215, 230, 255 };
            int tw = MeasureText(sc.title, 10);
            if (tw > cardW - 4) {
                DrawText(sc.title, cx + 2, cardY + 68, 10, titleCol);
            } else {
                DrawText(sc.title, cx + (cardW - tw) / 2, cardY + 68, 10, titleCol);
            }

            // Boss / Objective target
            char bossBuf[48];
            snprintf(bossBuf, sizeof(bossBuf), "%s", sc.boss);
            int bw = MeasureText(bossBuf, 10);
            Color bossCol = isSel ? Color{ 255, 120, 120, 255 } : Color{ 190, 110, 110, 255 };
            if (bw > cardW - 4) {
                DrawText(bossBuf, cx + 2, cardY + 82, 10, bossCol);
            } else {
                DrawText(bossBuf, cx + (cardW - bw) / 2, cardY + 82, 10, bossCol);
            }

            // Relic pickup icon and text
            DrawSprite(sc.pickupSpr, cx + 6, cardY + 98);
            DrawText(sc.pickupText, cx + 22, cardY + 102, 10, isSel ? COLOR_ROBE_WHITE : Color{ 170, 165, 180, 255 });

            // Selection frame
            if (isSel) {
                float pulse = 0.5f + 0.5f * sinf(time * 8.0f);
                Color borderCol = ColorAlpha(COLOR_GOLD_BRIGHT, 0.75f + 0.25f * pulse);
                DrawRectangleLines(cx - 2, cardY - 2, cardW + 4, cardH + 4, borderCol);
                DrawRectangleLines(cx - 1, cardY - 1, cardW + 2, cardH + 2, COLOR_ROMAN_CRIMSON);

                // Prompt banner
                DrawRectangle(cx + 4, cardY + cardH - 16, cardW - 8, 13, Color{ 140, 15, 20, 230 });
                DrawRectangleLines(cx + 4, cardY + cardH - 16, cardW - 8, 13, COLOR_GOLD_BRIGHT);
                const char* emb = ">> EMBARK <<";
                int ew = MeasureText(emb, 10);
                DrawText(emb, cx + (cardW - ew) / 2, cardY + cardH - 14, 10, COLOR_GOLD_BRIGHT);
            } else {
                DrawRectangleLines(cx, cardY, cardW, cardH, Color{ 60, 45, 25, 180 });
            }
        }

        // Bottom Navigation Footer
        DrawLine(7, 182, VIRTUAL_WIDTH - 7, 182, COLOR_ROMAN_GOLD);
        DrawRectangle(7, 183, VIRTUAL_WIDTH - 14, 36, Color{ 14, 10, 20, 240 });

        const char* hintNav = "[LEFT / RIGHT] CHOOSE BOARD    [START / JUMP] EMBARK";
        int nhw = MeasureText(hintNav, 10);
        DrawText(hintNav, (VIRTUAL_WIDTH - nhw) / 2, 187, 10, COLOR_ROMAN_GOLD);

        const char* hintBack = "[B / ESC] BACK TO MISSIONS";
        int bhw = MeasureText(hintBack, 10);
        DrawText(hintBack, (VIRTUAL_WIDTH - bhw) / 2, 200, 10, COLOR_ROMAN_STONE);

        char credStr[32];
        snprintf(credStr, sizeof(credStr), "CREDIT %02d", credits);
        DrawText(credStr, VIRTUAL_WIDTH - MeasureText(credStr, 10) - 14, 204, 10, COLOR_ROMAN_GOLD);
    }
}

void GraphicsManager::DrawArcadeCabinetBezel(int screenW, int screenH, int offsetX, int offsetY, int scaledW, int scaledH, int crtMode, int credits, float time) {
    if (offsetX <= 0 && screenW <= scaledW) return;

    int rightX = offsetX + scaledW;
    int rightW = screenW - rightX;

    // 1. Dark Textured Cabinet Base
    Color cabinetBase = Color{ 14, 12, 18, 255 };
    Color bezelGold   = Color{ 220, 180, 55, 255 };
    Color bezelGlow   = Color{ 255, 215, 80, 255 };

    DrawRectangle(0, 0, offsetX, screenH, cabinetBase);
    DrawRectangle(rightX, 0, rightW, screenH, cabinetBase);

    // Vertical Cabinet T-Molding extrusions along outer monitor seams
    DrawRectangle(offsetX - 6, 0, 6, screenH, Color{ 36, 28, 44, 255 });
    DrawLine(offsetX - 6, 0, offsetX - 6, screenH, Color{ 70, 58, 85, 255 });
    DrawLine(offsetX - 1, 0, offsetX - 1, screenH, Color{ 10, 8, 14, 255 });

    DrawRectangle(rightX, 0, 6, screenH, Color{ 36, 28, 44, 255 });
    DrawLine(rightX, 0, rightX, screenH, Color{ 70, 58, 85, 255 });
    DrawLine(rightX + 5, 0, rightX + 5, screenH, Color{ 10, 8, 14, 255 });

    // =========================================================================
    // LEFT BEZEL: THE NAZARENE ILLUSTRATED ARCADE MARQUEE & CHARACTER ART
    // =========================================================================
    if (offsetX >= 60) {
        int lw = offsetX - 12;
        int lx = 6;
        int cardY = 16;
        int cardH = screenH - 32;

        // Golden Outer Marquee Frame with Corner Rivets
        DrawRectangle(lx, cardY, lw, cardH, Color{ 20, 15, 28, 245 });
        DrawRectangleLines(lx, cardY, lw, cardH, bezelGold);
        DrawRectangleLines(lx + 2, cardY + 2, lw - 4, cardH - 4, Color{ 90, 65, 25, 255 });

        // Corner Gold Studs
        DrawRectangle(lx + 3, cardY + 3, 4, 4, bezelGlow);
        DrawRectangle(lx + lw - 7, cardY + 3, 4, 4, bezelGlow);
        DrawRectangle(lx + 3, cardY + cardH - 7, 4, 4, bezelGlow);
        DrawRectangle(lx + lw - 7, cardY + cardH - 7, 4, 4, bezelGlow);

        // Header: Arcade Title Banner
        DrawRectangle(lx + 4, cardY + 6, lw - 8, 44, Color{ 140, 18, 28, 255 });
        DrawRectangleLines(lx + 4, cardY + 6, lw - 8, 44, bezelGold);
        DrawLine(lx + 4, cardY + 50, lx + lw - 4, cardY + 50, bezelGlow);

        const char* title1 = "THE NAZARENE";
        const char* title2 = "WAY OF THE CROSS";
        int t1w = MeasureText(title1, 10);
        int t2w = MeasureText(title2, 10);
        if (lw > t1w + 10) {
            DrawText(title1, lx + (lw - t1w) / 2 + 1, cardY + 12, 10, Color{ 40, 5, 10, 255 });
            DrawText(title1, lx + (lw - t1w) / 2, cardY + 11, 10, COLOR_GOLD_BRIGHT);
            DrawText(title2, lx + (lw - t2w) / 2, cardY + 26, 10, Color{ 255, 245, 220, 255 });
        } else {
            DrawText("NAZARENE", lx + 6, cardY + 14, 10, COLOR_GOLD_BRIGHT);
        }

        // Illustrated Character Artwork Panel (Christ with Golden Radiant Halo)
        int artY = cardY + 56;
        int artH = std::min(cardH - 130, 160);
        if (artH > 60) {
            DrawRectangle(lx + 6, artY, lw - 12, artH, Color{ 12, 10, 20, 255 });
            DrawRectangleLines(lx + 6, artY, lw - 12, artH, Color{ 60, 48, 75, 255 });

            // Radiant Holy Sunburst Halo behind Christ
            int haloCx = lx + lw / 2;
            int haloCy = artY + 36;
            DrawCircleGradient(haloCx, haloCy, 32.0f, Color{ 255, 235, 120, 180 }, Color{ 255, 180, 40, 0 });
            for (int r = 0; r < 8; ++r) {
                float ang = r * (3.14159f / 4.0f) + time * 0.4f;
                DrawLine(haloCx, haloCy, static_cast<int>(haloCx + cosf(ang) * 34.0f), static_cast<int>(haloCy + sinf(ang) * 34.0f), Color{ 255, 245, 160, 120 });
            }

            // Hero Jesus Character Sprite scaled & centered
            int sprScale = (lw >= 120) ? 2 : 1;
            Texture2D jesusTex = s_sprites[SPR_PLAYER_IDLE];
            if (jesusTex.id > 0) {
                Rectangle src = { 0, 0, (float)jesusTex.width, (float)jesusTex.height };
                Rectangle dst = { (float)(haloCx - (jesusTex.width * sprScale) / 2), (float)(haloCy - 12), (float)(jesusTex.width * sprScale), (float)(jesusTex.height * sprScale) };
                DrawTexturePro(jesusTex, src, dst, { 0, 0 }, 0.0f, WHITE);
            }

            // Divine Motto Scroll
            DrawRectangle(lx + 8, artY + artH - 22, lw - 16, 16, Color{ 24, 18, 32, 230 });
            DrawRectangleLines(lx + 8, artY + artH - 22, lw - 16, 16, bezelGold);
            const char* motto = "DIVINE JUSTICE";
            int mw = MeasureText(motto, 10);
            if (lw > mw + 16) {
                DrawText(motto, lx + (lw - mw) / 2, artY + artH - 18, 10, COLOR_GOLD_BRIGHT);
            }
        }

        // Hardware Specification Plate
        int specY = cardY + cardH - 64;
        if (specY > artY + artH) {
            DrawRectangle(lx + 6, specY, lw - 12, 26, Color{ 16, 14, 22, 255 });
            DrawRectangleLines(lx + 6, specY, lw - 12, 26, Color{ 70, 60, 80, 255 });
            DrawText("SEGA SYSTEM 16", lx + 10, specY + 4, 10, bezelGold);
            DrawText("68000 CPU @ 10MHz", lx + 10, specY + 15, 10, Color{ 160, 165, 185, 255 });
        }

        // Diagonal Yellow/Black Warning Hazard Stripes along Bottom (Iconic Arcade Bezel Detail!)
        int hazY = cardY + cardH - 24;
        DrawRectangle(lx + 4, hazY, lw - 8, 18, Color{ 15, 12, 10, 255 });
        for (int hx = lx + 4; hx < lx + lw - 8; hx += 12) {
            DrawTriangle(
                Vector2{ (float)hx, (float)(hazY + 18) },
                Vector2{ (float)(hx + 8), (float)hazY },
                Vector2{ (float)(hx + 14), (float)hazY },
                Color{ 240, 195, 30, 230 }
            );
            DrawTriangle(
                Vector2{ (float)hx, (float)(hazY + 18) },
                Vector2{ (float)(hx + 14), (float)hazY },
                Vector2{ (float)(hx + 6), (float)(hazY + 18) },
                Color{ 240, 195, 30, 230 }
            );
        }
        DrawRectangleLines(lx + 4, hazY, lw - 8, 18, bezelGold);
    }

    // =========================================================================
    // RIGHT BEZEL: AUTHENTIC ARCADE INSTRUCTIONS, CONTROLS & COIN DOOR
    // =========================================================================
    if (rightW >= 60) {
        int rw = rightW - 12;
        int rx = rightX + 6;
        int cardY = 16;
        int cardH = screenH - 32;

        // Golden Outer Marquee Frame with Corner Rivets
        DrawRectangle(rx, cardY, rw, cardH, Color{ 20, 15, 28, 245 });
        DrawRectangleLines(rx, cardY, rw, cardH, bezelGold);
        DrawRectangleLines(rx + 2, cardY + 2, rw - 4, cardH - 4, Color{ 90, 65, 25, 255 });

        // Corner Gold Studs
        DrawRectangle(rx + 3, cardY + 3, 4, 4, bezelGlow);
        DrawRectangle(rx + rw - 7, cardY + 3, 4, 4, bezelGlow);
        DrawRectangle(rx + 3, cardY + cardH - 7, 4, 4, bezelGlow);
        DrawRectangle(rx + rw - 7, cardY + cardH - 7, 4, 4, bezelGlow);

        // Header: "HOW TO PLAY"
        DrawRectangle(rx + 4, cardY + 6, rw - 8, 22, Color{ 140, 18, 28, 255 });
        DrawRectangleLines(rx + 4, cardY + 6, rw - 8, 22, bezelGold);
        const char* htp = "HOW TO PLAY";
        int htpW = MeasureText(htp, 10);
        DrawText(htp, rx + (rw - htpW) / 2, cardY + 12, 10, COLOR_GOLD_BRIGHT);

        // Control Panel Instructions
        int instY = cardY + 34;
        DrawRectangle(rx + 6, instY, rw - 12, 110, Color{ 14, 12, 20, 255 });
        DrawRectangleLines(rx + 6, instY, rw - 12, 110, Color{ 60, 50, 75, 255 });

        // 8-Way Joystick
        DrawCircle(rx + 20, instY + 20, 7.0f, Color{ 230, 35, 45, 255 });
        DrawCircleLines(rx + 20, instY + 20, 7.0f, WHITE);
        DrawRectangle(rx + 18, instY + 25, 4, 9, Color{ 180, 185, 195, 255 });
        DrawText("8-WAY STICK", rx + 34, instY + 12, 10, COLOR_GOLD_BRIGHT);
        DrawText("UP: HIGH JUMP", rx + 34, instY + 23, 10, Color{ 180, 180, 195, 255 });

        // Push Buttons A, B, C
        // Button A (Red)
        DrawCircle(rx + 20, instY + 48, 6.0f, Color{ 220, 30, 40, 255 });
        DrawCircleLines(rx + 20, instY + 48, 6.0f, WHITE);
        DrawText("A", rx + 18, instY + 43, 10, WHITE);
        DrawText("WHIP / MELEE", rx + 34, instY + 44, 10, Color{ 255, 220, 140, 255 });

        // Button B (Blue)
        DrawCircle(rx + 20, instY + 70, 6.0f, Color{ 35, 115, 235, 255 });
        DrawCircleLines(rx + 20, instY + 70, 6.0f, WHITE);
        DrawText("B", rx + 18, instY + 65, 10, WHITE);
        DrawText("JUMP / SOMERSAULT", rx + 34, instY + 66, 10, Color{ 160, 210, 255, 255 });

        // Button C (Gold)
        DrawCircle(rx + 20, instY + 92, 6.0f, Color{ 245, 180, 30, 255 });
        DrawCircleLines(rx + 20, instY + 92, 6.0f, WHITE);
        DrawText("C", rx + 18, instY + 87, 10, WHITE);
        DrawText("DIVINE WRATH", rx + 34, instY + 88, 10, COLOR_GOLD_BRIGHT);

        // Mission Rule Briefing
        int ruleY = instY + 116;
        int ruleH = std::min(cardH - 240, 60);
        if (ruleH > 30) {
            DrawRectangle(rx + 6, ruleY, rw - 12, ruleH, Color{ 16, 14, 22, 255 });
            DrawRectangleLines(rx + 6, ruleY, rw - 12, ruleH, Color{ 60, 50, 70, 255 });
            DrawText("COLLECT 4 RELICS", rx + 10, ruleY + 6, 10, Color{ 80, 255, 120, 255 });
            DrawText("TO BREACH GATES", rx + 10, ruleY + 18, 10, Color{ 210, 210, 225, 255 });
        }

        // Realistic Illuminated Arcade Coin Door
        int coinY = cardY + cardH - 85;
        if (coinY > ruleY + ruleH) {
            DrawRectangle(rx + 6, coinY, rw - 12, 75, Color{ 10, 8, 14, 255 });
            DrawRectangleLines(rx + 6, coinY, rw - 12, 75, Color{ 75, 60, 45, 255 });
            DrawRectangleLines(rx + 8, coinY + 2, rw - 16, 71, Color{ 35, 28, 40, 255 });

            // Glowing Orange 25¢ Insert Button with Soft Ambient Glow
            int btnX = rx + rw / 2 - 18;
            int btnY = coinY + 12;
            DrawRectangleGradientV(btnX, btnY, 36, 26, Color{ 255, 150, 40, 255 }, Color{ 200, 65, 10, 255 });
            DrawRectangleLines(btnX, btnY, 36, 26, Color{ 255, 225, 130, 255 });
            DrawText("25c", btnX + 9, btnY + 7, 10, WHITE);

            // Pulsing "INSERT COIN"
            bool coinBlink = (static_cast<int>(time * 3.0f) % 2 == 0);
            Color coinPromptCol = coinBlink ? COLOR_GOLD_BRIGHT : Color{ 180, 140, 40, 255 };
            const char* ins = "INSERT COIN";
            DrawText(ins, rx + (rw - MeasureText(ins, 10)) / 2, coinY + 44, 10, coinPromptCol);

            // Credit Counter
            char cbuf[24];
            snprintf(cbuf, sizeof(cbuf), "CREDIT %02d", credits);
            DrawText(cbuf, rx + (rw - MeasureText(cbuf, 10)) / 2, coinY + 58, 10, COLOR_ROMAN_GOLD);
        }
    }

    // =========================================================================
    // CURVED CRT BEZEL MASK & SCREEN GLASS REFLECTIONS
    // =========================================================================
    // 3D Chamfered CRT Frame
    DrawRectangleLinesEx({ (float)offsetX - 4, (float)offsetY - 4, (float)scaledW + 8, (float)scaledH + 8 }, 4.0f, Color{ 28, 22, 36, 255 });
    DrawRectangleLinesEx({ (float)offsetX - 2, (float)offsetY - 2, (float)scaledW + 4, (float)scaledH + 4 }, 2.0f, Color{ 68, 55, 85, 255 });
    DrawRectangleLinesEx({ (float)offsetX - 1, (float)offsetY - 1, (float)scaledW + 2, (float)scaledH + 2 }, 1.0f, Color{ 10, 8, 14, 255 });

    // CRT Curved Corner Shadows (simulating authentic curved cathode tube glass)
    int crtRadius = 14;
    Color cornerShadow = Color{ 8, 6, 12, 230 };
    DrawTriangle(Vector2{ (float)offsetX, (float)offsetY }, Vector2{ (float)(offsetX + crtRadius), (float)offsetY }, Vector2{ (float)offsetX, (float)(offsetY + crtRadius) }, cornerShadow);
    DrawTriangle(Vector2{ (float)(offsetX + scaledW), (float)offsetY }, Vector2{ (float)(offsetX + scaledW - crtRadius), (float)offsetY }, Vector2{ (float)(offsetX + scaledW), (float)(offsetY + crtRadius) }, cornerShadow);
    DrawTriangle(Vector2{ (float)offsetX, (float)(offsetY + scaledH) }, Vector2{ (float)(offsetX + crtRadius), (float)(offsetY + scaledH) }, Vector2{ (float)offsetX, (float)(offsetY + scaledH - crtRadius) }, cornerShadow);
    DrawTriangle(Vector2{ (float)(offsetX + scaledW), (float)(offsetY + scaledH) }, Vector2{ (float)(offsetX + scaledW - crtRadius), (float)(offsetY + scaledH) }, Vector2{ (float)(offsetX + scaledW), (float)(offsetY + scaledH - crtRadius) }, cornerShadow);

    // Subtle Glass Glare Sheen across top-left corner
    DrawLineEx(Vector2{ (float)(offsetX + 8), (float)(offsetY + 4) }, Vector2{ (float)(offsetX + scaledW * 0.35f), (float)(offsetY + 4) }, 2.0f, Color{ 255, 255, 255, 35 });
    DrawLineEx(Vector2{ (float)(offsetX + 4), (float)(offsetY + 8) }, Vector2{ (float)(offsetX + 4), (float)(offsetY + scaledH * 0.35f) }, 2.0f, Color{ 255, 255, 255, 28 });
}

void GraphicsManager::DrawArcadeHud(int score, int lives, int hp, int maxHp, int shatteredHearts, bool wrathReady, int collectedBaskets, int totalBaskets, int stageId, float stageTimeRemaining, float time) {
    // 1. TOP-LEFT: 1P & SCORE (Floating with Arcade Drop Shadow)
    DrawText("1P", 9, 5, 10, Color{ 0, 0, 0, 220 });
    DrawText("1P", 8, 4, 10, WHITE);

    char scoreBuf[16];
    snprintf(scoreBuf, sizeof(scoreBuf), "%06d", score);
    DrawText(scoreBuf, 27, 5, 10, Color{ 0, 0, 0, 220 });
    DrawText(scoreBuf, 26, 4, 10, COLOR_GOLD_BRIGHT);

    // Remaining Lives Icons directly beneath 1P / score (Exact Shinobi style!)
    for (int l = 0; l < std::min(lives, 5); ++l) {
        int lx = 26 + l * 8;
        int ly = 15;
        // Little golden cross / head icon
        DrawRectangle(lx + 2, ly, 2, 6, Color{ 0, 0, 0, 200 });
        DrawRectangle(lx, ly + 2, 6, 2, Color{ 0, 0, 0, 200 });
        DrawRectangle(lx + 2, ly, 2, 6, COLOR_GOLD_BRIGHT);
        DrawRectangle(lx, ly + 2, 6, 2, COLOR_GOLD_BRIGHT);
    }
    if (lives > 5) {
        char lb[8];
        snprintf(lb, sizeof(lb), "+%d", lives - 5);
        DrawText(lb, 68, 14, 10, COLOR_GOLD_BRIGHT);
    }

    // 2. ROMAN JEWELED VITALITY METER (Floating beside Score)
    int meterX = 86;
    int meterY = 4;
    DrawText("LIFE", meterX + 1, meterY + 1, 10, Color{ 0, 0, 0, 200 });
    DrawText("LIFE", meterX, meterY, 10, Color{ 220, 215, 230, 255 });

    int barX = meterX + 26;
    DrawRectangle(barX - 2, meterY - 1, 36, 12, Color{ 14, 10, 18, 200 });
    DrawRectangleLines(barX - 2, meterY - 1, 36, 12, COLOR_ROMAN_GOLD);

    bool lowHpFlash = (hp == 1 && (static_cast<int>(time * 8.0f) % 2 == 0));
    for (int i = 0; i < PLAYER_MAX_HP; ++i) {
        int px = barX + i * 11;
        int py = meterY + 1;
        if (i < hp) {
            Color gemCol = lowHpFlash ? WHITE : COLOR_CRIMSON_BRIGHT;
            DrawRectangle(px, py, 9, 8, gemCol);
            DrawRectangle(px, py, 9, 2, Color{ 255, 220, 220, 255 }); // Facet gleam
            DrawRectangleLines(px, py, 9, 8, Color{ 90, 10, 18, 255 });
        } else if (i < (PLAYER_MAX_HP - shatteredHearts)) {
            DrawRectangle(px, py, 9, 8, Color{ 35, 28, 40, 255 });
            DrawRectangleLines(px, py, 9, 8, Color{ 75, 65, 80, 255 });
        } else {
            // Shattered socket
            DrawRectangle(px, py, 9, 8, Color{ 25, 8, 12, 255 });
            DrawLine(px, py, px + 8, py + 7, Color{ 160, 20, 30, 255 });
            DrawLine(px + 8, py, px, py + 7, Color{ 160, 20, 30, 255 });
        }
    }

    // 3. TOP-RIGHT: HI SCORE (Slanted Arcade Crimson Typography)
    const char* hiLabel = "HI";
    const char* hiScore = "350140";
    DrawText(hiLabel, 240, 5, 10, Color{ 0, 0, 0, 220 });
    DrawText(hiLabel, 239, 4, 10, COLOR_CRIMSON_BRIGHT);

    DrawText(hiScore, 260, 5, 10, Color{ 0, 0, 0, 220 });
    DrawText(hiScore, 259, 4, 10, Color{ 255, 245, 180, 255 });

    // 4. BOTTOM-RIGHT: SHINOBI-STYLE MAGIC MEDALLION + BIG YELLOW ARCADE TIMER
    int botY = VIRTUAL_HEIGHT - 22;

    // Divine Wrath Super Medallion
    int medX = VIRTUAL_WIDTH - 68;
    DrawRectangle(medX - 1, botY - 1, 16, 16, Color{ 12, 10, 18, 220 });
    DrawRectangleLines(medX - 1, botY - 1, 16, 16, wrathReady ? COLOR_GOLD_BRIGHT : Color{ 70, 60, 80, 255 });

    if (wrathReady) {
        float pulse = 0.5f + 0.5f * sinf(time * 8.0f);
        DrawRectangle(medX, botY, 14, 14, Color{ 20, 50, 120, 255 });
        // Blazing Lion / Flame Super Icon
        DrawCircle(medX + 7, botY + 7, 5.0f, ColorAlpha(COLOR_GOLD_BRIGHT, 0.7f + 0.3f * pulse));
        DrawCircle(medX + 7, botY + 7, 2.5f, WHITE);
        // Spinning holy spark
        float spAng = time * 4.0f;
        DrawCircle(static_cast<int>(medX + 7 + cosf(spAng) * 5.0f), static_cast<int>(botY + 7 + sinf(spAng) * 5.0f), 1.5f, WHITE);
    } else {
        DrawRectangle(medX, botY, 14, 14, Color{ 24, 20, 30, 200 });
        DrawRectangleLines(medX + 3, botY + 3, 8, 8, Color{ 55, 48, 65, 255 });
    }

    // Big Arcade Timer (Minutes:Seconds in iconic large yellow font with black outline!)
    int tSeconds = static_cast<int>(std::max(0.0f, stageTimeRemaining));
    int tMin = tSeconds / 60;
    int tSec = tSeconds % 60;
    char timerBuf[16];
    snprintf(timerBuf, sizeof(timerBuf), "%d:%02d", tMin, tSec);

    Color timerColor = (tSeconds <= 30 && (static_cast<int>(time * 4.0f) % 2 == 0)) ? Color{ 255, 60, 60, 255 } : Color{ 255, 235, 40, 255 };
    int timerX = VIRTUAL_WIDTH - 48;
    int timerY = botY - 1;

    // Drop Shadow & 1px dark perimeter
    DrawText(timerBuf, timerX + 1, timerY + 1, 14, Color{ 0, 0, 0, 240 });
    DrawText(timerBuf, timerX - 1, timerY, 14, Color{ 0, 0, 0, 240 });
    DrawText(timerBuf, timerX, timerY + 1, 14, Color{ 0, 0, 0, 240 });
    DrawText(timerBuf, timerX, timerY, 14, timerColor);

    // 5. BOTTOM-LEFT: FLOATING RELIC REPAIR PROGRESS
    SpriteId relicSpr = SPR_ICON_BASKET;
    if (stageId == 2 || stageId == 3) relicSpr = SPR_CENSER_GOLD;
    else if (stageId == 4) relicSpr = SPR_SILVER_PITCHER;
    else if (stageId == 5) relicSpr = SPR_OLIVE_WREATH;
    else if (stageId == 6) relicSpr = SPR_SACRED_CHALICE;
    else if (stageId == 7) relicSpr = SPR_BITTER_HERBS;
    else if (stageId == 8) relicSpr = SPR_PASSOVER_MATZOT;
    else if (stageId == 9) relicSpr = SPR_HOLY_GRAIL;
    else if (stageId == 10) relicSpr = SPR_ROMAN_SCROLL;
    else if (stageId == 11) relicSpr = SPR_CAESAR_SEAL;
    else if (stageId == 12) relicSpr = SPR_PILATE_EWER;

    DrawDropShadow(14.0f, VIRTUAL_HEIGHT - 6.0f, 16.0f, 4.0f);
    DrawSprite(relicSpr, 6.0f, VIRTUAL_HEIGHT - 20.0f);

    char relBuf[16];
    snprintf(relBuf, sizeof(relBuf), "%d/%d", collectedBaskets, totalBaskets);
    Color relCol = (collectedBaskets >= totalBaskets) ? Color{ 80, 255, 130, 255 } : COLOR_ROMAN_GOLD;
    DrawText(relBuf, 26, VIRTUAL_HEIGHT - 17, 10, Color{ 0, 0, 0, 220 });
    DrawText(relBuf, 25, VIRTUAL_HEIGHT - 18, 10, relCol);
}

void GraphicsManager::DrawSprite(SpriteId id, float x, float y, bool flipH, Color tint) {
    if (id < 0 || id >= SPRITE_TOTAL_COUNT) return;
    Texture2D tex = s_sprites[id];
    if (tex.id == 0) return;

    float baseW = (s_baseWidth[id] > 0) ? (float)s_baseWidth[id] : (float)tex.width;
    float baseH = (s_baseHeight[id] > 0) ? (float)s_baseHeight[id] : (float)tex.height;

    Rectangle src = { 0.0f, 0.0f, (float)tex.width * (flipH ? -1.0f : 1.0f), (float)tex.height };
    Rectangle dst = { x, y, baseW, baseH };
    DrawTexturePro(tex, src, dst, { 0.0f, 0.0f }, 0.0f, tint);
}

void GraphicsManager::DrawTile(TileId id, float x, float y, Color tint) {
    if (id <= TILE_AIR || id >= TILE_TOTAL_COUNT) return;
    Texture2D tex = s_tiles[id];
    if (tex.id == 0) return;

    DrawTexture(tex, static_cast<int>(x), static_cast<int>(y), tint);
}

void GraphicsManager::DrawBonusStageBackground(float time) {
    if (s_bgBonusSunset.id > 0) {
        DrawTexture(s_bgBonusSunset, 0, 0, WHITE);
    }
    if (s_bgBonusTerraces.id > 0) {
        DrawTexture(s_bgBonusTerraces, 0, 0, WHITE);
    }
}

void GraphicsManager::DrawParallaxBackground(float cameraX, float time, int stageId) {
    if (stageId == 2) {
        // --- STAGE 2: INSIDE THE TEMPLE OF JERUSALEM ---
        float sX = -fmodf(cameraX * 0.15f, (float)s_bgTempleSanctuary.width);
        DrawTexture(s_bgTempleSanctuary, static_cast<int>(sX), 0, WHITE);
        DrawTexture(s_bgTempleSanctuary, static_cast<int>(sX + s_bgTempleSanctuary.width), 0, WHITE);
        if (sX > 0) DrawTexture(s_bgTempleSanctuary, static_cast<int>(sX - s_bgTempleSanctuary.width), 0, WHITE);

        float cX = -fmodf(cameraX * 0.50f, (float)s_bgTempleColonnade.width);
        DrawTexture(s_bgTempleColonnade, static_cast<int>(cX), 10, WHITE);
        DrawTexture(s_bgTempleColonnade, static_cast<int>(cX + s_bgTempleColonnade.width), 10, WHITE);
        if (cX > 0) DrawTexture(s_bgTempleColonnade, static_cast<int>(cX - s_bgTempleColonnade.width), 10, WHITE);
        return;
    } else if (stageId == 3) {
        // --- STAGE 3: GOLGOTHA CITADEL & CRAGS ---
        float gSkyX = -fmodf(cameraX * 0.10f, (float)s_bgGolgothaSky.width);
        DrawTexture(s_bgGolgothaSky, static_cast<int>(gSkyX), 0, WHITE);
        DrawTexture(s_bgGolgothaSky, static_cast<int>(gSkyX + s_bgGolgothaSky.width), 0, WHITE);
        if (gSkyX > 0) DrawTexture(s_bgGolgothaSky, static_cast<int>(gSkyX - s_bgGolgothaSky.width), 0, WHITE);

        float gCragX = -fmodf(cameraX * 0.45f, (float)s_bgGolgothaCrags.width);
        DrawTexture(s_bgGolgothaCrags, static_cast<int>(gCragX), 14, WHITE);
        DrawTexture(s_bgGolgothaCrags, static_cast<int>(gCragX + s_bgGolgothaCrags.width), 14, WHITE);
        if (gCragX > 0) DrawTexture(s_bgGolgothaCrags, static_cast<int>(gCragX - s_bgGolgothaCrags.width), 14, WHITE);
        return;
    } else if (stageId == 4) {
        // --- STAGE 4: VALLEY OF THE KIDRON & RAVINE ---
        float kSkyX = -fmodf(cameraX * 0.08f, (float)s_bgKidronSky.width);
        DrawTexture(s_bgKidronSky, static_cast<int>(kSkyX), 0, WHITE);
        DrawTexture(s_bgKidronSky, static_cast<int>(kSkyX + s_bgKidronSky.width), 0, WHITE);
        if (kSkyX > 0) DrawTexture(s_bgKidronSky, static_cast<int>(kSkyX - s_bgKidronSky.width), 0, WHITE);

        float kRavineX = -fmodf(cameraX * 0.45f, (float)s_bgKidronRavine.width);
        DrawTexture(s_bgKidronRavine, static_cast<int>(kRavineX), 12, WHITE);
        DrawTexture(s_bgKidronRavine, static_cast<int>(kRavineX + s_bgKidronRavine.width), 12, WHITE);
        if (kRavineX > 0) DrawTexture(s_bgKidronRavine, static_cast<int>(kRavineX - s_bgKidronRavine.width), 12, WHITE);
        return;
    } else if (stageId == 5) {
        // --- STAGE 5: THE ROYAL CYPRESS HEDGE MAZE ---
        float hSkyX = -fmodf(cameraX * 0.10f, (float)s_bgHedgeSky.width);
        DrawTexture(s_bgHedgeSky, static_cast<int>(hSkyX), 0, WHITE);
        DrawTexture(s_bgHedgeSky, static_cast<int>(hSkyX + s_bgHedgeSky.width), 0, WHITE);
        if (hSkyX > 0) DrawTexture(s_bgHedgeSky, static_cast<int>(hSkyX - s_bgHedgeSky.width), 0, WHITE);

        float hLabX = -fmodf(cameraX * 0.40f, (float)s_bgHedgeLabyrinth.width);
        DrawTexture(s_bgHedgeLabyrinth, static_cast<int>(hLabX), 10, WHITE);
        DrawTexture(s_bgHedgeLabyrinth, static_cast<int>(hLabX + s_bgHedgeLabyrinth.width), 10, WHITE);
        if (hLabX > 0) DrawTexture(s_bgHedgeLabyrinth, static_cast<int>(hLabX - s_bgHedgeLabyrinth.width), 10, WHITE);
        return;
    } else if (stageId == 6) {
        // --- STAGE 6: THE GARDEN OF GETHSEMANE ---
        float gSkyX = -fmodf(cameraX * 0.08f, (float)s_bgGethsemaneSky.width);
        DrawTexture(s_bgGethsemaneSky, static_cast<int>(gSkyX), 0, WHITE);
        DrawTexture(s_bgGethsemaneSky, static_cast<int>(gSkyX + s_bgGethsemaneSky.width), 0, WHITE);
        if (gSkyX > 0) DrawTexture(s_bgGethsemaneSky, static_cast<int>(gSkyX - s_bgGethsemaneSky.width), 0, WHITE);

        float gGroveX = -fmodf(cameraX * 0.42f, (float)s_bgGethsemaneGrove.width);
        DrawTexture(s_bgGethsemaneGrove, static_cast<int>(gGroveX), 8, WHITE);
        DrawTexture(s_bgGethsemaneGrove, static_cast<int>(gGroveX + s_bgGethsemaneGrove.width), 8, WHITE);
        if (gGroveX > 0) DrawTexture(s_bgGethsemaneGrove, static_cast<int>(gGroveX - s_bgGethsemaneGrove.width), 8, WHITE);
        return;
    } else if (stageId == 7) {
        // --- STAGE 7: SILOAM BAZAAR & AQUEDUCTS ---
        float bzSkyX = -fmodf(cameraX * 0.08f, (float)s_bgBazaarSky.width);
        DrawTexture(s_bgBazaarSky, static_cast<int>(bzSkyX), 0, WHITE);
        DrawTexture(s_bgBazaarSky, static_cast<int>(bzSkyX + s_bgBazaarSky.width), 0, WHITE);
        if (bzSkyX > 0) DrawTexture(s_bgBazaarSky, static_cast<int>(bzSkyX - s_bgBazaarSky.width), 0, WHITE);

        float bzRoofsX = -fmodf(cameraX * 0.45f, (float)s_bgBazaarRoofs.width);
        DrawTexture(s_bgBazaarRoofs, static_cast<int>(bzRoofsX), 12, WHITE);
        DrawTexture(s_bgBazaarRoofs, static_cast<int>(bzRoofsX + s_bgBazaarRoofs.width), 12, WHITE);
        if (bzRoofsX > 0) DrawTexture(s_bgBazaarRoofs, static_cast<int>(bzRoofsX - s_bgBazaarRoofs.width), 12, WHITE);
        return;
    } else if (stageId == 8) {
        // --- STAGE 8: MOUNT ZION & PALACE OF CAIAPHAS ---
        float caSkyX = -fmodf(cameraX * 0.08f, (float)s_bgCaiaphasSky.width);
        DrawTexture(s_bgCaiaphasSky, static_cast<int>(caSkyX), 0, WHITE);
        DrawTexture(s_bgCaiaphasSky, static_cast<int>(caSkyX + s_bgCaiaphasSky.width), 0, WHITE);
        if (caSkyX > 0) DrawTexture(s_bgCaiaphasSky, static_cast<int>(caSkyX - s_bgCaiaphasSky.width), 0, WHITE);

        float caPalaceX = -fmodf(cameraX * 0.42f, (float)s_bgCaiaphasPalace.width);
        DrawTexture(s_bgCaiaphasPalace, static_cast<int>(caPalaceX), 10, WHITE);
        DrawTexture(s_bgCaiaphasPalace, static_cast<int>(caPalaceX + s_bgCaiaphasPalace.width), 10, WHITE);
        if (caPalaceX > 0) DrawTexture(s_bgCaiaphasPalace, static_cast<int>(caPalaceX - s_bgCaiaphasPalace.width), 10, WHITE);
        return;
    } else if (stageId == 9) {
        // --- STAGE 9: THE CENACLE & UPPER ROOM (THE LAST SUPPER) ---
        float ceSkyX = -fmodf(cameraX * 0.05f, (float)s_bgCenacleSky.width);
        DrawTexture(s_bgCenacleSky, static_cast<int>(ceSkyX), 0, WHITE);
        DrawTexture(s_bgCenacleSky, static_cast<int>(ceSkyX + s_bgCenacleSky.width), 0, WHITE);
        if (ceSkyX > 0) DrawTexture(s_bgCenacleSky, static_cast<int>(ceSkyX - s_bgCenacleSky.width), 0, WHITE);

        float ceChambX = -fmodf(cameraX * 0.38f, (float)s_bgCenacleChamber.width);
        DrawTexture(s_bgCenacleChamber, static_cast<int>(ceChambX), 8, WHITE);
        DrawTexture(s_bgCenacleChamber, static_cast<int>(ceChambX + s_bgCenacleChamber.width), 8, WHITE);
        if (ceChambX > 0) DrawTexture(s_bgCenacleChamber, static_cast<int>(ceChambX - s_bgCenacleChamber.width), 8, WHITE);
        return;
    } else if (stageId == 10) {
        // --- STAGE 10: VIA DOLOROSA & STREETS OF JUDGMENT ---
        float doSkyX = -fmodf(cameraX * 0.08f, (float)s_bgDolorosaSky.width);
        DrawTexture(s_bgDolorosaSky, static_cast<int>(doSkyX), 0, WHITE);
        DrawTexture(s_bgDolorosaSky, static_cast<int>(doSkyX + s_bgDolorosaSky.width), 0, WHITE);
        if (doSkyX > 0) DrawTexture(s_bgDolorosaSky, static_cast<int>(doSkyX - s_bgDolorosaSky.width), 0, WHITE);

        float doStX = -fmodf(cameraX * 0.45f, (float)s_bgDolorosaStreet.width);
        DrawTexture(s_bgDolorosaStreet, static_cast<int>(doStX), 12, WHITE);
        DrawTexture(s_bgDolorosaStreet, static_cast<int>(doStX + s_bgDolorosaStreet.width), 12, WHITE);
        if (doStX > 0) DrawTexture(s_bgDolorosaStreet, static_cast<int>(doStX - s_bgDolorosaStreet.width), 12, WHITE);
        return;
    } else if (stageId == 11) {
        // --- STAGE 11: THE GABBATHA COURT & LITHOSTROTOS TRIBUNAL ---
        float coSkyX = -fmodf(cameraX * 0.06f, (float)s_bgCourtSky.width);
        DrawTexture(s_bgCourtSky, static_cast<int>(coSkyX), 0, WHITE);
        DrawTexture(s_bgCourtSky, static_cast<int>(coSkyX + s_bgCourtSky.width), 0, WHITE);
        if (coSkyX > 0) DrawTexture(s_bgCourtSky, static_cast<int>(coSkyX - s_bgCourtSky.width), 0, WHITE);

        float coBasX = -fmodf(cameraX * 0.40f, (float)s_bgCourtBasilica.width);
        DrawTexture(s_bgCourtBasilica, static_cast<int>(coBasX), 10, WHITE);
        DrawTexture(s_bgCourtBasilica, static_cast<int>(coBasX + s_bgCourtBasilica.width), 10, WHITE);
        if (coBasX > 0) DrawTexture(s_bgCourtBasilica, static_cast<int>(coBasX - s_bgCourtBasilica.width), 10, WHITE);
        return;
    } else if (stageId == 12) {
        // --- STAGE 12: THE PRAETORIUM THRONE ROOM & BEMA OF PILATE ---
        float thSkyX = -fmodf(cameraX * 0.04f, (float)s_bgThroneSky.width);
        DrawTexture(s_bgThroneSky, static_cast<int>(thSkyX), 0, WHITE);
        DrawTexture(s_bgThroneSky, static_cast<int>(thSkyX + s_bgThroneSky.width), 0, WHITE);
        if (thSkyX > 0) DrawTexture(s_bgThroneSky, static_cast<int>(thSkyX - s_bgThroneSky.width), 0, WHITE);

        float thPalX = -fmodf(cameraX * 0.35f, (float)s_bgThronePalace.width);
        DrawTexture(s_bgThronePalace, static_cast<int>(thPalX), 8, WHITE);
        DrawTexture(s_bgThronePalace, static_cast<int>(thPalX + s_bgThronePalace.width), 8, WHITE);
        if (thPalX > 0) DrawTexture(s_bgThronePalace, static_cast<int>(thPalX - s_bgThronePalace.width), 8, WHITE);
        return;
    }

    // --- STAGE 1: STREETS OF JERUSALEM & TEMPLE OF DAVID ---
    // 1. Layer 1: Far Sky & Sunset (0.05x parallax)
    float skyX = -fmodf(cameraX * 0.05f, (float)s_bgSkyDistant.width);
    DrawTexture(s_bgSkyDistant, static_cast<int>(skyX), 0, WHITE);
    DrawTexture(s_bgSkyDistant, static_cast<int>(skyX + s_bgSkyDistant.width), 0, WHITE);
    if (skyX > 0) DrawTexture(s_bgSkyDistant, static_cast<int>(skyX - s_bgSkyDistant.width), 0, WHITE);

    // 2. Layer 2: The Temple & Citadel of David Panorama (0.20x parallax, 960px wide)
    float davidX = -fmodf(cameraX * 0.20f, (float)s_bgTempleOfDavid.width);
    DrawTexture(s_bgTempleOfDavid, static_cast<int>(davidX), 8, WHITE);
    DrawTexture(s_bgTempleOfDavid, static_cast<int>(davidX + s_bgTempleOfDavid.width), 8, WHITE);
    if (davidX > 0) DrawTexture(s_bgTempleOfDavid, static_cast<int>(davidX - s_bgTempleOfDavid.width), 8, WHITE);

    // 3. Layer 3: Midground Ancient Jerusalem Villas & Aqueducts (0.45x parallax, 640px wide)
    float midX = -fmodf(cameraX * 0.45f, (float)s_bgMidJerusalemVillas.width);
    DrawTexture(s_bgMidJerusalemVillas, static_cast<int>(midX), 18, WHITE);
    DrawTexture(s_bgMidJerusalemVillas, static_cast<int>(midX + s_bgMidJerusalemVillas.width), 18, WHITE);
    if (midX > 0) DrawTexture(s_bgMidJerusalemVillas, static_cast<int>(midX - s_bgMidJerusalemVillas.width), 18, WHITE);

    // 4. Layer 4: Near Streetscape Columns & Braziers (0.70x parallax, 480px wide)
    float nearX = -fmodf(cameraX * 0.70f, (float)s_bgNearStreetscape.width);
    DrawTexture(s_bgNearStreetscape, static_cast<int>(nearX), 24, WHITE);
    DrawTexture(s_bgNearStreetscape, static_cast<int>(nearX + s_bgNearStreetscape.width), 24, WHITE);
    if (nearX > 0) DrawTexture(s_bgNearStreetscape, static_cast<int>(nearX - s_bgNearStreetscape.width), 24, WHITE);
}

void GraphicsManager::DrawDropShadow(float x, float y, float width, float height) {
    DrawEllipse(static_cast<int>(x), static_cast<int>(y), width * 0.5f, height * 0.5f, Color{ 14, 12, 18, 120 });
    DrawEllipse(static_cast<int>(x), static_cast<int>(y), width * 0.35f, height * 0.35f, Color{ 8, 6, 12, 80 });
}

void GraphicsManager::DrawDivineBeam(float targetX, float alpha) {
    unsigned char a = static_cast<unsigned char>(std::clamp(alpha * 240.0f, 0.0f, 240.0f));
    Color beamCore = Color{ 255, 255, 255, a };
    Color beamGlow = Color{ 255, 235, 120, static_cast<unsigned char>(a * 0.65f) };
    Color beamOuter = Color{ 255, 210,  60, static_cast<unsigned char>(a * 0.35f) };

    // Outer holy halo
    DrawRectangle(static_cast<int>(targetX - 28), 0, 56, VIRTUAL_HEIGHT, beamOuter);
    DrawRectangle(static_cast<int>(targetX - 18), 0, 36, VIRTUAL_HEIGHT, beamGlow);
    // Core brilliant beam
    DrawRectangle(static_cast<int>(targetX - 8), 0, 16, VIRTUAL_HEIGHT, beamCore);
}

void GraphicsManager::DrawDivineLionAura(float x, float y, float timer) {
    float pulse = 1.0f + 0.18f * sinf(timer * 16.0f);
    int radius = static_cast<int>(22.0f * pulse);
    DrawCircleLines(static_cast<int>(x), static_cast<int>(y), static_cast<float>(radius), COLOR_GOLD_BRIGHT);
    DrawCircleLines(static_cast<int>(x), static_cast<int>(y), static_cast<float>(radius + 4), Color{ 255, 255, 255, 190 });
    DrawCircleLines(static_cast<int>(x), static_cast<int>(y), static_cast<float>(radius + 8), Color{ 255, 220, 90, 120 });
}

void GraphicsManager::DrawMeleeSlashEffect(float x, float y, bool flipH, float progress) {
    float arcAngle = progress * 170.0f;
    Color outerGlow = Color{ 255, 215, 60, static_cast<unsigned char>((1.0f - progress) * 200) };
    Color innerCore = Color{ 255, 255, 255, static_cast<unsigned char>((1.0f - progress) * 255) };
    float offsetX = flipH ? -16.0f : 16.0f;
    float startA = flipH ? 130.0f - arcAngle : -50.0f + arcAngle;
    float endA   = flipH ? 190.0f : 10.0f;

    // Radiant double-layered crescent energy wave
    DrawCircleSector({ x + offsetX, y }, 26.0f, startA, endA, 12, outerGlow);
    DrawCircleSector({ x + offsetX, y }, 20.0f, startA, endA, 12, innerCore);

    // Dynamic holy sparks
    DrawCircle(static_cast<int>(x + offsetX * 1.3f), static_cast<int>(y - 6.0f), 3.0f, COLOR_GOLD_BRIGHT);
    DrawCircle(static_cast<int>(x + offsetX * 1.5f), static_cast<int>(y + 6.0f), 2.5f, Color{ 255, 255, 255, 220 });
}

void GraphicsManager::DrawSpark(float x, float y, float radius, Color color) {
    DrawCircle(static_cast<int>(x), static_cast<int>(y), radius, color);
}

void GraphicsManager::DrawStormClouds(float cloudCoverage, float time, bool lightningFlash) {
    if (cloudCoverage <= 0.0f) return;

    float cov = std::clamp(cloudCoverage, 0.0f, 1.0f);
    unsigned char baseAlpha = static_cast<unsigned char>(cov * 230.0f);
    unsigned char darkAlpha = static_cast<unsigned char>(cov * 140.0f);

    // Deep stormy ambient darkening vignette
    DrawRectangle(0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT, Color{ 15, 12, 28, darkAlpha });

    // Layer 1: Distant dark rolling thunderheads (y = 0 to 60)
    for (int x = -20; x < VIRTUAL_WIDTH + 40; x += 36) {
        float wave1 = sinf(time * 0.8f + x * 0.04f) * 8.0f;
        int cy = static_cast<int>(25.0f + wave1);
        DrawCircle(x, cy, 32.0f, Color{ 28, 22, 42, baseAlpha });
        DrawCircle(x + 18, cy + 4, 26.0f, Color{ 38, 30, 56, baseAlpha });
    }

    // Layer 2: Heavy mid-level storm billows (y = 20 to 95)
    for (int x = -30; x < VIRTUAL_WIDTH + 50; x += 44) {
        float wave2 = sinf(time * 1.2f + x * 0.03f) * 10.0f;
        int cy = static_cast<int>(55.0f + wave2);
        Color cloudBody = lightningFlash ? Color{ 180, 195, 230, baseAlpha } : Color{ 48, 38, 68, baseAlpha };
        Color cloudEdge = lightningFlash ? Color{ 255, 255, 255, 255 } : Color{ 68, 55, 92, static_cast<unsigned char>(baseAlpha * 0.9f) };
        DrawCircle(x, cy, 30.0f, cloudBody);
        DrawCircleLines(x, cy, 30.0f, cloudEdge);
        DrawCircle(x + 22, cy - 6, 24.0f, cloudBody);
    }

    // Layer 3: Low rolling fog clouds
    for (int x = -10; x < VIRTUAL_WIDTH + 30; x += 50) {
        float wave3 = cosf(time * 1.5f + x * 0.05f) * 6.0f;
        int cy = static_cast<int>(80.0f + wave3);
        DrawCircle(x, cy, 28.0f, Color{ 20, 16, 32, static_cast<unsigned char>(baseAlpha * 0.95f) });
    }

    // Dynamic Lightning Strike when lightningFlash is active
    if (lightningFlash) {
        int startX = VIRTUAL_WIDTH / 2 + static_cast<int>(sinf(time * 30.0f) * 70.0f);
        Vector2 p0 = { (float)startX, 10.0f };
        Vector2 p1 = { (float)(startX - 18), 45.0f };
        Vector2 p2 = { (float)(startX + 14), 85.0f };
        Vector2 p3 = { (float)(startX - 8), 125.0f };
        Vector2 p4 = { (float)(startX + 6), 165.0f };

        // Outer electric blue aura
        DrawLineEx(p0, p1, 5.0f, Color{ 120, 190, 255, 200 });
        DrawLineEx(p1, p2, 5.0f, Color{ 120, 190, 255, 200 });
        DrawLineEx(p2, p3, 5.0f, Color{ 120, 190, 255, 200 });
        DrawLineEx(p3, p4, 5.0f, Color{ 120, 190, 255, 200 });

        // Inner pure white electric core
        DrawLineEx(p0, p1, 2.0f, WHITE);
        DrawLineEx(p1, p2, 2.0f, WHITE);
        DrawLineEx(p2, p3, 2.0f, WHITE);
        DrawLineEx(p3, p4, 2.0f, WHITE);

        // Flash burst at impact
        DrawCircleGradient(static_cast<int>(p4.x), static_cast<int>(p4.y), 32.0f, Color{ 255, 255, 255, 220 }, Color{ 140, 200, 255, 0 });
    }
}

void GraphicsManager::DrawDivineAscensionBeam(float jesusX, float beamIntensity, float cloudsPartAmount, float time) {
    if (beamIntensity <= 0.0f && cloudsPartAmount <= 0.0f) return;

    // 1. Clouds Parting Opening: radiant golden heavens revealed in the rift
    if (cloudsPartAmount > 0.0f) {
        float riftX = jesusX;
        unsigned char riftAlpha = static_cast<unsigned char>(std::clamp(cloudsPartAmount * 255.0f, 0.0f, 255.0f));
        DrawCircleGradient(static_cast<int>(riftX), -10, 65.0f, Color{ 255, 245, 180, riftAlpha }, Color{ 255, 215, 60, 0 });

        // Crepuscular Angled God Rays spreading out from the rift
        for (int r = -3; r <= 3; ++r) {
            float angleDeg = r * 14.0f + sinf(time * 2.0f + r) * 3.0f;
            float rad = angleDeg * (3.14159f / 180.0f);
            Vector2 topP = { riftX + r * 6.0f, 0.0f };
            Vector2 botP = { riftX + sinf(rad) * 220.0f, 224.0f };
            float rayAlpha = (0.25f + 0.15f * sinf(time * 3.0f + r * 1.5f)) * cloudsPartAmount;
            unsigned char ra = static_cast<unsigned char>(std::clamp(rayAlpha * 255.0f, 0.0f, 255.0f));
            DrawLineEx(topP, botP, 12.0f, Color{ 255, 235, 140, ra });
            DrawLineEx(topP, botP, 4.0f, Color{ 255, 255, 255, static_cast<unsigned char>(ra * 0.8f) });
        }
    }

    // 2. Direct Divine Ascension Pillar centered over Jesus
    if (beamIntensity > 0.0f) {
        float bi = std::clamp(beamIntensity, 0.0f, 1.0f);
        float pulse = 1.0f + 0.08f * sinf(time * 8.0f);

        unsigned char outerA = static_cast<unsigned char>(bi * 110.0f);
        unsigned char midA   = static_cast<unsigned char>(bi * 170.0f);
        unsigned char coreA  = static_cast<unsigned char>(bi * 240.0f);

        int outerW = static_cast<int>(60.0f * pulse);
        int midW   = static_cast<int>(32.0f * pulse);
        int coreW  = static_cast<int>(14.0f * pulse);

        // Outer soft golden holy aura
        DrawRectangle(static_cast<int>(jesusX - outerW / 2), 0, outerW, VIRTUAL_HEIGHT, Color{ 255, 215, 60, outerA });
        // Mid brilliant warm beam
        DrawRectangle(static_cast<int>(jesusX - midW / 2), 0, midW, VIRTUAL_HEIGHT, Color{ 255, 240, 130, midA });
        // Core pure celestial white pillar
        DrawRectangle(static_cast<int>(jesusX - coreW / 2), 0, coreW, VIRTUAL_HEIGHT, Color{ 255, 255, 255, coreA });

        // Ascending sacred light motes drifting up into heaven
        for (int i = 0; i < 14; ++i) {
            float moteSpeed = 45.0f + (i * 7 % 25);
            float moteY = fmodf(VIRTUAL_HEIGHT - (time * moteSpeed + i * 22.0f), (float)VIRTUAL_HEIGHT);
            if (moteY < 0.0f) moteY += VIRTUAL_HEIGHT;
            float moteOffsetX = sinf(time * 3.0f + i) * 16.0f;
            float moteX = jesusX + moteOffsetX;
            float moteAlpha = sinf(moteY / VIRTUAL_HEIGHT * 3.14159f) * bi;
            unsigned char ma = static_cast<unsigned char>(std::clamp(moteAlpha * 255.0f, 0.0f, 255.0f));

            DrawCircle(static_cast<int>(moteX), static_cast<int>(moteY), (i % 2 == 0) ? 2.5f : 1.5f, Color{ 255, 255, 200, ma });
            DrawCircleLines(static_cast<int>(moteX), static_cast<int>(moteY), 3.5f, Color{ 255, 220, 80, static_cast<unsigned char>(ma * 0.6f) });
        }
    }
}

void GraphicsManager::DrawFinishHimBanner(float time, float alpha) {
    if (alpha <= 0.0f) return;

    float a = std::clamp(alpha, 0.0f, 1.0f);
    unsigned char bgAlpha = static_cast<unsigned char>(a * 220.0f);
    unsigned char textAlpha = static_cast<unsigned char>(a * 255.0f);

    const char* text = "FINISH HIM!";
    int fontSize = 20;
    int tw = MeasureText(text, fontSize);
    int bw = tw + 40;
    int bh = 32;
    int bx = (VIRTUAL_WIDTH - bw) / 2;
    int by = 60;

    // Dramatic pulsing border
    float pulse = 0.85f + 0.15f * sinf(time * 14.0f);
    Color goldBorder = ColorAlpha(COLOR_GOLD_BRIGHT, a * pulse);
    Color crimsonBorder = ColorAlpha(COLOR_ROMAN_CRIMSON, a);

    // Beveled background box
    DrawRectangle(bx, by, bw, bh, Color{ 15, 8, 14, bgAlpha });
    DrawRectangleLines(bx - 1, by - 1, bw + 2, bh + 2, crimsonBorder);
    DrawRectangleLines(bx - 3, by - 3, bw + 6, bh + 6, goldBorder);

    // Corner Roman studs
    DrawRectangle(bx - 4, by - 4, 3, 3, COLOR_GOLD_BRIGHT);
    DrawRectangle(bx + bw + 1, by - 4, 3, 3, COLOR_GOLD_BRIGHT);
    DrawRectangle(bx - 4, by + bh + 1, 3, 3, COLOR_GOLD_BRIGHT);
    DrawRectangle(bx + bw + 1, by + bh + 1, 3, 3, COLOR_GOLD_BRIGHT);

    // Text with deep drop shadow
    int tx = (VIRTUAL_WIDTH - tw) / 2;
    int ty = by + 6;
    DrawText(text, tx + 2, ty + 2, fontSize, Color{ 0, 0, 0, textAlpha });
    DrawText(text, tx + 1, ty + 1, fontSize, Color{ 120, 15, 20, textAlpha });
    // Blinking golden text
    bool blink = (static_cast<int>(time * 8.0f) % 2 == 0);
    DrawText(text, tx, ty, fontSize, blink ? COLOR_GOLD_BRIGHT : Color{ 255, 245, 180, textAlpha });
}

void GraphicsManager::DrawRainbow(float alpha, float time) {
    if (alpha <= 0.0f) return;
    float a = std::clamp(alpha, 0.0f, 1.0f);
    Vector2 center = { (float)VIRTUAL_WIDTH * 0.5f, 218.0f };
    float startAngle = 186.0f;
    float endAngle = 354.0f;
    int segments = 48;

    struct RainbowBand {
        float innerR;
        float outerR;
        Color col;
    };

    RainbowBand bands[7] = {
        { 188.0f, 196.0f, Color{ 255,  55,  65, static_cast<unsigned char>(185 * a) } }, // Red
        { 180.0f, 188.0f, Color{ 255, 140,  35, static_cast<unsigned char>(190 * a) } }, // Orange
        { 172.0f, 180.0f, Color{ 255, 230,  45, static_cast<unsigned char>(195 * a) } }, // Yellow
        { 164.0f, 172.0f, Color{  65, 225,  85, static_cast<unsigned char>(190 * a) } }, // Green
        { 156.0f, 164.0f, Color{  55, 210, 255, static_cast<unsigned char>(190 * a) } }, // Cyan
        { 148.0f, 156.0f, Color{  75, 110, 255, static_cast<unsigned char>(185 * a) } }, // Blue
        { 140.0f, 148.0f, Color{ 180,  65, 245, static_cast<unsigned char>(180 * a) } }  // Violet
    };

    // 1. Soft glowing outer backdrop aura
    DrawRing(center, 134.0f, 202.0f, startAngle - 2.0f, endAngle + 2.0f, segments, Color{ 255, 255, 235, static_cast<unsigned char>(70 * a) });

    // 2. Render each luminous spectral band
    for (int i = 0; i < 7; ++i) {
        DrawRing(center, bands[i].innerR, bands[i].outerR, startAngle, endAngle, segments, bands[i].col);
    }

    // 3. Shimmering celestial light motes along the rainbow arc
    for (int i = 0; i < 18; ++i) {
        float angleDeg = 190.0f + fmodf(time * 35.0f + i * 9.5f, 160.0f);
        float rad = angleDeg * (3.14159f / 180.0f);
        float r = 142.0f + (i * 13 % 52);
        float px = center.x + cosf(rad) * r;
        float py = center.y + sinf(rad) * r;
        float sparkAlpha = (0.5f + 0.5f * sinf(time * 8.0f + i * 1.5f)) * a;
        unsigned char sa = static_cast<unsigned char>(std::clamp(sparkAlpha * 255.0f, 0.0f, 255.0f));
        DrawCircle(static_cast<int>(px), static_cast<int>(py), (i % 2 == 0) ? 2.0f : 1.2f, Color{ 255, 255, 255, sa });
    }
}

bool GraphicsManager::TryLoadExternalSpritesheet(const char* filepath) {
    if (FileExists(filepath)) {
        Texture2D sheet = LoadTexture(filepath);
        if (sheet.id > 0) {
            s_hasExternalTextures = true;
            return true;
        }
    }
    return false;
}
