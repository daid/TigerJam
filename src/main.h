#ifndef MAIN_H
#define MAIN_H

#include <sp2/window.h>
#include <sp2/io/keybinding.h>

#include "controller.h"

extern sp::P<sp::Window> window;
extern sp::io::Keybinding escape_key;
extern Controller controller;
extern double screen_shake;

void openMainMenu();

static inline sp::Rect2d tileUV(int tile_idx) {
    auto tile_sx = 49;
    auto tile_sy = 22;
    auto u = 1.0f / double(tile_sx);
    auto v = 1.0f / double(tile_sy);
    return {
        {(tile_idx % tile_sx) * u, (tile_idx / tile_sx) * v},
        {u, v}
    };
}

#endif//MAIN_H
