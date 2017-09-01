#ifndef GAME_H
#define GAME_H

typedef struct render_buffer_t
{
    void *memory;
    int width;
    int height;
    int buffer_size_in_bytes;
    int pixel_size_in_bytes;
} render_buffer_t;

typedef struct tile_map_t
{
    int tiles[126];
    int width;
    int height;
    int tile_size;
} tile_map_t;

typedef struct game_data_t
{
    render_buffer_t render_buffer;
    tile_map_t tile_map;

    int mouse_x;
    int mouse_y;
} game_data_t;

typedef struct point_t
{
    float x;
    float y;
} point_t;

#endif // GAME_H