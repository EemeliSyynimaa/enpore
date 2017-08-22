#include <Windows.h>
#include "game.c"

int g_running = 0;

HDC g_hdc;

BITMAPINFO g_bitmap_info;

game_data_t g_game_data;

RECT g_window_rect;

static void win32_resize_backbuffer(render_buffer_t *render_buffer, int width, int height)
{
    if (render_buffer)
    {
        if (render_buffer->memory)
        {
            HeapFree(GetProcessHeap(), 0, render_buffer->memory);
        }

        render_buffer->pixel_size_in_bytes = 4;
        render_buffer->width = width;
        render_buffer->height = height;
        render_buffer->buffer_size_in_bytes = width * height * render_buffer->pixel_size_in_bytes;

        render_buffer->memory = HeapAlloc(GetProcessHeap(), 0, render_buffer->buffer_size_in_bytes);

        g_bitmap_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        g_bitmap_info.bmiHeader.biWidth = width;
        g_bitmap_info.bmiHeader.biHeight = height;
        g_bitmap_info.bmiHeader.biPlanes = 1;
        g_bitmap_info.bmiHeader.biBitCount = 32;
        g_bitmap_info.bmiHeader.biCompression = BI_RGB;
    }
}

static void win32_update_draw(render_buffer_t *render_buffer, int width, int height)
{
    int offset_x = 10;
    int offset_y = 10;

    PatBlt(g_hdc, 0, 0, width, offset_y, BLACKNESS);
    PatBlt(g_hdc, 0, offset_y + render_buffer->height, width, height, BLACKNESS);
    PatBlt(g_hdc, 0, 0, offset_x, height, BLACKNESS);
    PatBlt(g_hdc, offset_x + render_buffer->width, 0, width, height, BLACKNESS);

    StretchDIBits(
        g_hdc,
        offset_x, 
        offset_y, 
        render_buffer->width, 
        render_buffer->height,
        0, 
        0, 
        render_buffer->width, 
        render_buffer->height,
        render_buffer->memory,
        &g_bitmap_info,
        DIB_RGB_COLORS,
        SRCCOPY);
}

LRESULT CALLBACK WindowProc(
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
        {
            g_running = 0;
        } break;
        case WM_DESTROY:
        {
            g_running = 0;
        } break;
        case WM_PAINT:
        {
            PAINTSTRUCT paintstruct;
            BeginPaint(hwnd, &paintstruct);

            GetClientRect(hwnd, &g_window_rect);

            win32_update_draw(
                &g_game_data.render_buffer,
                g_window_rect.right,
                g_window_rect.bottom);

            EndPaint(hwnd, &paintstruct);

            return 0;
        } break;
    }

    return DefWindowProcA(hwnd, uMsg, wParam, lParam);
}

int CALLBACK WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    int       nCmdShow)
{
    (void)hPrevInstance;
    (void)lpCmdLine;
    (void)nCmdShow;

    WNDCLASS window_class = {0};
    window_class.lpfnWndProc = WindowProc;
    window_class.hInstance = hInstance;
    window_class.lpszClassName = "EnporeWindowClass";
    window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

    win32_resize_backbuffer(&g_game_data.render_buffer, 1600, 900);

    if (RegisterClassA(&window_class))
    {
        HWND window = CreateWindowExA(
            0,
            window_class.lpszClassName,
            "Enpore: The RPG project",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            0,
            0,
            hInstance,
            0);

        if (window)
        {
            g_hdc = GetDC(window);
            GetClientRect(window, &g_window_rect);

            ShowWindow(window, SW_SHOWDEFAULT);

            g_running = 1;

            while (g_running)
            {
                MSG message;

                while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
                {
                    if (WM_QUIT == message.message)
                    {
                        g_running = 0;
                    }

                    TranslateMessage(&message);
                    DispatchMessageA(&message);
                }

                en_game_draw(&g_game_data);

                win32_update_draw(
                    &g_game_data.render_buffer,
                    g_window_rect.right,
                    g_window_rect.bottom);
            }
        }
    }

    return 0;
}