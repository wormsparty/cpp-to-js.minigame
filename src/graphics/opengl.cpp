/*
 * Copyright (C) 2012 Marc-Olivier Bloch <wormsparty [at] gmail [dot] com>
 *
 * This file is part of the 'Beautiful, absurd, subtle.' project.
 *
 * 'Beautiful, absurd, subtle.' is free software: you can redistribute it
 * and/or modify it under the terms of the 'New BSD License'.
 *
 */

#include <GL/glew.h>
#include <SDL_opengl.h>

#include "../include/opengl.h"
#include "../include/window.h"
#include "../include/texture.h"

#include <iostream>
#include <cstring>

static GLfloat projection[16];
static opengl_state_t current_state;
static texture_t* rectangle_texture;

#ifdef DEBUG
#define check_gl_error() logOpenGLError(__FILE__,__LINE__)

static void logOpenGLError(const char *file, int line)
{
    GLenum err;

    while((err = glGetError()) != GL_NO_ERROR)
    {
        std::string errorStr;

        switch(err) {
            case GL_INVALID_OPERATION:
                errorStr = "GL_INVALID_OPERATION";
                break;
            case GL_INVALID_ENUM:
                errorStr = "GL_INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                errorStr = "GL_INVALID_VALUE";
                break;
            case GL_OUT_OF_MEMORY:
                errorStr = "GL_OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                errorStr = "GL_INVALID_FRAMEBUFFER_OPERATION";
                break;
            default:
                errorStr = "UNKNOWN ERROR";
                break;
        }

        std::cout << errorStr << " at " << file << ":" << line << std::endl;
    }
}
#endif

void opengl_projection(
        GLfloat* m, // 16 floats
        GLfloat width,
        GLfloat height)
{
    m[0]  = 2.0f / width;
    m[4]  = 0.0f;
    m[8]  = 0.0f;
    m[12] = -1.0f;
    m[1]  = 0.0f;
    m[5]  = -2.0f / height;
    m[9]  = 0.0f;
    m[13] = 1.0f;
    m[2]  = 0.0f;
    m[6]  = 0.0f;
    m[10] = -1.0f;
    m[14] = 0.0f;
    m[3]  = 0.0f;
    m[7]  = 0.0f;
    m[11] = 0.0f;
    m[15] = 1.0f;
}

void opengl_save(opengl_state_t* state)
{
    memcpy(state, &current_state, sizeof(opengl_state_t));
}

void opengl_restore(opengl_state_t* state)
{
    memcpy(&current_state, state, sizeof(opengl_state_t));
}

void opengl_move(int dx, int dy)
{
    current_state.px += dx * current_state.scalex;
    current_state.py += dy * current_state.scaley;
}

void opengl_color(float r, float g, float b)
{
    current_state.r = r;
    current_state.g = g;
    current_state.b = b;
	current_state.a = 1.0f;
}

void opengl_color(float r, float g, float b, float a)
{
	current_state.r = r;
	current_state.g = g;
	current_state.b = b;
	current_state.a = a;
}

void opengl_scale(int sx, int sy)
{
    current_state.scalex *= sx;
    current_state.scaley *= sy;
}

int opengl_begin()
{
	GLenum err = glewInit();

	if (err != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW: " << glewGetErrorString(err) << std::endl;
		return 0;
	}

    // A colored rectangle is equivalent to a scaled colored one-pixel texture.
    unsigned char white_texture[] = { 255, 255, 255, 255 };
    rectangle_texture = texture_from_bytes(white_texture, 1, 1);

	return 1;
}

void opengl_finish()
{
    texture_close(rectangle_texture);
}

void opengl_scissor_enable(int x, int y, int width, int height)
{
    glEnable(GL_SCISSOR_TEST);
    glScissor(x, y, (GLsizei) width, (GLsizei) height);
}

void opengl_scissor_disable()
{
    glDisable(GL_SCISSOR_TEST);
}

static void opengl_reset(
        unsigned int wx,
        unsigned int wy)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    glViewport(0, 0, (GLsizei) wx, (GLsizei) wy);

    opengl_projection(projection, window_width_get(), window_height_get());

    //check_gl_error();
}

void opengl_clear(void)
{
    current_state.px = 0.0f;
    current_state.py = 0.0f;
    current_state.r = 1.0f;
    current_state.g = 1.0f;
    current_state.b = 1.0f;
	current_state.a = 1.0f;
    current_state.scalex = 1.0f;
    current_state.scaley = 1.0f;

    if(window_was_reset())
    {
        opengl_reset(window_width_get(), window_height_get());
    }
}

void opengl_rectangle(int width, int height)
{
    opengl_state_t state;

    opengl_save(&state);
    opengl_scale(width, height);

    texture_render(rectangle_texture, 0, projection, &current_state);

    opengl_restore(&state);
}

void opengl_texture(texture_t* texture, int frame_current)
{
    texture_render(texture, frame_current, projection, &current_state);
}
