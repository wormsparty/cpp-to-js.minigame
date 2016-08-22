#include <string>

#include "../include/opengl.h"
#include "../include/font.h"

struct dialog_t
{
	float start_timer;
	float position_timer;
	int position;
	std::string text;
	float r, g, b;
};

dialog_t* dialog_init(std::string text, float r, float g, float b, float start_time)
{
	auto dialog = new dialog_t;

	dialog->position = 0;
	dialog->position_timer = 0.0f;
	dialog->start_timer = start_time;
	dialog->text = text;
	dialog->r = r;
	dialog->g = g;
	dialog->b = b;

	return dialog;
}

void dialog_update(dialog_t* dialog, float dt)
{
#define DIALOG_CHARACTER_DURATION 0.125f
	if (dialog->start_timer > 0.0f)
	{
		dialog->start_timer -= dt;

		if (dialog->start_timer <= 0.0f)
		{
			dialog->position_timer = DIALOG_CHARACTER_DURATION;
		}
	}

	if (dialog->start_timer <= 0.0f)
	{
		dialog->position_timer -= dt;

		if (dialog->position_timer <= 0.0f)
		{
			if (dialog->position < dialog->text.length())
			{
				dialog->position++;
			}

			dialog->position_timer += DIALOG_CHARACTER_DURATION;
		}
	}
}

void dialog_render(dialog_t* dialog, font_t* font)
{
	opengl_color(dialog->r, dialog->g, dialog->b);
	font_render(font, dialog->text, dialog->position);
}

int dialog_position_get(dialog_t* dialog)
{
	return dialog->position;
}

int dialog_position_max(dialog_t* dialog)
{
    return (int)dialog->text.length();
}

void dialog_delete(dialog_t* dialog)
{
	delete dialog;
}