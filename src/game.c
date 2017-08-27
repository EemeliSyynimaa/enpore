#include "game.h"
#include "math.h"

#define PIXEL_SIZE 16

static void en_rect_fill(
    render_buffer_t *render_buffer, 
    int rect_x, 
    int rect_y, 
    int rect_width, 
    int rect_height,
    int rect_color)
{
    for (int y = 0; y < rect_height; y++)
    {
        int* pixel_position = (int*)render_buffer->memory + (rect_y + y) * render_buffer->width + rect_x;

        for (int x = 0; x < rect_width; x++, pixel_position++)
        {
            *pixel_position = rect_color;
        }
    }
}

static void en_pixel_fill(
    render_buffer_t *render_buffer,
    int x,
    int y,
    int color)
{
    int* pixel_position = (int*)render_buffer->memory + y * render_buffer->width + x;

    *pixel_position = color;    
}

static void en_line_draw(
    render_buffer_t *render_buffer,
    en_v2i start,
    en_v2i end,
    int color)
{
    int steep = 0;

    if (en_abs_i(end.x - start.x) < en_abs_i(end.y - start.y))
    {
        en_swap_i(&start.x, &start.y);
        en_swap_i(&end.x, &end.y);

        steep = 1;
    }

    if (start.x > end.x)
    {
        en_swap_v2i(&start, &end);
    }

    int dx = end.x - start.x;
    int dy = end.y - start.y;
    int y = start.y;
    int increment = (start.y > end.y) ? -1 : 1;
    float slope = en_abs_f(dy / (float)dx);
    float error = 0.0f;

    for (int x = start.x; x <= end.x; x++)
    {
        if (steep)
        {
            en_pixel_fill(render_buffer, y, x, color);
        }
        else
        {
            en_pixel_fill(render_buffer, x, y, color);
        }

        if ((error += slope) > 0.5f)
        {
            y += increment;
            error -= 1.0f;
        }
    }
}

static void en_triangle_fill(
    render_buffer_t *render_buffer,
    en_v2i a,
    en_v2i b,
    en_v2i c,
    int color_fill,
    int color_border)
{
    if (a.y > b.y) en_swap_v2i(&a, &b);
    if (a.y > c.y) en_swap_v2i(&a, &c);
    if (b.y > c.y) en_swap_v2i(&b, &c);

    int total_height = c.y - a.y;

    for (int y = a.y; y < b.y; y++)
    {
        int segment_height = b.y - a.y;
        float alpha = (y - a.y) / (float)total_height;
        float beta = (y - a.y) / (float)segment_height;

        en_v2i ta;
        ta.x = (int)(a.x + (c.x - a.x) * alpha);
        ta.y = (int)(a.y + (c.y - a.y) * alpha);

        en_v2i tb;
        tb.x = (int)(a.x + (b.x - a.x) * beta);
        tb.y = (int)(a.y + (b.y - a.y) * beta);

        if (ta.x > tb.x)
        {
            en_swap_v2i(&ta, &tb);
        }

        for (int j = ta.x; j <= tb.x; j++)
        {
            en_pixel_fill(render_buffer, j, y, color_fill);
        }
    }

    for (int y = b.y; y <= c.y; y++)
    {
        int segment_height = c.y - b.y;
        float alpha = (y - a.y) / (float)total_height;
        float beta = (y - b.y) / (float)segment_height;

        en_v2i ta;
        ta.x = (int)(a.x + (c.x - a.x) * alpha);
        ta.y = (int)(a.y + (c.y - a.y) * alpha);

        en_v2i tb;
        tb.x = (int)(b.x + (c.x - b.x) * beta);
        tb.y = (int)(b.y + (c.y - b.y) * beta);

        if (ta.x > tb.x)
        {
            en_swap_v2i(&ta, &tb);
        }

        for (int j = ta.x; j <= tb.x; j++)
        {
            en_pixel_fill(render_buffer, j, y, color_fill);
        }
    }

/*    en_line_draw(render_buffer, a, b, color_border);
    en_line_draw(render_buffer, b, c, color_border);
    en_line_draw(render_buffer, c, a, color_border);*/
    (void)color_border;
}

static void en_hexagon_fill(
    render_buffer_t *render_buffer,
    en_v2i center,
    int size,
    int color)
{
    float height_multiplier = 0.866f; // sqrt(3) / 2
    float width = 2 * size * height_multiplier;
    int half_width = (int)(width * 0.5f);
    int half_size = (int)(size * 0.5f);

    // Calculate points
    en_v2i tl, tc, tr; // top left, top center, top right
    en_v2i bl, bc, br; // bottom left, bottom center, bottom right

    tl = tc = tr = center;
    bl = bc = br = center;

    tc.y += size;
    tl.x -= half_width;
    tl.y += half_size;
    tr.x += half_width;
    tr.y += half_size;

    bc.y -= size;
    bl.x -= half_width;
    bl.y -= half_size;
    br.x += half_width;
    br.y -= half_size;

    // Render triangles.
    en_triangle_fill(render_buffer, tc, tl, bl, color, color);
    en_triangle_fill(render_buffer, tc, bl, bc, color, color);
    en_triangle_fill(render_buffer, tc, tr, bc, color, color);
    en_triangle_fill(render_buffer, tr, br, bc, color, color);
}

static int en_tile_map_get_tile(tile_map_t *tile_map, int x, int y)
{
    int result = tile_map->tiles[tile_map->width * y + x];

    return result;
}

static void en_tile_map_set_tile(tile_map_t *tile_map, int x, int y, int value)
{
    tile_map->tiles[tile_map->width * y + x] = value;
}

static void en_game_init(game_data_t *game_data)
{
    game_data->tile_map.width = 14;
    game_data->tile_map.height = 9;
    game_data->tile_map.tile_size = 64;

    for (int y = 0; y < game_data->tile_map.height; y++)
    {
        for (int x = 0; x < game_data->tile_map.width; x++)
        {
            en_tile_map_set_tile(&game_data->tile_map, x, y, x % 2);
        }
    }
}

static en_v2i en_get_tile_offset_coordinates(int x, int y)
{
    en_v2i result;

    result.x = x + (y - (en_abs_i(y) % 2)) / 2;
    result.y = y;

    return result;
}

static en_v2i en_get_tile_screen_coordinates(tile_map_t *tile_map, int x, int y)
{
    en_v2i result;
    result.x = x;
    result.y = y;

    float height_multiplier = 0.866f; // sqrt(3) / 2
    float width = 2 * tile_map->tile_size * height_multiplier;
    int half_width = (int)(width * 0.5f);
    int h_dist = (int)width;
    int v_dist = (int)(2 * tile_map->tile_size * (3.0 / 4.0f));

    result.x *= h_dist;
    result.y *= v_dist;

    // Add start marginal.
    result.x += 58 + (y % 2 ? half_width : 0);
    result.y += 64;

    return result;
}

static void en_game_draw(game_data_t *game_data)
{
    en_rect_fill(
        &game_data->render_buffer,
        0,
        0,
        game_data->render_buffer.width,
        game_data->render_buffer.height,
        0x000d89c0);

    for (int y = 0; y < game_data->tile_map.height; y++)
    {
        for (int x = 0; x < game_data->tile_map.width; x++)
        {
            en_v2i screen_coords = en_get_tile_screen_coordinates(&game_data->tile_map, x, y);
            int color = 0;
            int tile = en_tile_map_get_tile(&game_data->tile_map, x, y);

            if (tile)
            {
                color = 0x0032b557;
            }
            else
            {
                color = 0x0060D057;
            }

            en_hexagon_fill(
                &game_data->render_buffer,
                screen_coords,
                game_data->tile_map.tile_size-2,
                color);
        }
    }

    en_v2i player_position = en_get_tile_offset_coordinates(-2, 4);
    player_position = en_get_tile_screen_coordinates(&game_data->tile_map, player_position.x, player_position.y);

    en_rect_fill(
        &game_data->render_buffer,
        player_position.x - 16,
        player_position.y - 16,
        32,
        32,
        0xFFFFFFFF);
}