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

static void en_game_draw(game_data_t *game_data)
{
    en_v2i start;
    en_v2i end;

    en_v2i a, b, c;
    a.x = 0;
    a.y = 0;

    b.x = 100;
    b.y = 25;

    c.x = 50;
    c.y = 50;

    start.x = game_data->render_buffer.width / 2;
    start.y = game_data->render_buffer.height / 2;

    end.x = start.x - 100;
    end.y = start.y + 5;

    en_rect_fill(
        &game_data->render_buffer,
        0,
        0,
        game_data->render_buffer.width,
        game_data->render_buffer.height,
        0x0000A2E8);

    en_rect_fill(
        &game_data->render_buffer,
        5 * PIXEL_SIZE,
        5 * PIXEL_SIZE,
        PIXEL_SIZE,
        PIXEL_SIZE,
        0x000000FF);

    en_line_draw(
        &game_data->render_buffer,
        start,
        end,
        0x00FF0000);

    en_triangle_fill(
        &game_data->render_buffer,
        a, b, c, 
        0x0000FF00,
        0);

    en_v2i hex_start_pos;
    hex_start_pos.x = 32;
    hex_start_pos.y = 432;

    int size = 16;
    float height_multiplier = 0.866f; // sqrt(3) / 2
    float width = 2 * size * height_multiplier;
    int half_width = (int)(width * 0.5f);
    int h_dist = (int)width;
    int v_dist = (int)(2 * size * (3.0 / 4.0f));

    for (int y = 0; y < 5; y++)
    {
        for (int x = 0; x < 5; x++)
        {
            en_v2i hex_pos = hex_start_pos;

            hex_pos.x += x * h_dist + (y % 2 ? half_width : 0);
            hex_pos.y -= y * v_dist;

            en_hexagon_fill(
                &game_data->render_buffer,
                hex_pos,
                size-1,
                0x000000FF);
        }
    }

}