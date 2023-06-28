#include "game/states/gameplay.h"

#include <assert.h>
#include <math.h>

#include <kvec.h>
#include <raylib.h>

#include "system/defines.h"
#include "system/strid.h"
#define SCOPE_NAME "gameplay"
#include "system/logging.h"

#include "game/game.h"


const float scroll_speed = 75;
const float space_between_pipes = 200;
const float pipe_window_height = 80;
const float pipe_min_y = 100;
const float pipe_max_y = 300;
const float bird_up_force = 150;
const float bird_gravity_force = 400;

static struct {
    Texture2D bg_day;
    Texture2D base;
    Texture2D pipe;
    Texture2D bird;
} sprites = {0};

typedef struct gameplay_state_s {
    bool is_paused;
    float bird_y;
    float bird_y_speed;
    kvec_t(Vector2) pipes;
} gameplay_state_t;


static void draw_pipe(Vector2 center, gameplay_state_t* gameplay);
static void draw_pipes(gameplay_state_t* gameplay, game_t* game, update_context_t ctx);
static void draw_base(gameplay_state_t* gameplay, game_t* game, update_context_t ctx);
static void draw_bird(gameplay_state_t* gameplay, game_t* game);
static void update_bird(gameplay_state_t* gameplay, game_t* game);
static void update_pipes(gameplay_state_t* gameplay, game_t* game);

static void _gameplay_state_enter(game_state_t* state, game_t* game) {
    state->data = malloc(sizeof(gameplay_state_t));
    assert(state->data);
    gameplay_state_t* gameplay = (gameplay_state_t*)state->data;

    *gameplay = (gameplay_state_t) {0};

    if (sprites.bg_day.id == 0) sprites.bg_day = LoadTexture("assets/background-day.png");
    if (sprites.base.id == 0) sprites.base = LoadTexture("assets/base.png");
    if (sprites.pipe.id == 0) sprites.pipe = LoadTexture("assets/pipe.png");
    if (sprites.bird.id == 0) sprites.bird = LoadTexture("assets/midflap.png");

    gameplay->bird_y = ((float)game->canvas.texture.height - sprites.base.height) / 2;

    kv_init(gameplay->pipes);
    for (int i = 0; i < game->canvas.texture.width / space_between_pipes + 1; i++) {
        Vector2 v = {
            game->canvas.texture.width + (float)sprites.pipe.width / 2 + i * space_between_pipes,
            GetRandomValue(pipe_min_y, pipe_max_y)
        };
        kv_push(Vector2, gameplay->pipes, v);
    }
}

static void _gameplay_state_update(game_state_t* state, game_t* game, update_context_t ctx) {
    gameplay_state_t* gameplay = (gameplay_state_t*)state->data;

    ClearBackground(WHITE);
    DrawTexture(sprites.bg_day, 0, 0, WHITE);

    if (!gameplay->is_paused) {
        update_pipes(gameplay, game);
        update_bird(gameplay, game);
    }

    draw_pipes(gameplay, game, ctx);
    draw_base(gameplay, game, ctx);
    draw_bird(gameplay, game);

    if (IsKeyPressed(KEY_ENTER))
        gameplay->is_paused = !gameplay->is_paused;
    if (IsKeyPressed(KEY_ESCAPE))
        game->is_running = false;
}

static void _gameplay_state_exit(game_state_t* state, game_t* game) {
    assert(state != NULL);
    assert(game != NULL);
    gameplay_state_t* gameplay = (gameplay_state_t*)state->data;

    kv_destroy(gameplay->pipes);
    kv_init(gameplay->pipes);

    free(state->data);
    memset(gameplay, 0, sizeof(*gameplay));
}

game_state_t gameplay_state_create() {
    return (game_state_t) {
        .name = strid_get_strid(SCOPE_NAME),
        .data = NULL,
        .on_enter = _gameplay_state_enter,
        .on_update = _gameplay_state_update,
        .on_exit = _gameplay_state_exit
    };
}

void draw_pipe(Vector2 center, gameplay_state_t* gameplay) {
    DrawTextureEx(
        sprites.pipe,
        (Vector2)
        {
            center.x - (float)sprites.pipe.width / 2,
            center.y + pipe_window_height / 2
        },
        0,
        1,
        WHITE
    );
    DrawTextureEx(
        sprites.pipe,
        (Vector2)
        {
            center.x + (float)sprites.pipe.width / 2,
            center.y - pipe_window_height / 2
        },
        180,
        1,
        WHITE
    );
}

void draw_pipes(gameplay_state_t* gameplay, game_t* game, update_context_t ctx) {
    for (int i = 0; i < kv_size(gameplay->pipes); i++)
        if (kv_A(gameplay->pipes, i).y != 0)
            draw_pipe(kv_A(gameplay->pipes, i), gameplay);
}

void draw_base(gameplay_state_t* gameplay, game_t* game, update_context_t ctx) {
    // FIXME: flickering every second
    
    float shift = (gameplay->is_paused) ? 0 : scroll_speed * (GetTime() - (int)GetTime());
    for (int i = 0; sprites.base.width * i - shift <= game->canvas.texture.width; i++) {
        DrawTexture(
            sprites.base,
            sprites.base.width * i - shift,
            game->canvas.texture.height - sprites.base.height,
            WHITE
        );
    }
}

void draw_bird(gameplay_state_t* gameplay, game_t* game) {
    DrawTexturePro(
        sprites.bird,
        (Rectangle){0,0,sprites.bird.width,sprites.bird.height},
        (Rectangle){
            game->canvas.texture.width / 2.0f,
            gameplay->bird_y,
            sprites.bird.width,
            sprites.bird.height
        },
        (Vector2){sprites.bird.width/2.0f, sprites.bird.height/2.0f},
        gameplay->bird_y_speed / 6.0f,
        WHITE
    );
}

void update_bird(gameplay_state_t* gameplay, game_t* game) {
    gameplay->bird_y_speed += bird_gravity_force * GetFrameTime();
    if (IsKeyPressed(KEY_SPACE))
        gameplay->bird_y_speed = -bird_up_force;

    gameplay->bird_y += gameplay->bird_y_speed * GetFrameTime();

    if (gameplay->bird_y + sprites.bird.height >= game->canvas.texture.height - sprites.base.height) {
        game_switch_state(game, gameplay_state_create());
        return;
    }

    Rectangle bird_rect = {
        (float)game->canvas.texture.width / 2 - (float)sprites.bird.width / 2,
        gameplay->bird_y - (float)sprites.bird.height / 2,
        sprites.bird.width,
        sprites.bird.height
    };
    DrawRectangleLinesEx(bird_rect, 1, RED);

    for (int i = 0; i < kv_size(gameplay->pipes); i++) {
        Vector2 center = kv_A(gameplay->pipes, i);
        Vector2 pipe_size = {(float)sprites.pipe.width, (float)sprites.pipe.height};
        Rectangle top_pipe_rect = {
            .x = center.x - pipe_size.x / 2,
            .y = center.y - pipe_window_height / 2 - pipe_size.y,
            .width = pipe_size.x,
            .height = pipe_size.y
        };
        Rectangle bottom_pipe_rect = {
            .x = center.x - pipe_size.x / 2,
            .y = center.y + pipe_window_height / 2,
            .width = pipe_size.x,
            .height = pipe_size.y
        };

        DrawRectangleLinesEx(top_pipe_rect, 1, ORANGE);
        DrawRectangleLinesEx(bottom_pipe_rect, 1, ORANGE);

        if (CheckCollisionRecs(bird_rect, top_pipe_rect) || CheckCollisionRecs(bird_rect, bottom_pipe_rect)) {
            game_switch_state(game, gameplay_state_create());
            return;
        }
    }
}

void update_pipes(gameplay_state_t* gameplay, game_t* game) {
    for (int i = 0; i < kv_size(gameplay->pipes); i++)
        kv_A(gameplay->pipes, i).x -= scroll_speed * GetFrameTime();

    if (kv_A(gameplay->pipes, 0).x + (float)sprites.pipe.width / 2 <= 0) {
        for (int i = 0; i < kv_size(gameplay->pipes) - 1; i++)
            kv_A(gameplay->pipes, i) = kv_A(gameplay->pipes, i + 1);
        kv_A(gameplay->pipes, kv_size(gameplay->pipes) - 1) = (Vector2) {
            kv_A(gameplay->pipes, kv_size(gameplay->pipes) - 2).x + space_between_pipes,
            GetRandomValue(pipe_min_y, pipe_max_y)
        };
    }
}
