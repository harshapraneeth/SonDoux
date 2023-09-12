#pragma once

#include <dirent.h>
#include <time.h>
#include <stdio.h>

#include "sd_constants.h"
#include "sd_queue.h"
#include "sd_song.h"

typedef struct SDPlaylist
{
    SDQueue* song_queue;
    SDQElem* current_elem;
    SDQElem* display_elem;

    int current_index;
    int display_index;

    SDSong* current_song;
    void (*data_callback)(ma_device*, void*, const void*, ma_uint32);
} SDPlaylist;

void delete_playlist(SDPlaylist* _playlist)
{
    if(!_playlist) return;
    
    delete_queue(_playlist->song_queue);
    delete_qelem(_playlist->current_elem);
    delete_qelem(_playlist->display_elem);
    delete_song(_playlist->current_song);
    free(_playlist);
}

void jump_to(SDPlaylist*, int);
SDPlaylist* create_playlist(
    char* directory, 
    void (*data_callback)(ma_device*, void*, const void*, ma_uint32)
){
    DIR* d = NULL;
    struct dirent* dir = NULL;
    SDPlaylist* playlist = NULL;

    char filename[MAX_BUFFER_SIZE];
    char extension[10];

    int i, n;

    if(!directory) return NULL;
    if(!data_callback) return NULL;

    n = strlen(directory);
    if(n <= 0) return NULL;

    playlist = (SDPlaylist *) malloc(sizeof(SDPlaylist));
    if(!playlist) return NULL;

    playlist->song_queue = create_queue(MAX_PLAYLIST_SIZE, false);
    if(!playlist->song_queue)
    {
        free(playlist);
        return NULL;
    }

    i = sd_find(directory, '.');
    if(i < n)
    {
        sd_copy_(extension, directory, 0, n, i+1, n);
        extension[n-i-1] = '\0';

        if(
            strcmp(extension, "mp3") != 0 &&
            strcmp(extension, "wav") != 0 &&
            strcmp(extension, "flac")
        ){
            delete_playlist(playlist);
            return NULL;
        }
        
        push(
            playlist->song_queue, 
            create_qelem(
                (void *) directory,
                strlen(directory) + 1
            )
        );
    }

    else
    {
        if(directory[n-1] != '/') strcat(directory, "/");

        d = opendir(directory);
        if(!d)
        {
            delete_playlist(playlist);
            return NULL;
        }

        while ((dir = readdir(d)) != NULL)
        {
            strcpy(filename, dir->d_name);

            i = sd_rfind(filename, '.');
            if(i<0) continue;

            n = strlen(filename);
            sd_copy_(extension, filename, 0, n, i+1, n);
            extension[n-i-1] = '\0';

            if(
                strcmp(extension, "mp3") != 0 &&
                strcmp(extension, "wav") != 0 &&
                strcmp(extension, "flac")
            ) continue;

            strcpy(filename, directory);
            strcat(filename, dir->d_name);

            push(
                playlist->song_queue, 
                create_qelem(
                    (void *) filename,
                    strlen(filename) + 1
                )
            );
        }

        closedir(d);
    }

    if(playlist->song_queue->size <= 0)
    {
        delete_playlist(playlist);
        return NULL;
    }

    playlist->current_elem = playlist->song_queue->head;
    playlist->current_index = 0;

    playlist->display_elem = playlist->song_queue->head;
    playlist->display_index = 0;

    playlist->data_callback = data_callback;
    srand((unsigned) time(NULL));

    playlist->current_song = NULL;
    jump_to(playlist, 0);
    return playlist;
}

void jump_to(SDPlaylist* playlist, int index)
{
    SDQElem* elem;
    int i, n;

    if(!playlist) return;
    if(!playlist->song_queue) return;
    if(playlist->song_queue->size <= 0) return;
    if(!playlist->current_elem) return;

    i = playlist->current_index;
    n = playlist->song_queue->size - 1; 

    if(index < 0) index += playlist->song_queue->size;
    index = index % playlist->song_queue->size;
    elem = playlist->current_elem;

    if(index < (n-index) && index < abs(i-index))
    {
        elem = playlist->song_queue->head;
        i = 0;
    }

    else if((n-index) < abs(i-index))
    {
        elem = playlist->song_queue->tail;
        i = n;
    }

    while(elem && i<index)
    {
        elem = elem->next;
        i++;
    }

    while(elem && i>index)
    {
        elem = elem->prev;
        i--;
    }

    if(!elem) return;

    playlist->current_elem = elem;
    playlist->current_index = index;

    playlist->display_elem = elem;
    playlist->display_index = index;

    delete_song(playlist->current_song);
    playlist->current_song = create_song(
        (char *) elem->data,
        playlist->data_callback
    );

    play_song(playlist->current_song);
}

void play_next(SDPlaylist* playlist)
{
    if(!playlist) return;
    
    jump_to(
        playlist,
        SHUFFLE ? rand() : (playlist->current_index + 1)
    );

}

void play_prev(SDPlaylist* playlist)
{
    if(!playlist) return;

    jump_to(
        playlist,
        SHUFFLE ? rand() : (playlist->current_index - 1)
    );
}