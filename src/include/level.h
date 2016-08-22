#ifndef __LEVEL_H__
#define __LEVEL_H__

struct opengl_state_t;
struct font_t;
struct game_t;

struct level_t
{
	int number;
	float father_x, father_y;
	bool initialized;

	bool(*init)();
	bool(*update)(float dt, game_t* game);
	void(*render)(font_t* font, opengl_state_t* state, game_t* game);
	void(*change)(level_t* old_level);
	void(*finish)();
}; 

#endif