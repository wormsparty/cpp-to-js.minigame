/*
 * Copyright (C) 2012 Marc-Olivier Bloch <wormsparty [at] gmail [dot] com>
 *
 * This file is part of the 'Beautiful, absurd, subtle.' project.
 *
 * 'Beautiful, absurd, subtle.' is free software: you can redistribute it
 * and/or modify it under the terms of the 'New BSD License'.
 *
 */

#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#include <stdlib.h>
#include <iostream>

#include "../include/vorbis.h"

struct vorbis_t
{
    OggVorbis_File vf;
    int current_section;
};

vorbis_t* vorbis_open(
        const std::string filename,
        int* channels,
        long* samplespersecond)
{
    // Vorbis output is always 16-bits.
    vorbis_t* data;
    FILE* file;

    // Just to remove the header warning.
    (void)OV_CALLBACKS_NOCLOSE;
    (void)OV_CALLBACKS_STREAMONLY;
    (void)OV_CALLBACKS_STREAMONLY_NOCLOSE;

    if ((data = (vorbis_t*)malloc(sizeof(vorbis_t))) == NULL)
    {
        return NULL;
    }

    if((file = fopen(filename.c_str(), "rb")) == NULL)
    {
        free(data);
        return NULL;
    }

    if(ov_open_callbacks(file, &data->vf, NULL, 0, OV_CALLBACKS_DEFAULT) < 0)
    {
        free(data);
        fclose(file);
        return NULL;
    }

    vorbis_info* vi = ov_info(&data->vf, -1);

    if (vi->channels > 2)
    {
        std::cout << "Only Mono and Stereo sound files are supported!" << std::endl;

        ov_clear(&data->vf);
        free(data);
        return NULL;
    }

    *channels         = vi->channels;
    *samplespersecond = vi->rate;

    data->current_section = 0;

    return data;
}

size_t vorbis_read(vorbis_t* data, const char* buffer, size_t buffersize)
{
    size_t totread = 0;

    while(totread != buffersize)
    {
        long ret = ov_read(&data->vf, (char*)(buffer + totread), (int)(buffersize - totread), 0, 2, 1, &data->current_section);

        if (ret == 0) // End of file.
        {
            return totread;
        }

        if (ret > 0)
        {
            totread += ret;
        }
        else
        {
            break;
        }
    }

    return totread;
}

void vorbis_close(vorbis_t* data)
{
    ov_clear(&data->vf);
    free(data);
}