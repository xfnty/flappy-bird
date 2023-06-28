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


const float pipe_speed = 100;
const float space_between_pipes = 100;

typedef struct gameplay_state_s {
    struct {
        Texture2D bg_day;
        Texture2D base;
        Texture2D pipe;
    } sprites;

    kvec_t(Vector2) pipes;
} gameplay_state_t;


static void draw_pipes(gameplay_state_t* gameplay, game_t* game, update_context_t ctx);
static void draw_base(gameplay_state_t* gameplay, game_t* game, update_context_t ctx);

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
}

static void _gameplay_state_update(game_state_t* state, game_t* game, update_context_t ctx) {
    gameplay_state_t* gameplay = (gameplay_state_t*)state->data;

    if (IsKeyPressed(KEY_ESCAPE))
        game->is_running = false;

    ClearBackground(WHITE);
    DrawTexture(gameplay->sprites.bg_day, 0, 0, WHITE);
 
    draw_pipes(gameplay, game, ctx);
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
    float center_y = ctx.mouse.y;
    float window_height = 50;

    DrawTextureEx(
        gameplay->sprites.pipe,
        (Vector2)
        {
            (float)game->canvas.texture.width / 2 - (float)gameplay->sprites.pipe.width / 2,
            center_y + window_height / 2
        },
        0,
        1,
        WHITE
    );
    DrawTextureEx(
        gameplay->sprites.pipe,
        (Vector2)
        {
            (float)game->canvas.texture.width / 2 + (float)gameplay->sprites.pipe.width / 2,
            center_y - window_height / 2
        },
        180,
        1,
        WHITE
    );
}

void draw_base(gameplay_state_t* gameplay, game_t* game, update_context_t ctx) {
    // FIXME: flickering
    
    float shift = (GetTime() - (int)GetTime()) * pipe_speed;
    for (int i = 0; gameplay->sprites.base.width * i - shift <= game->canvas.texture.width; i++) {
        DrawTexture(
            gameplay->sprites.base,
            gameplay->sprites.base.width * i - shift,
            game->canvas.texture.height - gameplay->sprites.base.height,
            WHITE
        );
    }
}
