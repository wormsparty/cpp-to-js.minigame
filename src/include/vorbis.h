/*
 * Copyright (C) 2012 Marc-Olivier Bloch <wormsparty [at] gmail [dot] com>
 *
 * This file is part of the 'Beautiful, absurd, subtle.' project.
 *
 * 'Beautiful, absurd, subtle.' is free software: you can redistribute it
 * and/or modify it under the terms of the 'New BSD License'.
 *
 */

#ifndef _VORBIS_H_
#define _VORBIS_H_

#include <stdlib.h> // size_t
#include <string>

struct vorbis_t;

extern vorbis_t* vorbis_open (const std::string, int*, long*);
extern size_t    vorbis_read(vorbis_t*, const char*, size_t);
extern void      vorbis_close(vorbis_t*);

#endif