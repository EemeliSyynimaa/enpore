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
    render_buffer_t* render_buffer,
    en_vec2_f start,
    en_vec2_f end,
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

static void en_game_draw(game_data_t *game_data)
{
    en_vec2_f start;
    en_vec2_f end;

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
}