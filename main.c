#include <assert.h>
#include "game.h"
#include <raylib.h>

#define FACTOR 100
#define WIDTH (16*FACTOR)
#define HEIGHT (9*FACTOR)

void platform_fill_rect(i32 x, i32 y, i32 w, i32 h, u32 color)
{
    DrawRectangle(x, y, w, h, *(Color*)&color);
}

void platform_stroke_rect(i32 x, i32 y, i32 w, i32 h, u32 color)
{
    assert(0 && "TODO: platform_stroke_rect");
}

void platform_fill_text(i32 x1, i32 y1, i32 x2, i32 y2, u32 color)
{
}

void platform_panic(const char *file_path, i32 line, const char *message)
{
    TraceLog(LOG_FATAL, "%s", message);
}

void platform_log(const char *message)
{
    TraceLog(LOG_INFO, "%s", message);
}

int main(void)
{
    InitWindow(WIDTH, HEIGHT, "Snake");
    game_init(WIDTH, HEIGHT);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        game_update(GetFrameTime());
        game_render();
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
