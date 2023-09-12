#include <stdio.h>

#include "includes/sd_comm.h"
#include "includes/sd_exec.h"

SDComm* server_comm = NULL;

void* server_packet_handler(SDPacket* _packet)
{
    char* output;
    struct sockaddr_in temp;

    if(!_packet) return NULL;

    /*
    printf(
        "Received [%s] from [%s].\n", 
        _packet->data, 
        inet_ntoa(_packet->src_addr.sin_addr)
    );
    */

    output = exec(_packet->data);
    if(!output)
    {
        delete_packet(_packet);
        return NULL;
    }

    temp = _packet->src_addr;
    _packet->src_addr = _packet->dest_addr;
    _packet->dest_addr = temp;

    strcpy(_packet->data, output);

    if(strcmp(output, EXIT_MSG) == 0)
    {
        send_packet(server_comm, _packet);
        free(output);

        server_comm->stop_signal = true;
        return NULL;
    }

    free(output);
    return _packet;
}

int main(int argc, char** argv)
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);

    server_comm = create_comm(
        LOCAL_HOST, SERVER_PORT,
        server_packet_handler, 10, 3
    );
    
    if(server_comm)
    {
        printf("Server started.\n");

        while(!server_comm->stop_signal)
        {
            sd_block(&server_comm->stop_signal, 1000, 10);
        }
    }

    else printf("Failed to Create the server.\n");

    delete_comm(server_comm);
    delete_playlist(current_playlist);

    WSACleanup();
    return 0;
}