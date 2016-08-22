/*
 * Copyright (C) 2012 Marc-Olivier Bloch <wormsparty [at] gmail [dot] com>
 *
 * This file is part of the 'Beautiful, absurd, subtle.' project.
 *
 * 'Beautiful, absurd, subtle.' is free software: you can redistribute it
 * and/or modify it under the terms of the 'New BSD License'.
 *
 */

#include "../include/font.h"
#include "../include/opengl.h"
#include "../include/texture.h"

#define CHARACTER_COUNT 95 // The number of printable characters in ASCII

font_t* font_open(const std::string filename)
{
	return (font_t*)texture_open(filename, CHARACTER_COUNT, 0.0f);
}

void font_render(font_t* font, const std::string str)
{
	font_render(font, str, str.length());
}

void font_render(font_t* font, const std::string str, int charcount)
{
	auto texture = (texture_t*)font;
    opengl_state_t state;
	int height = 0;

    opengl_save(&state);

	for (int i = 0; i < charcount; i++)
    {
		if (str[i] == '\n')
		{
			opengl_restore(&state);
			height += texture_frame_height(texture) + 2;
			opengl_move(0, height);
		}

        int frame = str[i] - ' ';

        if (frame >= 0 && frame < CHARACTER_COUNT)
        {
            opengl_texture(texture, frame);
            opengl_move(texture_frame_width(texture), 0.0f);
        }
    }

    opengl_restore(&state);
}

void font_close(font_t* font)
{
	texture_close((texture_t*)font);
}

int font_height(font_t* font)
{
	auto texture = (texture_t*)font;
	return texture_frame_height(texture);
}

int font_width(font_t* font, const std::string str)
{
	auto texture = (texture_t*)font;
	return texture_frame_width(texture) * str.length();
}