/*
 * Copyright (C) 2012 Marc-Olivier Bloch <wormsparty [at] gmail [dot] com>
 *
 * This file is part of the 'Beautiful, absurd, subtle.' project.
 *
 * 'Beautiful, absurd, subtle.' is free software: you can redistribute it
 * and/or modify it under the terms of the 'New BSD License'.
 *
 */

#ifndef __OPENAL_H__
#define __OPENAL_H__

struct openal_t;

extern int  openal_begin(void);
extern void openal_finish(void);

extern openal_t* openal_static_create(int channels, long samplerate, char* buffer, int buffersize);

extern int openal_static_play(openal_t* source);
extern void openal_static_loop(openal_t* source);
extern void openal_static_stop(openal_t* source);

extern void openal_source_close(openal_t* source);

#endif