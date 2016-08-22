
#include "../include/level.h"
#include "../include/opengl.h"
#include "../include/texture.h"
#include "../include/game.h"
#include "../include/window.h"
#include "../include/audio.h"
#include "../include/font.h"

#include <iostream>

static texture_t* background;
static level_t this_level;

static int father_frame;
static float father_timer;

static float text_timer;
static bool sound_played;

static float color_current;

static void level3_finish()
{
	if (!this_level.initialized)
		return;

	texture_close(background);
}

static bool level3_init()
{
	if (this_level.initialized)
		return true;

	background = texture_open("data/level3.png", 1, 0.0f);

	father_frame = 0;
	father_timer = 0.0f;
    text_timer = 0.0f;
    sound_played = false;
    color_current = 1.0f;

    return background != nullptr;
}

static void color_get(bool state)
{
	if (state)
		opengl_color(color_current / 2.0f, color_current / 2.0f, color_current / 2.0f);
	else
		opengl_color(color_current, color_current, color_current);
}

static void level3_render(font_t* font, opengl_state_t* state, game_t* game)
{
    // The screen will fade to black as the character moves left.
    opengl_color(color_current, color_current, color_current);

	opengl_texture(background, 0);

	opengl_move((int)this_level.father_x, (int)this_level.father_y);
	opengl_texture(game->father, father_frame);

    if (text_timer > 1.0f && text_timer <= 2.1f)
    {
        std::string text;

        if (game->right_disabled)
            text = "No turning back now.";
        else
            text = "Nope!";

        int width = font_width(font, text);

        opengl_move(texture_frame_width(game->father) / 2 - width / 2, -10);

        // Make sure all the text stays on screen. This can only happen on the right in this case
        if (this_level.father_x + texture_frame_width(game->father) / 2 + width / 2 > REFERENCE_WIDTH)
        {
            opengl_move(REFERENCE_WIDTH - width / 2 - (int)this_level.father_x - texture_frame_width(game->father) / 2, 0);
        }

        opengl_color(0.0f, 0.0f, 0.0f);
        font_render(font, text);
    }

	opengl_restore(state);
	opengl_move(0, REFERENCE_HEIGHT - 25);
	opengl_color(0.0f, 0.0f, 0.0f);
	opengl_rectangle(REFERENCE_WIDTH, 1);
	opengl_move(0, 1);
	opengl_color(color_current, color_current, color_current);
	opengl_rectangle(REFERENCE_WIDTH, 24);

	int width = texture_frame_width(game->menu);
	int height = texture_frame_height(game->menu);

	opengl_move(REFERENCE_WIDTH / 2 - width * 2 - 4, 12 - height / 2);

	color_get(game->input.up);
	opengl_texture(game->menu, 2);
	opengl_move(width + 2, 0);

	color_get(game->input.down);
	opengl_texture(game->menu, 3);
	opengl_move(width + 2, 0);

	color_get(game->input.left);
	opengl_texture(game->menu, 0);
	opengl_move(width + 2, 0);

	if (!game->right_disabled)
	{
		color_get(game->input.right);
		opengl_texture(game->menu, 1);
		opengl_move(width + 2, 0);
	}
}

static void father_update(float dt, game_t* game)
{
	float duration = texture_frame_duration(game->father);

	father_timer += dt;

	if (father_timer >= duration)
	{
		// Talking animation
		if (father_frame == 0)
			father_frame = 12;
		else
			father_frame = 0;

		father_timer -= duration;
	}
}

static bool level3_update(float dt, game_t* game)
{
    text_timer += dt;

    if (text_timer >= 1.0f)
    {
        if (!sound_played)
        {
            audio_sound_play(game->talk);
            sound_played = true;
        }

        // If we didn't get full to the right, simply go back
        if (!game->right_disabled)
        {
            father_update(dt, game);

            if (text_timer >= 2.1f)
            {
                game->current_level = 0;
            }
        }
        else
        {
            if (text_timer <= 2.1f)
            {
                father_update(dt, game);
            }
            else if (text_timer >= 3.1f)
            {
                // We already went right and can't turn right.
                // So we go left.
                father_frame = 3;

                this_level.father_x -= 5.0f * dt;
                color_current -= dt / 20.0f;

                if (color_current <= 0.0f)
                {
                    game->current_level = 3;
                }
            }
        }
    }

	return true;
}

static void level3_change(level_t* old_level)
{
	if (old_level->number == 0)
	{
		this_level.father_y = old_level->father_y;
		this_level.father_x = REFERENCE_WIDTH - 50;

        text_timer = 0.0f;
        sound_played = false;
	}
	else if (old_level->number == 1)
	{
		this_level.father_y = old_level->father_y;
		this_level.father_x = 50;
	}
	else
	{
		std::cout << "Going to level " << this_level.number + 1 << " from unknown level: " << old_level->number + 1 << std::endl;
	}
}

level_t* level3_get()
{
	this_level.number = 2;
	this_level.father_x = 0.0f;
	this_level.father_y = 0.0f;
	this_level.initialized = false;

	this_level.init = level3_init;
	this_level.update = level3_update;
	this_level.render = level3_render;
	this_level.change = level3_change;
	this_level.finish = level3_finish;

	return &this_level;
}
