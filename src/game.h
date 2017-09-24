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

typedef struct hero_t
{
    v2i position;
} hero_t;

typedef struct game_data_t
{
    render_buffer_t render_buffer;
    tile_map_t tile_map;
    hero_t heroes[1];
    s32 hero_count;
    s32 hero_selected;
    v2i mouse_pos;
    b32 mouse_last_frame;
    b32 mouse_this_frame;
} game_data_t;

#endif // GAME_H