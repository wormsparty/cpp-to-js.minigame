/*
 * Copyright (C) 2012 Marc-Olivier Bloch <wormsparty [at] gmail [dot] com>
 *
 * This file is part of the 'Beautiful, absurd, subtle.' project.
 *
 * 'Beautiful, absurd, subtle.' is free software: you can redistribute it
 * and/or modify it under the terms of the 'New BSD License'.
 *
 */

#include "../include/openal.h"
#include "../include/vorbis.h"

#ifdef __APPLE__
    #include <OpenAL/al.h>
    #include <OpenAL/alc.h>
#else
    #include <AL/al.h>
    #include <AL/alc.h>
#endif

#include <iostream>

#define check_al_error() logOpenAlError(__FILE__,__LINE__)

static ALCcontext *internal_context = nullptr;

struct openal_t
{
    int channels;
    long samplerate;

    ALuint buffer;
    ALuint source;
};

static void logOpenAlError(const char *file, int line)
{
    ALenum error = alGetError();

    if(error != AL_NO_ERROR) {
        std::string errorStr;

        switch (error) {
            case AL_INVALID_NAME:
                errorStr = "AL_INVALID_NAME";
                break;
            case AL_INVALID_ENUM:
                errorStr = "AL_INVALID_ENUM";
                break;
            case AL_INVALID_VALUE:
                errorStr = "AL_INVALID_VALUE";
                break;
            case AL_INVALID_OPERATION:
                errorStr = "AL_INVALID_OPERATION";
                break;
            case AL_OUT_OF_MEMORY:
                errorStr = "AL_OUT_OF_MEMORY";
                break;
            default:
                errorStr = "UNKNOWN ERROR";
                break;
        }

        std::cout << errorStr << " " << error << " at " << file << ":" << line << std::endl;
    }
}

static ALenum internal_get_format(size_t channels)
{
    return channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
}

int openal_begin()
{
    if (internal_context != nullptr)
    {
        // openal_open already called! This can happen if you video_init and audio_init.
        return 1;
    }

    ALCdevice *device;

    if ((device = alcOpenDevice(nullptr)) == nullptr)
    {
        return 0;
    }

    if ((internal_context = alcCreateContext(device, nullptr)) == nullptr)
    {
        alcCloseDevice(device);
        return 0;
    }

    if (!alcMakeContextCurrent(internal_context))
    {
        alcDestroyContext(internal_context);
        alcCloseDevice(device);
        return 0;
    }

    return 1;
}

openal_t* openal_static_create(int channels, long samplerate, char* buffer, int buffersize)
{
    auto source = new openal_t;

    source->channels = channels;
    source->samplerate = samplerate;

    alGenSources(1, &source->source);
    alGenBuffers(1, &source->buffer);

    alSource3f(source->source, AL_POSITION,        0.0, 0.0, 0.0);
    alSource3f(source->source, AL_VELOCITY,        0.0, 0.0, 0.0);
    alSource3f(source->source, AL_DIRECTION,       0.0, 0.0, 0.0);
    alSourcef (source->source, AL_ROLLOFF_FACTOR,  0.0          );
    alSourcei (source->source, AL_SOURCE_RELATIVE, AL_TRUE      );

    ALenum format = internal_get_format(channels);

    alBufferData(source->buffer, format, buffer, buffersize, (ALsizei)samplerate);
    alSourceQueueBuffers(source->source, 1, &source->buffer);

    return source;
}

int openal_static_play(openal_t* source)
{
    ALenum state;

    alGetSourcei(source->source, AL_SOURCE_STATE, &state);
    check_al_error();

    if (state != AL_PLAYING)
    {
        alSourcePlay(source->source);
        check_al_error();

        return 1;
    }

    // Already playing
    return 0;
}

extern void openal_static_loop(openal_t* source)
{
    alSourcei(source->source, AL_LOOPING, AL_TRUE);
    openal_static_play(source);
}

extern void openal_static_stop(openal_t* source)
{
    alSourcei(source->source, AL_LOOPING, AL_FALSE);
    alSourceStop(source->source);
}

void openal_source_close(openal_t* source)
{
    alSourceStop(source->source);

    alDeleteSources(1, &source->source);
    alDeleteBuffers(1, &source->buffer);

    delete source;
}

void openal_finish()
{
    if (internal_context == nullptr)
    {
        return;
    }

    ALCdevice* device = alcGetContextsDevice(internal_context);
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(internal_context);
    alcCloseDevice(device);
}
