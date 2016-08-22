/*
 * Copyright (C) 2012 Marc-Olivier Bloch <wormsparty [at] gmail [dot] com>
 *
 * This file is part of the 'Beautiful, absurd, subtle.' project.
 *
 * 'Beautiful, absurd, subtle.' is free software: you can redistribute it
 * and/or modify it under the terms of the 'New BSD License'.
 *
 */

#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <string>

#define REFERENCE_WIDTH 256
#define REFERENCE_HEIGHT 240

struct input_state_t
{
	bool up, down, left, right;
};

extern int window_was_reset(void);

extern int  window_begin(const std::string program_name);
extern void window_finish(void);

extern bool window_step(void);
extern int window_toggle_fullscreen(void);

extern int window_width_get(void);
extern int window_height_get(void);

input_state_t window_input_state_get();

#endif
