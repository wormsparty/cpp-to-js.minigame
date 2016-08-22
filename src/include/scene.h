#ifndef __SCENE_H__
#define __SCENE_H__

#include "window.h"

struct font_t;
struct opengl_state_t;

struct scene_t
{
	bool(*init)();
	bool(*update)(float dt, input_state_t input);
	void(*render)(font_t* font, opengl_state_t* state);
	void(*finish)();
};

extern void intro_scene_get(scene_t* scene);
extern void game_scene_get(scene_t* scene);

#endif