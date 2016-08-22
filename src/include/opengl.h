/*
 * Copyright (C) 2012 Marc-Olivier Bloch <wormsparty [at] gmail [dot] com>
 *
 * This file is part of the 'Beautiful, absurd, subtle.' project.
 *
 * 'Beautiful, absurd, subtle.' is free software: you can redistribute it
 * and/or modify it under the terms of the 'New BSD License'.
 *
 */

#ifndef _OPENGL_H_
#define _OPENGL_H_

struct texture_t;

struct opengl_state_t
{
    int px, py;
    float r, g, b, a;
    int scalex, scaley;
};

extern int opengl_begin(void);
extern void opengl_finish(void);

extern void opengl_save(opengl_state_t* state);
extern void opengl_restore(opengl_state_t* state);

extern void opengl_color(float r, float g, float b);
extern void opengl_color(float r, float g, float b, float a);
extern void opengl_move(int dx, int dy);
extern void opengl_scale(int sx, int sy);

extern void opengl_scissor_enable(int x, int y, int width, int height);
extern void opengl_scissor_disable(void);

extern void opengl_clear(void);

extern void opengl_rectangle(int width, int height);
extern void opengl_texture(texture_t* texture, int frame_current);

#endif