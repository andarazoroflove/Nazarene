#ifndef GBA_GRAPHICS_H
#define GBA_GRAPHICS_H

#include "gba_types.h"

// Color Palette Indices
enum PaletteIndex {
    COL_TRANSPARENT = 0,
    
    // Sky Sunset & Mountains (1..15)
    COL_SKY_INDIGO,
    COL_SKY_PURPLE,
    COL_SKY_CRIMSON,
    COL_SKY_AMBER,
    COL_SKY_GOLD,
    COL_SKY_YELLOW,
    COL_SUN_WHITE,
    COL_SUN_GOLD,
    COL_MTN_PURPLE,
    COL_MTN_UMBER,
    COL_MTN_DARK,
    COL_DOVE_WHITE,
    COL_DOVE_SHADOW,
    
    // Jerusalem Architecture & Temple of David (16..35)
    COL_STONE_LIGHT,
    COL_STONE_MID,
    COL_STONE_DARK,
    COL_STONE_SHADOW,
    COL_BRONZE_LIGHT,
    COL_BRONZE_MID,
    COL_BRONZE_DARK,
    COL_GOLD_BRIGHT,
    COL_GOLD_MID,
    COL_GOLD_DARK,
    COL_WOOD_LIGHT,
    COL_WOOD_MID,
    COL_WOOD_DARK,
    COL_BANNER_BLUE,
    COL_BANNER_GOLD,
    COL_ROOF_TERRACOTTA,
    COL_VILLA_WALL,
    COL_CYPRESS_GREEN,
    COL_OLIVE_LEAF,
    COL_WINDOW_GOLD,
    
    // Ground, Cobblestones, Barricades (36..45)
    COL_ROAD_LIGHT,
    COL_ROAD_MID,
    COL_ROAD_DARK,
    COL_CRATE_LIGHT,
    COL_CRATE_MID,
    COL_CRATE_DARK,
    COL_SHADOW_SOFT,
    COL_SHADOW_CORE,
    COL_IRON_GATE,
    COL_GATE_GLOW,
    
    // Characters: Common Skin, Outline & Effects (46..55)
    COL_OUTLINE,
    COL_SKIN_HI,
    COL_SKIN_MID,
    COL_SKIN_FLUSH,
    COL_SKIN_DARK,
    COL_WHITE_PURE,
    COL_WHITE_SHADOW,
    COL_WHITE_DEEP,
    COL_BLOOD_RED,
    COL_SPARK_GOLD,
    
    // Jesus Nazarene specifics (56..65)
    COL_HAIR_HI,
    COL_HAIR_MID,
    COL_HAIR_DARK,
    COL_ROBE_WHITE,
    COL_ROBE_MID,
    COL_ROBE_DARK,
    COL_ROBE_DEEP,
    COL_SASH_HI,
    COL_SASH_MID,
    COL_SASH_DARK,
    COL_TZITZIT_BLUE,
    
    // Roman Armor & Enemies (66..85)
    COL_STEEL_HI,
    COL_STEEL_MID,
    COL_STEEL_DARK,
    COL_CREST_HI,
    COL_CREST_MID,
    COL_CREST_DARK,
    COL_TUNIC_RED,
    COL_SCUTUM_RED,
    COL_SCUTUM_DARK,
    COL_LAUREL_GREEN,
    COL_LAUREL_HI,
    COL_ARCHER_GREEN,
    COL_ARCHER_DARK,
    COL_LEATHER_HI,
    COL_LEATHER_MID,
    COL_LEATHER_DARK,
    COL_RUBY_EYE,
    COL_ASH_CHARCOAL,
    COL_ASH_EMBER,
    COL_ASH_GOLD,
    
    // UI & Text (86..95)
    COL_TEXT_WHITE,
    COL_TEXT_GOLD,
    COL_TEXT_RED,
    COL_TEXT_DARK,
    COL_HEART_RED,
    COL_HEART_BORDER,
    COL_BASKET_WICKER,
    COL_BASKET_BREAD,
    COL_BASKET_FISH
};

void gba_graphics_init(void);
void gba_graphics_flip(void);
void gba_graphics_clear(uint8_t color);

// Primitive drawing
void gba_set_pixel(int x, int y, uint8_t color);
void gba_draw_rect(int x, int y, int w, int h, uint8_t color);
void gba_draw_rect_outline(int x, int y, int w, int h, uint8_t color);
void gba_draw_line(int x0, int y0, int x1, int y1, uint8_t color);
void gba_draw_drop_shadow(int cx, int cy, int rx, int ry);

// Text
void gba_draw_char(char c, int x, int y, uint8_t color);
void gba_draw_string(const char* str, int x, int y, uint8_t color);
void gba_draw_number(int num, int x, int y, uint8_t color);

// Parallax Backgrounds
void gba_draw_parallax_background(int cameraX, int timer);

// Sprites
void gba_draw_player(int x, int y, bool facingLeft, int state, int animFrame, bool hurt);
void gba_draw_guard(int x, int y, bool facingLeft, bool attack, int animFrame, bool hurt);
void gba_draw_archer(int x, int y, bool facingLeft, bool duck, bool fire, bool hurt);
void gba_draw_heavy(int x, int y, bool facingLeft, bool attack, int animFrame, bool hurt);
void gba_draw_boss(int x, int y, bool facingLeft, int state, int animFrame, bool hurt);
void gba_draw_basket(int x, int y);
void gba_draw_nail(int x, int y, bool facingLeft);
void gba_draw_arrow(int x, int y, bool facingLeft);

#endif // GBA_GRAPHICS_H
