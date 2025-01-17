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
#define CELL_SIZE 12

void game_init()
{
    InitWindow(WIDTH, HEIGHT, "Snake");
    SetTargetFPS(60);
}

void game_update(f32 dt)
{
    BeginDrawing();
    ClearBackground((Color){18,18,18,255});
    
    for (i32 y = 0; y < HEIGHT; y+=CELL_SIZE)
    {
        for(i32 x = 0; x < WIDTH; x+=CELL_SIZE)
        {
            if (((x/CELL_SIZE)+(y/CELL_SIZE))%2 == 0)
            {
                DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, (Color){18,18,18,255});
            }
            else
            {
                DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, (Color){32,32,32,255});
            }
        }
    }

    DrawRectangle(12*20, 12*20, CELL_SIZE, CELL_SIZE, RED);
    DrawCircle(12*5-7, 12*5-7, CELL_SIZE*0.5, GREEN);
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
