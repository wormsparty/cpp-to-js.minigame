#include "../include/dialog.h"
#include "../include/audio.h"
#include "../include/texture.h"
#include "../include/window.h"
#include "../include/scene.h"

#include <list>
#include <random>
#include <iostream>

static texture_t* water, *cloud, *seagull, *silhouette, *transition;
static audio_t* seagull_sound, *wave_sound;

struct pos_t
{
	float posx, posy;
	int velocity;
	int frame_current;
	float frame_timer;
};

static std::list<pos_t*> seagulls;
static float seagull_timer, seagull_threshold;
static std::uniform_real_distribution<float> seagull_time_dist(5.0f, 15.0f);
static std::uniform_int_distribution<int> seagull_speed_dist(-2, 3);
static std::uniform_int_distribution<int> seagull_height_dist(0, 9);

static float wave_timer;
static std::uniform_real_distribution<float> wave_dist(5.0f, 10.0f);

#define SKY_DELTA_MAX 60
static float sky_timer, transition_timer;
static float sky_delta, water_delta, cloud_delta;

static dialog_t* dialog_father;
static dialog_t* dialog_son;

static std::random_device random_device;
static std::default_random_engine random_engine(random_device());

static void intro_finish()
{
	dialog_delete(dialog_father);
	dialog_delete(dialog_son);

	audio_sound_unload(seagull_sound);
	audio_sound_unload(wave_sound);

	texture_close(transition);
	texture_close(silhouette);
	texture_close(seagull);
	texture_close(water);
	texture_close(cloud);
}

static bool intro_init()
{
	seagulls.clear();

	seagull_timer = 0.0f;
	seagull_threshold = 5.0f;
	
	wave_timer = 2.0f;

	sky_delta = 0.0f;
	water_delta = 0.0f;
	cloud_delta = 0.0f;
	
	sky_timer = 5.0f;
	transition_timer = -25.0f;

	water = texture_open("data/water.png", 1, 0.0f);
	cloud = texture_open("data/cloud.png", 1, 0.0f);
	seagull = texture_open("data/seagull.png", 10, 1 / 5.0f);
	silhouette = texture_open("data/silhouette.png", 3, 0.0f);
	transition = texture_open("data/transition.png", 1, 0.0f);

	seagull_sound = audio_sound_load("data/seagull.ogg");
	wave_sound = audio_sound_load("data/wave.ogg");

	// Orange -> creativity/inspiration
	dialog_father = dialog_init("I wish this moment would never end.", 0.9f, 0.45f, 0.0f, 5.0f);
	// Green -> love/compassion
	dialog_son = dialog_init("Don't worry. You are about to wake up.", 0.0f, 0.5f, 0.0f, 15.0f);

	if (water == nullptr
		|| cloud == nullptr
		|| seagull == nullptr
		|| silhouette == nullptr
		|| transition == nullptr
		|| seagull_sound == nullptr
		|| wave_sound == nullptr)
	{
		intro_finish();
		return false;
	}

	return true;
}

static void update_object(texture_t* texture, int* frame_current, float* timer, float dt)
{
	int frame_count = texture_frame_count(texture);
	float threshold = texture_frame_duration(texture);

	if (threshold > 0.0f)
	{
		*timer += dt;

		if (*timer >= threshold)
		{
			*frame_current = (*frame_current + 1) % frame_count;
			*timer -= threshold;
		}
	}
}

static bool intro_update(float dt, input_state_t input_state)
{
	// Not used
	(void)input_state;

	if (sky_timer > 0.0f)
	{
		sky_timer -= dt;
	}

	if (sky_timer <= 0.0f && sky_delta < SKY_DELTA_MAX)
	{
		sky_delta += dt * 5.0f;

		if (sky_delta > SKY_DELTA_MAX)
		{
			sky_delta = SKY_DELTA_MAX;
		}
	}

	if (sky_delta >= SKY_DELTA_MAX)
	{
		dialog_update(dialog_father, dt);
		dialog_update(dialog_son, dt);

		transition_timer += dt;
	}

	// Move the water and cloud left and right.
	water_delta -= dt * 2;
	cloud_delta += dt * 2;

	if (water_delta < 0)
	{
		water_delta += REFERENCE_WIDTH;
	}

	if (cloud_delta >= REFERENCE_WIDTH)
	{
		cloud_delta -= REFERENCE_WIDTH;
	}

	wave_timer -= dt;

	if (wave_timer <= 0.0f)
	{
		audio_sound_play(wave_sound);
		wave_timer = wave_dist(random_engine);
	}

	for (auto it = seagulls.begin(); it != seagulls.end();)
	{
		pos_t* pos = *it;
		update_object(seagull, &pos->frame_current, &pos->frame_timer, dt);
		pos->posx += pos->velocity * dt;

		if ((pos->velocity > 0.0f && pos->posx >= REFERENCE_WIDTH)
			|| (pos->velocity < 0.0f && pos->posx <= -texture_frame_width(seagull)))
		{
			it = seagulls.erase(it);
		}
		else
		{
			++it;
		}
	}

	seagull_timer += dt;

	if (seagull_timer >= seagull_threshold)
	{
		auto pos = new pos_t;

		pos->frame_current = 0;
		pos->velocity = seagull_speed_dist(random_engine);
		pos->frame_timer = 0.0f;

		// We don't want speed 0
		if (pos->velocity <= 0)
		{
			pos->velocity--;
			pos->posx = REFERENCE_WIDTH;
		}
		else
		{
			pos->posx = -texture_frame_width(seagull);
		}

		// 10 possible heights
		pos->posy = seagull_height_dist(random_engine) * 200 / 10;
		pos->velocity *= 5;

		seagulls.push_back(pos);

		audio_sound_play(seagull_sound);

		seagull_timer -= seagull_threshold;
		seagull_threshold = seagull_time_dist(random_engine);
	}

	// After the transition, switch to the next scene.
	return transition_timer < 5.0f;
}

static void intro_render(font_t* font, opengl_state_t* state)
{
	//
	// Draw the background
	//

	// Blue sky
	opengl_color(146.0f / 256.0f, 211.0f / 256.0f, 1.0f);
	opengl_rectangle(REFERENCE_WIDTH, 200 - (int)sky_delta);
	opengl_color(1.0f, 1.0f, 1.0f);

    // Cloud - we draw two of them to make a 'scrolling' effect
    opengl_move(-(int)cloud_delta, 100 - (int)sky_delta);
    opengl_texture(cloud, 0);
    opengl_move(REFERENCE_WIDTH, 0);
    opengl_texture(cloud, 0);
    opengl_move((int)cloud_delta - REFERENCE_WIDTH, 0);

    // Water - we draw two of them to make a 'scrolling' effect
    opengl_move(-(int)water_delta, 100);
    opengl_texture(water, 0);
    opengl_move(REFERENCE_WIDTH, 0);
    opengl_texture(water, 0);
    opengl_move((int)water_delta - REFERENCE_WIDTH, texture_frame_height(water));

	// Silhouette
    int silhouette_frame;
    int position_father = dialog_position_get(dialog_father);

    if (position_father != 0 && position_father != dialog_position_max(dialog_father))
    {
        silhouette_frame = (position_father % 2) * 2; // 0 or 2
    }
    else
    {
        int position_son = dialog_position_get(dialog_son);

        if (position_son != 0 && position_son != dialog_position_max(dialog_son))
        {
            silhouette_frame = position_son % 2; // 0 or 1
        }
        else
        {
            silhouette_frame = 0;
        }
    }

	opengl_move(0, -2 * (int)sky_delta);
	opengl_texture(silhouette, silhouette_frame);

	opengl_move(20, -40);
	dialog_render(dialog_father, font);
	opengl_move(0, 20);
	dialog_render(dialog_son, font);

	if (transition_timer >= 0.0f)
	{
		int delta = (int)(transition_timer * (REFERENCE_WIDTH + texture_frame_width(transition)) / 3.0f) / 10;

		opengl_restore(state);
		opengl_move(REFERENCE_WIDTH - delta * 10, 0);
		opengl_texture(transition, 0);

		if (delta * 10 - texture_frame_width(transition) > 0)
		{
			opengl_color(0.0f, 0.0f, 0.0f);
			opengl_move(texture_frame_width(transition), 0);
			opengl_rectangle(delta * 10 - texture_frame_width(transition), REFERENCE_HEIGHT);
		}
	}

	for (auto it = seagulls.begin(); it != seagulls.end(); ++it)
	{
		pos_t* pos = *it;

		opengl_restore(state);
		opengl_move((int)pos->posx, (int)pos->posy + SKY_DELTA_MAX - (int)sky_delta);
		opengl_texture(seagull, pos->frame_current);
	}
}

void intro_scene_get(scene_t* scene)
{
	scene->init = intro_init;
	scene->update = intro_update;
	scene->render = intro_render;
	scene->finish = intro_finish;
}
