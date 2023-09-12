# pragma once

#define true 1
#define false 0

#define EXIT_MSG "Adios.\n"

// IO settings

#define MAX_BUFFER_SIZE 4096
#define MAX_IOQUEUE_SIZE 256

// Socket settings

#define MAX_CLIENTS 5
#define SERVER_PORT 9676
#define CLIENT_PORT 9686
#define LOCAL_HOST "127.0.0.1"

// Player settings

int SHUFFLE = false;
int REPEAT = false;
int AUTOPLAY = true;

// Playlist settings

#define MAX_PLAYLIST_SIZE 4096
#define PLAYLIST_DISPLAY 20

// Exec settings

#define EXEC_DELAY 3