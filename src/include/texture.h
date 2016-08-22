/*
 * Copyright (C) 2012 Marc-Olivier Bloch <wormsparty [at] gmail [dot] com>
 *
 * This file is part of the 'Beautiful, absurd, subtle.' project.
 *
 * 'Beautiful, absurd, subtle.' is free software: you can redistribute it 
 * and/or modify it under the terms of the 'New BSD License'.
 *
 */

#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <string>
#include <SDL_opengl.h>

#include "opengl.h"

struct texture_t;

int texture_begin(void);
void texture_finish(void);

// From file
texture_t* texture_open(const std::string filename, int frame_count, float frame_duration);
texture_t* texture_from_bytes(unsigned char* bytes, int width, int height);

void texture_render(texture_t* texture, int frame_current, GLfloat* projection, opengl_state_t* state);

// Returns the frame width, ie. width / frame_count
int texture_frame_width(texture_t* texture);
int texture_frame_height(texture_t* texture);
int texture_frame_count(texture_t* texture);
float texture_frame_duration(texture_t* texture);

void texture_close(texture_t* texture);

#endif
