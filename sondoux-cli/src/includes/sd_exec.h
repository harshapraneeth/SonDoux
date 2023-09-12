#pragma once

#include "sd_constants.h"
#include "sd_playlist.h"

SDPlaylist* current_playlist = NULL;

void data_callback(
    ma_device* pDevice, 
    void* pOutput, 
    const void* pInput, 
    ma_uint32 frameCount
){
    ma_decoder* pDecoder;
    
    pDecoder = (ma_decoder *) pDevice->pUserData;
    if (pDecoder == NULL) return;

    ma_decoder_read_pcm_frames(
        pDecoder, 
        pOutput, 
        frameCount, 
        NULL
    );

    if(
        current_playlist && 
        current_playlist->current_song
    )
        current_playlist->current_song->cursor += frameCount;

    (void) pInput;
}

void sd_quit(char*, char*);
void sd_clear(char*, char*);
void sd_play(char*, char*);
void sd_pause(char*, char*);
void sd_open(char*, char*);
void sd_prev(char*, char*);
void sd_next(char*, char*);
void sd_time(char*, char*);
void sd_seek(char*, char*);
void sd_shuffle(char*, char*);
void sd_repeat(char*, char*);
void sd_autoplay(char*, char*);
void sd_toggle(char*, char*);
void sd_scroll(char*, char*);
void sd_jump(char*, char*);

char* exec(char* input)
{
    char cmd[MAX_BUFFER_SIZE];
    char* output;

    output = (char *) malloc(MAX_BUFFER_SIZE);
    if(!output) return NULL;
    
    strcpy(output, "");
    sd_parse(input, cmd);

    if(
        strcmp(cmd, "quit") == 0 || 
        strcmp(cmd, "exit") == 0
    ) 
        sd_quit(input, output);

    else if(
        strcmp(cmd, "clear") == 0 || 
        strcmp(cmd, "cls") == 0
    ) 
        sd_clear(input, output);

    else if(
        strcmp(cmd, "play") == 0
    ) 
        sd_play(input, output);

    else if(
        strcmp(cmd, "pause") == 0
    ) 
        sd_pause(input, output);

    else if(
        strcmp(cmd, "open") == 0
    )  
        sd_open(input, output);

    else if(
        strcmp(cmd, "prev") == 0
    )  
        sd_prev(input, output);

    else if(
        strcmp(cmd, "next") == 0
    )  
        sd_next(input, output);

    else if(
        strcmp(cmd, "time") == 0
    )  
        sd_time(input, output);
    
    else if(
        strcmp(cmd, "seek") == 0
    )  
        sd_seek(input, output);

    else if(
        strcmp(cmd, "shuffle") == 0
    )  
        sd_shuffle(input, output);

    else if(
        strcmp(cmd, "repeat") == 0
    )  
        sd_repeat(input, output);

    else if(
        strcmp(cmd, "autoplay") == 0
    )  
        sd_autoplay(input, output);

    else if(
        strcmp(cmd, "toggle") == 0
    )  
        sd_toggle(input, output);

    else if(
        strcmp(cmd, "scroll")==0
    )
        sd_scroll(input, output);

    else if(
        strcmp(cmd, "jump")==0
    )
        sd_jump(input, output);

    return output;
}

void sd_quit(char* input, char* output)
{
    delete_playlist(current_playlist);
    strcpy(output, EXIT_MSG);
}

void sd_clear(char* input, char* output)
{
    sd_clear_screen();
}

void sd_play(char* input, char* output)
{
    if(!current_playlist) return;

    play_song(current_playlist->current_song);

    if(!current_playlist->current_elem) return;

    strcat(output, "Playing: ");
    strcat(
        output, 
        (char *) current_playlist->current_elem->data + 
        sd_rfind(
            (char *) current_playlist->current_elem->data, 
            '/'
        ) + 1
    );
    strcat(output, ".\n");
}

void sd_pause(char* input, char* output)
{
    if(!current_playlist) return;

    pause_song(current_playlist->current_song);

    if(!current_playlist->current_elem) return;

    strcat(output, "Paused: ");
    strcat(
        output, 
        (char *) current_playlist->current_elem->data + 
        sd_rfind(
            (char *) current_playlist->current_elem->data, 
            '/'
        ) + 1
    );
    strcat(output, ".\n");
}

void sd_open(char* input, char* output)
{
    delete_playlist(current_playlist);
    current_playlist = create_playlist(
        input,
        data_callback
    );
    
    if(!current_playlist) return;
    if(!current_playlist->current_elem) return;

    strcat(output, "Playing: ");
    strcat(
        output, 
        (char *) current_playlist->current_elem->data + 
        sd_rfind(
            (char *) current_playlist->current_elem->data, 
            '/'
        ) + 1
    );
    strcat(output, ".\n");

    strcpy(input, "0");
    sd_scroll(input, output);
}

void sd_prev(char* input, char* output)
{
    char temp[10];

    if(!current_playlist) return;

    play_prev(current_playlist);
    
    if(!current_playlist->current_elem) return;

    strcat(output, "Playing: ");
    strcat(
        output, 
        (char *) current_playlist->current_elem->data + 
        sd_rfind(
            (char *) current_playlist->current_elem->data, 
            '/'
        ) + 1
    );
    strcat(output, ".\n");
    
    strcat(input, itoa(current_playlist->display_index, temp, 10));
    sd_scroll(input, output);
}

void sd_next(char* input, char* output)
{
    char temp[10];

    if(!current_playlist) return;

    play_next(current_playlist);

    if(!current_playlist->current_elem) return;

    strcat(output, "Playing: ");
    strcat(
        output, 
        (char *) current_playlist->current_elem->data + 
        sd_rfind(
            (char *) current_playlist->current_elem->data, 
            '/'
        ) + 1
    );
    strcat(output, ".\n");
    
    strcat(input, itoa(current_playlist->display_index, temp, 10));
    sd_scroll(input, output);
}

void sd_time(char* input, char* output)
{
    int n;
    int curr = 0;
    int total = 0;
    char temp[9];
    char time[60];

    if(!current_playlist) return;
    if(!current_playlist->current_song) return;

    curr = (
        current_playlist->current_song->cursor / 
        current_playlist->current_song->sample_rate
    );

    total = (
        current_playlist->current_song->length /
        current_playlist->current_song->sample_rate
    );

    sd_fill(time, ' ', 0, 59);
    time[59] = '\0';

    sd_ttoa(curr, temp);
    sd_copy_(time, temp, 0, 9, 0, 9);
    temp[9] = '\0';

    sd_ttoa(total, temp);
    sd_copy_(time, temp, 50, 59, 0, 9);
    time[59] = '\0';

    n = 9 + round((curr*40.0) / (total*1.0));
    sd_fill(time, '=', 9, n);
    sd_fill(time, '.', n, 50);

    strcat(output, time);
    strcat(output, "\n");
}

void sd_seek(char* input, char* output)
{
    if(!current_playlist) return;
    if(!current_playlist->current_song) return;

    if(
        input[0]=='\0' || 
        !(
            input[0]=='-' || 
            input[0]=='+' || 
            (input[0]>47 && input[0]<58)
        )
    ) 
        return;

    if(input[0]=='+')
    {
        input++;

        seek_time(
            current_playlist->current_song,
            atoi(input) * 
            current_playlist->current_song->sample_rate + 
            current_playlist->current_song->cursor
        );
    }

    else if(input[0]=='-')
    {
        seek_time(
            current_playlist->current_song,
            atoi(input) * 
            current_playlist->current_song->sample_rate + 
            current_playlist->current_song->cursor
        );
    }

    else
    {
        seek_time(
            current_playlist->current_song,
            atoi(input) * 
            current_playlist->current_song->sample_rate
        );
    }

    sd_time(input, output);
}

void sd_shuffle(char* input, char* output)
{
    sd_trim(input);
    SHUFFLE = strcmp(input, "on")==0;

    strcat(output, "shuffle: ");
    strcat(output, SHUFFLE ? "on" : "off");
}

void sd_repeat(char* input, char* output)
{
    sd_trim(input);
    REPEAT = strcmp(input, "on")==0;

    strcat(output, "repeat: ");
    strcat(output, REPEAT ? "on" : "off");
}

void sd_autoplay(char* input, char* output)
{
    sd_trim(input);
    AUTOPLAY = strcmp(input, "on")==0;

    strcat(output, "autoplay: ");
    strcat(output, AUTOPLAY ? "on" : "off");
}

void sd_toggle(char* input, char* output)
{
    if(strcmp(input, "play")==0)
    {
        if(!current_playlist) return;
        if(!current_playlist->current_song) return;

        if(current_playlist->current_song->state == playing) 
            sd_pause(input, output);

        else if(current_playlist->current_song->state == paused) 
            sd_play(input, output);
    }

    else if(strcmp(input, "shuffle")==0)
    {
        strcpy(input, SHUFFLE ? "off":"on");
        sd_shuffle(input, output);
    }

    else if(strcmp(input, "repeat")==0)
    {
        strcpy(input, REPEAT ? "off":"on");
        sd_repeat(input, output);
    }

    else if(strcmp(input, "autoplay")==0)
    {
        strcpy(input, AUTOPLAY ? "off":"on");
        sd_repeat(input, output);
    }
}

void sd_scroll(char* input, char* output)
{
    int i, n, index;
    char temp[100];

    SDQElem* elem;
    
    if(!current_playlist) return;

    if(
        input[0]=='\0' || 
        !(
            input[0]=='-' || 
            input[0]=='+' || 
            (input[0]>47 && input[0]<58)
        )
    ) 
        return;

    index = 0;

    if(input[0] == '+')
    {
        input++;
        index = current_playlist->display_index + atoi(input);
    }

    else if(input[0] == '-')
    {
        index = current_playlist->display_index + atoi(input);
    }

    else
    {
        index = atoi(input);
    }

    i = current_playlist->display_index;
    n = current_playlist->song_queue->size - 1; 

    if(index < 0) index += current_playlist->song_queue->size;
    index = index % current_playlist->song_queue->size;
    elem = current_playlist->display_elem;

    if(index < (n-index) && index < abs(i-index))
    {
        elem = current_playlist->song_queue->head;
        i = 0;
    }

    else if((n-index) < abs(i-index))
    {
        elem = current_playlist->song_queue->tail;
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

    current_playlist->display_elem = elem;
    current_playlist->display_index = index;

    i = 0;
    strcat(output, "\n");

    while(elem && i<PLAYLIST_DISPLAY)
    {
        sd_itoa(
            current_playlist->display_index+i+1,
            temp, 10
        );

        strcat(temp, ". ");
        strcat(
            temp, 
            (char *) elem->data + sd_rfind((char *) elem->data, '/') + 1
        );

        strcat(output, temp);
        strcat(output, "\n");

        elem = elem->next;
        i++;
    }

    if(i>0) strcat(output, "\n");
}

void sd_jump(char* input, char* output)
{
    char temp[10];

    if(input[0]=='\0') return;
    if(!current_playlist) return;

    jump_to(current_playlist, atoi(input)-1);

    if(!current_playlist->current_elem) return;

    strcat(output, "Playing: ");
    strcat(
        output, 
        (char *) current_playlist->current_elem->data + 
        sd_rfind(
            (char *) current_playlist->current_elem->data, 
            '/'
        ) + 1
    );
    strcat(output, ".\n");

    strcpy(input, itoa(current_playlist->display_index, temp, 10));
    sd_scroll(input, output);
}