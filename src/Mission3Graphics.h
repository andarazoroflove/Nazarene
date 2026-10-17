#pragma once

#include "raylib.h"
#include "Graphics.h"

class Mission3Graphics {
public:
    static void GenerateAll(
        Texture2D& bgBazaarSky,
        Texture2D& bgBazaarRoofs,
        Texture2D& bgCaiaphasSky,
        Texture2D& bgCaiaphasPalace,
        Texture2D& bgCenacleSky,
        Texture2D& bgCenacleChamber
    );
};

