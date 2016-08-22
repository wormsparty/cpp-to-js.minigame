#ifndef __DIALOG_H__
#define __DIALOG_H__

#include <string>

struct dialog_t;
struct audio_t;
struct font_t;

dialog_t* dialog_init(std::string text, float r, float g, float b, float start_time);
void dialog_delete(dialog_t* dialog);

void dialog_update(dialog_t* dialog, float dt);
void dialog_render(dialog_t* dialog, font_t* font);
int dialog_position_get(dialog_t* dialog);
int dialog_position_max(dialog_t* dialog);

#endif
