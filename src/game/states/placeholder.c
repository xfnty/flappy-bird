#include <game/states/placeholder.h>

#include <assert.h>
#include <math.h>

#include <raylib.h>

#include "system/strid.h"
#define SCOPE_NAME "placeholder"
#include "system/logging.h"

#include "game/game.h"


typedef struct placeholder_state_s {
    int _;
} placeholder_state_t;

static void _placeholder_state_enter(game_state_t* state, game_t* game) {
    state->data = malloc(sizeof(placeholder_state_t));
    assert(state->data);
    placeholder_state_t* placeholder = (placeholder_state_t*)state->data;

    *placeholder = (placeholder_state_t) {0};
}

static void _placeholder_state_update(game_state_t* state, game_t* game, update_context_t ctx) {
    placeholder_state_t* placeholder = (placeholder_state_t*)state->data;

    if (IsKeyPressed(KEY_ESCAPE))
        game->is_running = false;
}

static void _placeholder_state_exit(game_state_t* state, game_t* game) {
    free(state->data);
}

game_state_t placeholder_state_create() {
    return (game_state_t) {
        .name = strid_get_strid(SCOPE_NAME),
        .data = NULL,
        .on_enter = _placeholder_state_enter,
        .on_update = _placeholder_state_update,
        .on_exit = _placeholder_state_exit
    };
}
