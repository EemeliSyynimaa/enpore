#include "game.h"
#include "math.h"

static void en_rect_fill(
    render_buffer_t *render_buffer, 
    int x1, 
    int y1, 
    int x2, 
    int y2,
    int color)
{
    if (x1 < render_buffer->width && y1 < render_buffer->height && x2 > 0 && y2 > 0)
    {
        x1 = x1 < 0 ? 0 : x1;
        y1 = y1 < 0 ? 0 : y1;
        x2 = x2 > (render_buffer->width) ? render_buffer->width : x2;
        y2 = y2 > (render_buffer->height) ? render_buffer->height : y2;

        for (int y = y1; y < y2; y++)
        {
            int* pixel_position = (int*)render_buffer->memory + y * render_buffer->width + x1;

            for (int x = x1; x < x2; x++, pixel_position++)
            {
                *pixel_position = color; 
            }
        }   
    }
}

static void en_pixel_fill(
    render_buffer_t *render_buffer,
    int x,
    int y,
    int color)
{
    if (x >= 0 && x < render_buffer->width && y >= 0 && y < render_buffer->height)
    {
        *((int*)render_buffer->memory + y * render_buffer->width + x) = color;
    }
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
    int index = tile_map->width * y + x + y / 2;
    int result = tile_map->tiles[index];

    return result;
}

static void en_tile_map_set_tile(tile_map_t *tile_map, int x, int y, int value)
{
    int index = tile_map->width * y + x + y / 2;
    
    tile_map->tiles[index] = value;
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
            game_data->tile_map.tiles[y * game_data->tile_map.width + x] = x % 2;
        }
    }
}

static en_v2i en_convert_axial_to_screen(int x, int y, int size)
{
    en_v2i result;

    float xf = (float)x;
    float yf = (float)y;
    float sizef = (float)size;
    float sqrt3f = 1.7320508075688772935274463415059f;

    result.x = (int)(sizef * sqrt3f * (xf + yf / 2.0f));
    result.y = (int)(sizef * 3.0f / 2.0f * yf);

    return result;
}

static en_v2i en_round_cube_coordinates(float x, float y, float z)
{
    en_v2i result;

    int rx = en_round(x);
    int ry = en_round(y);
    int rz = en_round(z);

    float x_diff = en_abs_f((float)rx - x);
    float y_diff = en_abs_f((float)ry - y);
    float z_diff = en_abs_f((float)rz - z);

    if (x_diff > y_diff && x_diff > z_diff)
    {
        rx = -ry - rz;
    }
    else if (y_diff > z_diff)
    {
        ry = -rx - rz;
    }
    else
    {
        rz = -rx - ry;
    }

    result.x = rx;
    result.y = rz;

    return result;
}

static en_v2i en_convert_screen_to_axial(int x, int y, int size)
{
    en_v2i result;
    en_v2f fcoords;

    float xf = (float)x;
    float yf = (float)y;
    float sizef = (float)size;
    float sqrt3f = 1.7320508075688772935274463415059f;

    fcoords.x = (xf * sqrt3f / 3.0f - yf / 3.0f) / sizef;
    fcoords.y = yf * 2.0f / 3.0f / sizef; 

    result = en_round_cube_coordinates(fcoords.x, -fcoords.x - fcoords.y, fcoords.y);

    return result;
}

/*
function cube_to_axial(cube):
    var q = cube.x
    var r = cube.z
    return Hex(q, r)

function axial_to_cube(hex):
    var x = hex.q
    var z = hex.r
    var y = -x-z
    return Cube(x, y, z)
*/

static void en_game_draw(game_data_t *game_data)
{
    en_v2i tile_offset = { 55, 64 };

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
            int tile = game_data->tile_map.tiles[y * game_data->tile_map.width + x];
            
            en_v2i screen_coords = en_convert_axial_to_screen(x - y/2, y, game_data->tile_map.tile_size);

            screen_coords = en_sum_v2i(screen_coords, tile_offset);

            int color = 0;

            if (tile == 0)
            {
                color = 0x0032b557;
            }
            else if (tile == 1)
            {
                color = 0x0060D057;
            }
            else if (tile == 2)
            {
                color = 0x00FF5B8D;
            }

            en_hexagon_fill(
                &game_data->render_buffer,
                screen_coords,
                game_data->tile_map.tile_size-2,
                color);
        }
    }

    en_v2i test = en_convert_screen_to_axial(290-300, 265-132, 32);

    (void)test;
    en_v2i mouse_tile_map_pos = en_sub_v2i(game_data->mouse_pos, tile_offset);
    en_v2i mouse = en_convert_screen_to_axial(mouse_tile_map_pos.x, mouse_tile_map_pos.y, game_data->tile_map.tile_size);
    en_v2i player_position = en_convert_axial_to_screen(mouse.x, mouse.y, game_data->tile_map.tile_size);

    player_position = en_sum_v2i(player_position, tile_offset);

    en_hexagon_fill(
        &game_data->render_buffer,
        player_position,
        game_data->tile_map.tile_size-2,
        0xFFFFFFFF);
}