#include <raylib.h>

typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef int i32;
typedef int b32;
typedef float f32;

#define FACTOR 80
#define WIDTH (16*FACTOR)
#define HEIGHT (9*FACTOR)

void game_init()
{
    InitWindow(WIDTH, HEIGHT, "Snake");
    SetTargetFPS(60);
}

void game_update(f32 dt)
{
    BeginDrawing();
    ClearBackground((Color){18,18,18,255});
    DrawRectangle(WIDTH*0.5, HEIGHT*0.5, 100, 100, RED);
    DrawCircle(100, 100, 500, BLUE);
    EndDrawing();
}

#ifdef PLATFORM_NATIVE
int main(void)
{
    game_init();
    while (!WindowShouldClose())
    {
        BeginDrawing();
        game_update(GetFrameTime());
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
#endif
