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

#ifndef NULL
#define NULL ((void*)0)
#endif
typedef unsigned int size_t;


typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef int i32;
typedef int b32;
typedef float f32;

#define FACTOR 180
#define WIDTH (7*FACTOR)
#define HEIGHT (4*FACTOR)
#define CELL_SIZE 12
#define MAX_TIME 0.05f

__attribute__((aligned(4))) unsigned char memory[65536]; // 64KB of linear memory
unsigned char* brk = memory;

__attribute__((import_module("env"), import_name("GameOver")))
void GameOver(void);

void* malloc(size_t size) {
    size = (size + 3) & ~3;
    if (brk + size > memory + sizeof(memory)) {
        return NULL;
    }
    void* allocated = brk;
    brk += size;
    return allocated;
}

void free(void* ptr) {
    // No-op for bump allocator
}

typedef struct Node {
    Vector2 cell;
    struct Node* previous;
    struct Node* next;
} Node;

typedef struct {
    Node* head;
    Node* tail;
    int length;
} Snake;

typedef struct {
    Vector2 cell;
    Vector2 target;
    u8 direction;
    u8 next_direction;
    Snake* snake;
    float timer;
} Player;

Snake* create_snake(Vector2 start_cell) {
    Snake* snake = malloc(sizeof(Snake));
    if (!snake) {
        TraceLog(LOG_FATAL, "%s", "Failed to malloc Snake");
    }
    Node* initial_segment = malloc(sizeof(Node));
    if (!initial_segment) {
        TraceLog(LOG_FATAL, "%s", "Failed to malloc Node");
    }
    initial_segment->cell = start_cell;
    initial_segment->previous = NULL;
    initial_segment->next = NULL;
    snake->head = initial_segment;
    snake->tail = initial_segment;
    snake->length = 1;
    return snake;
}

void add_snake_segment(Snake* snake, Vector2 cell) {
    Node* new_segment = malloc(sizeof(Node));
    if (!new_segment) {
        TraceLog(LOG_FATAL, "%s", "Failed to malloc Node");
    }
    new_segment->cell = cell;
    new_segment->previous = snake->tail;
    new_segment->next = NULL;
    if (snake->tail) {
        snake->tail->next = new_segment;
    }
    snake->tail = new_segment;
    if (snake->length == 0) {
        snake->head = new_segment;
    }
    snake->length++;
}

void move_snake(Snake* snake, Vector2 new_cell) {
    Node* current = snake->tail;
    while (current->previous) {
        current->cell = current->previous->cell;
        current = current->previous;
    }
    snake->head->cell = new_cell;
}

typedef struct {
    Vector2 position;
    Vector2 cell;
} Food;

static Player player = { 
    .timer = 0,
    .cell = { .x = 20, .y = 20 },
    .target = { .x = 20, .y = 20 },
    .direction = 1,
    .next_direction = 1,
};
bool game_over = false;
bool debug_hitbox = false;

void game_init(bool debug)
{
    debug_hitbox = debug;
    player.snake = create_snake((Vector2){ .x = 20, .y = 20});
    add_snake_segment(player.snake, (Vector2){ .x = 20, .y = 21});
    add_snake_segment(player.snake, (Vector2){ .x = 20, .y = 22});
    InitWindow(WIDTH, HEIGHT, "Snake");
    SetTargetFPS(60);
}

void render_background()
{
    for (i32 y = 0; y < HEIGHT/CELL_SIZE; y++)
    {
        for(i32 x = 0; x < WIDTH/CELL_SIZE; x++)
        {
            if ((x+y)%2 == 0)
            {
                DrawRectangle(x*CELL_SIZE, y*CELL_SIZE, CELL_SIZE, CELL_SIZE, (Color){18,18,18,255});
            }
            else
            {
                DrawRectangle(x*CELL_SIZE, y*CELL_SIZE, CELL_SIZE, CELL_SIZE, (Color){24,24,24,255});
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

    player.timer += dt;

    if (player.timer >= MAX_TIME) {
        player.timer = 0;
        player.direction = player.next_direction;
        switch(player.direction){
            case 1:
                player.target.y--;
                break;
            case 2:
                player.target.x++;
                break;
            case 3:
                player.target.y++;
                break;
            case 4:
                player.target.x--;
                break;
            default:
                break;
        }
        move_snake(player.snake, player.target);
        if (
            player.snake->head->cell.x < 0 || player.snake->head->cell.x >= (int)(WIDTH/CELL_SIZE) ||
            player.snake->head->cell.y < 0 || player.snake->head->cell.y >= (int)(HEIGHT/CELL_SIZE)
        )
        {
            game_over = true;
            GameOver();
        }
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

        Node* current = player.snake->tail;
        int idx = 1;
        while (current) {
            Vector2 displayPos = { .x = current->cell.x*CELL_SIZE, .y = current->cell.y*CELL_SIZE};
            Color clr = GREEN;
            if (idx == player.snake->length) clr = RED;
            DrawRectangle(displayPos.x, displayPos.y, CELL_SIZE, CELL_SIZE, clr);
            idx++;
            current = current->previous;
        }
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
