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
const float pipe_window_height = 60;
const float pipe_min_y = 100;
const float pipe_max_y = 300;

typedef struct gameplay_state_s {
    struct {
        Texture2D bg_day;
        Texture2D base;
        Texture2D pipe;
    } sprites;

    kvec_t(Vector2) pipes;
} gameplay_state_t;


static void draw_pipe(Vector2 center, gameplay_state_t* gameplay);
static void draw_pipes(gameplay_state_t* gameplay, game_t* game, update_context_t ctx);
static void draw_base(gameplay_state_t* gameplay, game_t* game, update_context_t ctx);
static void update_pipes(gameplay_state_t* gameplay, game_t* game);

static void _gameplay_state_enter(game_state_t* state, game_t* game) {
    state->data = malloc(sizeof(gameplay_state_t));
    assert(state->data);
    gameplay_state_t* gameplay = (gameplay_state_t*)state->data;

    *gameplay = (gameplay_state_t) {0};

    gameplay->sprites.bg_day = LoadTexture("assets/background-day.png");
    assert(IsTextureReady(gameplay->sprites.bg_day));
    gameplay->sprites.base = LoadTexture("assets/base.png");
    assert(IsTextureReady(gameplay->sprites.base));
    gameplay->sprites.pipe = LoadTexture("assets/pipe.png");
    assert(IsTextureReady(gameplay->sprites.pipe));

    kv_init(gameplay->pipes);
    for (int i = 0; i < game->canvas.texture.width / space_between_pipes + 1; i++) {
        Vector2 v = {
            game->canvas.texture.width + (float)gameplay->sprites.pipe.width / 2 + i * space_between_pipes,
            GetRandomValue(pipe_min_y, pipe_max_y)
        };
        kv_push(Vector2, gameplay->pipes, v);
    }
}

static void _gameplay_state_update(game_state_t* state, game_t* game, update_context_t ctx) {
    gameplay_state_t* gameplay = (gameplay_state_t*)state->data;

    update_pipes(gameplay, game);

    ClearBackground(WHITE);
    DrawTexture(gameplay->sprites.bg_day, 0, 0, WHITE);
    draw_pipes(gameplay, game, ctx);
    draw_base(gameplay, game, ctx);

    for (int i = 0; i < kv_size(gameplay->pipes); i++)
        DrawText(TextFormat("%.1f", kv_A(gameplay->pipes, i).y), 0, 20 * i, 16, BLACK);

    if (IsKeyPressed(KEY_ESCAPE))
        game->is_running = false;
}

static void _gameplay_state_exit(game_state_t* state, game_t* game) {
    free(state->data);
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
        gameplay->sprites.pipe,
        (Vector2)
        {
            center.x - (float)gameplay->sprites.pipe.width / 2,
            center.y + pipe_window_height / 2
        },
        0,
        1,
        WHITE
    );
    DrawTextureEx(
        gameplay->sprites.pipe,
        (Vector2)
        {
            center.x + (float)gameplay->sprites.pipe.width / 2,
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
    
    float shift = scroll_speed * (GetTime() - (int)GetTime());
    for (int i = 0; gameplay->sprites.base.width * i - shift <= game->canvas.texture.width; i++) {
        DrawTexture(
            gameplay->sprites.base,
            gameplay->sprites.base.width * i - shift,
            game->canvas.texture.height - gameplay->sprites.base.height,
            WHITE
        );
    }
}

void update_pipes(gameplay_state_t* gameplay, game_t* game) {
    for (int i = 0; i < kv_size(gameplay->pipes); i++)
        kv_A(gameplay->pipes, i).x -= scroll_speed * GetFrameTime();

    if (kv_A(gameplay->pipes, 0).x + (float)gameplay->sprites.pipe.width / 2 <= 0) {
        for (int i = 0; i < kv_size(gameplay->pipes) - 1; i++)
            kv_A(gameplay->pipes, i) = kv_A(gameplay->pipes, i + 1);
        kv_A(gameplay->pipes, kv_size(gameplay->pipes) - 1) = (Vector2) {
            kv_A(gameplay->pipes, kv_size(gameplay->pipes) - 2).x + space_between_pipes,
            GetRandomValue(pipe_min_y, pipe_max_y)
        };
    }
}
