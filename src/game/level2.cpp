
#include "../include/level.h"
#include "../include/opengl.h"
#include "../include/texture.h"
#include "../include/game.h"
#include "../include/window.h"
#include "../include/audio.h"
#include "../include/font.h"

#include <iostream>

static texture_t* background;
static audio_t* drop;

static level_t this_level;

static int father_frame;
static float father_timer;
static int repeat_count;

static int boredom_current;
static float text_timer;

static float disable_timer;

#define BOREDOM_MAX 6

static std::string boredom_text[] = {
	"This looks familiar.",
	"Is this really getting somewhere?",
	"Again?",
	"Why are you making me do this?",
	"Don't you understand?",
	"See? It's the same, again!",
	"You know what? @#% this."
};

static void level2_finish()
{
	if (!this_level.initialized)
		return;

	texture_close(background);
    audio_sound_unload(drop);
}

static bool level2_init()
{
	if (this_level.initialized)
		return true;

	background = texture_open("data/level2.png", 1, 0.0f);
    drop = audio_sound_load("data/drop.ogg");

	father_frame = 0;
	father_timer = 0.0f;

	repeat_count = 0;
	boredom_current = -1;
	text_timer = 0.0f;

	disable_timer = 2.0f;

    if (background == nullptr
     || drop == nullptr)
    {
        level2_finish();
        return false;
    }

    return true;
}

static void color_get(bool state)
{
	if (state)
		opengl_color(0.5f, 0.5f, 0.5f);
	else
		opengl_color(1.0f, 1.0f, 1.0f);
}

static void level2_render(font_t* font, opengl_state_t* state, game_t* game)
{
	opengl_texture(background, 0);

	opengl_move((int)this_level.father_x, (int)this_level.father_y);
	opengl_texture(game->father, father_frame);

	if (text_timer > 0.0f)
	{
		std::string text = boredom_text[boredom_current];
		int width = font_width(font, text);

		opengl_move(texture_frame_width(game->father) / 2 - width / 2, -10);

		// Make sure all the text stays on screen. This can only happen on the left in this case
		if (this_level.father_x + texture_frame_width(game->father) / 2 - width / 2 < 0)
		{
			opengl_move(width / 2 - (int)this_level.father_x - texture_frame_width(game->father) / 2 + 1, 0);
		}

		opengl_color(0.0f, 0.0f, 0.0f);
		font_render(font, text);
	}

	opengl_restore(state);
	opengl_move(0, REFERENCE_HEIGHT - 25);
	opengl_color(0.0f, 0.0f, 0.0f);
	opengl_rectangle(REFERENCE_WIDTH, 1);
	opengl_move(0, 1);
	opengl_color(1.0f, 1.0f, 1.0f);
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

static void move_horizontal(float delta, float old_father_x, float old_father_y)
{
	this_level.father_x += delta;

	// Don't allow to talk on water
	if (((this_level.father_x < 86 || this_level.father_x > 150) && old_father_y < 57)
		|| old_father_y < 24)
	{
		this_level.father_x = old_father_x;
	}
}

static void move_vertical(float delta, float old_father_x, float old_father_y, game_t* game)
{
	this_level.father_y += delta;

	// Don't allow to talk on water
	if (this_level.father_y < 57)
	{
		this_level.father_y = old_father_y;
	}

	if (this_level.father_y >= REFERENCE_HEIGHT - texture_frame_height(game->father) - 25)
	{
		this_level.father_y = REFERENCE_HEIGHT - texture_frame_height(game->father) - 25;
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

static bool level2_update(float dt, game_t* game)
{
	bool mouvement = false;
	int direction;

	if (text_timer <= 0.0f)
	{
		// Go slower each repetition
		int father_speed = FATHER_SPEED - repeat_count;

		if (boredom_current != BOREDOM_MAX || disable_timer <= 0.0f)
		{
			if (game->input.left || game->input.right || game->input.up || game->input.down)
			{
				float delta_horizontal = 0.0f;
				float delta_vertical = 0.0f;

				if (game->input.left)
					delta_horizontal -= father_speed * dt;

				if (!game->right_disabled)
				{
					if (game->input.right)
						delta_horizontal += father_speed * dt;
				}

				if (game->input.up)
					delta_vertical -= father_speed * dt;

				if (game->input.down)
					delta_vertical += father_speed * dt;

				float old_father_x = this_level.father_x;
				float old_father_y = this_level.father_y;

				move_horizontal(delta_horizontal, old_father_x, old_father_y);
				move_vertical(delta_vertical, old_father_x, old_father_y, game);

				mouvement = true;
			}
		}
		else
		{
			int father_height = texture_frame_height(game->father);

			if (!game->right_disabled && this_level.father_y < REFERENCE_HEIGHT - 25 - father_height)
			{
				this_level.father_y += father_speed * dt;
				mouvement = true;
			}

			if (this_level.father_y >= REFERENCE_HEIGHT - 25 - father_height)
			{
				this_level.father_y = REFERENCE_HEIGHT - 25 - father_height;

				if (disable_timer > 1.0f)
				{
					disable_timer -= dt;

					if (disable_timer <= 1.0f)
					{
						game->right_disabled = true;
						audio_sound_play(drop);
					}
				}
				else if(disable_timer > 0.0f)
				{
					disable_timer -= dt;
				}
			}
		}
	}
	else
	{
		text_timer -= dt;
		father_update(dt, game);
	}

	if (mouvement)
	{
		float duration = texture_frame_duration(game->father);
		father_timer += dt;

		// 0 -> down
		// 1 -> up
		// 2 -> right
		// 3 -> left
		if (game->input.left)
			direction = 3;
		else if (game->input.right && !game->right_disabled)
			direction = 2;
		else if (game->input.up)
			direction = 1;
		else
			direction = 0;

		if (father_timer >= duration)
		{
			father_timer -= duration;

			int frame = father_frame / 4;
			frame = (frame + 1) % 3;
			father_frame = direction + frame * 4;
		}
	}

    if (this_level.father_x < 0)
    {
        if (repeat_count > 0)
        {
            this_level.father_x = REFERENCE_WIDTH - texture_frame_width(game->father);
            repeat_count--;
        }
        else
        {
            this_level.father_x++;
            game->current_level--;
        }
    }

	if (boredom_current < BOREDOM_MAX)
	{
		if (repeat_count > 3 && boredom_current % 2
			&& this_level.father_x > REFERENCE_WIDTH / 2)
		{
			boredom_current++;
			audio_sound_play(game->talk);
			text_timer = 1.2f;
			father_frame = 0;
		}
	}

	if(this_level.father_x > REFERENCE_WIDTH - texture_frame_width(game->father))
	{
		repeat_count++;
		this_level.father_x = 0.0f;

		if (boredom_current < BOREDOM_MAX && repeat_count >= 2)
		{
			audio_sound_play(game->talk);
			text_timer = 1.2f;
			father_frame = 0;
			boredom_current++;
		}
	}

	return true;
}

static void level2_change(level_t* old_level)
{
	if (old_level->number == 0)
	{
		this_level.father_y = old_level->father_y;
        father_frame = 2;
	}
	else
	{
		std::cout << "Going to level " << this_level.number + 1 << " from unknown level: " << old_level->number + 1 << std::endl;
	}
}

level_t* level2_get()
{
	this_level.number = 1;
	this_level.father_x = 0.0f;
	this_level.father_y = 0.0f;
	this_level.initialized = false;

	this_level.init = level2_init;
	this_level.update = level2_update;
	this_level.render = level2_render;
	this_level.change = level2_change;
	this_level.finish = level2_finish;

	return &this_level;
}
