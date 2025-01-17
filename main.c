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
#define SPEED 150

typedef struct {
    Vector2 position;
    Vector2 cell;
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
    .direction = 1,
    .next_direction = 1,
};
static Vector2 player_velocity = {0,SPEED*-1};

void game_init()
{
    InitWindow(WIDTH, HEIGHT, "Snake");
    SetTargetFPS(60);
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
                DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, (Color){32,32,32,255});
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


    if (
        player.direction != player.next_direction && 
        ((int)player.position.x % CELL_SIZE) == 0 && 
        ((int)player.position.y % CELL_SIZE) == 0
    )
    {
        player.direction = player.next_direction;
        player.position.x = (int)player.position.x;
        player.position.y = (int)player.position.y;
        switch(player.direction){
            case 1:
                player_velocity.x = 0;
                player_velocity.y = SPEED*-1;
                break;
            case 2:
                player_velocity.x = SPEED;
                player_velocity.y = 0;
                break;
            case 3:
                player_velocity.x = 0;
                player_velocity.y = SPEED;
                break;
            case 4:
                player_velocity.x = SPEED*-1;
                player_velocity.y = 0;
                break;
            default:
                break;
        }
    }

    Vector2 new_player_position = Vector2Add(player.position, Vector2Scale(player_velocity, dt));
    player.position.x = new_player_position.x;
    player.position.y = new_player_position.y;

    render_background();

    // Player
    DrawRectangle(player.position.x, player.position.y, CELL_SIZE, CELL_SIZE, RED);

    // Food
    Food food = create_food(10, 10);
    DrawCircle(food.position.x, food.position.y, CELL_SIZE*0.5, GREEN);

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
