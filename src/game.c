#include "game.h"
#include "math.h"

static void rect_fill(
    render_buffer_t *buffer, 
    s32 x1, 
    s32 y1, 
    s32 x2, 
    s32 y2,
    s32 color)
{
    if (x1 < buffer->width && y1 < buffer->height && x2 > 0 && y2 > 0)
    {
        x1 = x1 < 0 ? 0 : x1;
        y1 = y1 < 0 ? 0 : y1;
        x2 = x2 > buffer->width ? buffer->width : x2;
        y2 = y2 > buffer->height ? buffer->height : y2;

        for (s32 y = y1; y < y2; y++)
        {
            s32 *pixel_position = (s32*)buffer->memory + y * buffer->width + x1;

            for (s32 x = x1; x < x2; x++, pixel_position++)
            {
                *pixel_position = color; 
            }
        }   
    }
}

static void bitmap_fill(
    render_buffer_t *buffer,
    s32 dest_x,
    s32 dest_y,
    s32 src_x1,
    s32 src_y1,
    s32 src_x2,
    s32 src_y2,
    bitmap_t *bitmap)
{
    s32 x1 = dest_x;
    s32 y1 = dest_y;
    s32 x2 = x1 + bitmap->width;
    s32 y2 = y1 + bitmap->height;

    if (x1 < buffer->width && y1 < buffer->height && x2 > 0 && y2 > 0)
    {
        if (x1 < 0)
        {
            src_x1 += x1;
            x1 = 0;
        }

        if (y1 < 0)
        {
            src_y1 += y1;
            y1 = 0;
        }

        if (x2 > buffer->width)
        {
            src_x2 -= x2 - buffer->width;
            x2 = buffer->width;
        }

        if (y2 > buffer->height)
        {
            src_y2 -= y2 - buffer->height;
            y2 = buffer->height;
        }

        for (s32 y = y1, j = src_y1; y < y2; y++, j++)
        {
            s32 *pixel_position = (s32*)buffer->memory + y * buffer->width + x1;
            s32 *bitmap_position = (s32*)bitmap->data + j * bitmap->width + src_x1;

            for (s32 x = x1; x < x2; x++, pixel_position++, bitmap_position++)
            {
                *pixel_position = *bitmap_position; 
            }
        }   
    }
}

static void pixel_fill(
    render_buffer_t *buffer,
    s32 x,
    s32 y,
    s32 color)
{
    if (x >= 0 && x < buffer->width && y >= 0 && y < buffer->height)
    {
        *((s32*)buffer->memory + y * buffer->width + x) = color;
    }
}

static void line_draw(
    render_buffer_t *buffer,
    v2i start,
    v2i end,
    s32 color)
{
    s32 steep = 0;

    if (abs_i(end.x - start.x) < abs_i(end.y - start.y))
    {
        swap_i(&start.x, &start.y);
        swap_i(&end.x, &end.y);

        steep = 1;
    }

    if (start.x > end.x)
    {
        swap_v2i(&start, &end);
    }

    s32 dx = end.x - start.x;
    s32 dy = end.y - start.y;
    s32 y = start.y;
    s32 increment = (start.y > end.y) ? -1 : 1;
    f32 slope = abs_f(dy / (f32)dx);
    f32 error = 0.0f;

    for (s32 x = start.x; x <= end.x; x++)
    {
        if (steep)
        {
            pixel_fill(buffer, y, x, color);
        }
        else
        {
            pixel_fill(buffer, x, y, color);
        }

        if ((error += slope) > 0.5f)
        {
            y += increment;
            error -= 1.0f;
        }
    }
}

static void triangle_fill(
    render_buffer_t *buffer,
    v2i a,
    v2i b,
    v2i c,
    s32 color_fill,
    s32 color_border)
{
    if (a.y > b.y) swap_v2i(&a, &b);
    if (a.y > c.y) swap_v2i(&a, &c);
    if (b.y > c.y) swap_v2i(&b, &c);

    s32 total_height = c.y - a.y;

    for (s32 y = a.y; y < b.y; y++)
    {
        s32 segment_height = b.y - a.y;
        f32 alpha = (y - a.y) / (f32)total_height;
        f32 beta = (y - a.y) / (f32)segment_height;

        v2i ta;
        ta.x = (s32)(a.x + (c.x - a.x) * alpha);
        ta.y = (s32)(a.y + (c.y - a.y) * alpha);

        v2i tb;
        tb.x = (s32)(a.x + (b.x - a.x) * beta);
        tb.y = (s32)(a.y + (b.y - a.y) * beta);

        if (ta.x > tb.x)
        {
            swap_v2i(&ta, &tb);
        }

        for (s32 j = ta.x; j <= tb.x; j++)
        {
            pixel_fill(buffer, j, y, color_fill);
        }
    }

    for (s32 y = b.y; y <= c.y; y++)
    {
        s32 segment_height = c.y - b.y;
        f32 alpha = (y - a.y) / (f32)total_height;
        f32 beta = (y - b.y) / (f32)segment_height;

        v2i ta;
        ta.x = (s32)(a.x + (c.x - a.x) * alpha);
        ta.y = (s32)(a.y + (c.y - a.y) * alpha);

        v2i tb;
        tb.x = (s32)(b.x + (c.x - b.x) * beta);
        tb.y = (s32)(b.y + (c.y - b.y) * beta);

        if (ta.x > tb.x)
        {
            swap_v2i(&ta, &tb);
        }

        for (s32 j = ta.x; j <= tb.x; j++)
        {
            pixel_fill(buffer, j, y, color_fill);
        }
    }

/*    line_draw(buffer, a, b, color_border);
    line_draw(buffer, b, c, color_border);
    line_draw(buffer, c, a, color_border);*/
    (void)color_border;
}

static void hexagon_fill(
    render_buffer_t *buffer,
    v2i center,
    s32 size,
    s32 color)
{
    f32 height_multiplier = 0.866f; // sqrt(3) / 2
    f32 width = 2 * size * height_multiplier;
    s32 half_width = (s32)(width * 0.5f);
    s32 half_size = (s32)(size * 0.5f);

    // Calculate points.
    v2i tl, tc, tr; // top left, top center, top right
    v2i bl, bc, br; // bottom left, bottom center, bottom right

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
    triangle_fill(buffer, tc, tl, bl, color, color);
    triangle_fill(buffer, tc, bl, bc, color, color);
    triangle_fill(buffer, tc, tr, bc, color, color);
    triangle_fill(buffer, tr, br, bc, color, color);
}

static s32 tile_get(tile_map_t *tile_map, s32 x, s32 y)
{
    s32 index = tile_map->width * y + x + y / 2;
    s32 result = tile_map->tiles[index];

    return result;
}

static void tile_set(tile_map_t *tile_map, s32 x, s32 y, s32 value)
{
    s32 index = tile_map->width * y + x + y / 2;
    tile_map->tiles[index] = value;
}

static void game_init(game_data_t *game_data)
{
    game_data->tile_map.width = 25;
    game_data->tile_map.height = 14;
    game_data->tile_map.tile_size = 64;

    for (s32 y = 0; y < game_data->tile_map.height; y++)
    {
        for (s32 x = 0; x < game_data->tile_map.width; x++)
        {
            s32 index = y * game_data->tile_map.width + x;
            game_data->tile_map.tiles[index] = x % 2;
        }
    }

    game_data->heroes[0].position.x = 3;
    game_data->heroes[0].position.y = 6;
    game_data->hero_count = 1;
}

static v2i convert_axial_to_pixel(s32 x, s32 y, s32 size)
{
    v2i result;

    f32 xf = (f32)x;
    f32 yf = (f32)y;
    f32 sizef = (f32)size;
    f32 sqrt3f = 1.7320508075688772935274463415059f;

    result.x = (s32)(sizef * sqrt3f * (xf + yf / 2.0f));
    result.y = (s32)(sizef * 3.0f / 2.0f * yf);

    return result;
}

static v2i round_cube_coordinates(f32 x, f32 y, f32 z)
{
    v2i result;

    s32 rx = round(x);
    s32 ry = round(y);
    s32 rz = round(z);

    f32 x_diff = abs_f((f32)rx - x);
    f32 y_diff = abs_f((f32)ry - y);
    f32 z_diff = abs_f((f32)rz - z);

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

static v2i convert_pixel_to_axial(s32 x, s32 y, s32 size)
{
    v2i result;
    v2f fcoords;

    f32 xf = (f32)x;
    f32 yf = (f32)y;
    f32 sizef = (f32)size;
    f32 sqrt3f = 1.7320508075688772935274463415059f;

    fcoords.x = (xf * sqrt3f / 3.0f - yf / 3.0f) / sizef;
    fcoords.y = yf * 2.0f / 3.0f / sizef; 

    result = round_cube_coordinates(fcoords.x, -fcoords.x - fcoords.y, fcoords.y);

    return result;
}

static b32 mouse_clicked(game_data_t *game_data)
{
    b32 result = (game_data->mouse_last_frame == 1 && game_data->mouse_this_frame == 0);

    return result;
}

static void game_draw(game_data_t *game_data)
{
    rect_fill(
        &game_data->render_buffer,
        0,
        0,
        game_data->render_buffer.width,
        game_data->render_buffer.height,
        0x000d89c0);

    for (s32 y = 0; y < game_data->tile_map.height; y++)
    {
        for (s32 x = 0; x < game_data->tile_map.width; x++)
        {
            v2i tile;
            tile.x = x * game_data->tile_map.tile_size;
            tile.y = y * game_data->tile_map.tile_size;

            s32 color;

            if ((y % 2 && x % 2) || (!(y % 2) && !(x % 2)))
            {
                color = 0x295915;
            }
            else
            {
                color = 0x306504;
            }

            rect_fill(
                &game_data->render_buffer,
                tile.x,
                tile.y,
                tile.x + game_data->tile_map.tile_size,
                tile.y + game_data->tile_map.tile_size,
                color);
        }
    }

    bitmap_fill(
        &game_data->render_buffer,
        800,
        450,
        0,
        0,
        game_data->bitmap.width,
        game_data->bitmap.height,
        &game_data->bitmap);
}