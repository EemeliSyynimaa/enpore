#ifndef GAME_H
#define GAME_H

#include "platform.h"
#include "math.h"

typedef struct render_buffer_t
{
    void *memory;
    s32 width;
    s32 height;
    s32 buffer_size_in_bytes;
    s32 pixel_size_in_bytes;
} render_buffer_t;

typedef struct tile_map_t
{
    s32 tiles[126];
    s32 width;
    s32 height;
    s32 tile_size;
} tile_map_t;

typedef struct game_data_t
{
    render_buffer_t render_buffer;
    tile_map_t tile_map;

    v2i mouse_pos;
} game_data_t;

#endif // GAME_H