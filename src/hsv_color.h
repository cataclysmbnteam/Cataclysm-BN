#pragma once

#if defined(TILES)

#include "sdl_wrappers.h"

struct HSVColor {
    /// Hue: 0~393210 ( ( 1 << 16 ) - 1) * 6)
    uint32_t H;
    // Saturation: 0~65535
    uint16_t S;
    // Value: 0~255
    uint8_t V;
    // Alpha: 0~255
    uint8_t A;
};

using RGBColor = SDL_Color;

auto hsv2rgb( HSVColor color ) -> RGBColor;
auto rgb2hsv( RGBColor color ) -> HSVColor;

#endif