#include <raylib.h>
float floorf(float);
float fabsf(float);
double fabs(double);
float fmaxf(float, float);
float fminf(float, float);
float sqrtf(float);
float atan2f(float, float);
float cosf(float);
float sinf(float);
double tan(double);
float acosf(float);
float asinf(float);
#include <raymath.h>

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
#define SPEED 0.25f

typedef struct {
    Vector2 position;
    Vector2 cell;
    Vector2 target;
    u8 direction;
    u8 next_direction;
} Player;

typedef struct {
    Vector2 position;
    Vector2 cell;
} Food;

static Player player = { 
    .position = { .x = CELL_SIZE*50, .y = CELL_SIZE*50 }, 
    .cell = { .x = 50, .y = 50 },
    .target = { .x = 50, .y = 49 },
    .direction = 1,
    .next_direction = 1,
};
static Vector2 player_velocity = {0,-CELL_SIZE};
bool game_over = false;
bool debug_hitbox = false;

__attribute__((import_module("env"), import_name("GameOver")))
void GameOver(void);

void game_init(bool debug)
{
    InitWindow(WIDTH, HEIGHT, "Snake");
    SetTargetFPS(60);
    debug_hitbox = debug;
}

void render_background()
{
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
                DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, (Color){24,24,24,255});
            }
        }
    }
}

Food create_food(i32 cell_x, i32 cell_y)
{
    Food food = { 
        .cell = { .x = cell_x, .y = cell_y }, 
        .position = { .x = CELL_SIZE * cell_x - (CELL_SIZE*0.5) - 1, .y = CELL_SIZE * cell_y - (CELL_SIZE*0.5) - 1 } 
    };
    return food;
}

void game_keydown(i32 key)
{
    switch(key)
    {
        case 75:
        case 38:
        case 87:
            if (player.direction != 3) {
                player.next_direction = 1;
            }
            break;
        case 74:
        case 40:
        case 83:
            if (player.direction != 1) {
                player.next_direction = 3;
            }
            break;
        case 72:
        case 37:
        case 65:
            if (player.direction != 2) {
                player.next_direction = 4;
            }
            break;
        case 76:
        case 39:
        case 68:
            if (player.direction != 4) {
                player.next_direction = 2;
            }
            break;
        default:
            break;
    }
}

void game_update(f32 dt)
{
    BeginDrawing();
    ClearBackground((Color){18,18,18,255});

    player.position = Vector2Add(player.position, Vector2Scale(player_velocity, SPEED));
    player.cell.x = (int)(player.position.x/CELL_SIZE);
    player.cell.y = (int)(player.position.y/CELL_SIZE);

    i32 xTileOffset = 0;
    i32 yTileOffset = 0;
    /*switch(player.direction){*/
        /*case 1:*/
            /*yTileOffset = -1;*/
            /*break;*/
        /*case 2:*/
            /*xTileOffset = 1;*/
            /*break;*/
        /*case 3:*/
            /*yTileOffset = 1;*/
            /*break;*/
        /*case 4:*/
            /*xTileOffset = -1;*/
            /*break;*/
        /*default:*/
            /*break;*/
    /*}*/

    if (
       (int)player.cell.x == (int)player.target.x + xTileOffset &&
       (int)player.cell.y == (int)player.target.y + yTileOffset
    )
    {
        player.direction = player.next_direction;
        switch(player.direction){
            case 1:
                player_velocity.x = 0;
                player_velocity.y = -CELL_SIZE;
                player.target.x = player.cell.x;
                player.target.y = player.cell.y - 1;
                player.position.x = player.cell.x * CELL_SIZE;
                break;
            case 2:
                player_velocity.x = CELL_SIZE;
                player_velocity.y = 0;
                player.target.x = player.cell.x + 1;
                player.target.y = player.cell.y;
                player.position.y = player.cell.y * CELL_SIZE;
                break;
            case 3:
                player_velocity.x = 0;
                player_velocity.y = CELL_SIZE;
                player.target.x = player.cell.x;
                player.target.y = player.cell.y + 1;
                player.position.x = player.cell.x * CELL_SIZE;
                break;
            case 4:
                player_velocity.x = -CELL_SIZE;
                player_velocity.y = 0;
                player.target.x = player.cell.x - 1;
                player.target.y = player.cell.y;
                player.position.y = player.cell.y * CELL_SIZE;
                break;
            default:
                break;
        }
    }

    i32 xOffset = 0;
    i32 yOffset = 0;
    switch(player.direction){
        case 1:
            yOffset = -CELL_SIZE*0.5;
            break;
        case 2:
            xOffset = CELL_SIZE*0.5;
            break;
        case 3:
            yOffset = CELL_SIZE*0.5;
            break;
        case 4:
            xOffset = -CELL_SIZE*0.5;
            break;
        default:
            break;
    }


    if (
        player.position.x < 0 || player.position.x >= WIDTH ||
        player.position.y < 0 || player.position.y >= HEIGHT
    )
    {
        game_over = true;
        GameOver();
    }

    render_background();

    if (!game_over)
    {
        // Player target cell
        if (debug_hitbox)
        {
            DrawRectangle(player.target.x*CELL_SIZE, player.target.y*CELL_SIZE, CELL_SIZE, CELL_SIZE, BLUE);
            DrawRectangle(player.cell.x*CELL_SIZE, player.cell.y*CELL_SIZE, CELL_SIZE, CELL_SIZE, ORANGE);
        }

        // Player
        Vector2 displayPos = { .x = player.position.x+xOffset, .y = player.position.y+yOffset};
        if (displayPos.x < 0) displayPos.x = 0;
        else if (displayPos.x + CELL_SIZE - xOffset > WIDTH) displayPos.x = WIDTH - CELL_SIZE;
        if (displayPos.y < 0) displayPos.y = 0;
        else if (displayPos.y + CELL_SIZE - yOffset > HEIGHT) displayPos.y = HEIGHT - CELL_SIZE;
        DrawRectangle(displayPos.x, displayPos.y, CELL_SIZE, CELL_SIZE, RED);
    }

    // Food
    Food food = create_food(10, 10);
    DrawCircle(food.position.x, food.position.y, CELL_SIZE*0.5, GREEN);

    EndDrawing();
}

#ifdef PLATFORM_NATIVE
void GameOver(){}

int main(void)
{
    game_init(false);
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
