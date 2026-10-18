#include "Mission5Graphics.h"
#include "Constants.h"
#include <vector>
#include <cmath>
#include <algorithm>
#include <cstdlib>

namespace M5Pixel {
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

void Mission5Graphics::GenerateAll(
    Texture2D& bgSummitSky,
    Texture2D& bgSummitMount
) {
    using namespace M5Pixel;

    constexpr int W = 320;
    constexpr int H = 224;

    // =========================================================================
    // 1. SUMMIT PARALLAX SKY (bgSummitSky)
    // Dark ominous storm clouds with purple/charcoal lightning hues
    // =========================================================================
    std::vector<Color> skyPixels(W * H);
    for (int y = 0; y < H; ++y) {
        float normY = (float)y / H;
        unsigned char r = static_cast<unsigned char>(18 + 24 * normY);
        unsigned char g = static_cast<unsigned char>(14 + 18 * normY);
        unsigned char b = static_cast<unsigned char>(32 + 35 * normY);
        for (int x = 0; x < W; ++x) {
            // Billowing thunderhead clouds in distance
            float cloudNoise = sinf(x * 0.05f + y * 0.04f) * cosf(x * 0.02f - y * 0.06f);
            int cr = r + static_cast<int>(cloudNoise * 14.0f);
            int cg = g + static_cast<int>(cloudNoise * 10.0f);
            int cb = b + static_cast<int>(cloudNoise * 20.0f);
            cr = std::clamp(cr, 10, 75);
            cg = std::clamp(cg, 8, 65);
            cb = std::clamp(cb, 20, 110);
            skyPixels[y * W + x] = Color{ static_cast<unsigned char>(cr), static_cast<unsigned char>(cg), static_cast<unsigned char>(cb), 255 };
        }
    }
    bgSummitSky = GraphicsManager::CreateTextureFromPixels(W, H, skyPixels.data());

    // =========================================================================
    // 2. MOUNT GOLGOTHA SUMMIT WITH THE CROSSES (bgSummitMount)
    // Mountain ridge, 1 LARGE center cross, 2 medium side crosses, distant crosses
    // =========================================================================
    std::vector<Color> mountPixels(W * H, Color{ 0, 0, 0, 0 });
    Color* p = mountPixels.data();

    // Distant mountain silhouettes (Deep dark violet crags)
    for (int x = 0; x < W; ++x) {
        float ridgeHeight = 110.0f + 25.0f * sinf(x * 0.035f) + 12.0f * cosf(x * 0.09f);
        int startY = static_cast<int>(ridgeHeight);
        for (int y = startY; y < H; ++y) {
            Color rock = ((x + y) % 2 == 0) ? Color{ 36, 28, 48, 255 } : Color{ 28, 22, 38, 255 };
            Set(p, W, H, x, y, rock);
        }
    }

    // Midground Golgotha Mount (The Place of the Skull)
    for (int x = 0; x < W; ++x) {
        // High central hill (peak around x = 160)
        float distFromCenter = fabsf(x - 160.0f) / 160.0f;
        float peak = 135.0f + distFromCenter * 30.0f + 8.0f * sinf(x * 0.06f);
        int startY = static_cast<int>(peak);
        for (int y = startY; y < H; ++y) {
            Color mColor = Color{ 48, 38, 44, 255 };
            if (y == startY) mColor = Color{ 72, 60, 68, 255 }; // Top ridge highlight
            else if (y > startY + 20) mColor = Color{ 32, 26, 32, 255 };
            Set(p, W, H, x, y, mColor);
        }
    }

    // Skull Rock Formations on Golgotha Cliff Face
    // Left eye cavity, right eye cavity, nasal cavity in crags
    FillRect(p, W, H, 142, 168, 12, 10, Color{ 18, 14, 22, 255 });
    FillRect(p, W, H, 166, 168, 12, 10, Color{ 18, 14, 22, 255 });
    FillRect(p, W, H, 156, 182, 8, 8, Color{ 18, 14, 22, 255 });

    // Distant small crosses on background peaks (X = 45, X = 95, X = 225, X = 275)
    auto DrawSmallCross = [&](int cx, int cy) {
        VLine(p, W, H, cx, cy, cy + 18, Color{ 20, 16, 24, 255 });
        HLine(p, W, H, cx - 4, cx + 4, cy + 5, Color{ 20, 16, 24, 255 });
    };
    DrawSmallCross(45, 105);
    DrawSmallCross(80, 118);
    DrawSmallCross(240, 115);
    DrawSmallCross(285, 102);

    // TWO MEDIUM CROSSES ON THE SIDES (Left: X = 110, Right: X = 210)
    // Left Medium Cross
    FillRect(p, W, H, 108, 85, 4, 55, Color{ 24, 18, 15, 255 }); // Vertical post
    FillRect(p, W, H, 96, 98, 28, 4, Color{ 24, 18, 15, 255 });  // Crossbeam
    OutlineRect(p, W, H, 108, 85, 4, 55, Color{ 14, 10, 8, 255 });
    OutlineRect(p, W, H, 96, 98, 28, 4, Color{ 14, 10, 8, 255 });

    // Right Medium Cross
    FillRect(p, W, H, 208, 88, 4, 55, Color{ 24, 18, 15, 255 }); // Vertical post
    FillRect(p, W, H, 196, 101, 28, 4, Color{ 24, 18, 15, 255 }); // Crossbeam
    OutlineRect(p, W, H, 208, 88, 4, 55, Color{ 14, 10, 8, 255 });
    OutlineRect(p, W, H, 196, 101, 28, 4, Color{ 14, 10, 8, 255 });

    // ONE LARGE CROSS IN THE MIDDLE (X = 157 to 163, Y = 50 to 142)
    // Standing prominently at the top of the mount!
    // Vertical Timber (6px wide, 92px tall)
    FillRect(p, W, H, 157, 50, 7, 92, Color{ 42, 28, 20, 255 });
    // Wood grain highlights & shadow
    VLine(p, W, H, 158, 52, 140, Color{ 65, 46, 32, 255 });
    VLine(p, W, H, 162, 52, 140, Color{ 20, 14, 10, 255 });

    // Horizontal Crossbeam (48px wide, 7px tall, Y = 70 to 76)
    FillRect(p, W, H, 136, 70, 49, 7, Color{ 42, 28, 20, 255 });
    HLine(p, W, H, 137, 183, 71, Color{ 65, 46, 32, 255 });
    HLine(p, W, H, 137, 183, 76, Color{ 20, 14, 10, 255 });

    // Titulus Inscription Tablet atop center cross ("INRI")
    FillRect(p, W, H, 155, 42, 11, 8, Color{ 210, 195, 160, 255 });
    OutlineRect(p, W, H, 155, 42, 11, 8, Color{ 50, 35, 20, 255 });
    // Tiny inscription marks
    Set(p, W, H, 157, 45, Color{ 80, 20, 25, 255 });
    Set(p, W, H, 159, 45, Color{ 80, 20, 25, 255 });
    Set(p, W, H, 161, 45, Color{ 80, 20, 25, 255 });
    Set(p, W, H, 163, 45, Color{ 80, 20, 25, 255 });

    // Outlines on Large Cross
    OutlineRect(p, W, H, 157, 50, 7, 92, Color{ 14, 10, 8, 255 });
    OutlineRect(p, W, H, 136, 70, 49, 7, Color{ 14, 10, 8, 255 });

    // Base rock mound / stone wedges at foot of center cross
    FillRect(p, W, H, 151, 138, 19, 8, Color{ 75, 65, 70, 255 });
    OutlineRect(p, W, H, 151, 138, 19, 8, Color{ 25, 20, 25, 255 });

    bgSummitMount = GraphicsManager::CreateTextureFromPixels(W, H, mountPixels.data());

    // =========================================================================
    // 3. STAGE 13 TILE TEXTURES
    // =========================================================================
    {
        // TILE_SUMMIT_ROCK_SURFACE (Jagged volcanic crag with dark slate highlights)
        std::vector<Color> t0(16 * 16, Color{ 48, 38, 44, 255 });
        HLine(t0.data(), 16, 16, 0, 15, 0, Color{ 82, 70, 78, 255 });
        HLine(t0.data(), 16, 16, 0, 15, 1, Color{ 68, 56, 64, 255 });
        for (int i = 0; i < 16; i += 3) {
            Set(t0.data(), 16, 16, i, 0, Color{ 110, 95, 105, 255 });
            Set(t0.data(), 16, 16, i + 1, 4, Color{ 28, 22, 28, 255 });
        }
        GraphicsManager::SetTileTexture(TILE_SUMMIT_ROCK_SURFACE, GraphicsManager::CreateTextureFromPixels(16, 16, t0.data()));

        // TILE_SUMMIT_ROCK_SUB (Deep solid basalt)
        std::vector<Color> t1(16 * 16, Color{ 28, 22, 26, 255 });
        for (int i = 0; i < 16; i += 4) {
            Set(t1.data(), 16, 16, i, i, Color{ 40, 32, 38, 255 });
            Set(t1.data(), 16, 16, (15 - i), i, Color{ 18, 14, 18, 255 });
        }
        GraphicsManager::SetTileTexture(TILE_SUMMIT_ROCK_SUB, GraphicsManager::CreateTextureFromPixels(16, 16, t1.data()));

        // TILE_SUMMIT_CRAG_LEDGE (Elevated rocky plateau ledge)
        std::vector<Color> t2(16 * 16, Color{ 0, 0, 0, 0 });
        FillRect(t2.data(), 16, 16, 0, 0, 16, 6, Color{ 58, 48, 54, 255 });
        HLine(t2.data(), 16, 16, 0, 15, 0, Color{ 105, 92, 100, 255 });
        HLine(t2.data(), 16, 16, 0, 15, 5, Color{ 22, 18, 22, 255 });
        GraphicsManager::SetTileTexture(TILE_SUMMIT_CRAG_LEDGE, GraphicsManager::CreateTextureFromPixels(16, 16, t2.data()));

        // TILE_SUMMIT_SKULL_ROCK (Golgotha weathered stone)
        std::vector<Color> t3(16 * 16, Color{ 55, 46, 50, 255 });
        OutlineRect(t3.data(), 16, 16, 1, 1, 14, 14, Color{ 25, 20, 24, 255 });
        FillRect(t3.data(), 16, 16, 3, 4, 3, 3, Color{ 16, 12, 16, 255 });
        FillRect(t3.data(), 16, 16, 10, 4, 3, 3, Color{ 16, 12, 16, 255 });
        FillRect(t3.data(), 16, 16, 7, 8, 2, 3, Color{ 16, 12, 16, 255 });
        GraphicsManager::SetTileTexture(TILE_SUMMIT_SKULL_ROCK, GraphicsManager::CreateTextureFromPixels(16, 16, t3.data()));

        // TILE_SUMMIT_BRAZIER (Dark iron brazier with burning fire)
        std::vector<Color> t4(16 * 16, Color{ 0, 0, 0, 0 });
        FillRect(t4.data(), 16, 16, 4, 8, 8, 8, Color{ 50, 45, 55, 255 });
        OutlineRect(t4.data(), 16, 16, 4, 8, 8, 8, Color{ 20, 18, 22, 255 });
        FillRect(t4.data(), 16, 16, 5, 3, 6, 6, Color{ 255, 120, 30, 255 });
        FillRect(t4.data(), 16, 16, 6, 1, 4, 5, Color{ 255, 220, 70, 255 });
        GraphicsManager::SetTileTexture(TILE_SUMMIT_BRAZIER, GraphicsManager::CreateTextureFromPixels(16, 16, t4.data()));
    }
}

// Animated Rain Streaks cascading across screen
void Mission5Graphics::DrawRain(float time, float intensity) {
    if (intensity <= 0.0f) return;
    int numDrops = static_cast<int>(65 * intensity);
    for (int i = 0; i < numDrops; ++i) {
        // Deterministic pseudo-random positions based on index and time
        float seed = static_cast<float>(i * 37 + 11);
        float speed = 260.0f + fmodf(seed * 19.0f, 140.0f);
        float x = fmodf(seed * 73.0f + time * (speed * 0.45f), (float)VIRTUAL_WIDTH + 40.0f) - 20.0f;
        float y = fmodf(seed * 91.0f + time * speed, (float)VIRTUAL_HEIGHT + 30.0f) - 15.0f;
        float len = 6.0f + fmodf(seed, 6.0f);

        Color dropCol = ColorAlpha(Color{ 180, 215, 255, 200 }, 0.45f * intensity);
        DrawLine(static_cast<int>(x), static_cast<int>(y), static_cast<int>(x - 3), static_cast<int>(y + len), dropCol);

        // Ground splash ripple
        if (y >= GROUND_Y - 4.0f && y <= GROUND_Y + 10.0f) {
            DrawPixel(static_cast<int>(x), static_cast<int>(y), ColorAlpha(WHITE, 0.6f * intensity));
            DrawPixel(static_cast<int>(x - 1), static_cast<int>(y), ColorAlpha(dropCol, 0.4f * intensity));
            DrawPixel(static_cast<int>(x + 1), static_cast<int>(y), ColorAlpha(dropCol, 0.4f * intensity));
        }
    }
}

// Glowing Rainbow Bridge from the heavens
void Mission5Graphics::DrawRainbowBridge(float alpha, float time) {
    if (alpha <= 0.0f) return;

    Color rainbowColors[7] = {
        Color{ 255,  50,  60, 200 }, // Red
        Color{ 255, 140,  30, 200 }, // Orange
        Color{ 255, 230,  50, 200 }, // Yellow
        Color{  50, 220,  80, 200 }, // Green
        Color{  40, 200, 255, 200 }, // Cyan
        Color{  60, 100, 255, 200 }, // Blue
        Color{ 180,  60, 240, 200 }  // Violet
    };

    // The bridge arcs diagonally from top right (X = 300, Y = 0) down to center stage (X = 140, Y = 186)
    Vector2 startPt = { 290.0f, -10.0f };
    Vector2 endPt = { 140.0f, GROUND_Y - 6.0f };
    Vector2 controlPt = { 260.0f, 130.0f };

    constexpr int SEGMENTS = 28;
    for (int band = 0; band < 7; ++band) {
        Color bCol = ColorAlpha(rainbowColors[band], alpha * 0.85f);
        float offset = (band - 3) * 2.2f;

        Vector2 prev = startPt;
        for (int s = 1; s <= SEGMENTS; ++s) {
            float t = (float)s / SEGMENTS;
            float invT = 1.0f - t;
            // Quadratic Bezier
            float bx = invT * invT * startPt.x + 2.0f * invT * t * controlPt.x + t * t * endPt.x + offset * 0.7f;
            float by = invT * invT * startPt.y + 2.0f * invT * t * controlPt.y + t * t * endPt.y + offset;
            Vector2 curr = { bx, by };
            DrawLineEx(prev, curr, 2.5f, bCol);
            prev = curr;
        }
    }

    // Sparkles on rainbow bridge
    for (int sp = 0; sp < 6; ++sp) {
        float st = fmodf(time * 0.8f + sp * 0.17f, 1.0f);
        float invT = 1.0f - st;
        float sx = invT * invT * startPt.x + 2.0f * invT * st * controlPt.x + st * st * endPt.x;
        float sy = invT * invT * startPt.y + 2.0f * invT * st * controlPt.y + st * st * endPt.y;
        float pulse = 0.5f + 0.5f * sinf(time * 12.0f + sp * 2.0f);
        DrawCircle(static_cast<int>(sx), static_cast<int>(sy), 2.0f * pulse, ColorAlpha(WHITE, alpha * 0.9f));
    }
}

// "God" Descending figure
void Mission5Graphics::DrawGodDescend(Vector2 pos, float time) {
    float bob = sinf(time * 3.5f) * 3.0f;
    float drawX = pos.x;
    float drawY = pos.y + bob;

    // Golden divine aura
    float pulse = 0.8f + 0.2f * sinf(time * 8.0f);
    DrawCircleGradient(static_cast<int>(drawX + 20), static_cast<int>(drawY + 24), 38.0f * pulse, ColorAlpha(COLOR_GOLD_BRIGHT, 0.45f), ColorAlpha(WHITE, 0.0f));

    // Draw God Sprite
    GraphicsManager::DrawSprite(SPR_GOD_DESCEND, drawX, drawY, false, WHITE);
}

// Interactive Mercy Dialogue Box
void Mission5Graphics::DrawMercyDialogue(bool selectAgree, float time) {
    int boxW = 288;
    int boxH = 82;
    int boxX = (VIRTUAL_WIDTH - boxW) / 2;
    int boxY = 24;

    // Dark parchment background with double ornate gold border
    DrawRectangle(boxX, boxY, boxW, boxH, Color{ 14, 10, 20, 245 });
    DrawRectangleLines(boxX, boxY, boxW, boxH, COLOR_ROMAN_GOLD);
    DrawRectangleLines(boxX + 2, boxY + 2, boxW - 4, boxH - 4, Color{ 60, 45, 20, 255 });

    // Voice of God Title
    const char* speaker = "VOICE FROM HEAVEN:";
    DrawText(speaker, boxX + 10, boxY + 8, 10, COLOR_GOLD_BRIGHT);

    // Speech lines
    const char* line1 = "\"MY BELOVED SON, THOU HAST CONQUERED SIN & DEATH.";
    const char* line2 = " WILT THOU ACCEPT MY MERCY AND REST THY SOUL?\"";
    DrawText(line1, boxX + 10, boxY + 22, 10, COLOR_ROBE_WHITE);
    DrawText(line2, boxX + 10, boxY + 34, 10, COLOR_ROBE_WHITE);

    // Option A: ACCEPT MERCY (AGREE)
    int btnW = 126;
    int btnH = 20;
    int btn1X = boxX + 12;
    int btn1Y = boxY + 52;

    Color b1Bg = selectAgree ? Color{ 50, 15, 20, 255 } : Color{ 22, 16, 28, 220 };
    Color b1Border = selectAgree ? COLOR_GOLD_BRIGHT : Color{ 70, 60, 75, 255 };
    Color b1Text = selectAgree ? COLOR_GOLD_BRIGHT : Color{ 180, 175, 190, 255 };
    DrawRectangle(btn1X, btn1Y, btnW, btnH, b1Bg);
    DrawRectangleLines(btn1X, btn1Y, btnW, btnH, b1Border);
    if (selectAgree) {
        float p = 0.5f + 0.5f * sinf(time * 8.0f);
        DrawRectangleLines(btn1X - 1, btn1Y - 1, btnW + 2, btnH + 2, ColorAlpha(COLOR_ROMAN_CRIMSON, p));
    }
    DrawText("> ACCEPT MERCY (YES)", btn1X + 8, btn1Y + 5, 10, b1Text);

    // Option B: REJECT / DISAGREE
    int btn2X = boxX + boxW - btnW - 12;
    int btn2Y = boxY + 52;
    bool selectDisagree = !selectAgree;

    Color b2Bg = selectDisagree ? Color{ 15, 35, 55, 255 } : Color{ 22, 16, 28, 220 };
    Color b2Border = selectDisagree ? COLOR_GOLD_BRIGHT : Color{ 70, 60, 75, 255 };
    Color b2Text = selectDisagree ? COLOR_GOLD_BRIGHT : Color{ 180, 175, 190, 255 };
    DrawRectangle(btn2X, btn2Y, btnW, btnH, b2Bg);
    DrawRectangleLines(btn2X, btn2Y, btnW, btnH, b2Border);
    if (selectDisagree) {
        float p = 0.5f + 0.5f * sinf(time * 8.0f);
        DrawRectangleLines(btn2X - 1, btn2Y - 1, btnW + 2, btnH + 2, ColorAlpha(COLOR_STEEL_BRIGHT, p));
    }
    DrawText("> REJECT FALSE LIGHT", btn2X + 8, btn2Y + 5, 10, b2Text);

    // Prompt hint at bottom
    const char* hint = "[LEFT / RIGHT] CHOOSE    [START / JUMP] CONFIRM";
    int hw = MeasureText(hint, 10);
    DrawText(hint, (VIRTUAL_WIDTH - hw) / 2, boxY + boxH + 6, 10, COLOR_ROMAN_GOLD);
}

// Tragic Crucifixion Bad Ending Cutscene (When Jesus Agrees)
void Mission5Graphics::DrawCrucifixionScene(float time, float fadeAlpha) {
    // Blood-red stormy background
    DrawRectangleGradientV(0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT, Color{ 45, 6, 12, 255 }, Color{ 12, 4, 8, 255 });

    // Lightning Flash occasionally
    if (fmodf(time, 2.5f) < 0.12f) {
        DrawRectangle(0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT, Color{ 255, 230, 235, 160 });
    }

    // The Mount of Golgotha with Jesus crucified on the large center cross
    int cx = VIRTUAL_WIDTH / 2;
    int cy = 40;

    // Cross post
    DrawRectangle(cx - 4, cy, 8, 120, Color{ 35, 22, 16, 255 });
    // Crossbeam
    DrawRectangle(cx - 36, cy + 24, 72, 8, Color{ 35, 22, 16, 255 });
    // Titulus tablet
    DrawRectangle(cx - 10, cy - 8, 20, 9, Color{ 210, 195, 160, 255 });
    DrawText("INRI", cx - 8, cy - 7, 10, Color{ 80, 20, 25, 255 });

    Color skin = COLOR_SKIN;
    // Jesus on the Cross (Head bowed, arms outstretched)
    DrawCircle(cx, cy + 22, 7, skin);
    // Crown of thorns
    DrawCircleLines(cx, cy + 20, 8, Color{ 60, 80, 40, 255 });
    // Arms on crossbeam
    DrawLineEx({ (float)cx - 32, (float)cy + 26 }, { (float)cx - 4, (float)cy + 26 }, 4.0f, skin);
    DrawLineEx({ (float)cx + 4, (float)cy + 26 }, { (float)cx + 32, (float)cy + 26 }, 4.0f, skin);
    // Torso & legs
    DrawRectangle(cx - 5, cy + 28, 10, 36, skin);
    // White loincloth
    DrawRectangle(cx - 7, cy + 44, 14, 12, Color{ 235, 235, 230, 255 });
    DrawRectangleLines(cx - 7, cy + 44, 14, 12, Color{ 140, 140, 150, 255 });
    DrawRectangle(cx - 4, cy + 64, 8, 28, skin);

    // Pontius Pilate smirking below on the right
    GraphicsManager::DrawSprite(SPR_PILATE_IDLE, cx + 55, GROUND_Y - 48, true, WHITE);
    // Praetorian Guard with spear on the left
    GraphicsManager::DrawSprite(SPR_PRAETORIAN_WALK_0, cx - 85, GROUND_Y - 48, false, WHITE);

    // Dramatic Tragic Ending Text
    DrawRectangle(0, VIRTUAL_HEIGHT - 64, VIRTUAL_WIDTH, 64, Color{ 10, 4, 8, 230 });
    DrawLine(0, VIRTUAL_HEIGHT - 64, VIRTUAL_WIDTH, VIRTUAL_HEIGHT - 64, COLOR_ROMAN_CRIMSON);

    const char* badTitle = "CRUCIFIXION - THOU HAST SURRENDERED!";
    int tw = MeasureText(badTitle, 10);
    DrawText(badTitle, (VIRTUAL_WIDTH - tw) / 2, VIRTUAL_HEIGHT - 56, 10, COLOR_ROMAN_CRIMSON);

    const char* badSub1 = "PILATE: \"FOOL! ROME OFFERS MERCY TO NO REBEL KING!\"";
    int s1w = MeasureText(badSub1, 10);
    DrawText(badSub1, (VIRTUAL_WIDTH - s1w) / 2, VIRTUAL_HEIGHT - 42, 10, COLOR_GOLD_BRIGHT);

    const char* badSub2 = "THE WORLD'S FALSE LIGHT OVERCAME THY SPIRIT. RESURRECT AND FIGHT ON!";
    int s2w = MeasureText(badSub2, 10);
    DrawText(badSub2, (VIRTUAL_WIDTH - s2w) / 2, VIRTUAL_HEIGHT - 28, 10, COLOR_ROBE_WHITE);

    // Fade overlay if transitioning
    if (fadeAlpha > 0.0f) {
        DrawRectangle(0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT, ColorAlpha(BLACK, fadeAlpha));
    }
}

// Hallucination Dizzy Stars orbiting Jesus's head
void Mission5Graphics::DrawHallucinationStars(Vector2 headPos, float time, float clearProgress) {
    if (clearProgress >= 1.0f) return;

    // 4 golden cartoon stars orbiting in an ellipse
    float radiusX = 18.0f * (1.0f + clearProgress * 1.5f);
    float radiusY = 7.0f * (1.0f + clearProgress * 1.5f);
    float alpha = 1.0f - clearProgress;

    for (int i = 0; i < 4; ++i) {
        float angle = time * 7.5f + i * (3.14159f / 2.0f);
        float sx = headPos.x + cosf(angle) * radiusX;
        float sy = headPos.y + sinf(angle) * radiusY;

        Color starCol = ColorAlpha(COLOR_GOLD_BRIGHT, alpha);
        // Draw 4-point star
        int ix = static_cast<int>(sx);
        int iy = static_cast<int>(sy);
        DrawPixel(ix, iy, WHITE);
        DrawPixel(ix - 1, iy, starCol);
        DrawPixel(ix + 1, iy, starCol);
        DrawPixel(ix, iy - 1, starCol);
        DrawPixel(ix, iy + 1, starCol);
        DrawPixel(ix - 2, iy, ColorAlpha(COLOR_ROMAN_GOLD, alpha));
        DrawPixel(ix + 2, iy, ColorAlpha(COLOR_ROMAN_GOLD, alpha));
        DrawPixel(ix, iy - 2, ColorAlpha(COLOR_ROMAN_GOLD, alpha));
        DrawPixel(ix, iy + 2, ColorAlpha(COLOR_ROMAN_GOLD, alpha));
    }
}

// Descending Choir of Angels
void Mission5Graphics::DrawAscendingAngels(Vector2 jesusPos, float timer) {
    // 4 angels hover around Jesus as he ascends
    float angelOffsets[4][2] = {
        { -42.0f,  18.0f },
        {  42.0f,  12.0f },
        { -60.0f, -24.0f },
        {  58.0f, -18.0f }
    };

    for (int a = 0; a < 4; ++a) {
        float ax = jesusPos.x + angelOffsets[a][0] + sinf(timer * 4.0f + a) * 4.0f;
        float ay = jesusPos.y + angelOffsets[a][1] + cosf(timer * 3.0f + a * 1.5f) * 3.0f;
        bool flip = (angelOffsets[a][0] > 0);
        SpriteId spr = (static_cast<int>(timer * 6.0f + a) % 2 == 0) ? SPR_ANGEL_SING_0 : SPR_ANGEL_SING_1;
        GraphicsManager::DrawSprite(spr, ax, ay, flip, WHITE);

        // Musical singing notes floating up
        float noteY = ay - fmodf(timer * 25.0f + a * 12.0f, 28.0f);
        float noteX = ax + 14.0f + sinf(noteY * 0.2f) * 3.0f;
        DrawText("♪", static_cast<int>(noteX), static_cast<int>(noteY), 10, ColorAlpha(COLOR_GOLD_BRIGHT, 0.8f));
    }
}

// =========================================================================
// 8. THE EPILOGUE: SKEE-BALL IN HEAVEN!
// Jesus bowling wooden skee-balls in heaven with cheering angels!
// =========================================================================
void Mission5Graphics::DrawHeavenSkeeBallScene(float time, int score, float ballRollProgress, int ballsRolled, bool ballInHole, int tickets) {
    // 1. Celestial Heaven Backdrop: Golden sunburst & fluffy pearly clouds
    DrawRectangleGradientV(0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT, Color{ 140, 195, 255, 255 }, Color{ 255, 235, 195, 255 });

    // Pearly fluffy clouds floating across bottom
    for (int c = 0; c < 8; ++c) {
        float cx = fmodf(c * 50.0f + time * 12.0f, (float)VIRTUAL_WIDTH + 60.0f) - 30.0f;
        float cy = 180.0f + sinf(c * 1.5f) * 10.0f;
        DrawCircle(static_cast<int>(cx), static_cast<int>(cy), 26.0f, Color{ 255, 255, 255, 210 });
        DrawCircle(static_cast<int>(cx + 18), static_cast<int>(cy - 4), 20.0f, Color{ 255, 255, 255, 230 });
    }

    // Golden Pearly Arch behind the Skee-Ball machine
    DrawCircleSector({ VIRTUAL_WIDTH / 2.0f, 130.0f }, 110.0f, 180.0f, 360.0f, 32, ColorAlpha(COLOR_GOLD_BRIGHT, 0.25f));
    DrawCircleSectorLines({ VIRTUAL_WIDTH / 2.0f, 130.0f }, 110.0f, 180.0f, 360.0f, 32, COLOR_ROMAN_GOLD);

    // 2. THE SKEE-BALL ALLEY MACHINE (Center: X = 110 to 210, Y = 60 to 195)
    int alleyX = 105;
    int alleyY = 62;
    int alleyW = 110;
    int alleyH = 135;

    // Wood & Chrome Machine Cabinet Body
    DrawRectangle(alleyX, alleyY, alleyW, alleyH, Color{ 90, 48, 24, 255 }); // Dark mahogany wood
    DrawRectangleLines(alleyX, alleyY, alleyW, alleyH, Color{ 220, 180, 70, 255 });
    DrawRectangleLines(alleyX + 2, alleyY + 2, alleyW - 4, alleyH - 4, Color{ 40, 20, 10, 255 });

    // Top Target Bullseye Board (Slanted target rings)
    int boardX = alleyX + 8;
    int boardY = alleyY + 8;
    int boardW = alleyW - 16;
    int boardH = 68;

    DrawRectangle(boardX, boardY, boardW, boardH, Color{ 32, 28, 38, 255 }); // Dark felt target backing
    DrawRectangleLines(boardX, boardY, boardW, boardH, Color{ 190, 150, 40, 255 });

    // Skee-Ball Target Rings (Concentric circles: 10, 20, 30, 40, 50, and 100 in corners)
    int tgtCenterX = boardX + boardW / 2;
    int tgtCenterY = boardY + 44;

    // Outer Ring: 10 PTS
    DrawCircle(tgtCenterX, tgtCenterY, 26, Color{ 48, 42, 54, 255 });
    DrawCircleLines(tgtCenterX, tgtCenterY, 26, Color{ 220, 200, 100, 255 });
    DrawText("10", tgtCenterX - 6, tgtCenterY + 16, 10, COLOR_GOLD_BRIGHT);

    // Mid Ring: 20 PTS
    DrawCircle(tgtCenterX, tgtCenterY, 20, Color{ 62, 54, 70, 255 });
    DrawCircleLines(tgtCenterX, tgtCenterY, 20, Color{ 240, 220, 120, 255 });
    DrawText("20", tgtCenterX - 6, tgtCenterY + 10, 10, COLOR_GOLD_BRIGHT);

    // Inner Ring: 30 PTS
    DrawCircle(tgtCenterX, tgtCenterY, 14, Color{ 82, 70, 92, 255 });
    DrawCircleLines(tgtCenterX, tgtCenterY, 14, Color{ 255, 235, 140, 255 });
    DrawText("30", tgtCenterX - 6, tgtCenterY + 4, 10, COLOR_GOLD_BRIGHT);

    // Bullseye Ring: 50 PTS
    DrawCircle(tgtCenterX, tgtCenterY - 4, 8, Color{ 180, 30, 45, 255 });
    DrawCircleLines(tgtCenterX, tgtCenterY - 4, 8, COLOR_GOLD_BRIGHT);
    DrawText("50", tgtCenterX - 5, tgtCenterY - 8, 10, WHITE);

    // Top Corners: THE 100 PTS HOLES!
    // Left 100 Hole
    int h100LeftX = boardX + 14;
    int h100LeftY = boardY + 14;
    DrawCircle(h100LeftX, h100LeftY, 8, Color{ 20, 15, 25, 255 });
    DrawCircleLines(h100LeftX, h100LeftY, 8, Color{ 255, 215, 50, 255 });
    DrawText("100", h100LeftX - 7, h100LeftY - 4, 10, COLOR_GOLD_BRIGHT);

    // Right 100 Hole (THE TARGET!)
    int h100RightX = boardX + boardW - 14;
    int h100RightY = boardY + 14;
    bool holeBlink = ballInHole && (static_cast<int>(time * 12.0f) % 2 == 0);
    DrawCircle(h100RightX, h100RightY, 8, holeBlink ? Color{ 255, 180, 40, 255 } : Color{ 20, 15, 25, 255 });
    DrawCircleLines(h100RightX, h100RightY, 8, Color{ 255, 215, 50, 255 });
    DrawText("100", h100RightX - 7, h100RightY - 4, 10, holeBlink ? WHITE : COLOR_GOLD_BRIGHT);

    // Skee-Ball Ball Jump Lip & Ramp
    int rampX = alleyX + 14;
    int rampY = boardY + boardH + 2;
    int rampW = alleyW - 28;
    int rampH = 50;

    // Wooden lane slats
    DrawRectangle(rampX, rampY, rampW, rampH, Color{ 175, 118, 62, 255 }); // Polished maple wood
    DrawRectangleLines(rampX, rampY, rampW, rampH, Color{ 70, 40, 15, 255 });
    // Wood grain lines
    for (int l = 1; l < 4; ++l) {
        DrawLine(rampX + l * 18, rampY, rampX + l * 18, rampY + rampH, Color{ 145, 95, 45, 255 });
    }
    // Launch curve lip at top of ramp
    DrawRectangle(rampX, rampY + 2, rampW, 5, Color{ 220, 155, 85, 255 });
    DrawLine(rampX, rampY + 2, rampX + rampW, rampY + 2, Color{ 255, 220, 150, 255 });

    // Ball rolling animation
    if (ballRollProgress >= 0.0f && ballRollProgress <= 1.0f) {
        float ballX, ballY;
        if (ballRollProgress < 0.6f) {
            // Rolling up ramp
            float t = ballRollProgress / 0.6f;
            ballX = rampX + rampW / 2.0f + (h100RightX - (rampX + rampW / 2.0f)) * t * 0.5f;
            ballY = rampY + rampH - 6.0f - t * (rampH - 10.0f);
        } else {
            // Airborne arc towards 100 ring
            float t = (ballRollProgress - 0.6f) / 0.4f;
            float startArcX = rampX + rampW / 2.0f + (h100RightX - (rampX + rampW / 2.0f)) * 0.5f;
            float startArcY = rampY + 8.0f;
            ballX = startArcX + (h100RightX - startArcX) * t;
            // Parabolic jump arc
            ballY = startArcY + (h100RightY - startArcY) * t - sinf(t * 3.14159f) * 16.0f;
        }
        // Draw wooden ball
        DrawCircle(static_cast<int>(ballX), static_cast<int>(ballY), 4, Color{ 190, 130, 65, 255 });
        DrawCircleLines(static_cast<int>(ballX), static_cast<int>(ballY), 4, Color{ 45, 25, 10, 255 });
        DrawCircle(static_cast<int>(ballX - 1), static_cast<int>(ballY - 1), 1, Color{ 245, 210, 160, 255 });
    }

    // 3. TOP LED MARQUEE SCOREBOARD
    int ledX = alleyX + 6;
    int ledY = alleyY - 26;
    int ledW = alleyW - 12;
    int ledH = 22;

    DrawRectangle(ledX, ledY, ledW, ledH, Color{ 18, 12, 22, 255 });
    DrawRectangleLines(ledX, ledY, ledW, ledH, COLOR_ROMAN_GOLD);

    char scoreStr[32];
    snprintf(scoreStr, sizeof(scoreStr), "SCORE %06d", score);
    DrawText(scoreStr, ledX + 8, ledY + 6, 10, Color{ 255, 60, 60, 255 }); // Red arcade 7-segment LED

    // Flash "100! PERFECT ROLL!" when scored
    if (ballInHole) {
        const char* strikeMsg = "PERFECT 100 ROLL!";
        int smw = MeasureText(strikeMsg, 10);
        DrawRectangle((VIRTUAL_WIDTH - smw) / 2 - 6, 18, smw + 12, 16, Color{ 15, 10, 22, 230 });
        DrawRectangleLines((VIRTUAL_WIDTH - smw) / 2 - 6, 18, smw + 12, 16, COLOR_GOLD_BRIGHT);
        DrawText(strikeMsg, (VIRTUAL_WIDTH - smw) / 2, 22, 10, COLOR_GOLD_BRIGHT);
    }

    // 4. WATERFALL OF GOLDEN TICKETS (Spewing from machine side dispenser)
    int tickDispX = alleyX + alleyW + 2;
    int tickDispY = alleyY + 45;
    DrawRectangle(tickDispX, tickDispY, 6, 12, Color{ 70, 70, 80, 255 });
    // Streaming golden tickets
    for (int tk = 0; tk < 12; ++tk) {
        float ty = tickDispY + 12.0f + tk * 7.0f + sinf(time * 12.0f + tk) * 2.0f;
        float tx = tickDispX + sinf(time * 8.0f + tk * 0.7f) * 3.0f;
        DrawRectangle(static_cast<int>(tx), static_cast<int>(ty), 9, 5, Color{ 255, 215, 60, 255 });
        DrawRectangleLines(static_cast<int>(tx), static_cast<int>(ty), 9, 5, Color{ 170, 130, 20, 255 });
    }
    char tickStr[32];
    snprintf(tickStr, sizeof(tickStr), "TICKETS: %d", tickets);
    DrawText(tickStr, alleyX + alleyW + 8, tickDispY - 10, 10, COLOR_GOLD_BRIGHT);

    // 5. JESUS PLAYING SKEE-BALL (Standing on right side of alley)
    GraphicsManager::DrawSprite(SPR_SKEEBALL_JESUS, alleyX + alleyW - 22, alleyY + alleyH - 38, true, WHITE);

    // 6. CHEERING ANGELS ON LEFT AND RIGHT WAVING POM-POMS!
    // Left Angel 1
    SpriteId aSpr0 = (static_cast<int>(time * 5.0f) % 2 == 0) ? SPR_ANGEL_CHEER_0 : SPR_ANGEL_CHEER_1;
    GraphicsManager::DrawSprite(aSpr0, 20, 110, false, WHITE);

    // Left Angel 2
    SpriteId aSpr1 = (static_cast<int>(time * 5.0f + 1) % 2 == 0) ? SPR_ANGEL_CHEER_0 : SPR_ANGEL_CHEER_1;
    GraphicsManager::DrawSprite(aSpr1, 55, 132, false, WHITE);

    // Right Angel
    SpriteId aSpr2 = (static_cast<int>(time * 5.0f) % 2 == 0) ? SPR_ANGEL_CHEER_0 : SPR_ANGEL_CHEER_1;
    GraphicsManager::DrawSprite(aSpr2, 260, 115, true, WHITE);

    // Floating cheering bubbles
    const char* cheers[3] = { "HOLY ROLL!", "100 PTS!", "NEW HIGH SCORE!" };
    int cheerIdx = static_cast<int>(time * 0.7f) % 3;
    DrawText(cheers[cheerIdx], 34, 94, 10, COLOR_GOLD_BRIGHT);

    // 7. Footer Prompt
    const char* epPrompt = "[PRESS ANY BUTTON TO RETURN TO TITLE]";
    int epw = MeasureText(epPrompt, 10);
    DrawText(epPrompt, (VIRTUAL_WIDTH - epw) / 2, VIRTUAL_HEIGHT - 16, 10, COLOR_ROMAN_GOLD);
}
