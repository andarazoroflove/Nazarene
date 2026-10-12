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
            if (tx == gateCol + 1 && id == TILE_GATE_BARS) {
                drawY -= m_gateOpenOffsetY;
            }

            GraphicsManager::DrawTile(id, drawX, drawY);
        }
    }

    // Draw Baskets or Sacred Censers depending on stage
    for (const auto& b : m_baskets) {
        if (!b.collected) {
            float floatY = sinf(b.floatTimer * 4.0f) * 2.0f;
            SpriteId pickupSpr = (m_stageId == 2 || m_stageId == 3) ? SPR_CENSER_GOLD : SPR_BASKET_LOAVES_FISHES;
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

