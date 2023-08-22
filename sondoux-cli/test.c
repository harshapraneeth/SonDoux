#include <stdio.h>
#include <conio.h>
#include <string.h>

#include "sd_constants.h"
#include "sd_utils.h"
#include "sd_server.h"
#include "sd_client.h"

int main(int argc, char** argv)
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);

    if(argc > 1) run_server(NULL);
    else run_client(NULL);

    while(true)
    {
        if(server_comm && server_comm->stop_signal) break;
        if(client_comm && client_comm->stop_signal) break;

        sd_sleep(EXEC_DELAY);
    }

    delete_comm(server_comm);
    delete_comm(client_comm);
    delete_playlist(current_playlist);

    return 0;
}