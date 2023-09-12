#pragma once

#include <stdlib.h>

#define MINIAUDIO_IMPLEMENTATION
#include "../../../miniaudio/miniaudio.h"
#include "sd_utils.h"

typedef enum {playing, paused} SDSongState;

typedef struct SDSong
{
    ma_device device;
    ma_decoder decoder;
    ma_device_config config;
    
    ma_uint64 length;
    ma_int64 cursor;
    ma_int64 sample_rate;

    SDSongState state;
    void (*data_callback)(ma_device*, void*, const void*, ma_uint32);
} SDSong;

void delete_song(SDSong* _song)
{
    if(!_song) return;
    
    ma_device_uninit(&_song->device);
    ma_decoder_uninit(&_song->decoder);
    free(_song);
}

SDSong* create_song(
    char* filename, 
    void (*data_callback)(ma_device*, void*, const void*, ma_uint32)
){
    ma_result result;
    SDSong* song = NULL;

    if(!filename) return NULL;
    if(!data_callback) return NULL;
    
    song = (SDSong *) malloc(sizeof(SDSong));
    if(!song) return NULL;

    result = ma_decoder_init_file(filename, NULL, &song->decoder);
    if(result != MA_SUCCESS)
    {
        delete_song(song);
        return NULL;
    }

    song->config = ma_device_config_init(ma_device_type_playback);
    song->config.playback.format = song->decoder.outputFormat;
    song->config.playback.channels = song->decoder.outputChannels;
    song->config.sampleRate = song->decoder.outputSampleRate;
    song->config.dataCallback = data_callback;
    song->config.pUserData = &song->decoder;

    result = ma_device_init(NULL, &song->config, &song->device);
    if(result != MA_SUCCESS)
    {
        delete_song(song);
        return NULL;
    }

    song->cursor = 0;
    song->sample_rate = song->decoder.outputSampleRate;
    song->data_callback = data_callback;

    result = ma_data_source_get_length_in_pcm_frames(
        &song->decoder, 
        &song->length
    );

    if(result != MA_SUCCESS)
    {
        delete_song(song);
        return NULL;
    }

    song->state = paused;
    return song;
}

void pause_song(SDSong* song)
{
    ma_result result;

    if(!song) return;
    if(song->state != playing) return;

    result = ma_device_stop(&song->device);
    if(result == MA_SUCCESS) song->state = paused;
}

void play_song(SDSong* song)
{
    ma_result result;

    if(!song) return;
    if(song->state != paused) return;

    result = ma_device_start(&song->device);
    if(result == MA_SUCCESS) song->state = playing;
}

void seek_time(SDSong* song, ma_int64 cursor)
{
    ma_result result;
    int was_playing = 0;

    if(!song) return;

    if(song->state == playing)
    {
        pause_song(song);
        was_playing = 1;
    }

    cursor = sd_min(
        sd_max(cursor, 0),
        song->length
    );

    result = ma_data_source_seek_to_pcm_frame(
        &song->decoder, 
        cursor
    );
    
    if(result == MA_SUCCESS) song->cursor = cursor;
    if(was_playing) play_song(song);
}