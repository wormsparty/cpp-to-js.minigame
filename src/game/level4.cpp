
#include "../include/level.h"
#include "../include/opengl.h"
#include "../include/game.h"
#include "../include/window.h"
#include "../include/font.h"
#include "../include/texture.h"

static texture_t* heart;
static level_t this_level;
static float timer;

static void level4_finish()
{
    if (!this_level.initialized)
        return;

    texture_close(heart);
}

static bool level4_init()
{
    if (this_level.initialized)
        return true;

    heart = texture_open("data/heart.png", 1, 0.0f);
    timer = 0.0f;

    return heart != nullptr;
}

static void level4_render(font_t* font, opengl_state_t* state, game_t* game)
{
    // Draw everything in black
    opengl_color(0.0f, 0.0f, 0.0f);
    opengl_rectangle(REFERENCE_WIDTH, REFERENCE_HEIGHT);

    // Add some text.
    if (timer >= 2.0f)
    {
        float c = (timer - 2.0f) / 2.0f;
        std::string text1 = "I am here my love.";

        if (c > 1.0f)
            c = 1.0f;

        opengl_color(c, c, c);
        opengl_move(REFERENCE_WIDTH / 2 - font_width(font, text1) / 2, 50);
        font_render(font, text1);
    }

    if (timer >= 4.0f)
    {
        float c2 = (timer - 4.0f) / 2.0f;

        if (c2 > 1.0f)
            c2 = 1.0f;

        std::string text2 = "We are finally together again.";

        opengl_restore(state);
        opengl_color(c2, c2, c2);
        opengl_move(REFERENCE_WIDTH / 2 - font_width(font, text2) / 2, 70);
        font_render(font, text2);
    }

    if (timer >= 6.0f)
    {
        float c3 = (timer - 6.0f) / 2.0f;

        if (c3 > 1.0f)
            c3 = 1.0f;

        opengl_restore(state);
        opengl_color(c3, c3, c3);
        opengl_move(REFERENCE_WIDTH / 2 - texture_frame_width(heart) / 2, 90);
        opengl_texture(heart, 0);
    }
}

static bool level4_update(float dt, game_t* game)
{
    timer += dt;
    return true;
}

static void level4_change(level_t* old_level)
{
}

level_t* level4_get()
{
    this_level.number = 3;
    this_level.father_x = 0.0f;
    this_level.father_y = 0.0f;
    this_level.initialized = false;

    this_level.init = level4_init;
    this_level.update = level4_update;
    this_level.render = level4_render;
    this_level.change = level4_change;
    this_level.finish = level4_finish;

    return &this_level;
}
