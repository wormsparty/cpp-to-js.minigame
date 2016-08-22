#ifndef __GAME_H__
#define __GAME_H__

#include "window.h"

#define FATHER_SPEED 50

struct texture_t;
struct audio_t;

// Objects common across all levels
struct game_t
{
	texture_t *father, *menu;
	audio_t* talk, *wave;
	input_state_t input;
	int current_level;
	bool right_disabled;
};

#endif