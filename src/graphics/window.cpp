/*
 * Copyright (C) 2012 Marc-Olivier Bloch <wormsparty [at] gmail [dot] com>
 *
 * This file is part of the 'Beautiful, absurd, subtle.' project.
 *
 * 'Beautiful, absurd, subtle.' is free software: you can redistribute it
 * and/or modify it under the terms of the 'New BSD License'.
 *
 */

#include <SDL.h>
#include <iostream>

#include "../include/window.h"

static SDL_Window*internal_window;
static int internal_has_reset;
static int internal_window_width, internal_window_height;
static bool internal_is_fullscreen;
static input_state_t input_state;

int window_begin(const std::string program_name)
{
    unsigned int flags;
    SDL_Renderer* renderer;

    internal_has_reset = 1;
	memset(&input_state, 0, sizeof(input_state_t));

    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "Could not initialize SDL: " << SDL_GetError();
        return 0;
    }

    flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	// Default NES screen size
    internal_window_width = REFERENCE_WIDTH * 3;
    internal_window_height = REFERENCE_HEIGHT * 3;
    internal_is_fullscreen = false;

    internal_window = SDL_CreateWindow(
            program_name.c_str(),
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            internal_window_width,
            internal_window_height,
            flags);

    if(internal_window == nullptr)
    {
        return 0;
    }

	SDL_GLContext glContext = SDL_GL_CreateContext(internal_window);
	SDL_GL_MakeCurrent(internal_window, glContext);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 0);

    SDL_SetWindowMinimumSize(internal_window, REFERENCE_WIDTH, REFERENCE_HEIGHT);
    SDL_GL_SetSwapInterval(1);

    renderer = SDL_CreateRenderer(
            internal_window,
            -1,
            SDL_RENDERER_ACCELERATED);

    if(renderer == nullptr)
    {
        SDL_DestroyWindow(internal_window);
        return 0;
    }

    return 1;
}

int window_was_reset(void)
{
    int ret = internal_has_reset;
    internal_has_reset = 0;
    return ret;
}

void window_finish()
{
    SDL_Quit();
}

int window_toggle_fullscreen()
{
    // This will trigger a SDL_WINDOWEVENT_SIZE_CHANGED in any case.
    if(SDL_SetWindowFullscreen(internal_window, SDL_WINDOW_FULLSCREEN_DESKTOP * !internal_is_fullscreen) == 0)
    {
        internal_is_fullscreen = !internal_is_fullscreen;
        return 1;
    }

    return 0;
}

// Returns true if no 'close' events were triggered.
static bool internal_window_events(void)
{
    bool do_continue = true;
    int t;
    SDL_Event event;

    while(true)
    {
        if(!SDL_PollEvent(&event))
        {
            break;
        }

        t = event.type;

        if(t == SDL_KEYDOWN && event.key.repeat == 0)
        {
            int key = event.key.keysym.scancode;

            if(key == SDL_SCANCODE_F4)
            {
                const Uint8* keys = SDL_GetKeyboardState(nullptr);

                if(keys[SDL_SCANCODE_RALT]
                   || keys[SDL_SCANCODE_LALT])
                {
                    do_continue = false;
                }
            }
            else if (key == SDL_SCANCODE_Q)
            {
                const Uint8* keys = SDL_GetKeyboardState(nullptr);

                if(keys[SDL_SCANCODE_MODE]
                   || keys[SDL_SCANCODE_LCTRL]
                   || keys[SDL_SCANCODE_RCTRL])
                {
                    do_continue = false;
                }
            }
            else if (key == SDL_SCANCODE_F)
            {
                const Uint8* keys = SDL_GetKeyboardState(nullptr);

                if(keys[SDL_SCANCODE_MODE]
                   || keys[SDL_SCANCODE_LCTRL]
                   || keys[SDL_SCANCODE_RCTRL])
                {
                    window_toggle_fullscreen();
                }
            }
            else if (key == SDL_SCANCODE_RETURN)
            {
                const Uint8* keys = SDL_GetKeyboardState(nullptr);

                if(keys[SDL_SCANCODE_RALT]
                   || keys[SDL_SCANCODE_LALT])
                {
                    window_toggle_fullscreen();
                }
            }
            else if (key == SDL_SCANCODE_F11)
            {
                window_toggle_fullscreen();
            }
			else if (key == SDL_SCANCODE_LEFT)
			{
				input_state.left = true;
			}
			else if (key == SDL_SCANCODE_RIGHT)
			{
				input_state.right = true;
			}
			else if (key == SDL_SCANCODE_UP)
			{
				input_state.up = true;
			}
			else if (key == SDL_SCANCODE_DOWN)
			{
				input_state.down = true;
			}
		}
        else if (t == SDL_KEYUP)
        {
            int key = event.key.keysym.scancode;

			if (key == SDL_SCANCODE_LEFT)
				input_state.left = false;
			else if (key == SDL_SCANCODE_RIGHT)
				input_state.right = false;
			else if (key == SDL_SCANCODE_UP)
				input_state.up = false;
			else if (key == SDL_SCANCODE_DOWN)
				input_state.down = false;
		}
        else if(t == SDL_WINDOWEVENT)
        {
            if(event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
            {
                internal_has_reset = 1;
                internal_window_width = event.window.data1;
                internal_window_height = event.window.data2;
            }
        }
        else if(t == SDL_QUIT)
        {
            do_continue = false;
        }
    }

    return do_continue;
}

bool window_step()
{
    SDL_GL_SwapWindow(internal_window);
    return internal_window_events();
}

int window_width_get()
{
    return internal_window_width;
}

int window_height_get()
{
    return internal_window_height;
}

input_state_t window_input_state_get()
{
	return input_state;
}
