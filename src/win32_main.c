#include <Windows.h>
#include "game.c"

s32 g_running = 0;
HDC g_hdc;

BITMAPINFO g_bitmap_info;
game_data_t g_game_data;
RECT g_window_rect;

static void win32_load_bitmap(bitmap_t *bitmap, s8 *path)
{
    HANDLE file;

    file = CreateFile(
        path,
        GENERIC_READ,
        0,
        0,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        0);

    if (INVALID_HANDLE_VALUE != file)
    {
        BITMAPFILEHEADER file_header;
        BITMAPINFO info;

        ReadFile(
            file,
            &file_header,
            sizeof(BITMAPFILEHEADER),
            0,
            0);

        // TODO check header type?
        ReadFile(
            file,
            &info,
            sizeof(BITMAPINFO),
            0,
            0);

        bitmap->width  = info.bmiHeader.biWidth;
        bitmap->height = info.bmiHeader.biHeight;
        bitmap->data   = HeapAlloc(GetProcessHeap(), 0, bitmap->width * bitmap->height * sizeof(s32));

        s8 *data = HeapAlloc(GetProcessHeap(), 0, info.bmiHeader.biSizeImage);

        SetFilePointer(
            file,
            file_header.bfOffBits,
            0,
            FILE_BEGIN);

        ReadFile(
            file,
            data,
            info.bmiHeader.biSizeImage,
            0,
            0);

        CloseHandle(file);

        s8 *bitmap_iter = (s8*)bitmap->data;
        s8 *data_iter = (s8*)data;

        u32 index = 0;

        s32 tempo = 0xff0000;
        (void)tempo;

        while (index < info.bmiHeader.biSizeImage)
        {
            *bitmap_iter++ = *data_iter++; // blue
            *bitmap_iter++ = *data_iter++; // green
            *bitmap_iter++ = *data_iter++; // red
            *bitmap_iter++ = 0; // alpha

            index += 3;
            // TODO add alpha
        }

        HeapFree(GetProcessHeap(), 0, data);
    }
}

static void win32_resize_backbuffer(render_buffer_t *render_buffer, s32 width, s32 height)
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
        g_bitmap_info.bmiHeader.biHeight = -height;
        g_bitmap_info.bmiHeader.biPlanes = 1;
        g_bitmap_info.bmiHeader.biBitCount = 32;
        g_bitmap_info.bmiHeader.biCompression = BI_RGB;
    }
}

static void win32_update_draw(render_buffer_t *render_buffer, s32 width, s32 height)
{
    s32 offset_x = 10;
    s32 offset_y = 10;

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

s32 CALLBACK WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    s32       nCmdShow)
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

            win32_load_bitmap(&g_game_data.bitmap, "../data/monster.bmp");

            game_init(&g_game_data);

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

                game_draw(&g_game_data);

                win32_update_draw(
                    &g_game_data.render_buffer,
                    g_window_rect.right,
                    g_window_rect.bottom);

                POINT mouse;
                
                GetCursorPos(&mouse);
                ScreenToClient(window, &mouse);

                mouse.x -= 10;
                mouse.y -= 10;

                if (mouse.x < 0)
                {
                    mouse.x = 0;
                }
                else if (mouse.x > g_game_data.render_buffer.width)
                {
                    mouse.x = g_game_data.render_buffer.width;
                }

                if (mouse.y < 0)
                {
                    mouse.y = 0;
                }
                else if (mouse.y > g_game_data.render_buffer.height)
                {
                    mouse.y = g_game_data.render_buffer.height;
                }
  
                g_game_data.mouse_last_frame = g_game_data.mouse_this_frame;
                g_game_data.mouse_this_frame = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
                
                g_game_data.mouse_pos.x = mouse.x;
                g_game_data.mouse_pos.y = mouse.y;
            }
        }
    }

    return 0;
}