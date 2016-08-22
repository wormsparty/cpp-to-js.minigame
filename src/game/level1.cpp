#include "../include/scene.h"
#include "../include/texture.h"
#include "../include/font.h"
#include "../include/window.h"
#include "../include/audio.h"
#include "../include/level.h"
#include "../include/game.h"

#include <cmath>
#include <iostream>

static level_t this_level;
static texture_t* background;

static float monologue_timer;
static int monologue_played;

static float father_timer;
static int father_frame;

static bool dont_move_until_release;

static std::string complaints_texts[] = {
	"I still think we should go right.",
	"Not left, right.",
	"Let's stay on track.",
	"I said no.",
	"Come on, let it go.",
	"Give me a break.",
	"Are you serious?",
	"Left it is, then.",
};

static int complaint_current;
static float text_timer;

static void level1_finish()
{
	if (!this_level.initialized)
		return;

	texture_close(background);
}

static bool level1_init()
{
	if (this_level.initialized)
		return true;
	
	background = texture_open("data/level1.png", 1, 0.0f);

    monologue_timer = 5.0f;
    father_timer = 0.0f;
    father_frame = 0;
    monologue_played = 0;

	dont_move_until_release = false;

	complaint_current = -1;
	text_timer = 0.0f;

    if (background == nullptr)
    {
        level1_finish();
        return false;
    }

    return true;
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
    if (((old_father_x < 86 || old_father_x > 150) && this_level.father_y < 57)
        || this_level.father_y < 24)
    {
        this_level.father_y = old_father_y;
    }

    if (this_level.father_y >= REFERENCE_HEIGHT - texture_frame_height(game->father) - 25)
    {
        this_level.father_y = REFERENCE_HEIGHT - texture_frame_height(game->father) - 25;
    }
}

static bool level1_update(float dt, game_t* game)
{
	float duration = texture_frame_duration(game->father);

	if (monologue_timer > 0.0f)
    {
        monologue_timer -= dt;

		if ((monologue_timer <= 4.2f && monologue_timer >= 3.0f) || monologue_timer <= 2.0f)
        {
            // To play the "talk" sound in the first two monologues
            if ((monologue_timer <= 4.2f && monologue_played == 0)
                || (monologue_timer <= 2.0f && monologue_played == 1))
            {
                audio_sound_play(game->talk);
                monologue_played++;
            }

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

		if (monologue_timer <= 0.0f)
		{
			father_timer = 0.0f;
		}
    }

	if (text_timer > 0.0f)
	{
		text_timer -= dt;
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

		if (text_timer <= 0.0f)
		{
			father_timer = 0.0f;
			dont_move_until_release = true;
		}
	}

	// Don't allow moving until the any text is over
	if (monologue_timer <= 0.0f)
	{
		bool mouvement = false;
		int direction = 0;

		if (text_timer <= 0.0f)
		{
			father_timer += dt;
		}

		if (game->input.left || game->input.right || game->input.up || game->input.down)
		{
			if (!dont_move_until_release && text_timer <= 0.0f)
			{
                float delta_horizontal = 0.0f;
                float delta_vertical = 0.0f;

				if (game->input.left)
					delta_horizontal -= FATHER_SPEED * dt;

                if (!game->right_disabled)
                {
                    if (game->input.right)
                        delta_horizontal += FATHER_SPEED * dt;
                }

				if (game->input.up)
                    delta_vertical -= FATHER_SPEED * dt;

				if (game->input.down)
                    delta_vertical += FATHER_SPEED * dt;

                float old_father_x = this_level.father_x;
                float old_father_y = this_level.father_y;

                move_horizontal(delta_horizontal, old_father_x, old_father_y);
                move_vertical(delta_vertical, old_father_x, old_father_y, game);

				mouvement = true;
			}

			// 0 -> down
			// 1 -> up
			// 2 -> right
			// 3 -> left
			if (game->input.left)
				direction = 3;
			else if (game->input.right)
				direction = 2;
			else if (game->input.up)
				direction = 1;
			else
				direction = 0;
		}
		
		if (mouvement)
		{
			if (father_timer >= duration)
			{
				father_timer -= duration;

				int frame = father_frame / 4;
				frame = (frame + 1) % 3;
				father_frame = direction + frame * 4;
			}
		}
		else
		{
			// Reset to the original position of the direction
			if (father_timer >= duration)
			{
				father_timer -= duration;
				father_frame = father_frame % 4;
			}
		}

		if (this_level.father_x <= 45)
		{
			if (complaint_current < 7)
			{
				complaint_current++;

                // Speed things up if we already visited the far left.
                if (game->right_disabled)
                {
                    complaint_current = 7;
                }

				audio_sound_play(game->talk);

				this_level.father_x = 46;

				text_timer = 1.2f;
				father_frame = 0;
			}
		}

		// Test if all keys are released.
		if (!game->input.left && !game->input.right 
			&& !game->input.up && !game->input.down 
			&& dont_move_until_release)
		{
			dont_move_until_release = false;
		}
	}

	if (this_level.father_x > REFERENCE_WIDTH - texture_frame_width(game->father))
	{
		// Make sure that if we come back to this screen we won't instantly 
		// go back to the other scene
		this_level.father_x--;
		game->current_level = 1;
	}
	else if (this_level.father_x < 0)
	{
		this_level.father_x++;
		game->current_level = 2;
        dont_move_until_release = true;
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

static void level1_render(font_t* font, opengl_state_t* state, game_t* game)
{	
	int father_x = (int)this_level.father_x;
	int father_y = (int)this_level.father_y;

    opengl_texture(background, 0);
    opengl_move(father_x + texture_frame_width(game->father) / 2, father_y - 10);
	opengl_color(0.0f, 0.0f, 0.0f);

    if (monologue_timer <= 4.0f && monologue_timer >= 3.0f)
    {
		std::string text = "He's gone.";

		opengl_move(-font_width(font, text) / 2, 0);
		font_render(font, text);
	}
	else if (monologue_timer <= 2.0f && monologue_timer >= 0.0f)
	{
		std::string text = "I guess I should go right.";

		opengl_move(-font_width(font, text) / 2, 0);
		font_render(font, text);
	}
	else if(text_timer > 0.0f)
	{
		std::string text = complaints_texts[complaint_current];
		int width = font_width(font, text);

		opengl_move(-width / 2, 0);

		// Make sure all the text stays on screen, left and right.
		if (father_x + texture_frame_width(game->father) / 2 + width / 2 > REFERENCE_WIDTH)
		{
			opengl_move(REFERENCE_WIDTH - father_x - texture_frame_width(game->father) / 2 - width / 2 - 1, 0);
		}
		else if (father_x + texture_frame_width(game->father) / 2 - width / 2 < 0)
		{
			opengl_move(1 - father_x - texture_frame_width(game->father) / 2 + width / 2, 0);
		}

		font_render(font, text);
	}

	opengl_restore(state);
	opengl_move(father_x, father_y);
	opengl_texture(game->father, father_frame);

	opengl_restore(state);
	opengl_move(0, REFERENCE_HEIGHT - 25);
	opengl_color(0.0f, 0.0f, 0.0f);
	opengl_rectangle(REFERENCE_WIDTH, 1);
	opengl_move(0, 1);
	opengl_color(1.0f, 1.0f, 1.0f);
	opengl_rectangle(REFERENCE_WIDTH, 24);

	if (monologue_timer <= 0.0f)
	{
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
}

static void level1_change(level_t* old_level)
{
	if (old_level->number == 1 || old_level->number == 2)
	{
		this_level.father_y = old_level->father_y;
	}
	else
	{
		std::cout << "Going to level " << this_level.number + 1 << " from unknown level: " << old_level->number + 1 << std::endl;
	}
}

level_t* level1_get()
{
	this_level.number = 0;
	this_level.father_x = 130.0f;
	this_level.father_y = 30.0f;
	this_level.initialized = false;

	this_level.init = level1_init;
	this_level.update = level1_update;
	this_level.render = level1_render;
	this_level.change = level1_change;
	this_level.finish = level1_finish;

	return &this_level;
}
