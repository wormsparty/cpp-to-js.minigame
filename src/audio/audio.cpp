#include "../include/audio.h"
#include "../include/vorbis.h"
#include "../include/openal.h"

#include <list>
#include <vector>
#include <iostream>

static int loaded_sound_count = 0;

int audio_begin()
{
    return openal_begin();
}

void audio_finish()
{
    if (loaded_sound_count != 0)
    {
        std::cout << "Asymmetrical call to audio load/unload !" << std::endl;
    }

    openal_finish();
}

audio_t* audio_sound_load(const std::string filename)
{
    vorbis_t* vorbis_data;

    int channels;
    long samplerate;

    size_t read;
    size_t buffsize = 0;

    if ((vorbis_data = vorbis_open(
            filename,
            &channels,
            &samplerate)) == nullptr)
    {
        std::cout << "Failed to read sound file: " << filename << std::endl;
        return nullptr;
    }

#define CHUNK_SIZE 512
    char* buffer = (char*)malloc(CHUNK_SIZE);

    while (1)
    {
        read = vorbis_read(vorbis_data, buffer + buffsize, CHUNK_SIZE);
        buffsize += read;

        if (read == CHUNK_SIZE)
        {
            buffer = (char*)realloc(buffer, buffsize + CHUNK_SIZE);
        }
        else
        {
            break;
        }
    }

    vorbis_close(vorbis_data);

    openal_t* source = openal_static_create(channels, samplerate, buffer, buffsize);
    free(buffer);

    if (source == nullptr)
    {
        return nullptr;
    }

    loaded_sound_count++;
    return (audio_t*)source;
}

extern void audio_sound_unload(audio_t* sound)
{
	if (sound != nullptr)
	{
		loaded_sound_count--;
		openal_source_close((openal_t*)sound);
	}
}

extern void audio_sound_play(audio_t* sound)
{
    openal_static_play((openal_t*)sound);
}

extern void audio_sound_loop(audio_t* sound)
{
    openal_static_loop((openal_t*)sound);
}

extern void audio_sound_stop(audio_t* sound)
{
    openal_static_stop((openal_t*)sound);
}