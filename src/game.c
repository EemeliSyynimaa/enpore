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
    en_v2f start,
    en_v2f end,
    int color)
{
    int steep = 0;

    if (en_abs_f(end.x - start.x) < en_abs_f(end.y - start.y))
    {
        en_swap_f(&start.x, &start.y);
        en_swap_f(&end.x, &end.y);

        steep = 1;
    }

    if (start.x > end.x)
    {
        en_swap_f(&start.x, &end.x);
        en_swap_f(&start.y, &end.y);
    }

    float dx = end.x - start.x;
    float dy = end.y - start.y;
    float slope = en_abs_f(dy / dx);
    float y = start.y;
    float error = 0.0f;
    float increment = (start.y > end.y) ? -1.0f : 1.0f;

    for (float x = start.x; x <= end.x; x++)
    {
        if (steep)
        {
            en_pixel_fill(
                render_buffer,
                (int)y,
                (int)x,
                color);
        }
        else
        {
            en_pixel_fill(
                render_buffer,
                (int)x,
                (int)y,
                color);
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
    en_v2f a,
    en_v2f b,
    en_v2f c,
    int color_fill,
    int color_border)
{
    if (a.y > b.y) en_swap_v2f(&a, &b);
    if (a.y > c.y) en_swap_v2f(&a, &c);
    if (b.y > c.y) en_swap_v2f(&b, &c);

    float total_height = c.y - a.y;

    for (float y = a.y; y < b.y; y++)
    {
        float segment_height = b.y - a.y;
        float alpha = (y - a.y) / total_height;
        float beta = (y - a.y) / segment_height;

        en_v2f ta;
        ta.x = a.x + (c.x - a.x) * alpha;
        ta.y = a.y + (c.y - a.y) * alpha;

        en_v2f tb;
        tb.x = a.x + (b.x - a.x) * beta;
        tb.y = a.y + (b.y - a.y) * beta;

        if (ta.x > tb.x)
        {
            en_swap_v2f(&ta, &tb);
        }

        for (float j = ta.x; j <= tb.x + 1; j++)
        {
            en_pixel_fill(render_buffer, (int)j, (int)y, color_fill);
        }
    }

    for (float y = b.y; y <= c.y; y++)
    {
        float segment_height = c.y - b.y;
        float alpha = (y - a.y) / total_height;
        float beta = (y - b.y) / segment_height;

        en_v2f ta;
        ta.x = a.x + (c.x - a.x) * alpha;
        ta.y = a.y + (c.y - a.y) * alpha;

        en_v2f tb;
        tb.x = b.x + (c.x - b.x) * beta;
        tb.y = b.y + (c.y - b.y) * beta;

        if (ta.x > tb.x)
        {
            en_swap_v2f(&ta, &tb);
        }

        for (float j = ta.x; j <= tb.x; j++)
        {
            en_pixel_fill(render_buffer, (int)j, (int)y, color_fill);
        }
    }

    en_line_draw(render_buffer, a, b, color_border);
    en_line_draw(render_buffer, b, c, color_border);
    en_line_draw(render_buffer, c, a, color_border);
}

static void en_hexagon_fill(
    render_buffer_t *render_buffer,
    en_v2f center,
    float size,
    int color)
{
    float height_multiplier = 0.866f; // sqrt(3) / 2
    float width = 2 * size * height_multiplier;

    // Calculate points
    en_v2f tl, tc, tr; // top left, top center, top right
    en_v2f bl, bc, br; // bottom left, bottom center, bottom right

    tl = tc = tr = center;
    bl = bc = br = center;

    tc.y += size;
    tl.x -= width * 0.5f;
    tl.y += size * 0.5f;
    tr.x += width * 0.5f;
    tr.y += size * 0.5f;

    bc.y -= size;
    bl.x -= width * 0.5f;
    bl.y -= size * 0.5f;
    br.x += width * 0.5f;
    br.y -= size* 0.5f;

    // Render triangles.
    en_triangle_fill(render_buffer, tc, tl, bl, color, 0);
    en_triangle_fill(render_buffer, tc, bl, bc, color, 0);
    en_triangle_fill(render_buffer, tc, tr, bc, color, 0);
    en_triangle_fill(render_buffer, tr, br, bc, color, 0);
}

static void en_game_draw(game_data_t *game_data)
{
    en_v2f start;
    en_v2f end;

    en_v2f a, b, c;
    a.x = 0.0f;
    a.y = 0.0f;

    b.x = 100.0f;
    b.y = 25.0f;

    c.x = 50.0f;
    c.y = 50.0f;

    start.x = game_data->render_buffer.width / 2.0f;
    start.y = game_data->render_buffer.height / 2.0f;

    end.x = start.x - 100.0f;
    end.y = start.y + 5.0f;

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

    en_v2f center;
    center.x = 200.0f;
    center.y = 50.0f;

    en_hexagon_fill(
        &game_data->render_buffer,
        center,
        32.0f,
        0x000000FF);
}