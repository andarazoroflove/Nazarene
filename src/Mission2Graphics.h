#pragma once

#include "raylib.h"
#include "Graphics.h"

class Mission2Graphics {
public:
    static void GenerateAll(
        Texture2D& bgKidronSky,
        Texture2D& bgKidronRavine,
        Texture2D& bgHedgeSky,
        Texture2D& bgHedgeLabyrinth,
        Texture2D& bgGethsemaneSky,
        Texture2D& bgGethsemaneGrove,
        Texture2D& bgBonusSunset,
        Texture2D& bgBonusTerraces
    );
};

