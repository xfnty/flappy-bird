#include "game/states/gameplay.h"

#include <assert.h>
#include <math.h>

#include <raylib.h>

#include "system/defines.h"
#include "system/strid.h"
#define SCOPE_NAME "gameplay"
#include "system/logging.h"

#include "game/game.h"


const float pipe_speed = 100;
const float space_between_pipes = 100;

typedef struct gameplay_state_s {
    Texture2D bg_day;
    Texture2D base;
    Texture2D pipe;
    float pipe_window_Ys[2];  // 0 means no pipe, gets shifted right to left
} gameplay_state_t;


static void draw_pipes(gameplay_state_t* gameplay, game_t* game, update_context_t ctx);
static void draw_base(gameplay_state_t* gameplay, game_t* game, update_context_t ctx);

static void _gameplay_state_enter(game_state_t* state, game_t* game) {
    state->data = malloc(sizeof(gameplay_state_t));
    assert(state->data);
    gameplay_state_t* gameplay = (gameplay_state_t*)state->data;

    *gameplay = (gameplay_state_t) {0};
    
    gameplay->bg_day = LoadTexture("assets/background-day.png");
    assert(IsTextureReady(gameplay->bg_day));
    gameplay->base = LoadTexture("assets/base.png");
    assert(IsTextureReady(gameplay->base));
    gameplay->pipe = LoadTexture("assets/pipe.png");
    assert(IsTextureReady(gameplay->pipe));
}

static void _gameplay_state_update(game_state_t* state, game_t* game, update_context_t ctx) {
    gameplay_state_t* gameplay = (gameplay_state_t*)state->data;

    if (IsKeyPressed(KEY_ESCAPE))
        game->is_running = false;

    ClearBackground(WHITE);
    DrawTexture(gameplay->bg_day, 0, 0, WHITE);
 
    draw_base(gameplay, game, ctx);
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

void draw_pipes(gameplay_state_t* gameplay, game_t* game, update_context_t ctx) {
    const int max_pipe_count = STACKARRAY_SIZE(gameplay->pipe_window_Ys);
    for (int i = max_pipe_count - 1; i >= 0; i--) {
        if (!gameplay->pipe_window_Ys[i])
            continue;

        // float pipe_y =
        //     game->canvas.texture.width
        //     - (game->canvas.texture.width / (float)max_pipe_count * i);
        
        // NOTE: max_pipe_count = (int)(game->canvas.texture.width / space_between_pipes)
    }
}

void draw_base(gameplay_state_t* gameplay, game_t* game, update_context_t ctx) {
    // FIXME: flickering
    
    float shift = (GetTime() - (int)GetTime()) * pipe_speed;
    for (int i = 0; gameplay->base.width * i - shift <= game->canvas.texture.width; i++) {
        DrawTexture(
            gameplay->base,
            gameplay->base.width * i - shift,
            game->canvas.texture.height - gameplay->base.height,
            WHITE
        );
    }
}
