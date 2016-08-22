
#include "../include/scene.h"
#include "../include/window.h"
#include "../include/level.h"
#include "../include/texture.h"
#include "../include/audio.h"
#include "../include/game.h"

#include <iostream>
#include <random>

static game_t game;

static float wave_timer;
static std::uniform_real_distribution<float> wave_dist(5.0f, 10.0f);
static std::random_device random_device;
static std::default_random_engine random_engine(random_device());

#define LEVEL_COUNT 4
static level_t* levels[LEVEL_COUNT];

extern level_t* level1_get();
extern level_t* level2_get();
extern level_t* level3_get();
extern level_t* level4_get();

static void game_finish()
{
	for (int i = 0; i < LEVEL_COUNT; i++)
	{
		levels[i]->finish();
	}

	audio_sound_unload(game.talk);
	audio_sound_unload(game.wave);

	texture_close(game.father);
	texture_close(game.menu);
}

static bool game_init()
{
	game.father = texture_open("data/father.png", 23, 0.2f);
	game.menu = texture_open("data/menu.png", 9, 0.0f);
	game.talk = audio_sound_load("data/talk.ogg");
	game.wave = audio_sound_load("data/wave.ogg");

	game.current_level = 0;
	game.right_disabled = false;

	if (game.father == nullptr
		|| game.menu == nullptr
		|| game.talk == nullptr
        || game.wave == nullptr)
	{
        audio_sound_unload(game.wave);
		audio_sound_unload(game.talk);
		texture_close(game.father);
		texture_close(game.menu);

		return false;
	}

	levels[0] = level1_get();
	levels[1] = level2_get();
	levels[2] = level3_get();
	levels[3] = level4_get();

	for (int i = 0; i < LEVEL_COUNT; i++)
	{
		if (!levels[i]->init())
		{
			game_finish();
			return false;
		}

		levels[i]->initialized = true;
	}

    wave_timer = wave_dist(random_engine);
	return true;
}

static bool game_update(float dt, input_state_t input_state)
{
	int old_level = game.current_level;
	level_t* level = levels[game.current_level];
	game.input = input_state;

	bool ret = level->update(dt, &game);

	// By definition the level change has to be done in the upate function!
	if (old_level != game.current_level)
	{
		level_t* new_level = levels[game.current_level];
		new_level->change(level);
	}

    wave_timer -= dt;

    if (wave_timer <= 0.0f)
    {
        audio_sound_play(game.wave);
        wave_timer = wave_dist(random_engine);
    }

	return ret;
}

static void game_render(font_t* font, opengl_state_t* state)
{
	level_t* level = levels[game.current_level];
	level->render(font, state, &game);
}

void game_scene_get(scene_t* scene)
{
	scene->init = game_init;
	scene->update = game_update;
	scene->render = game_render;
	scene->finish = game_finish;
}
