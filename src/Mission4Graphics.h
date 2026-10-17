#pragma once

#include "raylib.h"
#include "Graphics.h"

class Mission4Graphics {
public:
    static void GenerateAll(
        Texture2D& bgDolorosaSky,
        Texture2D& bgDolorosaStreet,
        Texture2D& bgCourtSky,
        Texture2D& bgCourtBasilica,
        Texture2D& bgThroneSky,
        Texture2D& bgThronePalace
    );
};

