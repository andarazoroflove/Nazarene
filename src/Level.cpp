#include "Level.h"
#include "Audio.h"
#include <cmath>
#include <algorithm>

Level::Level() {
    m_stageId = 1;
    Reset(1);
}

void Level::Reset(int stageId) {
    m_stageId = stageId;
    m_basketsCollected = 0;
    m_arenaUnlocked = false;
    m_arenaLockedDown = false;
    m_exitOpen = false;
    m_gateOpenOffsetY = 0.0f;
    m_oneWayPlatforms.clear();
    m_crates.clear();
    m_baskets.clear();

    if (m_stageId == 2) {
        m_arenaGateX = ARENA_GATE_X; // 2120.0f
        m_arenaMinX = ARENA_MIN_X;   // 2140.0f
        m_arenaMaxX = ARENA_MAX_X;   // 2540.0f
        BuildInsideTheTemple();
    } else if (m_stageId == 3) {
        // Stage 3: Golgotha Citadel (shorter run to boss, ~3.5 screens)
        m_arenaGateX = 1120.0f;
        m_arenaMinX = 1140.0f;
        m_arenaMaxX = 1540.0f;
        BuildGolgothaCitadel();
    } else if (m_stageId == 4) {
        // Stage 4: Valley of Kidron
        m_arenaGateX = ARENA_GATE_X; // 2120.0f
        m_arenaMinX = ARENA_MIN_X;   // 2140.0f
        m_arenaMaxX = ARENA_MAX_X;   // 2540.0f
        BuildValleyOfKidron();
    } else if (m_stageId == 5) {
        // Stage 5: Cypress Hedge Maze
        m_arenaGateX = ARENA_GATE_X; // 2120.0f
        m_arenaMinX = ARENA_MIN_X;   // 2140.0f
        m_arenaMaxX = ARENA_MAX_X;   // 2540.0f
        BuildCypressHedgeMaze();
    } else if (m_stageId == 6) {
        // Stage 6: Garden of Gethsemane (Sacred Grove & Stone of Agony)
        m_arenaGateX = 1600.0f;
        m_arenaMinX = 1620.0f;
        m_arenaMaxX = 2020.0f;
        BuildGardenOfGethsemane();
    } else if (m_stageId == 7) {
        // Stage 7: Siloam Bazaar & Aqueducts
        m_arenaGateX = ARENA_GATE_X; // 2120.0f
        m_arenaMinX = ARENA_MIN_X;   // 2140.0f
        m_arenaMaxX = ARENA_MAX_X;   // 2540.0f
        BuildSiloamBazaar();
    } else if (m_stageId == 8) {
        // Stage 8: Mount Zion & Palace of Caiaphas
        m_arenaGateX = ARENA_GATE_X; // 2120.0f
        m_arenaMinX = ARENA_MIN_X;   // 2140.0f
        m_arenaMaxX = ARENA_MAX_X;   // 2540.0f
        BuildMountZionPalace();
    } else if (m_stageId == 9) {
        // Stage 9: The Upper Room / Cenacle (The Last Supper)
        m_arenaGateX = 1440.0f;
        m_arenaMinX = 1460.0f;
        m_arenaMaxX = 1860.0f;
        BuildUpperRoomCenacle();
    } else if (m_stageId == 10) {
        // Stage 10: Via Dolorosa / Streets of Judgment
        m_arenaGateX = ARENA_GATE_X; // 2120.0f
        m_arenaMinX = ARENA_MIN_X;   // 2140.0f
        m_arenaMaxX = ARENA_MAX_X;   // 2540.0f
        BuildViaDolorosa();
    } else if (m_stageId == 11) {
        // Stage 11: The Gabbatha Courtroom / Lithostrotos
        m_arenaGateX = ARENA_GATE_X; // 2120.0f
        m_arenaMinX = ARENA_MIN_X;   // 2140.0f
        m_arenaMaxX = ARENA_MAX_X;   // 2540.0f
        BuildTheGabbathaCourt();
    } else if (m_stageId == 12) {
        // Stage 12: The Praetorium Throne Room & Bema of Pilate
        m_arenaGateX = 1440.0f;
        m_arenaMinX = 1460.0f;
        m_arenaMaxX = 1860.0f;
        BuildPraetoriumThrone();
    } else if (m_stageId == 13) {
        // Stage 13: Golgotha Summit (The Final Conflict - Single Board)
        m_arenaGateX = 0.0f;
        m_arenaMinX = 20.0f;
        m_arenaMaxX = 460.0f;
        m_arenaUnlocked = true;
        m_arenaLockedDown = true;
        BuildGolgothaSummit();
    } else {
        m_arenaGateX = ARENA_GATE_X;
        m_arenaMinX = ARENA_MIN_X;
        m_arenaMaxX = ARENA_MAX_X;
        BuildStreetsOfJerusalem();
    }
}

void Level::BuildStreetsOfJerusalem() {
    // 1. Initialize tilemap with air
    for (int y = 0; y < LEVEL_TILES_Y; ++y) {
        for (int x = 0; x < LEVEL_TILES_X; ++x) {
            m_tilemap[y][x] = TILE_AIR;
        }
    }

    // 2. Lay solid Ground Tier (Roman Paved Road) at Y = 12 and 13 (Y_pixels: 192 and 208)
    for (int x = 0; x < LEVEL_TILES_X; ++x) {
        m_tilemap[12][x] = TILE_GROUND_SURFACE;
        m_tilemap[13][x] = TILE_GROUND_SUBSURFACE;
    }

    // Helper to place one-way platform and matching visual tiles
    auto AddUpperBalcony = [this](int startTileX, int endTileX, int tileY) {
        float x = startTileX * TILE_SIZE;
        float y = tileY * TILE_SIZE;
        float w = (endTileX - startTileX + 1) * TILE_SIZE;
        float h = 6.0f; // Thin one-way surface
        m_oneWayPlatforms.push_back({ { x, y, w, h } });

        for (int tx = startTileX; tx <= endTileX; ++tx) {
            m_tilemap[tileY][tx] = TILE_BALCONY_PLATFORM;
        }
    };

    auto AddAqueductSpan = [this](int startTileX, int endTileX, int tileY) {
        float x = startTileX * TILE_SIZE;
        float y = tileY * TILE_SIZE;
        float w = (endTileX - startTileX + 1) * TILE_SIZE;
        m_oneWayPlatforms.push_back({ { x, y, w, 6.0f } });

        for (int tx = startTileX; tx <= endTileX; ++tx) {
            m_tilemap[tileY][tx] = TILE_AQUEDUCT_TOP;
            // Arches underneath
            if ((tx - startTileX) % 6 == 1) m_tilemap[tileY + 1][tx] = TILE_AQUEDUCT_ARCH_L;
            if ((tx - startTileX) % 6 == 4) m_tilemap[tileY + 1][tx] = TILE_AQUEDUCT_ARCH_R;
        }
    };

    // Helper to add classical decorative column
    auto AddColumn = [this](int tileX, int topTileY, int groundTileY) {
        m_tilemap[topTileY][tileX] = TILE_COLUMN_HEAD;
        for (int ty = topTileY + 1; ty < groundTileY - 1; ++ty) {
            m_tilemap[ty][tileX] = TILE_COLUMN_MID;
        }
        m_tilemap[groundTileY - 1][tileX] = TILE_COLUMN_BASE;
    };

    // Helper to add solid step crates
    auto AddCrate = [this](int tileX, int tileY) {
        m_tilemap[tileY][tileX] = TILE_WOOD_CRATE;
        Rectangle crateRect = { (float)tileX * TILE_SIZE, (float)tileY * TILE_SIZE, (float)TILE_SIZE, (float)TILE_SIZE };
        m_crates.push_back(crateRect);
    };

    // === SECTION 1: Starting Market Street (Tiles 0 - 30) ===
    AddColumn(4, 7, 12);
    AddColumn(12, 7, 12);
    m_tilemap[11][5] = TILE_OIL_LAMP;
    m_tilemap[6][7] = TILE_SPQR_BANNER;
    m_tilemap[9][9] = TILE_STONE_WINDOW;
    AddCrate(14, 11);
    AddCrate(18, 11);
    AddCrate(18, 10); // Double-stacked crate
    m_tilemap[11][20] = TILE_CLAY_JAR;
    m_tilemap[11][21] = TILE_OIL_LAMP;

    // Basket 1: Lower tier market corner
    m_baskets.push_back({ { 19.0f * TILE_SIZE, 11.0f * TILE_SIZE }, false, 0.0f, 0 });

    // === SECTION 2: First Upper Tier Balconies & Aqueduct (Tiles 25 - 65) ===
    AddUpperBalcony(24, 38, 7); // y = 112
    m_tilemap[5][26] = TILE_STONE_WINDOW;
    m_tilemap[6][31] = TILE_SPQR_BANNER;
    m_tilemap[5][36] = TILE_STONE_WINDOW;
    AddColumn(28, 8, 12);
    AddColumn(34, 8, 12);

    AddAqueductSpan(42, 62, 7); // y = 112
    m_tilemap[11][46] = TILE_OIL_LAMP;
    m_tilemap[11][56] = TILE_OIL_LAMP;
    AddColumn(45, 8, 12);
    AddColumn(51, 8, 12);
    AddColumn(57, 8, 12);

    // Basket 2: High atop the Aqueduct ledge
    m_baskets.push_back({ { 53.0f * TILE_SIZE, 5.8f * TILE_SIZE }, false, 0.0f, 1 });

    // Crates on the ground below aqueduct
    AddCrate(48, 11);
    AddCrate(59, 11);

    // === SECTION 3: Jerusalem Bazaar & Rooftop Terraces (Tiles 66 - 105) ===
    AddUpperBalcony(68, 80, 7);
    m_tilemap[5][70] = TILE_STONE_WINDOW;
    m_tilemap[6][72] = TILE_SPQR_BANNER;
    AddColumn(74, 8, 12);
    AddCrate(76, 6); // Crate on the upper balcony

    AddUpperBalcony(84, 98, 7);
    m_tilemap[5][86] = TILE_STONE_WINDOW;
    m_tilemap[6][89] = TILE_OIL_LAMP;
    m_tilemap[6][91] = TILE_SPQR_BANNER;
    AddColumn(88, 8, 12);
    AddColumn(94, 8, 12);

    // Basket 3: Upper rooftop terrace
    m_baskets.push_back({ { 90.0f * TILE_SIZE, 5.8f * TILE_SIZE }, false, 0.0f, 1 });

    AddCrate(85, 11);
    m_tilemap[11][87] = TILE_CLAY_JAR;
    AddCrate(96, 11);

    // === SECTION 4: Fortified Garrison Ramparts & Approach (Tiles 106 - 132) ===
    AddUpperBalcony(106, 124, 7);
    m_tilemap[5][108] = TILE_STONE_WINDOW;
    m_tilemap[6][115] = TILE_SPQR_BANNER;
    m_tilemap[11][116] = TILE_OIL_LAMP;
    m_tilemap[5][120] = TILE_STONE_WINDOW;
    m_tilemap[6][128] = TILE_SPQR_BANNER;
    AddColumn(110, 8, 12);
    AddColumn(118, 8, 12);

    // Basket 4: Lower tier near garrison barricade
    m_baskets.push_back({ { 117.0f * TILE_SIZE, 11.0f * TILE_SIZE }, false, 0.0f, 0 });

    AddCrate(114, 11);
    AddCrate(122, 11);
    AddCrate(123, 11);

    // === SECTION 5: Boss Arena Gate (Tiles 132 - 134) ===
    int gateTileX = static_cast<int>(ARENA_GATE_X / TILE_SIZE); // ~132
    for (int ty = 2; ty <= 11; ++ty) {
        m_tilemap[ty][gateTileX] = TILE_GATE_FRAME;
        m_tilemap[ty][gateTileX + 1] = TILE_GATE_BARS;
        m_tilemap[ty][gateTileX + 2] = TILE_GATE_FRAME;
    }
    m_tilemap[11][gateTileX - 1] = TILE_OIL_LAMP;

    // === SECTION 6: Gladiator Boss Arena (Tiles 135 - 160) ===
    // Enclosed arena with ornamental columns and stone battlements
    AddColumn(138, 5, 12);
    m_tilemap[4][142] = TILE_SPQR_BANNER;
    m_tilemap[10][139] = TILE_OIL_LAMP;
    AddColumn(146, 5, 12);
    m_tilemap[4][150] = TILE_SPQR_BANNER;
    m_tilemap[10][147] = TILE_OIL_LAMP;
    AddColumn(154, 5, 12);
    m_tilemap[10][155] = TILE_OIL_LAMP;

    // Arena back wall at end of level (Tile 159)
    for (int ty = 0; ty <= 11; ++ty) {
        m_tilemap[ty][159] = TILE_GATE_FRAME;
    }
}

void Level::BuildInsideTheTemple() {
    // 1. Initialize tilemap with air
    for (int y = 0; y < LEVEL_TILES_Y; ++y) {
        for (int x = 0; x < LEVEL_TILES_X; ++x) {
            m_tilemap[y][x] = TILE_AIR;
        }
    }

    // 2. Lay solid Ground Tier (Checkered Polished Marble Floor) at Y = 12 and 13 (Y_pixels: 192 and 208)
    for (int x = 0; x < LEVEL_TILES_X; ++x) {
        m_tilemap[12][x] = TILE_TEMPLE_FLOOR_CHECKER;
        m_tilemap[13][x] = TILE_TEMPLE_FLOOR_SUB;
    }

    // Platform and geometry helpers
    auto AddMezzanine = [this](int startTileX, int endTileX, int tileY) {
        float x = startTileX * TILE_SIZE;
        float y = tileY * TILE_SIZE;
        float w = (endTileX - startTileX + 1) * TILE_SIZE;
        m_oneWayPlatforms.push_back({ { x, y, w, 6.0f } });

        for (int tx = startTileX; tx <= endTileX; ++tx) {
            m_tilemap[tileY][tx] = TILE_TEMPLE_BALCONY_1;
        }
    };

    auto AddGallery = [this](int startTileX, int endTileX, int tileY) {
        float x = startTileX * TILE_SIZE;
        float y = tileY * TILE_SIZE;
        float w = (endTileX - startTileX + 1) * TILE_SIZE;
        m_oneWayPlatforms.push_back({ { x, y, w, 6.0f } });

        for (int tx = startTileX; tx <= endTileX; ++tx) {
            m_tilemap[tileY][tx] = TILE_TEMPLE_BALCONY_2;
        }
    };

    auto AddTempleColumn = [this](int tileX, int topTileY, int groundTileY) {
        m_tilemap[topTileY][tileX] = TILE_TEMPLE_COLUMN_HEAD;
        for (int ty = topTileY + 1; ty < groundTileY - 1; ++ty) {
            m_tilemap[ty][tileX] = TILE_TEMPLE_COLUMN_MID;
        }
        m_tilemap[groundTileY - 1][tileX] = TILE_TEMPLE_COLUMN_BASE;
    };

    auto AddStepBlock = [this](int tileX, int tileY) {
        m_tilemap[tileY][tileX] = TILE_WOOD_CRATE;
        m_crates.push_back({ (float)tileX * TILE_SIZE, (float)tileY * TILE_SIZE, (float)TILE_SIZE, (float)TILE_SIZE });
    };

    // === SECTION 1: Temple Outer Portico (Tiles 0 - 32) ===
    AddTempleColumn(4, 3, 12);
    AddTempleColumn(12, 3, 12);
    AddTempleColumn(20, 3, 12);
    AddTempleColumn(28, 3, 12);
    m_tilemap[11][6] = TILE_TEMPLE_MENORAH;
    m_tilemap[11][14] = TILE_TEMPLE_ALTAR;
    m_tilemap[11][22] = TILE_TEMPLE_INCENSE_BURNER;
    m_tilemap[4][8] = TILE_TEMPLE_DRAPERY_PURPLE;
    m_tilemap[4][16] = TILE_TEMPLE_DRAPERY_PURPLE;
    m_tilemap[4][24] = TILE_TEMPLE_DRAPERY_PURPLE;

    // Mezzanine balcony (Tier 1, Y=9, 144px)
    AddMezzanine(14, 28, 9);
    AddStepBlock(11, 11);

    // Censer 1: On Mezzanine at tile 21
    m_baskets.push_back({ { 21.0f * TILE_SIZE, 8.0f * TILE_SIZE }, false, 0.0f, 1 });

    // === SECTION 2: Multi-Tier Great Colonnade (Tiles 32 - 70) ===
    // DUAL-LEVEL UPPER PLATFORMS: Tier 1 (Y=9, 144px) and Tier 2 (Y=6, 96px)
    AddTempleColumn(36, 1, 12);
    AddTempleColumn(46, 1, 12);
    AddTempleColumn(56, 1, 12);
    AddTempleColumn(66, 1, 12);

    AddMezzanine(34, 64, 9);
    AddGallery(40, 60, 6);

    AddStepBlock(32, 11); // Step to Tier 1
    AddStepBlock(39, 8);  // Step from Tier 1 to Tier 2

    m_tilemap[11][38] = TILE_TEMPLE_ALTAR;
    m_tilemap[11][48] = TILE_TEMPLE_MENORAH;
    m_tilemap[11][58] = TILE_TEMPLE_INCENSE_BURNER;
    m_tilemap[3][42] = TILE_TEMPLE_DRAPERY_PURPLE;
    m_tilemap[3][50] = TILE_TEMPLE_DRAPERY_PURPLE;

    // Censer 2: High Tier 2 Parapet at tile 50
    m_baskets.push_back({ { 50.0f * TILE_SIZE, 5.0f * TILE_SIZE }, false, 0.0f, 2 });

    // === SECTION 3: Court of the Priests (Tiles 70 - 105) ===
    // Multi-tier upper platforms for dynamic jumping
    AddTempleColumn(76, 1, 12);
    AddTempleColumn(86, 1, 12);
    AddTempleColumn(96, 1, 12);

    AddMezzanine(74, 92, 9);
    AddGallery(80, 102, 6);

    AddStepBlock(73, 11);
    AddStepBlock(79, 8);

    m_tilemap[11][78] = TILE_TEMPLE_MENORAH;
    m_tilemap[11][88] = TILE_TEMPLE_ALTAR;
    m_tilemap[3][82] = TILE_TEMPLE_DRAPERY_PURPLE;
    m_tilemap[3][92] = TILE_TEMPLE_DRAPERY_PURPLE;

    // Censer 3: Ground level sacred alcove at tile 92
    m_baskets.push_back({ { 92.0f * TILE_SIZE, 11.0f * TILE_SIZE }, false, 0.0f, 0 });

    // === SECTION 4: Porch of Solomon (Tiles 105 - 132) ===
    AddTempleColumn(108, 1, 12);
    AddTempleColumn(116, 1, 12);
    AddTempleColumn(124, 1, 12);

    AddMezzanine(110, 128, 9);
    AddGallery(114, 126, 6);

    AddStepBlock(109, 11);
    AddStepBlock(113, 8);

    m_tilemap[11][112] = TILE_TEMPLE_MENORAH;
    m_tilemap[11][120] = TILE_TEMPLE_INCENSE_BURNER;
    m_tilemap[3][118] = TILE_TEMPLE_DRAPERY_PURPLE;

    // Censer 4: High Tier 2 at tile 121
    m_baskets.push_back({ { 121.0f * TILE_SIZE, 5.0f * TILE_SIZE }, false, 0.0f, 2 });

    // === SECTION 5: Sanctuary Gate / Golden Veil (Tiles 132 - 134) ===
    int gateTileX = static_cast<int>(ARENA_GATE_X / TILE_SIZE);
    for (int ty = 2; ty <= 11; ++ty) {
        m_tilemap[ty][gateTileX] = TILE_GATE_FRAME;
        m_tilemap[ty][gateTileX + 1] = TILE_GATE_BARS;
        m_tilemap[ty][gateTileX + 2] = TILE_GATE_FRAME;
    }
    m_tilemap[11][gateTileX - 1] = TILE_TEMPLE_MENORAH;

    // === SECTION 6: Holy of Holies Boss Arena (Tiles 135 - 160) ===
    AddTempleColumn(138, 2, 12);
    AddTempleColumn(146, 2, 12);
    AddTempleColumn(154, 2, 12);
    m_tilemap[11][141] = TILE_TEMPLE_MENORAH;
    m_tilemap[11][149] = TILE_TEMPLE_ALTAR;
    m_tilemap[11][157] = TILE_TEMPLE_MENORAH;
    for (int cx = 136; cx < 159; cx += 2) {
        m_tilemap[3][cx] = TILE_TEMPLE_DRAPERY_PURPLE;
    }

    // Arena back wall at end of level (Tile 159)
    for (int ty = 0; ty <= 11; ++ty) {
        m_tilemap[ty][159] = TILE_GATE_FRAME;
    }
}

void Level::Update(float dt, const Vector2& playerPos) {
    // Floating animation for baskets
    for (auto& b : m_baskets) {
        if (!b.collected) {
            b.floatTimer += dt;
        }
    }

    // Check if all baskets collected -> unlock gate
    if (m_basketsCollected >= TOTAL_BASKETS && !m_arenaUnlocked) {
        m_arenaUnlocked = true;
    }

    // Animate gate lifting when unlocked
    if (m_arenaUnlocked && !m_arenaLockedDown) {
        if (m_gateOpenOffsetY < 80.0f) {
            m_gateOpenOffsetY += 120.0f * dt;
            if (m_gateOpenOffsetY > 80.0f) m_gateOpenOffsetY = 80.0f;
        }
    }

    // If player crosses into arena, trigger lockdown!
    if (m_arenaUnlocked && !m_arenaLockedDown && playerPos.x > (m_arenaGateX + 40.0f)) {
        TriggerArenaLockdown();
    }

    // If locked down, close the gate rapidly
    if (m_arenaLockedDown && m_gateOpenOffsetY > 0.0f) {
        m_gateOpenOffsetY -= 240.0f * dt;
        if (m_gateOpenOffsetY < 0.0f) m_gateOpenOffsetY = 0.0f;
    }
}

void Level::TriggerArenaLockdown() {
    m_arenaLockedDown = true;
}

bool Level::CheckBasketPickup(const Rectangle& playerBox) {
    for (auto& b : m_baskets) {
        if (!b.collected) {
            Rectangle basketRect = { b.position.x, b.position.y - 4.0f, 18.0f, 16.0f };
            if (CheckCollisionRecs(playerBox, basketRect)) {
                b.collected = true;
                m_basketsCollected++;
                return true;
            }
        }
    }
    return false;
}

bool Level::IsOnGround(const Rectangle& bbox, float& outGroundY) const {
    float feetY = bbox.y + bbox.height;
    if (feetY >= GROUND_Y - 2.0f) {
        outGroundY = GROUND_Y;
        return true;
    }
    return false;
}

bool Level::CheckOneWayLanding(const Rectangle& bbox, float prevY, float& outLedgeY) const {
    float curFeet = bbox.y + bbox.height;
    float prevFeet = prevY + 30.0f;

    for (const auto& plat : m_oneWayPlatforms) {
        // Horizontal overlap
        if (bbox.x + bbox.width > plat.rect.x + 2.0f && bbox.x < plat.rect.x + plat.rect.width - 2.0f) {
            // Must have crossed or landed on platform top from above (or currently resting on it)
            if (prevFeet <= plat.rect.y + 6.0f && curFeet >= plat.rect.y - 1.0f) {
                outLedgeY = plat.rect.y;
                return true;
            }
        }
    }
    return false;
}

bool Level::CheckCrateLanding(const Rectangle& bbox, float prevY, float& outLedgeY) const {
    float curFeet = bbox.y + bbox.height;
    float prevFeet = prevY + 30.0f;

    for (const auto& crate : m_crates) {
        // Horizontal overlap with crate top
        if (bbox.x + bbox.width > crate.x + 1.0f && bbox.x < crate.x + crate.width - 1.0f) {
            // Must have crossed or landed on crate top from above (or currently resting on it)
            if (prevFeet <= crate.y + 6.0f && curFeet >= crate.y - 1.0f) {
                outLedgeY = crate.y;
                return true;
            }
        }
    }
    return false;
}

bool Level::IsDropThroughPossible(const Rectangle& bbox) const {
    float feetY = bbox.y + bbox.height;
    // Cannot drop through base ground
    if (feetY >= GROUND_Y - 4.0f) return false;

    // Check if resting on any one-way platform (crates are NOT droppable)
    for (const auto& plat : m_oneWayPlatforms) {
        if (bbox.x + bbox.width > plat.rect.x - 2.0f && bbox.x < plat.rect.x + plat.rect.width + 2.0f) {
            if (fabsf(feetY - plat.rect.y) < 6.0f) {
                return true;
            }
        }
    }
    return false;
}

bool Level::CheckWallCollision(const Rectangle& bbox, float& adjustedX, int facingDir) const {
    // Left boundary of world (offset by player hurtbox padding)
    if (bbox.x < 8.0f) {
        adjustedX = 8.0f - 4.0f;
        return true;
    }

    // Right boundary of arena:
    if (bbox.x + bbox.width > m_arenaMaxX) {
        adjustedX = m_arenaMaxX - bbox.width - 4.0f;
        return true;
    }

    // Crates: Solid obstacle from sides (must jump onto or over them)
    for (const auto& crate : m_crates) {
        float feetY = bbox.y + bbox.height;
        float headY = bbox.y;

        // Vertical overlap check: player's feet penetrate below crate top (by at least 3px)
        // and player's head is above crate bottom
        if (feetY > crate.y + 3.0f && headY < crate.y + crate.height - 2.0f) {
            if (bbox.x + bbox.width > crate.x && bbox.x < crate.x + crate.width) {
                float crateMidX = crate.x + crate.width * 0.5f;
                float playerMidX = bbox.x + bbox.width * 0.5f;
                if (playerMidX < crateMidX) {
                    adjustedX = crate.x - bbox.width - 4.0f;
                } else {
                    adjustedX = crate.x + crate.width - 4.0f;
                }
                return true;
            }
        }
    }

    // Arena gate collision:
    // If gate is NOT open (or locked down), acts as a solid wall
    if (!m_arenaUnlocked || m_arenaLockedDown) {
        float gateX = m_arenaGateX + 16.0f;
        if (m_arenaLockedDown) {
            // Player trapped inside arena
            if (bbox.x < m_arenaMinX) {
                adjustedX = m_arenaMinX - 4.0f;
                return true;
            }
        } else {
            // Player outside arena, cannot pass until all 4 baskets collected
            if (bbox.x + bbox.width > gateX && bbox.x < gateX + 16.0f) {
                adjustedX = gateX - bbox.width - 4.0f;
                return true;
            }
        }
    }

    return false;
}

void Level::OpenExit() {
    if (!m_exitOpen) {
        m_exitOpen = true;
        int exitTile = static_cast<int>(m_arenaMaxX / TILE_SIZE);
        if (exitTile < LEVEL_TILES_X) {
            m_tilemap[8][exitTile] = TILE_AIR;
            m_tilemap[9][exitTile] = TILE_AIR;
            m_tilemap[10][exitTile] = TILE_AIR;
            m_tilemap[11][exitTile] = TILE_AIR;
        }
        AudioManager::PlaySfx(SFX_DING_DING);
    }
}

void Level::Draw(float cameraX) {
    int startCol = std::max(0, static_cast<int>(cameraX / TILE_SIZE) - 1);
    int endCol = std::min(LEVEL_TILES_X - 1, static_cast<int>((cameraX + VIRTUAL_WIDTH) / TILE_SIZE) + 2);

    int gateCol = static_cast<int>(m_arenaGateX / TILE_SIZE);

    // Draw background/geometry tiles
    for (int ty = 0; ty < LEVEL_TILES_Y; ++ty) {
        for (int tx = startCol; tx <= endCol; ++tx) {
            TileId id = m_tilemap[ty][tx];
            if (id == TILE_AIR) continue;

            float drawX = tx * TILE_SIZE;
            float drawY = ty * TILE_SIZE;

            // Handle gate portcullis vertical slide
            if (tx == gateCol + 1 && (id == TILE_GATE_BARS || id == TILE_CAIAPHAS_GATE || id == TILE_DOLOROSA_GATE)) {
                drawY -= m_gateOpenOffsetY;
            }

            GraphicsManager::DrawTile(id, drawX, drawY);
        }
    }

    // Draw Baskets, Sacred Censers, Pitchers, Wreaths, or Chalices depending on stage
    for (const auto& b : m_baskets) {
        if (!b.collected) {
            float floatY = sinf(b.floatTimer * 4.0f) * 2.0f;
            SpriteId pickupSpr = SPR_BASKET_LOAVES_FISHES;
            if (m_stageId == 2 || m_stageId == 3) {
                pickupSpr = SPR_CENSER_GOLD;
            } else if (m_stageId == 4) {
                pickupSpr = SPR_SILVER_PITCHER;
            } else if (m_stageId == 5) {
                pickupSpr = SPR_OLIVE_WREATH;
            } else if (m_stageId == 6) {
                pickupSpr = SPR_SACRED_CHALICE;
            } else if (m_stageId == 7) {
                pickupSpr = SPR_BITTER_HERBS;
            } else if (m_stageId == 8) {
                pickupSpr = SPR_PASSOVER_MATZOT;
            } else if (m_stageId == 9) {
                pickupSpr = SPR_HOLY_GRAIL;
            } else if (m_stageId == 10) {
                pickupSpr = SPR_ROMAN_SCROLL;
            } else if (m_stageId == 11) {
                pickupSpr = SPR_CAESAR_SEAL;
            } else if (m_stageId == 12) {
                pickupSpr = SPR_PILATE_EWER;
            } else if (m_stageId == 13) {
                pickupSpr = SPR_HOLY_GRAIL;
            }
            GraphicsManager::DrawSprite(pickupSpr, b.position.x, b.position.y + floatY);

            // Shimmer gleam
            float gleam = 0.5f + 0.5f * sinf(b.floatTimer * 6.0f);
            if (gleam > 0.8f) {
                GraphicsManager::DrawSpark(b.position.x + 8.0f, b.position.y + floatY - 2.0f, 2.0f, Color{ 255, 255, 230, 200 });
            }
        }
    }
}

void Level::BuildGolgothaCitadel() {
    // 1. Initialize entire tilemap with air
    for (int y = 0; y < LEVEL_TILES_Y; ++y) {
        for (int x = 0; x < LEVEL_TILES_X; ++x) {
            m_tilemap[y][x] = TILE_AIR;
        }
    }

    // 2. Solid Ground Tier (Golgotha Crags & Roman Paved Road up to Antonia)
    for (int x = 0; x < LEVEL_TILES_X; ++x) {
        m_tilemap[12][x] = TILE_GOLGOTHA_ROCK_SURFACE;
        m_tilemap[13][x] = TILE_GOLGOTHA_ROCK_SUB;
    }

    // Platform helpers
    auto AddCragLedge = [this](int startTileX, int endTileX, int tileY) {
        float x = startTileX * TILE_SIZE;
        float y = tileY * TILE_SIZE;
        float w = (endTileX - startTileX + 1) * TILE_SIZE;
        m_oneWayPlatforms.push_back({ { x, y, w, 6.0f } });

        for (int tx = startTileX; tx <= endTileX; ++tx) {
            m_tilemap[tileY][tx] = TILE_GOLGOTHA_LEDGE_1;
        }
    };

    auto AddBattlement = [this](int startTileX, int endTileX, int tileY) {
        float x = startTileX * TILE_SIZE;
        float y = tileY * TILE_SIZE;
        float w = (endTileX - startTileX + 1) * TILE_SIZE;
        m_oneWayPlatforms.push_back({ { x, y, w, 6.0f } });

        for (int tx = startTileX; tx <= endTileX; ++tx) {
            m_tilemap[tileY][tx] = TILE_GOLGOTHA_LEDGE_2;
        }
    };

    auto AddRockColumn = [this](int tileX, int topTileY, int groundTileY) {
        m_tilemap[topTileY][tileX] = TILE_GOLGOTHA_COLUMN_HEAD;
        for (int ty = topTileY + 1; ty < groundTileY - 1; ++ty) {
            m_tilemap[ty][tileX] = TILE_GOLGOTHA_COLUMN_MID;
        }
        m_tilemap[groundTileY - 1][tileX] = TILE_GOLGOTHA_COLUMN_BASE;
    };

    auto AddRockBlock = [this](int tileX, int tileY) {
        m_tilemap[tileY][tileX] = TILE_WOOD_CRATE;
        m_crates.push_back({ (float)tileX * TILE_SIZE, (float)tileY * TILE_SIZE, (float)TILE_SIZE, (float)TILE_SIZE });
    };

    // === SECTION 1: Lower Crags of Golgotha (Tiles 0 - 24) ===
    AddRockColumn(5, 3, 12);
    AddRockColumn(12, 3, 12);
    AddRockColumn(20, 3, 12);
    m_tilemap[11][3] = TILE_GOLGOTHA_BRAZIER;
    m_tilemap[11][18] = TILE_GOLGOTHA_BRAZIER;
    m_tilemap[4][8] = TILE_GOLGOTHA_STANDARD;

    // Crag Shelf (Tier 1, Y=9, 144px)
    AddCragLedge(8, 22, 9);
    AddRockBlock(6, 11);

    // Sacred Relic #1: on Crag Shelf at tile 15
    m_baskets.push_back({ { 15.0f * TILE_SIZE, 8.0f * TILE_SIZE }, false, 0.0f, 1 });

    // === SECTION 2: Siege Ramp & Multi-Tier Crags (Tiles 25 - 48) ===
    AddRockColumn(26, 1, 12);
    AddRockColumn(34, 1, 12);
    AddRockColumn(42, 1, 12);

    AddCragLedge(25, 45, 9);     // Tier 1 (144px)
    AddBattlement(30, 40, 6);    // Tier 2 (96px)

    AddRockBlock(24, 11);        // Step to Tier 1
    AddRockBlock(29, 8);         // Step from Tier 1 to Tier 2

    m_tilemap[11][28] = TILE_GOLGOTHA_BRAZIER;
    m_tilemap[11][44] = TILE_GOLGOTHA_BRAZIER;
    m_tilemap[3][32] = TILE_GOLGOTHA_STANDARD;
    m_tilemap[3][38] = TILE_GOLGOTHA_STANDARD;

    // Sacred Relic #2: High Tier 2 Parapet at tile 35
    m_baskets.push_back({ { 35.0f * TILE_SIZE, 5.0f * TILE_SIZE }, false, 0.0f, 2 });

    // === SECTION 3: Fortress Antonia Approach (Tiles 48 - 70) ===
    AddRockColumn(50, 1, 12);
    AddRockColumn(58, 1, 12);
    AddRockColumn(66, 1, 12);

    AddCragLedge(48, 66, 9);     // Tier 1
    AddBattlement(52, 62, 6);    // Tier 2

    AddRockBlock(47, 11);
    AddRockBlock(51, 8);

    m_tilemap[11][49] = TILE_GOLGOTHA_BRAZIER;
    m_tilemap[11][65] = TILE_GOLGOTHA_BRAZIER;
    m_tilemap[3][54] = TILE_GOLGOTHA_STANDARD;
    m_tilemap[3][60] = TILE_GOLGOTHA_STANDARD;

    // Sacred Relic #3: Ground level alcove at tile 52
    m_baskets.push_back({ { 52.0f * TILE_SIZE, 11.0f * TILE_SIZE }, false, 0.0f, 0 });

    // Sacred Relic #4: High Tier 2 parapet at tile 57
    m_baskets.push_back({ { 57.0f * TILE_SIZE, 5.0f * TILE_SIZE }, false, 0.0f, 2 });

    // === SECTION 4: Fortress Antonia Gate / Iron Portcullis (Tiles 70 - 72, X = 1120) ===
    int gateTileX = static_cast<int>(m_arenaGateX / TILE_SIZE); // 70
    for (int ty = 2; ty <= 11; ++ty) {
        m_tilemap[ty][gateTileX] = TILE_GATE_FRAME;
        m_tilemap[ty][gateTileX + 1] = TILE_GATE_BARS;
        m_tilemap[ty][gateTileX + 2] = TILE_GATE_FRAME;
    }
    m_tilemap[11][gateTileX - 1] = TILE_GOLGOTHA_BRAZIER;

    // === SECTION 5: Summit Arena of Golgotha / Roman High Command (Tiles 73 - 96) ===
    AddRockColumn(76, 2, 12);
    AddRockColumn(84, 2, 12);
    AddRockColumn(92, 2, 12);
    m_tilemap[11][79] = TILE_GOLGOTHA_BRAZIER;
    m_tilemap[11][87] = TILE_GOLGOTHA_BRAZIER;
    m_tilemap[3][76] = TILE_GOLGOTHA_STANDARD;
    m_tilemap[3][84] = TILE_GOLGOTHA_STANDARD;
    m_tilemap[3][92] = TILE_GOLGOTHA_STANDARD;

    // Arena back wall at end of summit (Tile 96, X = 1536)
    int backWallX = static_cast<int>(m_arenaMaxX / TILE_SIZE);
    if (backWallX < LEVEL_TILES_X) {
        for (int ty = 0; ty <= 11; ++ty) {
            m_tilemap[ty][backWallX] = TILE_GATE_FRAME;
        }
    }
}

void Level::BuildValleyOfKidron() {
    // 1. Initialize tilemap with air
    for (int y = 0; y < LEVEL_TILES_Y; ++y) {
        for (int x = 0; x < LEVEL_TILES_X; ++x) {
            m_tilemap[y][x] = TILE_AIR;
        }
    }

    // 2. Solid Ground Tier (Dark Kidron Ravine Rocks) at Y = 12 and 13
    for (int x = 0; x < LEVEL_TILES_X; ++x) {
        m_tilemap[12][x] = TILE_KIDRON_ROCK_SURFACE;
        m_tilemap[13][x] = TILE_KIDRON_ROCK_SUB;
    }

    auto AddFootbridge = [this](int startTileX, int endTileX, int tileY) {
        float x = startTileX * TILE_SIZE;
        float y = tileY * TILE_SIZE;
        float w = (endTileX - startTileX + 1) * TILE_SIZE;
        m_oneWayPlatforms.push_back({ { x, y, w, 6.0f } });
        for (int tx = startTileX; tx <= endTileX; ++tx) {
            m_tilemap[tileY][tx] = TILE_KIDRON_BRIDGE_WOOD;
        }
    };

    auto AddCragLedge = [this](int startTileX, int endTileX, int tileY) {
        float x = startTileX * TILE_SIZE;
        float y = tileY * TILE_SIZE;
        float w = (endTileX - startTileX + 1) * TILE_SIZE;
        m_oneWayPlatforms.push_back({ { x, y, w, 6.0f } });
        for (int tx = startTileX; tx <= endTileX; ++tx) {
            m_tilemap[tileY][tx] = TILE_KIDRON_LEDGE;
        }
    };

    auto AddKidronColumn = [this](int tileX, int topTileY, int groundTileY) {
        m_tilemap[topTileY][tileX] = TILE_KIDRON_COLUMN_HEAD;
        for (int ty = topTileY + 1; ty < groundTileY - 1; ++ty) {
            m_tilemap[ty][tileX] = TILE_KIDRON_COLUMN_MID;
        }
        m_tilemap[groundTileY - 1][tileX] = TILE_KIDRON_COLUMN_BASE;
    };

    auto AddRockBlock = [this](int tileX, int tileY) {
        m_tilemap[tileY][tileX] = TILE_WOOD_CRATE;
        m_crates.push_back({ (float)tileX * TILE_SIZE, (float)tileY * TILE_SIZE, (float)TILE_SIZE, (float)TILE_SIZE });
    };

    // === SECTION 1: Ravine Descent (Tiles 0 - 35) ===
    AddKidronColumn(6, 4, 12);
    AddKidronColumn(14, 4, 12);
    AddKidronColumn(22, 4, 12);
    m_tilemap[11][4] = TILE_KIDRON_TORCH;
    m_tilemap[11][18] = TILE_KIDRON_TORCH;
    m_tilemap[11][30] = TILE_KIDRON_TORCH;

    AddFootbridge(10, 25, 8);
    AddRockBlock(8, 11);
    AddRockBlock(26, 11);

    // Silver Pitcher 1: Low ground crag
    m_baskets.push_back({ { 18.0f * TILE_SIZE, 11.0f * TILE_SIZE }, false, 0.0f, 0 });

    // === SECTION 2: Moonlit Chasm Span (Tiles 36 - 75) ===
    AddKidronColumn(38, 2, 12);
    AddKidronColumn(50, 2, 12);
    AddKidronColumn(62, 2, 12);
    AddKidronColumn(72, 2, 12);
    m_tilemap[11][42] = TILE_KIDRON_TORCH;
    m_tilemap[11][58] = TILE_KIDRON_TORCH;

    AddFootbridge(40, 64, 7);
    AddRockBlock(39, 11);
    AddRockBlock(65, 11);

    // Silver Pitcher 2: High atop the footbridge over the chasm
    m_baskets.push_back({ { 52.0f * TILE_SIZE, 5.8f * TILE_SIZE }, false, 0.0f, 1 });

    // === SECTION 3: Ancient Hasmonean Terraces (Tiles 76 - 115) ===
    AddKidronColumn(80, 2, 12);
    AddKidronColumn(94, 2, 12);
    AddKidronColumn(108, 2, 12);
    m_tilemap[11][82] = TILE_KIDRON_TORCH;
    m_tilemap[11][96] = TILE_KIDRON_TORCH;
    m_tilemap[11][110] = TILE_KIDRON_TORCH;

    AddCragLedge(78, 106, 9);
    AddCragLedge(84, 100, 6);
    AddRockBlock(77, 11);
    AddRockBlock(83, 8);

    // Silver Pitcher 3: High upper rock terrace
    m_baskets.push_back({ { 92.0f * TILE_SIZE, 4.8f * TILE_SIZE }, false, 0.0f, 2 });

    // === SECTION 4: Ravine Iron Portcullis Approach (Tiles 116 - 132) ===
    AddRockBlock(119, 11);
    AddRockBlock(123, 11);
    m_tilemap[11][125] = TILE_KIDRON_TORCH;

    // Silver Pitcher 4: Ground alcove near the gate
    m_baskets.push_back({ { 121.0f * TILE_SIZE, 11.0f * TILE_SIZE }, false, 0.0f, 0 });

    int gateTileX = static_cast<int>(m_arenaGateX / TILE_SIZE); // 132
    for (int ty = 2; ty <= 11; ++ty) {
        m_tilemap[ty][gateTileX] = TILE_GATE_FRAME;
        m_tilemap[ty][gateTileX + 1] = TILE_GATE_BARS;
        m_tilemap[ty][gateTileX + 2] = TILE_GATE_FRAME;
    }
    m_tilemap[11][gateTileX - 1] = TILE_KIDRON_TORCH;

    // === SECTION 5: Kidron Tormentor Arena (Tiles 134 - 158) ===
    AddKidronColumn(138, 2, 12);
    AddKidronColumn(146, 2, 12);
    AddKidronColumn(154, 2, 12);
    m_tilemap[11][141] = TILE_KIDRON_TORCH;
    m_tilemap[11][149] = TILE_KIDRON_TORCH;
    m_tilemap[11][156] = TILE_KIDRON_TORCH;

    int backWallX = static_cast<int>(m_arenaMaxX / TILE_SIZE);
    if (backWallX < LEVEL_TILES_X) {
        for (int ty = 0; ty <= 11; ++ty) {
            m_tilemap[ty][backWallX] = TILE_GATE_FRAME;
        }
    }
}

void Level::BuildCypressHedgeMaze() {
    // 1. Initialize tilemap with air
    for (int y = 0; y < LEVEL_TILES_Y; ++y) {
        for (int x = 0; x < LEVEL_TILES_X; ++x) {
            m_tilemap[y][x] = TILE_AIR;
        }
    }

    // 2. Solid Ground Tier (Marble Promenade Floor) at Y = 12 and 13
    for (int x = 0; x < LEVEL_TILES_X; ++x) {
        m_tilemap[12][x] = TILE_TEMPLE_FLOOR_CHECKER;
        m_tilemap[13][x] = TILE_TEMPLE_FLOOR_SUB;
    }

    auto AddHedgeLedge1 = [this](int startTileX, int endTileX, int tileY) {
        float x = startTileX * TILE_SIZE;
        float y = tileY * TILE_SIZE;
        float w = (endTileX - startTileX + 1) * TILE_SIZE;
        m_oneWayPlatforms.push_back({ { x, y, w, 6.0f } });
        for (int tx = startTileX; tx <= endTileX; ++tx) {
            m_tilemap[tileY][tx] = TILE_HEDGE_LEDGE_1;
        }
    };

    auto AddHedgeLedge2 = [this](int startTileX, int endTileX, int tileY) {
        float x = startTileX * TILE_SIZE;
        float y = tileY * TILE_SIZE;
        float w = (endTileX - startTileX + 1) * TILE_SIZE;
        m_oneWayPlatforms.push_back({ { x, y, w, 6.0f } });
        for (int tx = startTileX; tx <= endTileX; ++tx) {
            m_tilemap[tileY][tx] = TILE_HEDGE_LEDGE_2;
        }
    };

    auto AddUrnBlock = [this](int tileX, int tileY) {
        m_tilemap[tileY][tileX] = TILE_HEDGE_URN;
        m_crates.push_back({ (float)tileX * TILE_SIZE, (float)tileY * TILE_SIZE, (float)TILE_SIZE, (float)TILE_SIZE });
    };

    // === SECTION 1: Outer Topiary Courtyard (Tiles 0 - 35) ===
    m_tilemap[11][8] = TILE_HEDGE_FOUNTAIN;
    m_tilemap[11][14] = TILE_HEDGE_ARCH;
    m_tilemap[11][26] = TILE_HEDGE_FOUNTAIN;
    AddUrnBlock(12, 11);
    AddUrnBlock(30, 11);

    AddHedgeLedge1(16, 32, 7);
    AddUrnBlock(15, 11);

    // Olive Wreath 1: High topiary terrace
    m_baskets.push_back({ { 23.0f * TILE_SIZE, 5.8f * TILE_SIZE }, false, 0.0f, 1 });

    // === SECTION 2: Grand Fountain Promenade (Tiles 36 - 75) ===
    m_tilemap[11][42] = TILE_HEDGE_ARCH;
    m_tilemap[11][55] = TILE_HEDGE_FOUNTAIN;
    m_tilemap[11][68] = TILE_HEDGE_ARCH;

    AddHedgeLedge1(44, 70, 9);
    AddHedgeLedge2(50, 64, 6);
    AddUrnBlock(43, 11);
    AddUrnBlock(49, 8);

    // Olive Wreath 2: Beside the marble fountain
    m_baskets.push_back({ { 55.0f * TILE_SIZE, 11.0f * TILE_SIZE }, false, 0.0f, 0 });

    // === SECTION 3: Inner Cypress Labyrinth (Tiles 76 - 115) ===
    m_tilemap[11][82] = TILE_HEDGE_FOUNTAIN;
    m_tilemap[11][96] = TILE_HEDGE_ARCH;
    m_tilemap[11][110] = TILE_HEDGE_FOUNTAIN;

    AddHedgeLedge1(78, 108, 9);
    AddHedgeLedge2(84, 102, 6);
    AddUrnBlock(82, 11);
    AddUrnBlock(104, 11);

    // Olive Wreath 3: High Tier 2 hedge overlook
    m_baskets.push_back({ { 93.0f * TILE_SIZE, 4.8f * TILE_SIZE }, false, 0.0f, 2 });

    // === SECTION 4: Maze Exit Checkpoint (Tiles 116 - 132) ===
    m_tilemap[11][118] = TILE_HEDGE_ARCH;
    m_tilemap[11][126] = TILE_HEDGE_ARCH;
    AddUrnBlock(120, 11);

    // Olive Wreath 4: Ground corridor before the gate
    m_baskets.push_back({ { 122.0f * TILE_SIZE, 11.0f * TILE_SIZE }, false, 0.0f, 0 });

    int gateTileX = static_cast<int>(m_arenaGateX / TILE_SIZE); // 132
    for (int ty = 2; ty <= 11; ++ty) {
        m_tilemap[ty][gateTileX] = TILE_GATE_FRAME;
        m_tilemap[ty][gateTileX + 1] = TILE_GATE_BARS;
        m_tilemap[ty][gateTileX + 2] = TILE_GATE_FRAME;
    }

    // === SECTION 5: High Inquisitor's Marble Plaza (Tiles 134 - 158) ===
    m_tilemap[11][140] = TILE_HEDGE_ARCH;
    m_tilemap[11][146] = TILE_HEDGE_FOUNTAIN;
    m_tilemap[11][152] = TILE_HEDGE_ARCH;
    AddUrnBlock(138, 11);
    AddUrnBlock(154, 11);

    int backWallX = static_cast<int>(m_arenaMaxX / TILE_SIZE);
    if (backWallX < LEVEL_TILES_X) {
        for (int ty = 0; ty <= 11; ++ty) {
            m_tilemap[ty][backWallX] = TILE_GATE_FRAME;
        }
    }
}

void Level::BuildGardenOfGethsemane() {
    // 1. Initialize tilemap with air
    for (int y = 0; y < LEVEL_TILES_Y; ++y) {
        for (int x = 0; x < LEVEL_TILES_X; ++x) {
            m_tilemap[y][x] = TILE_AIR;
        }
    }

    // 2. Solid Ground Tier (Gethsemane Earth & Mossy Stones) at Y = 12 and 13
    for (int x = 0; x < LEVEL_TILES_X; ++x) {
        m_tilemap[12][x] = TILE_GETHSEMANE_EARTH_SURFACE;
        m_tilemap[13][x] = TILE_GETHSEMANE_EARTH_SUB;
    }

    auto AddOliveTree = [this](int trunkX, int branchLeftX, int branchRightX, int branchY) {
        // Vertical gnarled trunk
        for (int ty = branchY; ty < 12; ++ty) {
            m_tilemap[ty][trunkX] = TILE_GETHSEMANE_OLIVE_TRUNK;
        }
        // One-way platform across branch canopy
        float x = branchLeftX * TILE_SIZE;
        float y = branchY * TILE_SIZE;
        float w = (branchRightX - branchLeftX + 1) * TILE_SIZE;
        m_oneWayPlatforms.push_back({ { x, y, w, 6.0f } });

        for (int tx = branchLeftX; tx <= branchRightX; ++tx) {
            m_tilemap[branchY][tx] = (tx == trunkX) ? TILE_GETHSEMANE_OLIVE_TRUNK : TILE_GETHSEMANE_OLIVE_BRANCH;
            if (branchY > 0) {
                m_tilemap[branchY - 1][tx] = TILE_GETHSEMANE_OLIVE_LEAVES;
            }
        }
    };

    auto AddStoneLedge = [this](int startTileX, int endTileX, int tileY) {
        float x = startTileX * TILE_SIZE;
        float y = tileY * TILE_SIZE;
        float w = (endTileX - startTileX + 1) * TILE_SIZE;
        m_oneWayPlatforms.push_back({ { x, y, w, 6.0f } });
        for (int tx = startTileX; tx <= endTileX; ++tx) {
            m_tilemap[tileY][tx] = TILE_GETHSEMANE_LEDGE;
        }
    };

    auto AddStoneBlock = [this](int tileX, int tileY) {
        m_tilemap[tileY][tileX] = TILE_WOOD_CRATE;
        m_crates.push_back({ (float)tileX * TILE_SIZE, (float)tileY * TILE_SIZE, (float)TILE_SIZE, (float)TILE_SIZE });
    };

    // === SECTION 1: Entrance Grove (Tiles 0 - 30) ===
    AddOliveTree(12, 7, 18, 8);
    m_tilemap[8][9] = TILE_GETHSEMANE_LANTERN;
    m_tilemap[11][22] = TILE_GETHSEMANE_LANTERN;
    AddStoneBlock(6, 11);
    AddStoneBlock(20, 11);

    // Sacred Chalice 1: Under the first olive tree
    m_baskets.push_back({ { 15.0f * TILE_SIZE, 11.0f * TILE_SIZE }, false, 0.0f, 0 });

    // === SECTION 2: Ancient Grove & Stone Terraces (Tiles 31 - 65) ===
    AddOliveTree(44, 36, 52, 7);
    m_tilemap[7][38] = TILE_GETHSEMANE_LANTERN;
    m_tilemap[7][50] = TILE_GETHSEMANE_LANTERN;

    AddStoneLedge(48, 62, 9);
    AddStoneBlock(47, 11);

    // Sacred Chalice 2: High atop the sacred olive branch
    m_baskets.push_back({ { 44.0f * TILE_SIZE, 5.8f * TILE_SIZE }, false, 0.0f, 1 });

    // === SECTION 3: The Moonlit Path to the Agony (Tiles 66 - 98) ===
    AddStoneLedge(68, 82, 8);
    AddOliveTree(80, 74, 88, 7);
    m_tilemap[8][70] = TILE_GETHSEMANE_LANTERN;
    m_tilemap[7][84] = TILE_GETHSEMANE_LANTERN;
    AddStoneBlock(67, 11);

    // Sacred Chalice 3: Stone terrace ledge
    m_baskets.push_back({ { 75.0f * TILE_SIZE, 6.8f * TILE_SIZE }, false, 0.0f, 1 });

    // Sacred Chalice 4: Ground alcove before the sanctuary gate
    m_baskets.push_back({ { 90.0f * TILE_SIZE, 11.0f * TILE_SIZE }, false, 0.0f, 0 });

    // === SECTION 4: Gethsemane Sanctuary Gate (Tile 100, X = 1600) ===
    int gateTileX = static_cast<int>(m_arenaGateX / TILE_SIZE); // 100
    for (int ty = 2; ty <= 11; ++ty) {
        m_tilemap[ty][gateTileX] = TILE_GATE_FRAME;
        m_tilemap[ty][gateTileX + 1] = TILE_GATE_BARS;
        m_tilemap[ty][gateTileX + 2] = TILE_GATE_FRAME;
    }
    m_tilemap[11][gateTileX - 1] = TILE_GETHSEMANE_LANTERN;

    // === SECTION 5: The Stone of Agony & Ancient Olive Tree Arena (Tiles 103 - 126, X = 1620 to 2020) ===
    // The Great Olive Tree of Gethsemane spreading across the entire arena:
    AddOliveTree(114, 106, 122, 7);

    // The Sacred Stone of Agony / Altar where Jesus prayed:
    m_tilemap[11][114] = TILE_GETHSEMANE_ALTAR_STONE;

    // Lanterns hanging over the arena
    m_tilemap[7][108] = TILE_GETHSEMANE_LANTERN;
    m_tilemap[6][114] = TILE_GETHSEMANE_LANTERN;
    m_tilemap[7][120] = TILE_GETHSEMANE_LANTERN;

    int backWallX = static_cast<int>(m_arenaMaxX / TILE_SIZE);
    if (backWallX < LEVEL_TILES_X) {
        for (int ty = 0; ty <= 11; ++ty) {
            m_tilemap[ty][backWallX] = TILE_GATE_FRAME;
        }
    }
}

void Level::BuildSiloamBazaar() {
    // 1. Initialize tilemap with air
    for (int y = 0; y < LEVEL_TILES_Y; ++y) {
        for (int x = 0; x < LEVEL_TILES_X; ++x) {
            m_tilemap[y][x] = TILE_AIR;
        }
    }

    // 2. Lay solid sandstone ground tier
    for (int x = 0; x < LEVEL_TILES_X; ++x) {
        m_tilemap[12][x] = TILE_BAZAAR_STONE_SURFACE;
        m_tilemap[13][x] = TILE_BAZAAR_STONE_SUB;
    }

    auto AddStallLedge = [this](int startTileX, int endTileX, int tileY, bool isGold) {
        float x = startTileX * TILE_SIZE;
        float y = tileY * TILE_SIZE;
        float w = (endTileX - startTileX + 1) * TILE_SIZE;
        m_oneWayPlatforms.push_back({ { x, y, w, 6.0f } });

        for (int tx = startTileX; tx <= endTileX; ++tx) {
            m_tilemap[tileY][tx] = TILE_BAZAAR_LEDGE;
            if (tileY > 0) {
                m_tilemap[tileY - 1][tx] = isGold ? TILE_BAZAAR_AWNING_GOLD : TILE_BAZAAR_AWNING_RED;
            }
        }
    };

    auto AddCrate = [this](int tileX, int tileY, TileId type) {
        m_tilemap[tileY][tileX] = type;
        m_crates.push_back({ (float)tileX * TILE_SIZE, (float)tileY * TILE_SIZE, (float)TILE_SIZE, (float)TILE_SIZE });
    };

    // === SECTION 1: Silk & Spice Merchants (Tiles 0 - 38) ===
    AddStallLedge(8, 16, 9, false);
    m_tilemap[8][10] = TILE_BAZAAR_LANTERN;
    m_tilemap[8][15] = TILE_BAZAAR_LANTERN;
    AddCrate(7, 11, TILE_BAZAAR_SPICE_SACK);
    AddCrate(17, 11, TILE_BAZAAR_POTTERY);

    // Bitter Herbs 1
    m_baskets.push_back({ { 12.0f * TILE_SIZE, 11.0f * TILE_SIZE }, false, 0.0f, 0 });

    AddStallLedge(24, 34, 7, true);
    m_tilemap[6][26] = TILE_BAZAAR_LANTERN;
    m_tilemap[6][32] = TILE_BAZAAR_LANTERN;
    AddCrate(23, 11, TILE_BAZAAR_POTTERY);
    AddCrate(29, 11, TILE_BAZAAR_SPICE_SACK);

    // === SECTION 2: Upper Aqueduct & Pottery Bazaar (Tiles 39 - 80) ===
    AddStallLedge(42, 54, 8, false);
    AddStallLedge(58, 70, 6, true);
    m_tilemap[5][60] = TILE_BAZAAR_LANTERN;
    m_tilemap[5][68] = TILE_BAZAAR_LANTERN;
    AddCrate(46, 11, TILE_BAZAAR_SPICE_SACK);
    AddCrate(64, 11, TILE_BAZAAR_POTTERY);

    // Bitter Herbs 2: High awning platform
    m_baskets.push_back({ { 64.0f * TILE_SIZE, 4.8f * TILE_SIZE }, false, 0.0f, 1 });

    // === SECTION 3: Siloam Stone Terraces (Tiles 81 - 128) ===
    AddStallLedge(84, 96, 8, false);
    AddStallLedge(102, 114, 7, true);
    m_tilemap[7][88] = TILE_BAZAAR_LANTERN;
    m_tilemap[6][106] = TILE_BAZAAR_LANTERN;
    AddCrate(83, 11, TILE_BAZAAR_POTTERY);
    AddCrate(108, 11, TILE_BAZAAR_SPICE_SACK);

    // Bitter Herbs 3: High terrace
    m_baskets.push_back({ { 90.0f * TILE_SIZE, 6.8f * TILE_SIZE }, false, 0.0f, 1 });

    // Bitter Herbs 4: Merchant alcove before gate
    m_baskets.push_back({ { 122.0f * TILE_SIZE, 11.0f * TILE_SIZE }, false, 0.0f, 0 });

    // === SECTION 4: Gate to Bandit Hideout (Tile 132, X = 2120) ===
    int gateTileX = static_cast<int>(m_arenaGateX / TILE_SIZE);
    for (int ty = 2; ty <= 11; ++ty) {
        m_tilemap[ty][gateTileX] = TILE_GATE_FRAME;
        m_tilemap[ty][gateTileX + 1] = TILE_GATE_BARS;
        m_tilemap[ty][gateTileX + 2] = TILE_GATE_FRAME;
    }
    m_tilemap[11][gateTileX - 1] = TILE_BAZAAR_LANTERN;

    // === SECTION 5: Barabbas's Bandit Redoubt Arena (Tiles 135 - 158) ===
    AddStallLedge(140, 150, 7, false);
    m_tilemap[6][142] = TILE_BAZAAR_LANTERN;
    m_tilemap[6][148] = TILE_BAZAAR_LANTERN;
    AddCrate(138, 11, TILE_BAZAAR_SPICE_SACK);
    AddCrate(152, 11, TILE_BAZAAR_POTTERY);

    int backWallX = static_cast<int>(m_arenaMaxX / TILE_SIZE);
    if (backWallX < LEVEL_TILES_X) {
        for (int ty = 0; ty <= 11; ++ty) {
            m_tilemap[ty][backWallX] = TILE_GATE_FRAME;
        }
    }
}

void Level::BuildMountZionPalace() {
    // 1. Initialize tilemap with air
    for (int y = 0; y < LEVEL_TILES_Y; ++y) {
        for (int x = 0; x < LEVEL_TILES_X; ++x) {
            m_tilemap[y][x] = TILE_AIR;
        }
    }

    // 2. Lay solid marble ground tier
    for (int x = 0; x < LEVEL_TILES_X; ++x) {
        m_tilemap[12][x] = TILE_CAIAPHAS_MARBLE_SURFACE;
        m_tilemap[13][x] = TILE_CAIAPHAS_MARBLE_SUB;
    }

    auto AddBalustrade = [this](int startTileX, int endTileX, int tileY) {
        float x = startTileX * TILE_SIZE;
        float y = tileY * TILE_SIZE;
        float w = (endTileX - startTileX + 1) * TILE_SIZE;
        m_oneWayPlatforms.push_back({ { x, y, w, 6.0f } });

        for (int tx = startTileX; tx <= endTileX; ++tx) {
            m_tilemap[tileY][tx] = TILE_CAIAPHAS_BALUSTRADE;
        }
    };

    auto AddColumn = [this](int tileX, int topY, int bottomY) {
        m_tilemap[topY][tileX] = TILE_CAIAPHAS_COLUMN_HEAD;
        for (int ty = topY + 1; ty < bottomY; ++ty) {
            m_tilemap[ty][tileX] = TILE_CAIAPHAS_COLUMN_MID;
        }
        m_tilemap[bottomY][tileX] = TILE_CAIAPHAS_COLUMN_BASE;
    };

    auto AddBrazierBlock = [this](int tileX, int tileY) {
        m_tilemap[tileY][tileX] = TILE_CAIAPHAS_BRAZIER;
        m_crates.push_back({ (float)tileX * TILE_SIZE, (float)tileY * TILE_SIZE, (float)TILE_SIZE, (float)TILE_SIZE });
    };

    // === SECTION 1: Outer Colonnaded Courtyard (Tiles 0 - 38) ===
    AddColumn(6, 6, 11);
    AddColumn(18, 6, 11);
    AddBalustrade(8, 16, 8);
    AddBrazierBlock(5, 11);
    AddBrazierBlock(19, 11);

    // Passover Matzot 1
    m_baskets.push_back({ { 12.0f * TILE_SIZE, 11.0f * TILE_SIZE }, false, 0.0f, 0 });

    // === SECTION 2: Royal Mezzanine & Grand Portico (Tiles 39 - 80) ===
    AddColumn(36, 4, 11);
    AddColumn(48, 4, 11);
    AddColumn(60, 4, 11);
    AddBalustrade(38, 58, 6);
    AddBrazierBlock(35, 11);
    AddBrazierBlock(61, 11);

    // Passover Matzot 2: High balcony
    m_baskets.push_back({ { 48.0f * TILE_SIZE, 4.8f * TILE_SIZE }, false, 0.0f, 1 });

    // === SECTION 3: High Council Terrace (Tiles 81 - 128) ===
    AddBalustrade(78, 92, 8);
    AddBalustrade(98, 112, 7);
    AddColumn(94, 5, 11);
    AddColumn(114, 5, 11);
    AddBrazierBlock(84, 11);
    AddBrazierBlock(106, 11);

    // Passover Matzot 3: Council terrace
    m_baskets.push_back({ { 86.0f * TILE_SIZE, 6.8f * TILE_SIZE }, false, 0.0f, 1 });

    // Passover Matzot 4: Guard plinth before council hall
    m_baskets.push_back({ { 124.0f * TILE_SIZE, 11.0f * TILE_SIZE }, false, 0.0f, 0 });

    // === SECTION 4: High Priest Audience Gate (Tile 132, X = 2120) ===
    int gateTileX = static_cast<int>(m_arenaGateX / TILE_SIZE);
    for (int ty = 2; ty <= 11; ++ty) {
        m_tilemap[ty][gateTileX] = TILE_GATE_FRAME;
        m_tilemap[ty][gateTileX + 1] = TILE_CAIAPHAS_GATE;
        m_tilemap[ty][gateTileX + 2] = TILE_GATE_FRAME;
    }
    m_tilemap[11][gateTileX - 1] = TILE_CAIAPHAS_BRAZIER;

    // === SECTION 5: High Priest Judgment Hall Arena (Tiles 135 - 158) ===
    AddColumn(138, 3, 11);
    AddColumn(152, 3, 11);
    AddBalustrade(140, 150, 7);
    AddBrazierBlock(136, 11);
    AddBrazierBlock(154, 11);

    int backWallX = static_cast<int>(m_arenaMaxX / TILE_SIZE);
    if (backWallX < LEVEL_TILES_X) {
        for (int ty = 0; ty <= 11; ++ty) {
            m_tilemap[ty][backWallX] = TILE_GATE_FRAME;
        }
    }
}

void Level::BuildUpperRoomCenacle() {
    // 1. Initialize tilemap with air
    for (int y = 0; y < LEVEL_TILES_Y; ++y) {
        for (int x = 0; x < LEVEL_TILES_X; ++x) {
            m_tilemap[y][x] = TILE_AIR;
        }
    }

    // 2. Lay solid cedar wood plank floor
    for (int x = 0; x < LEVEL_TILES_X; ++x) {
        m_tilemap[12][x] = TILE_CENACLE_WOOD_FLOOR;
        m_tilemap[13][x] = TILE_CENACLE_WOOD_SUB;
    }

    auto AddBeamPlatform = [this](int startTileX, int endTileX, int tileY) {
        float x = startTileX * TILE_SIZE;
        float y = tileY * TILE_SIZE;
        float w = (endTileX - startTileX + 1) * TILE_SIZE;
        m_oneWayPlatforms.push_back({ { x, y, w, 6.0f } });

        for (int tx = startTileX; tx <= endTileX; ++tx) {
            m_tilemap[tileY][tx] = TILE_CENACLE_BEAM_ARCH;
        }
    };

    auto AddCommunionTable = [this](int startTileX, int endTileX, int tileY) {
        float x = startTileX * TILE_SIZE;
        float y = tileY * TILE_SIZE;
        float w = (endTileX - startTileX + 1) * TILE_SIZE;
        m_oneWayPlatforms.push_back({ { x, y, w, 6.0f } });

        for (int tx = startTileX; tx <= endTileX; ++tx) {
            m_tilemap[tileY][tx] = TILE_CENACLE_TABLE_CLOTH;
            if (tileY > 0 && (tx % 2 == 0)) {
                m_tilemap[tileY - 1][tx] = TILE_CENACLE_CHALICE_ROW;
            }
        }
    };

    auto AddChamberBlock = [this](int tileX, int tileY) {
        m_tilemap[tileY][tileX] = TILE_CENACLE_CANDELABRA;
        m_crates.push_back({ (float)tileX * TILE_SIZE, (float)tileY * TILE_SIZE, (float)TILE_SIZE, (float)TILE_SIZE });
    };

    // Arched windows showing Passover moon in background
    for (int wx = 10; wx < 85; wx += 18) {
        m_tilemap[3][wx] = TILE_CENACLE_WINDOW_MOON;
        m_tilemap[3][wx + 1] = TILE_CENACLE_WINDOW_MOON;
    }

    // === SECTION 1: Outer Vestibule & Timber Mezzanine (Tiles 0 - 30) ===
    AddBeamPlatform(8, 20, 8);
    m_tilemap[7][10] = TILE_CENACLE_CANDELABRA;
    m_tilemap[7][18] = TILE_CENACLE_CANDELABRA;
    AddChamberBlock(6, 11);
    AddChamberBlock(22, 11);

    // Holy Grail 1: Communion chalice on entrance table
    m_baskets.push_back({ { 14.0f * TILE_SIZE, 11.0f * TILE_SIZE }, false, 0.0f, 0 });

    // === SECTION 2: Central Vaulted Chamber (Tiles 31 - 60) ===
    AddBeamPlatform(34, 48, 7);
    AddCommunionTable(38, 46, 9);
    m_tilemap[6][36] = TILE_CENACLE_CANDELABRA;
    m_tilemap[6][46] = TILE_CENACLE_CANDELABRA;
    AddChamberBlock(32, 11);
    AddChamberBlock(50, 11);

    // Holy Grail 2: High timber beam platform
    m_baskets.push_back({ { 41.0f * TILE_SIZE, 5.8f * TILE_SIZE }, false, 0.0f, 1 });

    // === SECTION 3: Upper Room Passage (Tiles 61 - 88) ===
    AddBeamPlatform(64, 76, 8);
    m_tilemap[7][66] = TILE_CENACLE_CANDELABRA;
    m_tilemap[7][74] = TILE_CENACLE_CANDELABRA;
    AddChamberBlock(62, 11);

    // Holy Grail 3: Upper gallery
    m_baskets.push_back({ { 70.0f * TILE_SIZE, 6.8f * TILE_SIZE }, false, 0.0f, 1 });

    // Holy Grail 4: Table before the Cenacle sanctuary gate
    m_baskets.push_back({ { 84.0f * TILE_SIZE, 11.0f * TILE_SIZE }, false, 0.0f, 0 });

    // === SECTION 4: Cenacle Sanctuary Gate (Tile 90, X = 1440) ===
    int gateTileX = static_cast<int>(m_arenaGateX / TILE_SIZE);
    for (int ty = 2; ty <= 11; ++ty) {
        m_tilemap[ty][gateTileX] = TILE_CENACLE_STONE_WALL;
        m_tilemap[ty][gateTileX + 1] = TILE_GATE_BARS;
        m_tilemap[ty][gateTileX + 2] = TILE_CENACLE_STONE_WALL;
    }
    m_tilemap[11][gateTileX - 1] = TILE_CENACLE_CANDELABRA;

    // === SECTION 5: The Last Supper Communion Table Arena (Tiles 93 - 116, X = 1460 to 1860) ===
    // Sacred Communion Table spanning the center of the Upper Room
    AddCommunionTable(98, 110, 9);
    AddBeamPlatform(96, 112, 6); // High vaulted beams overhead

    // Menorahs illuminating the Last Supper Table
    m_tilemap[8][99] = TILE_CENACLE_CANDELABRA;
    m_tilemap[8][104] = TILE_CENACLE_CANDELABRA;
    m_tilemap[8][109] = TILE_CENACLE_CANDELABRA;

    // Arched Passover window over the arena
    m_tilemap[3][103] = TILE_CENACLE_WINDOW_MOON;
    m_tilemap[3][104] = TILE_CENACLE_WINDOW_MOON;

    int backWallX = static_cast<int>(m_arenaMaxX / TILE_SIZE);
    if (backWallX < LEVEL_TILES_X) {
        for (int ty = 0; ty <= 11; ++ty) {
            m_tilemap[ty][backWallX] = TILE_CENACLE_STONE_WALL;
        }
    }
}

void Level::BuildViaDolorosa() {
    // 1. Initialize tilemap with air
    for (int y = 0; y < LEVEL_TILES_Y; ++y) {
        for (int x = 0; x < LEVEL_TILES_X; ++x) {
            m_tilemap[y][x] = TILE_AIR;
        }
    }

    // 2. Cobblestone Paved Roman Road
    for (int x = 0; x < LEVEL_TILES_X; ++x) {
        m_tilemap[12][x] = TILE_DOLOROSA_STONE_SURFACE;
        m_tilemap[13][x] = TILE_DOLOROSA_STONE_SUB;
    }

    auto AddScaffoldPlatform = [this](int startTileX, int endTileX, int tileY) {
        float x = startTileX * TILE_SIZE;
        float y = tileY * TILE_SIZE;
        float w = (endTileX - startTileX + 1) * TILE_SIZE;
        m_oneWayPlatforms.push_back({ { x, y, w, 6.0f } });
        for (int tx = startTileX; tx <= endTileX; ++tx) {
            m_tilemap[tileY][tx] = TILE_DOLOROSA_BALCONY;
        }
    };

    auto AddAwningPlatform = [this](int startTileX, int endTileX, int tileY) {
        float x = startTileX * TILE_SIZE;
        float y = tileY * TILE_SIZE;
        float w = (endTileX - startTileX + 1) * TILE_SIZE;
        m_oneWayPlatforms.push_back({ { x, y, w, 6.0f } });
        for (int tx = startTileX; tx <= endTileX; ++tx) {
            m_tilemap[tileY][tx] = TILE_DOLOROSA_BALCONY;
        }
    };

    auto AddBarricadeBlock = [this](int tileX, int tileY) {
        m_tilemap[tileY][tileX] = TILE_DOLOROSA_BARRICADE;
        m_crates.push_back({ (float)tileX * TILE_SIZE, (float)tileY * TILE_SIZE, (float)TILE_SIZE, (float)TILE_SIZE });
    };

    auto AddArch = [this](int tileX, int topTileY, int groundTileY) {
        m_tilemap[topTileY][tileX] = TILE_DOLOROSA_ARCH_ECCE;
        for (int ty = topTileY + 1; ty < groundTileY; ++ty) {
            m_tilemap[ty][tileX] = TILE_COLUMN_MID;
        }
    };

    // === SECTION 1: Outer Roman Street & Aqueducts (Tiles 0 - 35) ===
    AddArch(6, 4, 12);
    AddArch(16, 4, 12);
    AddArch(28, 4, 12);
    m_tilemap[11][10] = TILE_DOLOROSA_TORCH_BRACKET;
    m_tilemap[11][22] = TILE_DOLOROSA_TORCH_BRACKET;
    AddBarricadeBlock(12, 11);
    AddBarricadeBlock(26, 11);

    AddScaffoldPlatform(14, 26, 8);
    // Roman Scroll 1: High scaffolding above street barricade
    m_baskets.push_back({ { 20.0f * TILE_SIZE, 6.8f * TILE_SIZE }, false, 0.0f, 1 });

    // === SECTION 2: Merchant Awnings & Barricaded Squares (Tiles 36 - 75) ===
    AddArch(40, 3, 12);
    AddArch(52, 3, 12);
    AddArch(66, 3, 12);
    m_tilemap[11][44] = TILE_DOLOROSA_TORCH_BRACKET;
    m_tilemap[11][60] = TILE_DOLOROSA_TORCH_BRACKET;

    AddScaffoldPlatform(38, 64, 9);
    AddAwningPlatform(44, 58, 6);
    AddBarricadeBlock(37, 11);
    AddBarricadeBlock(43, 8);

    // Roman Scroll 2: High merchant awning
    m_baskets.push_back({ { 50.0f * TILE_SIZE, 4.8f * TILE_SIZE }, false, 0.0f, 1 });

    // === SECTION 3: Antonia Garrison Checkpoint (Tiles 76 - 115) ===
    AddArch(80, 4, 12);
    AddArch(94, 4, 12);
    AddArch(108, 4, 12);
    m_tilemap[11][84] = TILE_DOLOROSA_TORCH_BRACKET;
    m_tilemap[11][102] = TILE_DOLOROSA_TORCH_BRACKET;

    AddScaffoldPlatform(78, 106, 8);
    AddBarricadeBlock(77, 11);
    AddBarricadeBlock(90, 11);
    AddBarricadeBlock(104, 11);

    // Roman Scroll 3: Upper garrison scaffold
    m_baskets.push_back({ { 92.0f * TILE_SIZE, 6.8f * TILE_SIZE }, false, 0.0f, 1 });

    // === SECTION 4: Praetor's Pavillion Approach (Tiles 116 - 132) ===
    AddBarricadeBlock(118, 11);
    AddBarricadeBlock(124, 11);
    m_tilemap[11][120] = TILE_DOLOROSA_TORCH_BRACKET;
    m_tilemap[11][128] = TILE_DOLOROSA_TORCH_BRACKET;

    // Roman Scroll 4: Street barricade before the judgment gate
    m_baskets.push_back({ { 122.0f * TILE_SIZE, 11.0f * TILE_SIZE }, false, 0.0f, 0 });

    // Arena Gate at Tile 132 (X = 2120)
    int gateTileX = static_cast<int>(m_arenaGateX / TILE_SIZE);
    for (int ty = 2; ty <= 11; ++ty) {
        m_tilemap[ty][gateTileX] = TILE_GATE_FRAME;
        m_tilemap[ty][gateTileX + 1] = TILE_DOLOROSA_GATE;
        m_tilemap[ty][gateTileX + 2] = TILE_GATE_FRAME;
    }
    m_tilemap[11][gateTileX - 1] = TILE_DOLOROSA_TORCH_BRACKET;

    // === SECTION 5: Praetor Prefect Quintus Arena (Tiles 135 - 158) ===
    AddArch(138, 3, 12);
    AddArch(148, 3, 12);
    AddArch(156, 3, 12);
    AddScaffoldPlatform(140, 154, 7);
    m_tilemap[11][142] = TILE_DOLOROSA_TORCH_BRACKET;
    m_tilemap[11][152] = TILE_DOLOROSA_TORCH_BRACKET;

    int backWallX = static_cast<int>(m_arenaMaxX / TILE_SIZE);
    if (backWallX < LEVEL_TILES_X) {
        for (int ty = 0; ty <= 11; ++ty) {
            m_tilemap[ty][backWallX] = TILE_GATE_FRAME;
        }
    }
}

void Level::BuildTheGabbathaCourt() {
    // 1. Initialize tilemap with air
    for (int y = 0; y < LEVEL_TILES_Y; ++y) {
        for (int x = 0; x < LEVEL_TILES_X; ++x) {
            m_tilemap[y][x] = TILE_AIR;
        }
    }

    // 2. Mosaic Tesserae Pavement Floor (Lithostrotos)
    for (int x = 0; x < LEVEL_TILES_X; ++x) {
        m_tilemap[12][x] = TILE_COURT_MOSAIC_SURFACE;
        m_tilemap[13][x] = TILE_COURT_MOSAIC_SUB;
    }

    auto AddBalconyPlatform = [this](int startTileX, int endTileX, int tileY) {
        float x = startTileX * TILE_SIZE;
        float y = tileY * TILE_SIZE;
        float w = (endTileX - startTileX + 1) * TILE_SIZE;
        m_oneWayPlatforms.push_back({ { x, y, w, 6.0f } });
        for (int tx = startTileX; tx <= endTileX; ++tx) {
            m_tilemap[tileY][tx] = TILE_COURT_BALUSTRADE;
        }
    };

    auto AddBenchBlock = [this](int tileX, int tileY) {
        m_tilemap[tileY][tileX] = TILE_COURT_BENCH_WOOD;
        m_crates.push_back({ (float)tileX * TILE_SIZE, (float)tileY * TILE_SIZE, (float)TILE_SIZE, (float)TILE_SIZE });
    };

    auto AddCourtColumn = [this](int tileX, int topTileY, int groundTileY) {
        m_tilemap[topTileY][tileX] = TILE_COURT_COLUMN_HEAD;
        for (int ty = topTileY + 1; ty < groundTileY; ++ty) {
            m_tilemap[ty][tileX] = TILE_COURT_COLUMN_MID;
        }
    };

    // === SECTION 1: Outer Basilica Portico (Tiles 0 - 35) ===
    AddCourtColumn(6, 3, 12);
    AddCourtColumn(18, 3, 12);
    AddCourtColumn(30, 3, 12);
    m_tilemap[11][10] = TILE_COURT_EAGLE_PEDESTAL;
    m_tilemap[11][24] = TILE_COURT_EAGLE_PEDESTAL;
    AddBenchBlock(12, 11);
    AddBenchBlock(22, 11);

    AddBalconyPlatform(10, 26, 8);
    // Caesar Seal 1: Outer basilica magistrate bench
    m_baskets.push_back({ { 16.0f * TILE_SIZE, 11.0f * TILE_SIZE }, false, 0.0f, 0 });

    // === SECTION 2: Lithostrotos Great Tribunal (Tiles 36 - 75) ===
    AddCourtColumn(40, 2, 12);
    AddCourtColumn(54, 2, 12);
    AddCourtColumn(68, 2, 12);
    m_tilemap[11][44] = TILE_COURT_EAGLE_PEDESTAL;
    m_tilemap[11][62] = TILE_COURT_EAGLE_PEDESTAL;

    AddBalconyPlatform(38, 70, 9);
    AddBalconyPlatform(46, 62, 6);
    AddBenchBlock(37, 11);
    AddBenchBlock(45, 8);

    // Caesar Seal 2: High tribunal balcony
    m_baskets.push_back({ { 54.0f * TILE_SIZE, 4.8f * TILE_SIZE }, false, 0.0f, 1 });

    // === SECTION 3: Advocate Assembly Hall (Tiles 76 - 115) ===
    AddCourtColumn(82, 3, 12);
    AddCourtColumn(96, 3, 12);
    AddCourtColumn(110, 3, 12);
    m_tilemap[11][88] = TILE_COURT_EAGLE_PEDESTAL;
    m_tilemap[11][104] = TILE_COURT_EAGLE_PEDESTAL;

    AddBalconyPlatform(80, 108, 8);
    AddBenchBlock(79, 11);
    AddBenchBlock(92, 11);
    AddBenchBlock(106, 11);

    // Caesar Seal 3: Assembly hall gallery
    m_baskets.push_back({ { 94.0f * TILE_SIZE, 6.8f * TILE_SIZE }, false, 0.0f, 1 });

    // === SECTION 4: Court Gate Approach (Tiles 116 - 132) ===
    AddBenchBlock(118, 11);
    AddBenchBlock(126, 11);
    m_tilemap[11][122] = TILE_COURT_EAGLE_PEDESTAL;

    // Caesar Seal 4: Magistrate bench before the tribunal gate
    m_baskets.push_back({ { 124.0f * TILE_SIZE, 11.0f * TILE_SIZE }, false, 0.0f, 0 });

    // Tribunal Gate at Tile 132 (X = 2120)
    int gateTileX = static_cast<int>(m_arenaGateX / TILE_SIZE);
    for (int ty = 2; ty <= 11; ++ty) {
        m_tilemap[ty][gateTileX] = TILE_GATE_FRAME;
        m_tilemap[ty][gateTileX + 1] = TILE_GATE_BARS;
        m_tilemap[ty][gateTileX + 2] = TILE_GATE_FRAME;
    }
    m_tilemap[11][gateTileX - 1] = TILE_COURT_EAGLE_PEDESTAL;

    // === SECTION 5: Imperial Advocate Tertullus Arena (Tiles 135 - 158) ===
    AddCourtColumn(138, 2, 12);
    AddCourtColumn(148, 2, 12);
    AddCourtColumn(156, 2, 12);
    AddBalconyPlatform(140, 154, 7);
    m_tilemap[11][142] = TILE_COURT_EAGLE_PEDESTAL;
    m_tilemap[11][152] = TILE_COURT_EAGLE_PEDESTAL;

    int backWallX = static_cast<int>(m_arenaMaxX / TILE_SIZE);
    if (backWallX < LEVEL_TILES_X) {
        for (int ty = 0; ty <= 11; ++ty) {
            m_tilemap[ty][backWallX] = TILE_GATE_FRAME;
        }
    }
}

void Level::BuildPraetoriumThrone() {
    // 1. Initialize tilemap with air
    for (int y = 0; y < LEVEL_TILES_Y; ++y) {
        for (int x = 0; x < LEVEL_TILES_X; ++x) {
            m_tilemap[y][x] = TILE_AIR;
        }
    }

    // 2. Imperial Praetorium Marble Floor
    for (int x = 0; x < LEVEL_TILES_X; ++x) {
        m_tilemap[12][x] = TILE_THRONE_MARBLE_SURFACE;
        m_tilemap[13][x] = TILE_THRONE_MARBLE_SUB;
    }

    auto AddDaisPlatform = [this](int startTileX, int endTileX, int tileY) {
        float x = startTileX * TILE_SIZE;
        float y = tileY * TILE_SIZE;
        float w = (endTileX - startTileX + 1) * TILE_SIZE;
        m_oneWayPlatforms.push_back({ { x, y, w, 6.0f } });
        for (int tx = startTileX; tx <= endTileX; ++tx) {
            m_tilemap[tileY][tx] = TILE_THRONE_DAIS_STEP;
        }
    };

    auto AddCanopyPlatform = [this](int startTileX, int endTileX, int tileY) {
        float x = startTileX * TILE_SIZE;
        float y = tileY * TILE_SIZE;
        float w = (endTileX - startTileX + 1) * TILE_SIZE;
        m_oneWayPlatforms.push_back({ { x, y, w, 6.0f } });
        for (int tx = startTileX; tx <= endTileX; ++tx) {
            m_tilemap[tileY][tx] = TILE_THRONE_VELVET_CANOPY;
        }
    };

    auto AddBrazierBlock = [this](int tileX, int tileY) {
        m_tilemap[tileY][tileX] = TILE_THRONE_CANDELABRA;
        m_crates.push_back({ (float)tileX * TILE_SIZE, (float)tileY * TILE_SIZE, (float)TILE_SIZE, (float)TILE_SIZE });
    };

    auto AddThroneColumn = [this](int tileX, int topTileY, int groundTileY) {
        for (int ty = topTileY; ty < groundTileY; ++ty) {
            m_tilemap[ty][tileX] = TILE_THRONE_PILLAR_ARCH;
        }
    };

    // === SECTION 1: Imperial Grand Vestibule (Tiles 0 - 30) ===
    AddThroneColumn(6, 2, 12);
    AddThroneColumn(16, 2, 12);
    AddThroneColumn(26, 2, 12);
    AddBrazierBlock(8, 11);
    AddBrazierBlock(24, 11);

    AddDaisPlatform(10, 22, 8);
    // Pilate Ewer 1: Entrance dais
    m_baskets.push_back({ { 16.0f * TILE_SIZE, 11.0f * TILE_SIZE }, false, 0.0f, 0 });

    // === SECTION 2: Hall of the Golden Eagles (Tiles 31 - 60) ===
    AddThroneColumn(36, 1, 12);
    AddThroneColumn(48, 1, 12);
    AddThroneColumn(60, 1, 12);
    AddBrazierBlock(34, 11);
    AddBrazierBlock(52, 11);

    AddDaisPlatform(34, 58, 9);
    AddCanopyPlatform(40, 52, 6);
    AddBrazierBlock(39, 8);

    // Pilate Ewer 2: High imperial canopy
    m_baskets.push_back({ { 46.0f * TILE_SIZE, 4.8f * TILE_SIZE }, false, 0.0f, 1 });

    // === SECTION 3: The Praetorium Antechamber (Tiles 61 - 88) ===
    AddThroneColumn(66, 2, 12);
    AddThroneColumn(78, 2, 12);
    AddBrazierBlock(64, 11);
    AddBrazierBlock(80, 11);

    AddDaisPlatform(64, 82, 8);
    // Pilate Ewer 3: Antechamber gallery
    m_baskets.push_back({ { 72.0f * TILE_SIZE, 6.8f * TILE_SIZE }, false, 0.0f, 1 });

    // Pilate Ewer 4: Pedestal before the Throne Gate
    m_baskets.push_back({ { 84.0f * TILE_SIZE, 11.0f * TILE_SIZE }, false, 0.0f, 0 });

    // === SECTION 4: Praetorium Throne Gate (Tile 90, X = 1440) ===
    int gateTileX = static_cast<int>(m_arenaGateX / TILE_SIZE);
    for (int ty = 2; ty <= 11; ++ty) {
        m_tilemap[ty][gateTileX] = TILE_GATE_FRAME;
        m_tilemap[ty][gateTileX + 1] = TILE_GATE_BARS;
        m_tilemap[ty][gateTileX + 2] = TILE_GATE_FRAME;
    }
    m_tilemap[11][gateTileX - 1] = TILE_THRONE_CANDELABRA;

    // === SECTION 5: The Bema of Pilate Arena (Tiles 93 - 116, X = 1460 to 1860) ===
    // Central Elevated Throne Dais
    AddDaisPlatform(98, 110, 9);
    AddCanopyPlatform(96, 112, 6); // High purple velvet canopies

    // Pontius Pilate's Ivory Throne and Golden Water Basin on the dais
    m_tilemap[8][104] = TILE_THRONE_GOLD_EAGLE;
    m_tilemap[8][105] = TILE_THRONE_BASIN_PEDESTAL;

    // Gold columns and braziers flanking the judgment seat
    AddThroneColumn(94, 2, 12);
    AddThroneColumn(114, 2, 12);
    m_tilemap[11][96] = TILE_THRONE_CANDELABRA;
    m_tilemap[11][112] = TILE_THRONE_CANDELABRA;

    int backWallX = static_cast<int>(m_arenaMaxX / TILE_SIZE);
    if (backWallX < LEVEL_TILES_X) {
        for (int ty = 0; ty <= 11; ++ty) {
            m_tilemap[ty][backWallX] = TILE_GATE_FRAME;
        }
    }
}

void Level::BuildGolgothaSummit() {
    // 1. Initialize tilemap with air
    for (int y = 0; y < LEVEL_TILES_Y; ++y) {
        for (int x = 0; x < LEVEL_TILES_X; ++x) {
            m_tilemap[y][x] = TILE_AIR;
        }
    }

    // 2. Basalt & Crag Surface Floor
    for (int x = 0; x < LEVEL_TILES_X; ++x) {
        m_tilemap[12][x] = TILE_SUMMIT_ROCK_SURFACE;
        m_tilemap[13][x] = TILE_SUMMIT_ROCK_SUB;
    }

    auto AddSummitLedge = [this](int startTileX, int endTileX, int tileY) {
        float x = startTileX * TILE_SIZE;
        float y = tileY * TILE_SIZE;
        float w = (endTileX - startTileX + 1) * TILE_SIZE;
        m_oneWayPlatforms.push_back({ { x, y, w, 6.0f } });
        for (int tx = startTileX; tx <= endTileX; ++tx) {
            m_tilemap[tileY][tx] = TILE_SUMMIT_CRAG_LEDGE;
        }
    };

    // Summit Arena (Tiles 0 to 30, Width = 480px, Arena bounds: 20 to 460)
    // Left Crag Ledge (Tier 1: Y = 9)
    AddSummitLedge(4, 9, 9);
    // Right Crag Ledge (Tier 1: Y = 9)
    AddSummitLedge(19, 24, 9);
    // Center High Skull Altar (Tier 2: Y = 6)
    AddSummitLedge(11, 17, 6);

    // Weathered skull rocks and iron braziers on the summit
    m_tilemap[11][2] = TILE_SUMMIT_BRAZIER;
    m_tilemap[11][10] = TILE_SUMMIT_SKULL_ROCK;
    m_tilemap[11][18] = TILE_SUMMIT_SKULL_ROCK;
    m_tilemap[11][26] = TILE_SUMMIT_BRAZIER;

    // Sacred Relics on the summit (Bonus pickups)
    m_baskets.push_back({ { 6.0f * TILE_SIZE, 8.0f * TILE_SIZE }, false, 0.0f, 1 });
    m_baskets.push_back({ { 14.0f * TILE_SIZE, 5.0f * TILE_SIZE }, false, 0.0f, 2 });
    m_baskets.push_back({ { 21.0f * TILE_SIZE, 8.0f * TILE_SIZE }, false, 0.0f, 1 });
    m_baskets.push_back({ { 25.0f * TILE_SIZE, 11.0f * TILE_SIZE }, false, 0.0f, 0 });
    m_basketsCollected = TOTAL_BASKETS; // Arena immediately unlocked for the boss rush!

    // Left and right arena boundary invisible walls
    for (int ty = 0; ty <= 11; ++ty) {
        m_tilemap[ty][0] = TILE_GATE_FRAME;
        m_tilemap[ty][29] = TILE_GATE_FRAME;
    }
}




