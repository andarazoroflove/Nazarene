#ifndef GBA_TYPES_H
#define GBA_TYPES_H

#include <stdint.h>
#include <stdbool.h>

#define GBA_SCREEN_WIDTH   240
#define GBA_SCREEN_HEIGHT  160

// I/O Registers
#define REG_DISPCNT      (*(volatile uint16_t*)0x04000000)
#define REG_DISPSTAT     (*(volatile uint16_t*)0x04000004)
#define REG_VCOUNT       (*(volatile uint16_t*)0x04000006)
#define REG_KEYINPUT     (*(volatile uint16_t*)0x04000130)

// DMA Channel 3 (General Purpose high-speed copy)
#define REG_DMA3SAD      (*(volatile uint32_t*)0x040000D4)
#define REG_DMA3DAD      (*(volatile uint32_t*)0x040000D8)
#define REG_DMA3CNT      (*(volatile uint32_t*)0x040000DC)

#define DMA_ENABLE       (1 << 31)
#define DMA_TIMING_IMM   (0 << 28)
#define DMA_TIMING_VBL   (1 << 28)
#define DMA_32BIT        (1 << 26)
#define DMA_16BIT        (0 << 26)

// Sound Registers (PSG / DirectSound)
#define REG_SOUND1CNT_L  (*(volatile uint16_t*)0x04000060)
#define REG_SOUND1CNT_H  (*(volatile uint16_t*)0x04000062)
#define REG_SOUND1CNT_X  (*(volatile uint16_t*)0x04000064)
#define REG_SOUND2CNT_L  (*(volatile uint16_t*)0x04000068)
#define REG_SOUND2CNT_H  (*(volatile uint16_t*)0x0400006C)
#define REG_SOUND4CNT_L  (*(volatile uint16_t*)0x04000078)
#define REG_SOUND4CNT_H  (*(volatile uint16_t*)0x0400007C)
#define REG_SOUNDCNT_L   (*(volatile uint16_t*)0x04000080)
#define REG_SOUNDCNT_H   (*(volatile uint16_t*)0x04000082)
#define REG_SOUNDCNT_X   (*(volatile uint16_t*)0x04000084)

// Memory Regions
#define MEM_VRAM         ((volatile uint16_t*)0x06000000)
#define MEM_VRAM_PAGE0   ((volatile uint16_t*)0x06000000)
#define MEM_VRAM_PAGE1   ((volatile uint16_t*)0x0600A000)
#define MEM_PALETTE_BG   ((volatile uint16_t*)0x05000000)

// Video Control Flags
#define MODE_4           0x0004
#define BG2_ENABLE       0x0400
#define DISPCNT_PAGE_1   0x0010

// Keys (Active LOW)
#define KEY_A            (1 << 0)
#define KEY_B            (1 << 1)
#define KEY_SELECT       (1 << 2)
#define KEY_START        (1 << 3)
#define KEY_RIGHT        (1 << 4)
#define KEY_LEFT         (1 << 5)
#define KEY_UP           (1 << 6)
#define KEY_DOWN         (1 << 7)
#define KEY_R            (1 << 8)
#define KEY_L            (1 << 9)

#define RGB15(r, g, b)   ((uint16_t)(((r) & 31) | (((g) & 31) << 5) | (((b) & 31) << 10)))
#define RGB8(r, g, b)    RGB15((r) >> 3, (g) >> 3, (b) >> 3)

static inline void gba_wait_vblank(void) {
    while (REG_VCOUNT >= 160);
    while (REG_VCOUNT < 160);
}

#endif // GBA_TYPES_H
