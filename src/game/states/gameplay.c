#include "game/states/gameplay.h"

#include <math.h>
#include <stdio.h>
#include <assert.h>

#include <kvec.h>
#include <raylib.h>

#include "system/defines.h"
#include "system/strid.h"
#define SCOPE_NAME "gameplay"
#include "system/logging.h"

#include "game/game.h"


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
    Texture2D bird_midflap;
    Texture2D bird_downflap;
    Texture2D bird_upflap;
    Texture2D gameover;
    Texture2D intro;
    Texture2D digits[10];
} sprites = {0};

static struct {
    Music flap;
    Sound gameover;
} sounds = {0};

typedef struct gameplay_state_s {
    bool is_paused;
    float bird_y;
    float bird_y_speed;
    bool is_bird_between_pipes;
    bool is_debug;
    bool is_gameover;
    bool is_started;
    float scroll_speed;
    int score;
    kvec_t(Vector2) pipes;
} gameplay_state_t;


static void draw_pipe(Vector2 center, gameplay_state_t* gameplay);
static void draw_pipes(gameplay_state_t* gameplay, game_t* game, update_context_t ctx);
static void draw_base(gameplay_state_t* gameplay, game_t* game, update_context_t ctx);
static void draw_bird(gameplay_state_t* gameplay, game_t* game);
static void draw_score(gameplay_state_t* gameplay, game_t* game);
static void debug_draw(gameplay_state_t* gameplay, game_t* game);
static void update_bird(gameplay_state_t* gameplay, game_t* game);
static void update_pipes(gameplay_state_t* gameplay, game_t* game);

static void _gameplay_state_enter(game_state_t* state, game_t* game) {
    state->data = malloc(sizeof(gameplay_state_t));
    assert(state->data);
    gameplay_state_t* gameplay = (gameplay_state_t*)state->data;

    *gameplay = (gameplay_state_t) {0};

    gameplay->scroll_speed = 75;

    if (sprites.bg_day.id == 0)         sprites.bg_day          = LoadTexture("assets/background-day.png");
    if (sprites.base.id == 0)           sprites.base            = LoadTexture("assets/base.png");
    if (sprites.pipe.id == 0)           sprites.pipe            = LoadTexture("assets/pipe.png");
    if (sprites.bird_downflap.id == 0)  sprites.bird_downflap   = LoadTexture("assets/downflap.png");
    if (sprites.bird_upflap.id == 0)    sprites.bird_upflap     = LoadTexture("assets/upflap.png");
    if (sprites.bird_midflap.id == 0)   sprites.bird_midflap    = LoadTexture("assets/midflap.png");
    if (sprites.gameover.id == 0)       sprites.gameover        = LoadTexture("assets/gameover.png");
    if (sprites.intro.id == 0)          sprites.intro           = LoadTexture("assets/message.png");
    for (int i = 0; i < STACKARRAY_SIZE(sprites.digits); i++)
        if (sprites.digits[i].id == 0) sprites.digits[i] = LoadTexture(TextFormat("assets/%d.png", i));

    if (!IsMusicReady(sounds.flap))     sounds.flap     = LoadMusicStream("assets/die.mp3");
    if (!IsSoundReady(sounds.gameover)) sounds.gameover = LoadSoundFromWave(LoadWave("assets/die.wav"));

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

    if (!gameplay->is_paused && gameplay->is_started) {
        update_pipes(gameplay, game);
        update_bird(gameplay, game);
    }

    draw_pipes(gameplay, game, ctx);
    draw_base(gameplay, game, ctx);
    if (gameplay->is_started) draw_bird(gameplay, game);
    draw_score(gameplay, game);

    if (gameplay->is_debug)
        debug_draw(gameplay, game);

    if (IsKeyPressed(KEY_ENTER))
        gameplay->is_paused = !gameplay->is_paused;
    if (IsKeyPressed(KEY_D))
        gameplay->is_debug = !gameplay->is_debug;
    if (IsKeyPressed(KEY_ESCAPE))
        game->is_running = false;
    if (gameplay->is_gameover && IsKeyPressed(KEY_SPACE))
        game_switch_state(game, gameplay_state_create());
    if (!gameplay->is_started && (GetKeyPressed() || IsMouseButtonPressed(MOUSE_LEFT_BUTTON))) {
        gameplay->is_started = true;
    }
}

static void _gameplay_state_exit(game_state_t* state, game_t* game) {
    assert(state != NULL);
    assert(game != NULL);
    gameplay_state_t* gameplay = (gameplay_state_t*)state->data;

    kv_destroy(gameplay->pipes);
    kv_init(gameplay->pipes);

    // memset(gameplay, 0, sizeof(*gameplay));
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
    float shift = (gameplay->is_paused || !gameplay->is_started) ? 0 : gameplay->scroll_speed * (GetTime() - (int)GetTime());
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
    float r = gameplay->bird_y_speed / 6.0f;
    Texture2D* sprite = (r < -10)
        ? &sprites.bird_downflap
        : (r < 10
            ? &sprites.bird_midflap
            : &sprites.bird_upflap);

    DrawTexturePro(
        *sprite,
        (Rectangle){0,0,sprite->width,sprite->height},
        (Rectangle){
            game->canvas.texture.width / 2.0f,
            gameplay->bird_y,
            sprite->width,
            sprite->height
        },
        (Vector2){sprite->width/2.0f, sprite->height/2.0f},
        MIN(gameplay->bird_y_speed / 6.0f, 90),
        WHITE
    );
}

void draw_score(gameplay_state_t* gameplay, game_t* game) {
    if (!gameplay->is_started) {
        DrawTexture(sprites.intro, game->canvas.texture.width / 2.0f - sprites.intro.width / 2.0f, 50, WHITE);
        return;
    }

    const char* s = TextFormat("%d", gameplay->score);
    float l = 0;

    for (int i = 0; i < TextLength(s); i++)
        l += sprites.digits[s[i] - '0'].width;

    float t = 0;
    for (int i = 0; i < TextLength(s); i++) {
        DrawTexture(
            sprites.digits[s[i] - '0'],
            game->canvas.texture.width / 2.0f - l / 2 + t,
            50,
            WHITE
        );
        t += sprites.digits[s[i] - '0'].width;
    }

    if (gameplay->is_gameover)
        DrawTexture(
            sprites.gameover,
            game->canvas.texture.width / 2.0f - sprites.gameover.width / 2.0f,
            100,
            WHITE
        );
}

void debug_draw(gameplay_state_t* gameplay, game_t* game) {
    DrawRectangleLinesEx(
        (Rectangle) {
            game->canvas.texture.width / 2.0f - sprites.bird_midflap.width / 2.0f,
            gameplay->bird_y - (float)sprites.bird_midflap.height / 2,
            sprites.bird_midflap.width,
            sprites.bird_midflap.height
        },
        1,
        RED
    );

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
        Rectangle score_trigger_rect = {
            .x = center.x - pipe_size.x / 2,
            .y = center.y - pipe_window_height / 2,
            .width = pipe_size.x,
            .height = pipe_window_height
        };

        DrawRectangleLinesEx(top_pipe_rect, 1, ORANGE);
        DrawRectangleLinesEx(bottom_pipe_rect, 1, ORANGE);
        DrawRectangleLinesEx(score_trigger_rect, 3, GREEN);
    }
}

void update_bird(gameplay_state_t* gameplay, game_t* game) {
    if (IsKeyPressed(KEY_SPACE) && !gameplay->is_gameover) {
        gameplay->bird_y_speed = -bird_up_force;
        PlayMusicStream(sounds.flap);
    }

    gameplay->bird_y += gameplay->bird_y_speed * GetFrameTime();

    if (gameplay->is_gameover) {
        gameplay->bird_y = CONSTRAIN(gameplay->bird_y, 0, game->canvas.texture.height - sprites.base.height - sprites.bird_midflap.height);
        if (gameplay->bird_y < game->canvas.texture.height - sprites.base.height - sprites.bird_midflap.height) {
            gameplay->bird_y_speed += bird_gravity_force * GetFrameTime();
        }
        return;
    }

    gameplay->bird_y_speed += bird_gravity_force * GetFrameTime();

    if (gameplay->bird_y + sprites.bird_midflap.height >= game->canvas.texture.height - sprites.base.height) {
        gameplay->is_gameover = true;
        gameplay->scroll_speed = 0;
    }

    Rectangle bird_rect = {
        (float)game->canvas.texture.width / 2 - (float)sprites.bird_midflap.width / 2,
        gameplay->bird_y - (float)sprites.bird_midflap.height / 2,
        sprites.bird_midflap.width,
        sprites.bird_midflap.height
    };

    bool bird_between_pipes = false;
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
        Rectangle score_trigger_rect = {
            .x = center.x - pipe_size.x / 2,
            .y = center.y - pipe_window_height / 2,
            .width = pipe_size.x,
            .height = pipe_window_height
        };

        if (CheckCollisionRecs(bird_rect, top_pipe_rect) || CheckCollisionRecs(bird_rect, bottom_pipe_rect)) {
            gameplay->is_gameover = true;
            gameplay->scroll_speed = 0;
        }
        
        bird_between_pipes |= CheckCollisionRecs(bird_rect, score_trigger_rect);
    }

    if (!bird_between_pipes && gameplay->is_bird_between_pipes)
        gameplay->score++;
    gameplay->is_bird_between_pipes = bird_between_pipes;
}

void update_pipes(gameplay_state_t* gameplay, game_t* game) {
    for (int i = 0; i < kv_size(gameplay->pipes); i++)
        kv_A(gameplay->pipes, i).x -= gameplay->scroll_speed * GetFrameTime();

    if (kv_A(gameplay->pipes, 0).x + (float)sprites.pipe.width / 2 <= 0) {
        for (int i = 0; i < kv_size(gameplay->pipes) - 1; i++)
            kv_A(gameplay->pipes, i) = kv_A(gameplay->pipes, i + 1);
        kv_A(gameplay->pipes, kv_size(gameplay->pipes) - 1) = (Vector2) {
            kv_A(gameplay->pipes, kv_size(gameplay->pipes) - 2).x + space_between_pipes,
            GetRandomValue(pipe_min_y, pipe_max_y)
        };
    }
}
