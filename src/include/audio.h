/*
* Copyright (C) 2012 Marc-Olivier Bloch <wormsparty [at] gmail [dot] com>
*
* This file is part of the 'Beautiful, absurd, subtle.' project.
*
* 'Beautiful, absurd, subtle.' is free software: you can redistribute it
* and/or modify it under the terms of the 'New BSD License'.
*
*/

#ifndef _AUDIO_H
#define _AUDIO_H

#include <string>

struct audio_t;

// First time init and uninit after application close
extern int  audio_begin(void);
extern void audio_finish(void);

extern audio_t* audio_sound_load(const std::string filename);
extern void audio_sound_unload(audio_t*);

extern void audio_sound_play(audio_t*);
extern void audio_sound_loop(audio_t*);
extern void audio_sound_stop(audio_t*);

#endif