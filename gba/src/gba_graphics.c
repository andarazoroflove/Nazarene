#include "gba_graphics.h"
#include <string.h>

// Screen backbuffer in fast EWRAM
static uint8_t gba_screen[GBA_SCREEN_HEIGHT][GBA_SCREEN_WIDTH] __attribute__((section(".ewram")));
static bool s_current_page = false;

// 5x7 compact arcade font bitmap (ASCII 32 to 90)
static const uint8_t s_font5x7[][5] = {
    { 0x00, 0x00, 0x00, 0x00, 0x00 }, // Space
    { 0x00, 0x00, 0x5F, 0x00, 0x00 }, // !
    { 0x00, 0x07, 0x00, 0x07, 0x00 }, // "
    { 0x14, 0x7F, 0x14, 0x7F, 0x14 }, // #
    { 0x24, 0x2A, 0x7F, 0x2A, 0x12 }, // $
    { 0x23, 0x13, 0x08, 0x64, 0x62 }, // %
    { 0x36, 0x49, 0x55, 0x22, 0x50 }, // &
    { 0x00, 0x05, 0x03, 0x00, 0x00 }, // '
    { 0x00, 0x1C, 0x22, 0x41, 0x00 }, // (
    { 0x00, 0x41, 0x22, 0x1C, 0x00 }, // )
    { 0x14, 0x08, 0x3E, 0x08, 0x14 }, // *
    { 0x08, 0x08, 0x3E, 0x08, 0x08 }, // +
    { 0x00, 0x50, 0x30, 0x00, 0x00 }, // ,
    { 0x08, 0x08, 0x08, 0x08, 0x08 }, // -
    { 0x00, 0x60, 0x60, 0x00, 0x00 }, // .
    { 0x20, 0x10, 0x08, 0x04, 0x02 }, // /
    { 0x3E, 0x51, 0x49, 0x45, 0x3E }, // 0
    { 0x00, 0x42, 0x7F, 0x40, 0x00 }, // 1
    { 0x42, 0x61, 0x51, 0x49, 0x46 }, // 2
    { 0x21, 0x41, 0x45, 0x4B, 0x31 }, // 3
    { 0x18, 0x14, 0x12, 0x7F, 0x10 }, // 4
    { 0x27, 0x45, 0x45, 0x45, 0x39 }, // 5
    { 0x3C, 0x4A, 0x49, 0x49, 0x30 }, // 6
    { 0x01, 0x71, 0x09, 0x05, 0x03 }, // 7
    { 0x36, 0x49, 0x49, 0x49, 0x36 }, // 8
    { 0x06, 0x49, 0x49, 0x29, 0x1E }, // 9
    { 0x00, 0x36, 0x36, 0x00, 0x00 }, // :
    { 0x00, 0x56, 0x36, 0x00, 0x00 }, // ;
    { 0x08, 0x14, 0x22, 0x41, 0x00 }, // <
    { 0x14, 0x14, 0x14, 0x14, 0x14 }, // =
    { 0x00, 0x41, 0x22, 0x14, 0x08 }, // >
    { 0x02, 0x01, 0x51, 0x09, 0x06 }, // ?
    { 0x32, 0x49, 0x79, 0x41, 0x3E }, // @
    { 0x7E, 0x11, 0x11, 0x11, 0x7E }, // A
    { 0x7F, 0x49, 0x49, 0x49, 0x36 }, // B
    { 0x3E, 0x41, 0x41, 0x41, 0x22 }, // C
    { 0x7F, 0x41, 0x41, 0x22, 0x1C }, // D
    { 0x7F, 0x49, 0x49, 0x49, 0x41 }, // E
    { 0x7F, 0x09, 0x09, 0x09, 0x01 }, // F
    { 0x3E, 0x41, 0x49, 0x49, 0x7A }, // G
    { 0x7F, 0x08, 0x08, 0x08, 0x7F }, // H
    { 0x00, 0x41, 0x7F, 0x41, 0x00 }, // I
    { 0x20, 0x40, 0x41, 0x3F, 0x01 }, // J
    { 0x7F, 0x08, 0x14, 0x22, 0x41 }, // K
    { 0x7F, 0x40, 0x40, 0x40, 0x40 }, // L
    { 0x7F, 0x02, 0x0C, 0x02, 0x7F }, // M
    { 0x7F, 0x04, 0x08, 0x10, 0x7F }, // N
    { 0x3E, 0x41, 0x41, 0x41, 0x3E }, // O
    { 0x7F, 0x09, 0x09, 0x09, 0x06 }, // P
    { 0x3E, 0x41, 0x51, 0x21, 0x5E }, // Q
    { 0x7F, 0x09, 0x19, 0x29, 0x46 }, // R
    { 0x46, 0x49, 0x49, 0x49, 0x31 }, // S
    { 0x01, 0x01, 0x7F, 0x01, 0x01 }, // T
    { 0x3F, 0x40, 0x40, 0x40, 0x3F }, // U
    { 0x1F, 0x20, 0x40, 0x20, 0x1F }, // V
    { 0x3F, 0x40, 0x38, 0x40, 0x3F }, // W
    { 0x63, 0x14, 0x08, 0x14, 0x63 }, // X
    { 0x07, 0x08, 0x70, 0x08, 0x07 }, // Y
    { 0x61, 0x51, 0x49, 0x45, 0x43 }  // Z
};

void gba_graphics_init(void) {
    // Mode 4 (240x160 8-bit double-buffered), BG2 enabled
    REG_DISPCNT = MODE_4 | BG2_ENABLE;
    s_current_page = false;

    // Load Master GBA Arcade Palette
    MEM_PALETTE_BG[COL_TRANSPARENT]     = RGB15(0, 0, 0);

    // Sunset Sky & Mountains
    MEM_PALETTE_BG[COL_SKY_INDIGO]       = RGB8(42, 28, 72);
    MEM_PALETTE_BG[COL_SKY_PURPLE]       = RGB8(78, 38, 92);
    MEM_PALETTE_BG[COL_SKY_CRIMSON]      = RGB8(148, 48, 82);
    MEM_PALETTE_BG[COL_SKY_AMBER]        = RGB8(218, 95, 48);
    MEM_PALETTE_BG[COL_SKY_GOLD]         = RGB8(248, 165, 52);
    MEM_PALETTE_BG[COL_SKY_YELLOW]       = RGB8(255, 218, 92);
    MEM_PALETTE_BG[COL_SUN_WHITE]        = RGB8(255, 255, 240);
    MEM_PALETTE_BG[COL_SUN_GOLD]         = RGB8(255, 235, 110);
    MEM_PALETTE_BG[COL_MTN_PURPLE]       = RGB8(62, 38, 55);
    MEM_PALETTE_BG[COL_MTN_UMBER]        = RGB8(96, 64, 72);
    MEM_PALETTE_BG[COL_MTN_DARK]         = RGB8(48, 30, 42);
    MEM_PALETTE_BG[COL_DOVE_WHITE]       = RGB8(255, 250, 240);
    MEM_PALETTE_BG[COL_DOVE_SHADOW]      = RGB8(195, 195, 210);

    // Temple of David & Jerusalem Stone
    MEM_PALETTE_BG[COL_STONE_LIGHT]      = RGB8(235, 222, 195);
    MEM_PALETTE_BG[COL_STONE_MID]        = RGB8(198, 185, 160);
    MEM_PALETTE_BG[COL_STONE_DARK]       = RGB8(145, 130, 110);
    MEM_PALETTE_BG[COL_STONE_SHADOW]     = RGB8(95, 82, 70);
    MEM_PALETTE_BG[COL_BRONZE_LIGHT]     = RGB8(255, 225, 110);
    MEM_PALETTE_BG[COL_BRONZE_MID]       = RGB8(205, 150, 60);
    MEM_PALETTE_BG[COL_BRONZE_DARK]      = RGB8(125, 80, 25);
    MEM_PALETTE_BG[COL_GOLD_BRIGHT]      = RGB8(255, 235, 110);
    MEM_PALETTE_BG[COL_GOLD_MID]         = RGB8(238, 195, 45);
    MEM_PALETTE_BG[COL_GOLD_DARK]        = RGB8(175, 125, 20);
    MEM_PALETTE_BG[COL_WOOD_LIGHT]       = RGB8(175, 120, 60);
    MEM_PALETTE_BG[COL_WOOD_MID]         = RGB8(125, 80, 38);
    MEM_PALETTE_BG[COL_WOOD_DARK]        = RGB8(75, 45, 20);
    MEM_PALETTE_BG[COL_BANNER_BLUE]      = RGB8(35, 75, 165);
    MEM_PALETTE_BG[COL_BANNER_GOLD]      = RGB8(245, 215, 65);
    MEM_PALETTE_BG[COL_ROOF_TERRACOTTA]  = RGB8(185, 82, 45);
    MEM_PALETTE_BG[COL_VILLA_WALL]       = RGB8(218, 205, 180);
    MEM_PALETTE_BG[COL_CYPRESS_GREEN]    = RGB8(28, 55, 28);
    MEM_PALETTE_BG[COL_OLIVE_LEAF]       = RGB8(58, 92, 52);
    MEM_PALETTE_BG[COL_WINDOW_GOLD]      = RGB8(255, 210, 75);

    // Road, Cobblestones, Barricades
    MEM_PALETTE_BG[COL_ROAD_LIGHT]       = RGB8(165, 155, 140);
    MEM_PALETTE_BG[COL_ROAD_MID]         = RGB8(125, 115, 102);
    MEM_PALETTE_BG[COL_ROAD_DARK]        = RGB8(75, 68, 60);
    MEM_PALETTE_BG[COL_CRATE_LIGHT]      = RGB8(170, 115, 60);
    MEM_PALETTE_BG[COL_CRATE_MID]        = RGB8(125, 80, 38);
    MEM_PALETTE_BG[COL_CRATE_DARK]       = RGB8(72, 42, 18);
    MEM_PALETTE_BG[COL_SHADOW_SOFT]      = RGB8(45, 40, 52);
    MEM_PALETTE_BG[COL_SHADOW_CORE]      = RGB8(22, 18, 28);
    MEM_PALETTE_BG[COL_IRON_GATE]        = RGB8(85, 95, 110);
    MEM_PALETTE_BG[COL_GATE_GLOW]        = RGB8(255, 220, 100);

    // Common Character Tones
    MEM_PALETTE_BG[COL_OUTLINE]          = RGB8(24, 16, 12);
    MEM_PALETTE_BG[COL_SKIN_HI]          = RGB8(255, 225, 190);
    MEM_PALETTE_BG[COL_SKIN_MID]         = RGB8(230, 168, 120);
    MEM_PALETTE_BG[COL_SKIN_FLUSH]       = RGB8(210, 138, 95);
    MEM_PALETTE_BG[COL_SKIN_DARK]        = RGB8(165, 105, 68);
    MEM_PALETTE_BG[COL_WHITE_PURE]       = RGB8(255, 255, 255);
    MEM_PALETTE_BG[COL_WHITE_SHADOW]     = RGB8(195, 202, 214);
    MEM_PALETTE_BG[COL_WHITE_DEEP]       = RGB8(135, 142, 158);
    MEM_PALETTE_BG[COL_BLOOD_RED]        = RGB8(220, 40, 50);
    MEM_PALETTE_BG[COL_SPARK_GOLD]       = RGB8(255, 240, 120);

    // Jesus Nazarene
    MEM_PALETTE_BG[COL_HAIR_HI]          = RGB8(165, 110, 60);
    MEM_PALETTE_BG[COL_HAIR_MID]         = RGB8(105, 58, 28);
    MEM_PALETTE_BG[COL_HAIR_DARK]        = RGB8(48, 24, 12);
    MEM_PALETTE_BG[COL_ROBE_WHITE]       = RGB8(248, 248, 244);
    MEM_PALETTE_BG[COL_ROBE_MID]         = RGB8(195, 202, 214);
    MEM_PALETTE_BG[COL_ROBE_DARK]        = RGB8(135, 142, 158);
    MEM_PALETTE_BG[COL_ROBE_DEEP]        = RGB8(85, 92, 105);
    MEM_PALETTE_BG[COL_SASH_HI]          = RGB8(245, 65, 75);
    MEM_PALETTE_BG[COL_SASH_MID]         = RGB8(185, 22, 32);
    MEM_PALETTE_BG[COL_SASH_DARK]        = RGB8(110, 12, 20);
    MEM_PALETTE_BG[COL_TZITZIT_BLUE]     = RGB8(45, 95, 185);

    // Roman Armor & Enemies
    MEM_PALETTE_BG[COL_STEEL_HI]         = RGB8(240, 245, 255);
    MEM_PALETTE_BG[COL_STEEL_MID]        = RGB8(175, 185, 200);
    MEM_PALETTE_BG[COL_STEEL_DARK]       = RGB8(95, 105, 120);
    MEM_PALETTE_BG[COL_CREST_HI]         = RGB8(255, 85, 95);
    MEM_PALETTE_BG[COL_CREST_MID]        = RGB8(235, 32, 42);
    MEM_PALETTE_BG[COL_CREST_DARK]       = RGB8(145, 15, 22);
    MEM_PALETTE_BG[COL_TUNIC_RED]        = RGB8(185, 25, 32);
    MEM_PALETTE_BG[COL_SCUTUM_RED]       = RGB8(190, 24, 30);
    MEM_PALETTE_BG[COL_SCUTUM_DARK]      = RGB8(120, 14, 20);
    MEM_PALETTE_BG[COL_LAUREL_GREEN]     = RGB8(48, 120, 52);
    MEM_PALETTE_BG[COL_LAUREL_HI]        = RGB8(110, 190, 95);
    MEM_PALETTE_BG[COL_ARCHER_GREEN]     = RGB8(52, 88, 52);
    MEM_PALETTE_BG[COL_ARCHER_DARK]      = RGB8(32, 56, 32);
    MEM_PALETTE_BG[COL_LEATHER_HI]       = RGB8(150, 100, 55);
    MEM_PALETTE_BG[COL_LEATHER_MID]      = RGB8(105, 65, 32);
    MEM_PALETTE_BG[COL_LEATHER_DARK]     = RGB8(62, 36, 16);
    MEM_PALETTE_BG[COL_RUBY_EYE]         = RGB8(255, 30, 40);
    MEM_PALETTE_BG[COL_ASH_CHARCOAL]     = RGB8(42, 36, 44);
    MEM_PALETTE_BG[COL_ASH_EMBER]        = RGB8(255, 95, 25);
    MEM_PALETTE_BG[COL_ASH_GOLD]         = RGB8(255, 215, 70);

    // UI & Text
    MEM_PALETTE_BG[COL_TEXT_WHITE]       = RGB8(255, 255, 255);
    MEM_PALETTE_BG[COL_TEXT_GOLD]        = RGB8(245, 210, 60);
    MEM_PALETTE_BG[COL_TEXT_RED]         = RGB8(235, 45, 55);
    MEM_PALETTE_BG[COL_TEXT_DARK]        = RGB8(28, 24, 32);
    MEM_PALETTE_BG[COL_HEART_RED]        = RGB8(245, 35, 45);
    MEM_PALETTE_BG[COL_HEART_BORDER]     = RGB8(140, 15, 25);
    MEM_PALETTE_BG[COL_BASKET_WICKER]    = RGB8(195, 140, 65);
    MEM_PALETTE_BG[COL_BASKET_BREAD]     = RGB8(230, 185, 115);
    MEM_PALETTE_BG[COL_BASKET_FISH]      = RGB8(120, 175, 215);
}

void gba_graphics_flip(void) {
    gba_wait_vblank();

    volatile uint16_t* vram_dest = s_current_page ? MEM_VRAM_PAGE0 : MEM_VRAM_PAGE1;

    // High-speed 32-bit DMA copy of 9,600 words (38,400 bytes) from EWRAM to VRAM
    REG_DMA3SAD = (uint32_t)&gba_screen[0][0];
    REG_DMA3DAD = (uint32_t)vram_dest;
    REG_DMA3CNT = (9600) | DMA_32BIT | DMA_ENABLE | DMA_TIMING_IMM;

    // Flip display page
    if (s_current_page) {
        REG_DISPCNT &= ~DISPCNT_PAGE_1;
    } else {
        REG_DISPCNT |= DISPCNT_PAGE_1;
    }
    s_current_page = !s_current_page;
}

void gba_graphics_clear(uint8_t color) {
    memset(gba_screen, color, sizeof(gba_screen));
}

void gba_set_pixel(int x, int y, uint8_t color) {
    if (x >= 0 && x < GBA_SCREEN_WIDTH && y >= 0 && y < GBA_SCREEN_HEIGHT) {
        gba_screen[y][x] = color;
    }
}

void gba_draw_rect(int x, int y, int w, int h, uint8_t color) {
    if (x >= GBA_SCREEN_WIDTH || y >= GBA_SCREEN_HEIGHT) return;
    int x0 = x < 0 ? 0 : x;
    int y0 = y < 0 ? 0 : y;
    int x1 = (x + w > GBA_SCREEN_WIDTH) ? GBA_SCREEN_WIDTH : (x + w);
    int y1 = (y + h > GBA_SCREEN_HEIGHT) ? GBA_SCREEN_HEIGHT : (y + h);

    for (int j = y0; j < y1; ++j) {
        memset(&gba_screen[j][x0], color, x1 - x0);
    }
}

void gba_draw_rect_outline(int x, int y, int w, int h, uint8_t color) {
    gba_draw_rect(x, y, w, 1, color);
    gba_draw_rect(x, y + h - 1, w, 1, color);
    gba_draw_rect(x, y, 1, h, color);
    gba_draw_rect(x + w - 1, y, 1, h, color);
}

void gba_draw_line(int x0, int y0, int x1, int y1, uint8_t color) {
    int dx = (x1 >= x0) ? (x1 - x0) : (x0 - x1);
    int sx = (x0 < x1) ? 1 : -1;
    int dy = (y1 >= y0) ? (y0 - y1) : (y1 - y0);
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx + dy;
    int cx = x0, cy = y0;

    while (1) {
        gba_set_pixel(cx, cy, color);
        if (cx == x1 && cy == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; cx += sx; }
        if (e2 <= dx) { err += dx; cy += sy; }
    }
}

void gba_draw_drop_shadow(int cx, int cy, int rx, int ry) {
    for (int dy = -ry; dy <= ry; ++dy) {
        int y = cy + dy;
        if (y < 0 || y >= GBA_SCREEN_HEIGHT) continue;
        int term = ry * ry - dy * dy;
        if (term < 0) continue;
        int dxMax = (rx * term) / (ry * ry);
        for (int dx = -dxMax; dx <= dxMax; ++dx) {
            int x = cx + dx;
            if (x >= 0 && x < GBA_SCREEN_WIDTH) {
                // Soft shadow dot pattern for authentic translucent ambient occlusion
                if ((dx * dx * ry * ry + dy * dy * rx * rx) <= (rx * rx * ry * ry / 2)) {
                    gba_screen[y][x] = COL_SHADOW_CORE;
                } else if ((x + y) % 2 == 0) {
                    gba_screen[y][x] = COL_SHADOW_SOFT;
                }
            }
        }
    }
}

void gba_draw_char(char c, int x, int y, uint8_t color) {
    if (c < 32 || c > 90) {
        if (c >= 'a' && c <= 'z') c -= 32; // Uppercase
        else return;
    }
    int idx = c - 32;
    for (int col = 0; col < 5; ++col) {
        uint8_t line = s_font5x7[idx][col];
        for (int row = 0; row < 7; ++row) {
            if (line & (1 << row)) {
                gba_set_pixel(x + col, y + row, color);
            }
        }
    }
}

void gba_draw_string(const char* str, int x, int y, uint8_t color) {
    // Drop shadow
    const char* p = str;
    int cx = x;
    while (*p) {
        gba_draw_char(*p, cx + 1, y + 1, COL_TEXT_DARK);
        gba_draw_char(*p, cx, y, color);
        cx += 6;
        p++;
    }
}

void gba_draw_number(int num, int x, int y, uint8_t color) {
    char buf[12];
    int idx = 0;
    if (num == 0) {
        buf[idx++] = '0';
    } else {
        int temp = num;
        char rev[12];
        int rIdx = 0;
        while (temp > 0) {
            rev[rIdx++] = '0' + (temp % 10);
            temp /= 10;
        }
        while (rIdx > 0) {
            buf[idx++] = rev[--rIdx];
        }
    }
    buf[idx] = '\0';
    gba_draw_string(buf, x, y, color);
}

// ---------------------------------------------------------------------------
// 4-LAYER PARALLAX BACKGROUND (Judean Sunset, Mountains, Temple of David, Villas)
// ---------------------------------------------------------------------------
void gba_draw_parallax_background(int cameraX, int timer) {
    // 1. SKY GRADIENT (Sunset Judean Dusk)
    for (int y = 0; y < 100; ++y) {
        uint8_t skyCol = COL_SKY_INDIGO;
        if (y >= 80) skyCol = COL_SKY_YELLOW;
        else if (y >= 64) skyCol = COL_SKY_GOLD;
        else if (y >= 48) skyCol = COL_SKY_AMBER;
        else if (y >= 32) skyCol = COL_SKY_CRIMSON;
        else if (y >= 16) skyCol = COL_SKY_PURPLE;
        memset(&gba_screen[y][0], skyCol, GBA_SCREEN_WIDTH);
    }

    // Blazing low sun (Sun center at x = 120 - cameraX * 0.03, y = 56)
    int sunX = 140 - (cameraX >> 5);
    int sunY = 54;
    for (int dy = -16; dy <= 16; ++dy) {
        int sy = sunY + dy;
        if (sy < 0 || sy >= 100) continue;
        for (int dx = -16; dx <= 16; ++dx) {
            int sx = sunX + dx;
            if (sx < 0 || sx >= GBA_SCREEN_WIDTH) continue;
            int distSq = dx * dx + dy * dy;
            if (distSq <= 49) {
                gba_screen[sy][sx] = COL_SUN_WHITE;
            } else if (distSq <= 120) {
                gba_screen[sy][sx] = COL_SUN_GOLD;
            } else if (distSq <= 240 && (sx + sy) % 2 == 0) {
                gba_screen[sy][sx] = COL_SKY_YELLOW;
            }
        }
    }

    // Distant Mountains of Moab (0.05x scroll)
    int mtnScroll = (cameraX * 5) / 100;
    for (int x = 0; x < GBA_SCREEN_WIDTH; ++x) {
        int wx = x + mtnScroll;
        // Mountain crests
        int my1 = 66 + ((wx * 7 + 13) % 19) / 3;
        int my2 = 78 + ((wx * 11 + 5) % 17) / 3;
        for (int y = my1; y < 100; ++y) {
            if (y >= my2) gba_screen[y][x] = COL_MTN_UMBER;
            else gba_screen[y][x] = COL_MTN_PURPLE;
        }
    }

    // White Doves soaring
    int doveX = (160 - (cameraX >> 4) + (timer >> 3)) % 280 - 20;
    if (doveX >= 0 && doveX < GBA_SCREEN_WIDTH - 8) {
        gba_screen[34][doveX + 2] = COL_DOVE_WHITE;
        gba_screen[35][doveX + 1] = COL_DOVE_WHITE;
        gba_screen[35][doveX + 3] = COL_DOVE_WHITE;
        gba_screen[36][doveX + 0] = COL_DOVE_WHITE;
        gba_screen[36][doveX + 4] = COL_DOVE_WHITE;
    }

    // -----------------------------------------------------------------------
    // LAYER 2: THE MONUMENTAL TEMPLE & CITADEL OF DAVID (MIGDAL DAVID)
    // Scrolling at 0.20x speed across screens 3-4 (x = 300 to 750)
    // 4 Architectural Tiers:
    // Tier 1: Stepped Stone Structure Bedrock Megaliths (y = 86 to 118)
    // Tier 2: Fortified Royal Terrace & Zion Gate (y = 66 to 85)
    // Tier 3: Palace of King David (Beit David) with Proto-Aeolic Colonnade (y = 44 to 65)
    // Tier 4: Citadel Keep - Tower of David with Bronze Cupola (y = 16 to 43)
    // -----------------------------------------------------------------------
    int davidScroll = (cameraX * 20) / 100;

    for (int sx = 0; sx < GBA_SCREEN_WIDTH; ++sx) {
        int worldX = sx + davidScroll;
        // Temple of David Panorama occupies worldX = 260 to 660 (400px wide)
        if (worldX >= 260 && worldX <= 660) {
            int dx = worldX - 260; // 0 to 400

            // Tier 1: Stepped Stone Bedrock Foundation (y = 88 to 118)
            int stepSlope = 88 + (dx < 200 ? (200 - dx) / 12 : (dx - 200) / 14);
            for (int y = stepSlope; y <= 118; ++y) {
                // Drafted ashlar blocks
                uint8_t stoneCol = ((dx % 18 == 0) || (y % 6 == 0)) ? COL_STONE_DARK : COL_STONE_MID;
                if ((y % 6 == 1) && (dx % 18 != 0)) stoneCol = COL_STONE_LIGHT;
                gba_screen[y][sx] = stoneCol;
            }

            // Tier 2: Fortified Royal Terrace & Gate of Zion (dx = 60 to 340, y = 68 to 87)
            if (dx >= 60 && dx <= 340) {
                for (int y = 68; y <= 87; ++y) {
                    bool crenel = (y <= 71 && (dx % 10 >= 6));
                    if (!crenel) {
                        uint8_t c = (dx % 14 == 0 || y % 5 == 0) ? COL_STONE_DARK : COL_STONE_MID;
                        gba_screen[y][sx] = c;
                    }
                }
                // Zion Gate archway at center (dx = 190 to 210, y = 74 to 87)
                if (dx >= 190 && dx <= 210 && worldX >= 260) {
                    for (int y = 74; y <= 87; ++y) {
                        gba_screen[y][sx] = (y <= 75 || dx == 190 || dx == 210) ? COL_BRONZE_MID : COL_STONE_SHADOW;
                    }
                }
            }

            // Tier 3: Palace of King David (Beit David) (dx = 110 to 290, y = 46 to 67)
            if (dx >= 110 && dx <= 290) {
                for (int y = 46; y <= 67; ++y) {
                    // Proto-Aeolic Fluted Columns (dx % 24 < 4)
                    bool pillar = (dx % 24 < 4);
                    // Glowing Clerestory Arched Windows (dx % 24 >= 10 && dx % 24 <= 14 && y >= 52 && y <= 60)
                    bool window = (dx % 24 >= 10 && dx % 24 <= 14 && y >= 52 && y <= 60);
                    // Royal Davidic Blue/Gold Standard (dx == 145 || dx == 255)
                    bool banner = ((dx >= 142 && dx <= 148) || (dx >= 252 && dx <= 258)) && (y >= 48 && y <= 64);

                    if (banner) {
                        gba_screen[y][sx] = (y % 4 == 0) ? COL_BANNER_GOLD : COL_BANNER_BLUE;
                    } else if (window) {
                        gba_screen[y][sx] = COL_WINDOW_GOLD;
                    } else if (pillar) {
                        gba_screen[y][sx] = (y == 46 || y == 47) ? COL_STONE_LIGHT : COL_WHITE_PURE;
                    } else {
                        gba_screen[y][sx] = (y == 46) ? COL_ROOF_TERRACOTTA : COL_STONE_MID;
                    }
                }
            }

            // Tier 4: Citadel Keep - Tower of David (Migdal David) (dx = 175 to 225, y = 18 to 45)
            if (dx >= 175 && dx <= 225) {
                // Mast & Pennant (dx = 200, y = 10 to 18)
                if (dx == 200) {
                    for (int y = 10; y <= 18; ++y) {
                        gba_screen[y][sx] = COL_WOOD_MID;
                    }
                }
                if (dx >= 201 && dx <= 212) {
                    for (int y = 11; y <= 14; ++y) {
                        gba_screen[y][sx] = COL_BANNER_GOLD;
                    }
                }
                // Gleaming Bronze Cupola Dome (dx = 188 to 212, y = 18 to 26)
                if (dx >= 188 && dx <= 212) {
                    int domeR = 12;
                    int ddx = dx - 200;
                    for (int y = 18; y <= 26; ++y) {
                        int ddy = y - 26;
                        if (ddx * ddx + ddy * ddy <= domeR * domeR) {
                            gba_screen[y][sx] = (ddx <= -3 && ddy <= -3) ? COL_BRONZE_LIGHT : COL_BRONZE_MID;
                        }
                    }
                }
                // Tower Keep walls (dx = 178 to 222, y = 27 to 45)
                for (int y = 27; y <= 45; ++y) {
                    bool crenel = (y <= 30 && (dx % 8 >= 5));
                    if (!crenel) {
                        uint8_t c = (dx == 178 || dx == 222 || y == 31) ? COL_STONE_DARK : COL_STONE_LIGHT;
                        if (dx >= 197 && dx <= 203 && y >= 34 && y <= 40) c = COL_STONE_SHADOW; // Arrow slit
                        gba_screen[y][sx] = c;
                    }
                }
            }
        }
    }

    // -----------------------------------------------------------------------
    // LAYER 3: MIDGROUND JERUSALEM VILLAS & CYPRESS TREES (0.45x scroll)
    // -----------------------------------------------------------------------
    int villaScroll = (cameraX * 45) / 100;
    for (int sx = 0; sx < GBA_SCREEN_WIDTH; ++sx) {
        int vx = (sx + villaScroll) % 180;
        // Terraced residential villa (vx = 20 to 65, y = 92 to 125)
        if (vx >= 20 && vx <= 65) {
            for (int y = 92; y <= 125; ++y) {
                if (y == 92 || y == 93) gba_screen[y][sx] = COL_ROOF_TERRACOTTA;
                else if (vx >= 35 && vx <= 45 && y >= 102 && y <= 112) gba_screen[y][sx] = COL_WINDOW_GOLD;
                else gba_screen[y][sx] = COL_VILLA_WALL;
            }
        }
        // Italian Cypress Trees (vx = 80 to 88, y = 84 to 125)
        if (vx >= 80 && vx <= 88) {
            int cx = vx - 84;
            for (int y = 84; y <= 125; ++y) {
                int maxW = (y - 84) / 7 + 1;
                if (cx >= -maxW && cx <= maxW) {
                    gba_screen[y][sx] = (cx <= 0) ? COL_CYPRESS_GREEN : COL_OLIVE_LEAF;
                }
            }
        }
    }

    // -----------------------------------------------------------------------
    // LAYER 4: NEAR STREET COBBLESTONES & LEVEL ARCHITECTURE (World Space)
    // Paved Roman Road (y = 136 to 160)
    // -----------------------------------------------------------------------
    for (int y = 136; y < GBA_SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < GBA_SCREEN_WIDTH; ++x) {
            int wx = x + cameraX;
            if (y == 136) {
                gba_screen[y][x] = COL_STONE_LIGHT; // Curbstone rim
            } else if ((wx % 16 == 0) || (y % 8 == 0)) {
                gba_screen[y][x] = COL_ROAD_DARK;  // Cobblestone mortar
            } else {
                gba_screen[y][x] = ((wx + y) % 5 == 0) ? COL_ROAD_LIGHT : COL_ROAD_MID;
            }
        }
    }
}

// ---------------------------------------------------------------------------
// CHARACTER & SPRITE DRAWING
// ---------------------------------------------------------------------------

void gba_draw_player(int x, int y, bool facingLeft, int state, int animFrame, bool hurt) {
    // State: 0=idle, 1=walk, 2=crouch, 3=jump, 4=somersault, 5=throw, 6=melee, 7=hurt, 8=lion, 9=ascend
    (void)facingLeft; // Symmetry helper can be expanded

    if (state == 8) {
        // LION OF JUDAH TRANSFORMATION (Glowing Golden Beast)
        gba_draw_drop_shadow(x + 12, y + 26, 14, 4);
        // Golden lion body & radiant mane
        gba_draw_rect(x + 2, y + 8, 20, 14, COL_GOLD_BRIGHT);
        gba_draw_rect(x + 6, y + 4, 14, 10, COL_GOLD_MID);
        gba_draw_rect(x + 16, y + 2, 8, 8, COL_SPARK_GOLD); // Radiant head
        gba_draw_rect(x + 21, y + 5, 2, 2, COL_RUBY_EYE);    // Ruby eyes
        // Paws
        gba_draw_rect(x + 3, y + 22, 5, 4, COL_GOLD_DARK);
        gba_draw_rect(x + 15, y + 22, 5, 4, COL_GOLD_DARK);
        return;
    }

    int headY = (state == 2) ? (y + 10) : (y + 2);
    int feetY = y + 26;

    // Ground Drop Shadow (omitted during holy ascension)
    if (state != 9) {
        gba_draw_drop_shadow(x + 10, feetY + 1, 10, 3);
    }

    // Divine Golden Halo around head
    gba_draw_rect(x + 6, headY - 2, 8, 2, COL_SPARK_GOLD);
    gba_draw_rect(x + 4, headY - 1, 12, 1, COL_GOLD_MID);

    // Head & Hair
    gba_draw_rect(x + 7, headY, 6, 6, COL_HAIR_MID);
    gba_draw_rect(x + 8, headY - 1, 4, 1, COL_SPARK_GOLD); // Golden celestial rim light
    // Flowing hair locks
    gba_draw_rect(x + 5, headY + 1, 2, 7, COL_HAIR_DARK);

    // Face Profile
    gba_draw_rect(x + 9, headY + 1, 4, 4, hurt ? COL_BLOOD_RED : COL_SKIN_MID);
    gba_set_pixel(x + 11, headY + 2, COL_WHITE_PURE); // Sclera
    gba_set_pixel(x + 12, headY + 2, COL_OUTLINE);    // Pupil
    gba_set_pixel(x + 11, headY + 1, COL_WHITE_PURE); // Catchlight
    gba_set_pixel(x + 13, headY + 3, COL_SKIN_HI);    // Nose
    gba_draw_rect(x + 9, headY + 5, 4, 3, COL_HAIR_DARK); // Full wavy beard

    if (state != 2) {
        // STANDING / WALKING / ATTACKING BODY
        int walkOff = (animFrame == 1) ? -1 : ((animFrame == 3) ? 1 : 0);

        // White Linen Robes with 5-tone Drapery
        for (int ry = headY + 8; ry <= feetY - 4; ++ry) {
            int flare = (ry - (headY + 8)) / 4;
            int xL = x + 6 - flare + walkOff;
            int xR = x + 13 + flare - walkOff;
            for (int rx = xL; rx <= xR; ++rx) {
                uint8_t c = COL_ROBE_WHITE;
                if (rx == xL || rx == xL + 1) c = COL_ROBE_DARK;
                else if (rx == x + 9 && ry > headY + 12) c = COL_ROBE_DEEP; // Ambient pleat
                else if (rx == x + 11) c = COL_WHITE_PURE;
                gba_set_pixel(rx, ry, hurt ? COL_BLOOD_RED : c);
            }
        }

        // Biblical Tzitzit Tassels at hem corners (white & tekhelet blue threads)
        gba_set_pixel(x + 4, feetY - 3, COL_WHITE_PURE);
        gba_set_pixel(x + 4, feetY - 2, COL_TZITZIT_BLUE);
        gba_set_pixel(x + 4, feetY - 1, COL_WHITE_PURE);
        gba_set_pixel(x + 15, feetY - 3, COL_WHITE_PURE);
        gba_set_pixel(x + 15, feetY - 2, COL_TZITZIT_BLUE);
        gba_set_pixel(x + 15, feetY - 1, COL_WHITE_PURE);

        // Royal Crimson Himation / Sash across chest with gold trim
        for (int sy = headY + 8; sy <= feetY - 6; ++sy) {
            int sx = x + 12 - (sy - (headY + 8));
            if (sx >= x + 6 && sx <= x + 14) {
                gba_set_pixel(sx, sy, COL_SASH_HI);
                gba_set_pixel(sx + 1, sy, COL_SASH_MID);
                gba_set_pixel(sx + 2, sy, COL_BANNER_GOLD); // Gold embroidered border
            }
        }

        // Arms & Attacks
        if (state == 5) {
            // Ranged Spike Toss: Arm forward + Stigmata spark
            gba_draw_rect(x + 12, headY + 8, 6, 2, COL_SKIN_MID);
            gba_set_pixel(x + 17, headY + 8, COL_BLOOD_RED);  // Stigmata
            gba_set_pixel(x + 18, headY + 8, COL_SPARK_GOLD); // Holy spark
        } else if (state == 6) {
            // Melee Strike: Olive Wood Quarterstaff thrust + radiant palm
            gba_draw_rect(x + 12, headY + 8, 6, 3, COL_SKIN_MID);
            gba_set_pixel(x + 17, headY + 8, COL_SPARK_GOLD);
            // Olive staff
            gba_draw_rect(x + 16, headY, 2, 22, COL_WOOD_MID);
            gba_set_pixel(x + 16, headY - 1, COL_GOLD_BRIGHT); // Brass ferrule
            gba_set_pixel(x + 16, headY + 22, COL_GOLD_BRIGHT);
        } else {
            // Relaxed arm with stigmata wrist mark
            gba_draw_rect(x + 7, headY + 9, 2, 4, COL_ROBE_MID);
            gba_set_pixel(x + 8, headY + 13, COL_BLOOD_RED); // Stigmata on wrist
        }

        // Legs & Sandals
        gba_draw_rect(x + 7 + walkOff, feetY - 3, 2, 3, COL_SKIN_MID);
        gba_draw_rect(x + 11 - walkOff, feetY - 3, 2, 3, COL_SKIN_MID);
        gba_draw_rect(x + 6 + walkOff, feetY, 4, 1, COL_LEATHER_DARK);
        gba_draw_rect(x + 10 - walkOff, feetY, 4, 1, COL_LEATHER_DARK);
    } else {
        // CROUCHED POSE
        gba_draw_rect(x + 6, headY + 8, 10, 8, hurt ? COL_BLOOD_RED : COL_ROBE_WHITE);
        gba_draw_rect(x + 7, headY + 9, 8, 3, COL_SASH_MID);
        gba_draw_rect(x + 5, feetY - 1, 12, 2, COL_LEATHER_DARK);
        if (state == 5) {
            gba_draw_rect(x + 13, headY + 10, 5, 2, COL_SKIN_MID);
            gba_set_pixel(x + 17, headY + 10, COL_SPARK_GOLD);
        }
    }

    if (state == 9) {
        // ASCENSION: Golden light pillar enveloping Jesus
        gba_draw_rect(x + 2, 0, 16, y + 26, COL_SPARK_GOLD);
    }
}

void gba_draw_guard(int x, int y, bool facingLeft, bool attack, int animFrame, bool hurt) {
    (void)facingLeft;
    int feetY = y + 26;
    gba_draw_drop_shadow(x + 10, feetY + 1, 9, 3);

    // Galea Helmet with Flowing Scarlet Horsehair Crest
    gba_draw_rect(x + 6, y + 1, 9, 2, COL_CREST_HI);
    gba_draw_rect(x + 7, y + 2, 7, 1, COL_CREST_MID);
    // Bronze Helmet Bowl & Brow Band
    gba_draw_rect(x + 7, y + 3, 7, 3, COL_BRONZE_MID);
    gba_set_pixel(x + 9, y + 3, COL_STEEL_HI); // Specular gleam
    // Soldier Face
    gba_draw_rect(x + 8, y + 6, 5, 3, hurt ? COL_BLOOD_RED : COL_SKIN_MID);
    gba_set_pixel(x + 11, y + 6, COL_OUTLINE); // Eye

    // Lorica Segmentata (Overlapping Steel Plates)
    gba_draw_rect(x + 6, y + 9, 9, 8, hurt ? COL_BLOOD_RED : COL_STEEL_MID);
    gba_set_pixel(x + 7, y + 10, COL_STEEL_HI); // Specular lame glint
    gba_set_pixel(x + 12, y + 10, COL_STEEL_HI);
    // Bronze Belt & Hanging Pteruges
    gba_draw_rect(x + 6, y + 17, 9, 1, COL_GOLD_MID);
    gba_draw_rect(x + 7, y + 18, 7, 3, COL_LEATHER_MID);
    gba_draw_rect(x + 6, y + 19, 9, 2, COL_TUNIC_RED);

    if (attack) {
        // Pilum Spear Thrust
        gba_draw_rect(x + 11, y + 11, 14, 2, COL_WOOD_MID);
        gba_draw_rect(x + 24, y + 10, 4, 3, COL_STEEL_HI); // Spearhead
    } else {
        // Upright Pilum Spear
        gba_draw_rect(x + 14, y - 2, 2, 28, COL_WOOD_MID);
        gba_draw_rect(x + 13, y - 5, 3, 4, COL_STEEL_HI);
    }

    // Muscular Legs & Hobnailed Caligae
    int step = (animFrame == 1) ? 1 : -1;
    gba_draw_rect(x + 7 - step, y + 21, 2, 4, COL_SKIN_MID);
    gba_draw_rect(x + 11 + step, y + 21, 2, 4, COL_SKIN_MID);
    gba_draw_rect(x + 6 - step, y + 25, 4, 2, COL_LEATHER_DARK);
    gba_draw_rect(x + 10 + step, y + 25, 4, 2, COL_LEATHER_DARK);
    gba_set_pixel(x + 6 - step, y + 26, COL_STEEL_HI); // Hobnail
    gba_set_pixel(x + 10 + step, y + 26, COL_STEEL_HI);
}

void gba_draw_archer(int x, int y, bool facingLeft, bool duck, bool fire, bool hurt) {
    (void)facingLeft;
    int feetY = y + 26;

    if (duck) {
        // Ducked behind fortified wicker sandbag
        gba_draw_drop_shadow(x + 10, feetY + 1, 9, 3);
        // Peeking helmet head
        gba_draw_rect(x + 8, y + 10, 6, 4, COL_BRONZE_MID);
        gba_set_pixel(x + 11, y + 11, COL_OUTLINE); // Eye
        // Wicker sandbag
        gba_draw_rect(x + 6, y + 14, 12, 12, COL_STONE_DARK);
        gba_draw_rect_outline(x + 6, y + 14, 12, 12, COL_ROAD_DARK);
        return;
    }

    gba_draw_drop_shadow(x + 10, feetY + 1, 9, 3);

    // Conical Bronze Helmet
    gba_draw_rect(x + 8, y + 2, 5, 4, COL_BRONZE_MID);
    gba_set_pixel(x + 10, y + 1, COL_BRONZE_LIGHT); // Tip
    // Archer Face
    gba_draw_rect(x + 8, y + 6, 5, 3, hurt ? COL_BLOOD_RED : COL_SKIN_MID);
    gba_set_pixel(x + 11, y + 6, COL_OUTLINE);

    // Lorica Squamata (Scale Armor)
    gba_draw_rect(x + 7, y + 9, 7, 8, hurt ? COL_BLOOD_RED : COL_LEATHER_MID);
    // Bronze scale reflections
    gba_set_pixel(x + 8, y + 11, COL_BRONZE_LIGHT);
    gba_set_pixel(x + 11, y + 13, COL_BRONZE_LIGHT);
    // Green Tunic
    gba_draw_rect(x + 7, y + 17, 7, 4, COL_ARCHER_GREEN);

    // Quiver with eagle feathers on back
    gba_draw_rect(x + 5, y + 6, 2, 10, COL_LEATHER_DARK);
    gba_set_pixel(x + 5, y + 5, COL_CREST_HI);
    gba_set_pixel(x + 6, y + 4, COL_WHITE_PURE);

    // Recurve Bow
    if (fire) {
        // Firing: Bow flexed + arrow
        gba_draw_line(x + 15, y + 4, x + 15, y + 20, COL_WOOD_MID);
        gba_draw_line(x + 9, y + 12, x + 15, y + 4, COL_WHITE_PURE); // String
        gba_draw_line(x + 9, y + 12, x + 15, y + 20, COL_WHITE_PURE);
        // Nocked Arrow
        gba_draw_rect(x + 10, y + 11, 10, 1, COL_WOOD_LIGHT);
        gba_set_pixel(x + 19, y + 11, COL_STEEL_HI); // Bodkin tip
    } else {
        // Vertical bow
        gba_draw_line(x + 14, y + 6, x + 14, y + 22, COL_WOOD_MID);
    }

    // Legs
    gba_draw_rect(x + 7, y + 21, 2, 4, COL_SKIN_MID);
    gba_draw_rect(x + 11, y + 21, 2, 4, COL_SKIN_MID);
    gba_draw_rect(x + 6, y + 25, 4, 2, COL_LEATHER_DARK);
    gba_draw_rect(x + 10, y + 25, 4, 2, COL_LEATHER_DARK);
}

void gba_draw_heavy(int x, int y, bool facingLeft, bool attack, int animFrame, bool hurt) {
    (void)facingLeft;
    int feetY = y + 26;
    gba_draw_drop_shadow(x + 11, feetY + 1, 11, 3);

    // Transverse Galea Crest
    gba_draw_rect(x + 6, y + 1, 11, 2, COL_CREST_HI);
    gba_draw_rect(x + 7, y + 3, 9, 3, COL_STEEL_MID);
    // Fierce Eyes peering over shield
    gba_set_pixel(x + 10, y + 5, COL_WHITE_PURE);
    gba_set_pixel(x + 11, y + 5, COL_OUTLINE);
    gba_set_pixel(x + 14, y + 5, COL_WHITE_PURE);
    gba_set_pixel(x + 15, y + 5, COL_OUTLINE);

    if (!attack) {
        // TOWER SCUTUM SHIELD (Curved Roman Masterpiece)
        gba_draw_rect(x + 8, y + 6, 14, 20, hurt ? COL_BLOOD_RED : COL_SCUTUM_RED);
        gba_draw_rect_outline(x + 8, y + 6, 14, 20, COL_GOLD_MID); // Brass rim
        // Central Bronze Umbo Boss
        gba_draw_rect(x + 13, y + 14, 4, 4, COL_BRONZE_MID);
        gba_set_pixel(x + 14, y + 15, COL_GOLD_BRIGHT); // 3D gleam
        // Golden Imperial Aquila Wings
        gba_draw_rect(x + 10, y + 10, 10, 1, COL_GOLD_BRIGHT);
        gba_draw_rect(x + 11, y + 11, 8, 1, COL_GOLD_BRIGHT);
        // SPQR Laurel Wreath
        gba_set_pixel(x + 11, y + 15, COL_LAUREL_HI);
        gba_set_pixel(x + 18, y + 15, COL_LAUREL_HI);
    } else {
        // Gladius Thrust: Shield pulled back, sword lunge!
        gba_draw_rect(x + 4, y + 8, 8, 17, COL_SCUTUM_RED);
        gba_draw_rect_outline(x + 4, y + 8, 8, 17, COL_GOLD_MID);
        // Arm & Gladius
        gba_draw_rect(x + 12, y + 12, 6, 2, COL_STEEL_MID);
        gba_draw_rect(x + 18, y + 12, 6, 2, COL_STEEL_HI);
        gba_set_pixel(x + 24, y + 12, COL_WHITE_PURE); // Sharp point!
    }

    // Greaved Legs
    int step = (animFrame == 1) ? 1 : 0;
    gba_draw_rect(x + 9 + step, y + 21, 3, 5, COL_BRONZE_MID);
    gba_draw_rect(x + 8 + step, y + 25, 4, 2, COL_STEEL_DARK);
}

void gba_draw_boss(int x, int y, bool facingLeft, int state, int animFrame, bool hurt) {
    (void)facingLeft;
    // States: 0=idle/walk, 1=windup, 2=crack, 3=slam, 4=kneel, 5=wither, 6=ash
    int feetY = y + 42;

    if (state == 6) {
        // Ash Heap & Dying Embers
        gba_draw_drop_shadow(x + 16, feetY, 14, 3);
        gba_draw_rect(x + 8, feetY - 2, 16, 3, COL_ASH_CHARCOAL);
        gba_set_pixel(x + 12, feetY - 2, COL_ASH_EMBER);
        gba_set_pixel(x + 16, feetY - 3, COL_ASH_GOLD);
        return;
    } else if (state == 5) {
        // Withering into Ash
        gba_draw_drop_shadow(x + 16, feetY, 14, 3);
        gba_draw_rect(x + 10, y + 18, 14, 22, COL_ASH_CHARCOAL);
        gba_set_pixel(x + 14, y + 24, COL_ASH_EMBER);
        gba_set_pixel(x + 18, y + 30, COL_ASH_GOLD);
        return;
    } else if (state == 4) {
        // Kneeling in Defeat
        gba_draw_drop_shadow(x + 16, feetY, 16, 4);
        int ky = y + 8;
        // Crest tilted down
        gba_draw_rect(x + 10, ky + 2, 14, 3, COL_CREST_HI);
        // Face down clutching wound
        gba_draw_rect(x + 12, ky + 6, 8, 6, COL_SKIN_MID);
        // Cuirass with penetrating wound
        gba_draw_rect(x + 9, ky + 12, 14, 12, COL_BRONZE_MID);
        gba_set_pixel(x + 16, ky + 16, COL_BLOOD_RED); // Penetration wound!
        // Dropped whip on cobblestones
        gba_draw_line(x + 22, feetY - 1, x + 30, feetY - 1, COL_LEATHER_MID);
        return;
    }

    gba_draw_drop_shadow(x + 16, feetY + 1, 16, 4);

    // Massive Transverse Scarlet Horsehair Crest
    gba_draw_rect(x + 8, y + 1, 18, 3, COL_CREST_HI);
    gba_draw_rect(x + 9, y + 3, 16, 2, COL_CREST_MID);
    // Polished Bronze Helmet Skullcap
    gba_draw_rect(x + 11, y + 5, 12, 5, COL_BRONZE_MID);
    gba_draw_rect(x + 12, y + 4, 10, 1, COL_BRONZE_LIGHT); // Gleam
    // Menacing Warrior Face
    gba_draw_rect(x + 12, y + 9, 10, 5, hurt ? COL_BLOOD_RED : COL_SKIN_MID);
    gba_set_pixel(x + 14, y + 10, COL_RUBY_EYE); // Glowing eye
    gba_set_pixel(x + 18, y + 10, COL_RUBY_EYE);

    // Sculpted Bronze Muscle Cuirass (Lorica Musculata)
    gba_draw_rect(x + 9, y + 14, 16, 14, hurt ? COL_BLOOD_RED : COL_BRONZE_MID);
    // Pectoral contours
    gba_draw_rect(x + 10, y + 16, 5, 4, COL_BRONZE_LIGHT);
    gba_draw_rect(x + 19, y + 16, 5, 4, COL_BRONZE_LIGHT);
    // Embossed Roaring Lion Gorgoneion with Ruby Eyes
    gba_draw_rect(x + 15, y + 17, 4, 4, COL_GOLD_BRIGHT);
    gba_set_pixel(x + 16, y + 18, COL_RUBY_EYE);
    gba_set_pixel(x + 17, y + 18, COL_RUBY_EYE);
    // Spiked Pauldrons
    gba_draw_rect(x + 6, y + 14, 4, 5, COL_BRONZE_MID);
    gba_set_pixel(x + 7, y + 13, COL_GOLD_BRIGHT); // Spike
    gba_draw_rect(x + 24, y + 14, 4, 5, COL_BRONZE_MID);
    gba_set_pixel(x + 25, y + 13, COL_GOLD_BRIGHT);

    // War Belt & Pteruges
    gba_draw_rect(x + 9, y + 28, 16, 2, COL_LEATHER_DARK);
    gba_draw_rect(x + 10, y + 30, 14, 4, COL_LEATHER_MID);
    gba_draw_rect(x + 10, y + 33, 14, 2, COL_TUNIC_RED);

    // Arms & Combat Whip
    if (state == 1) {
        // Whip Windup: Arm raised overhead
        gba_draw_rect(x + 21, y + 6, 4, 10, COL_SKIN_MID);
        gba_draw_line(x + 20, y + 3, x + 30, y + 3, COL_LEATHER_MID);
    } else if (state == 2) {
        // Whip Crack: Arm lunged straight forward
        gba_draw_rect(x + 21, y + 16, 10, 4, COL_SKIN_MID);
        // Heavy braided whip line across screen!
        gba_draw_line(x + 31, y + 17, x + 100, y + 17, COL_LEATHER_MID);
        gba_draw_line(x + 31, y + 18, x + 96, y + 18, COL_SPARK_GOLD); // Energy crack core
    } else if (state == 3) {
        // Overhead Slam: Powerful high strike
        gba_draw_rect(x + 21, y + 4, 4, 12, COL_SKIN_MID);
        gba_draw_line(x + 22, y + 2, x + 22, y + 20, COL_LEATHER_MID);
    } else {
        // Stalking / Idle: Arm at side with coiled whip
        gba_draw_rect(x + 22, y + 16, 4, 8, COL_SKIN_MID);
        gba_draw_line(x + 24, y + 22, x + 26, y + 30, COL_LEATHER_MID);
    }

    // Muscular Greaved Legs
    int walkOff = (animFrame == 1) ? 2 : -1;
    gba_draw_rect(x + 10 - walkOff, y + 34, 4, 6, COL_BRONZE_MID);
    gba_draw_rect(x + 19 + walkOff, y + 34, 4, 6, COL_BRONZE_MID);
    gba_draw_rect(x + 9 - walkOff, y + 40, 5, 2, COL_LEATHER_DARK);
    gba_draw_rect(x + 18 + walkOff, y + 40, 5, 2, COL_LEATHER_DARK);
}

void gba_draw_basket(int x, int y) {
    // Wicker basket with loaves & fishes
    gba_draw_rect(x + 2, y + 4, 10, 7, COL_BASKET_WICKER);
    gba_draw_rect_outline(x + 2, y + 4, 10, 7, COL_CRATE_DARK);
    // Woven cross-hatch
    gba_set_pixel(x + 4, y + 6, COL_CRATE_DARK);
    gba_set_pixel(x + 7, y + 6, COL_CRATE_DARK);
    gba_set_pixel(x + 9, y + 6, COL_CRATE_DARK);
    // Bread loaf
    gba_draw_rect(x + 3, y + 1, 4, 3, COL_BASKET_BREAD);
    // Fish
    gba_draw_rect(x + 7, y + 2, 4, 2, COL_BASKET_FISH);
}

void gba_draw_nail(int x, int y, bool facingLeft) {
    (void)facingLeft;
    // Radiant iron nail with golden cometary trail
    gba_draw_line(x - 4, y, x, y, COL_SPARK_GOLD);
    gba_draw_rect(x, y - 1, 5, 3, COL_STEEL_HI);
    gba_set_pixel(x + 5, y, COL_WHITE_PURE);
}

void gba_draw_arrow(int x, int y, bool facingLeft) {
    (void)facingLeft;
    gba_draw_line(x, y, x + 7, y, COL_WOOD_LIGHT);
    gba_set_pixel(x + 7, y, COL_STEEL_HI); // Tip
    gba_set_pixel(x, y, COL_CREST_HI);    // Feather
}
