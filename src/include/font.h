/*
 * Copyright (C) 2012 Marc-Olivier Bloch <wormsparty [at] gmail [dot] com>
 *
 * This file is part of the 'Beautiful, absurd, subtle.' project.
 *
 * 'Beautiful, absurd, subtle.' is free software: you can redistribute it
 * and/or modify it under the terms of the 'New BSD License'.
 *
 */

#ifndef _FONT_H_
#define _FONT_H_

#include <string>
struct font_t;

// Render the given text.
font_t* font_open(const std::string filename);
void font_close(font_t* font);

void font_render(font_t* font, const std::string str);
void font_render(font_t* font, const std::string str, int charcount);

int font_width(font_t* font, const std::string str);
int font_height(font_t* font);

#endif