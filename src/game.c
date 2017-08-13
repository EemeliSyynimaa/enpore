#include "game.h"

static void rect_fill(
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

static void pixel_fill(
    render_buffer_t *render_buffer,
    int x,
    int y,
    int color)
{
    int* pixel_position = (int*)render_buffer->memory + y * render_buffer->width + x;

    *pixel_position = color;    
}

static void update_draw(game_data_t *game_data)
{
    rect_fill(
        &game_data->render_buffer,
        0,
        0,
        game_data->render_buffer.width,
        game_data->render_buffer.height,
        0x0000A2E8);

    pixel_fill(&game_data->render_buffer, 5, 5, 0x000000FF);
}