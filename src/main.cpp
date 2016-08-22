#include "include/opengl.h"
#include "include/window.h"
#include "include/texture.h"
#include "include/font.h"
#include "include/audio.h"
#include "include/scene.h"

#include <chrono>
#include <thread>

#include <SDL.h> // If SDLmain is needed
#include <iostream>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#define FPS 60

//static texture_t* cursor;
static font_t* font;

static scene_t current_scene;
static bool window_continue = true;

static void prepare_drawing(
	opengl_state_t* initial_state, opengl_state_t* zoomed_state, int& zoom, input_state_t& input_state)
{
    // We only draw a 256x240 screen scaled with an integer factor.
    // Therefore, we will most of the time have to draw a border around
    // the drawing area.
    int window_width = window_width_get();
    int window_height = window_height_get();

    int zoom_x = window_width / REFERENCE_WIDTH;
    int zoom_y = window_height / REFERENCE_HEIGHT;
	zoom = zoom_x;

	if (zoom_y < zoom)
	{
		zoom = zoom_y;
	}

	if (zoom < 1)
	{
		zoom = 1;
	}

    int borderx = (window_width - REFERENCE_WIDTH * zoom) / 2;
    int bordery = (window_height - REFERENCE_HEIGHT * zoom) / 2;
    int ajustementx = window_width - REFERENCE_WIDTH * zoom - borderx * 2;
    int ajustementy = window_height - REFERENCE_HEIGHT * zoom - bordery * 2;

	input_state = window_input_state_get();

	opengl_clear();
    opengl_scissor_disable(); // Make sure we can draw everywhere

    // Left and right border
    opengl_save(initial_state);
    opengl_color(0.05f, 0.05f, 0.1f);
    opengl_rectangle(borderx + ajustementx, window_height);
    opengl_move(window_width - borderx - ajustementx, 0);
    opengl_rectangle(borderx + ajustementx, window_height);
    opengl_restore(initial_state);

    // Top and bottom border
    opengl_save(initial_state);
    opengl_color(0.05f, 0.05f, 0.1f);
    opengl_move(borderx, 0);
    opengl_rectangle(REFERENCE_WIDTH * zoom, bordery + ajustementy);
    opengl_move(0, window_height - bordery - ajustementy);
    opengl_rectangle(REFERENCE_WIDTH * zoom, bordery + ajustementy);
    opengl_restore(initial_state);

    // Prepare for further drawing:
    // We place ourself on the top-left
    // corner of the drawing area, and scale.
    opengl_move(borderx, bordery);
    opengl_scale(zoom, zoom);
	opengl_save(zoomed_state);

    // Now we should only draw in the content of the window
    opengl_scissor_enable(borderx, bordery, REFERENCE_WIDTH * zoom, REFERENCE_HEIGHT * zoom);
}

static bool scene_step(scene_t* scene)
{
	int zoom;
	input_state_t input_state;
	opengl_state_t zoomed_state, initial_state;

	prepare_drawing(&initial_state, &zoomed_state, zoom, input_state);

	float dt = 1.0f / FPS;

	if (!scene->update(dt, input_state))
		return false;

	scene->render(font, &zoomed_state);

	return true;
}

static bool init()
{
    if (!window_begin("a.man"))
    {
        return false;
    }

	if (!opengl_begin())
    {
		window_finish();
        return false;
    }

	if (!texture_begin())
	{
		opengl_finish();
		window_finish();
		return false;
	}

	if (!audio_begin())
    {
		texture_finish();
		opengl_finish();
		window_finish();
        return false;
    }

	font = font_open("data/font.png");
	intro_scene_get(&current_scene);

	if (font == nullptr
		|| !current_scene.init())
	{
		font_close(font);

		audio_finish();
		texture_finish();
		opengl_finish();
		window_finish();

		return false;
	}

	return true;
}

static void finish()
{
	current_scene.finish();

	font_close(font);

    audio_finish();
	opengl_finish();
	texture_finish();
    window_finish();
}

static void step()
{
    if (!window_step())
    {
        finish();

#ifdef __EMSCRIPTEN__
        emscripten_cancel_main_loop();
#endif
        window_continue = false;
        return;
    }

	if (!scene_step(&current_scene))
	{
		current_scene.finish();
		//editor_scene_get(&current_scene);
		game_scene_get(&current_scene);
		current_scene.init();
	}
}

int main(int argc, char* argv[])
{
	if (!init())
	{
		return 1;
	}

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(step, FPS, 1);
#else
    typedef std::chrono::duration<int, std::ratio<1, FPS>> frame_duration;

    while(window_continue)
    {
        auto endtime = std::chrono::system_clock::now() + frame_duration(1);
        step();
        std::this_thread::sleep_until(endtime);
    }
#endif

    return 0;
}