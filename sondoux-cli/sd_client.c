#include <stdio.h>

#include "sd_comm.h"
#include "sd_exec.h"

SDComm* client_comm = NULL;

void* client_packet_handler(SDPacket* _packet)
{
    if(!_packet) return NULL;

    printf("%s", _packet->data);
    client_comm->stop_signal = true;

    delete_packet(_packet);
    return NULL;
}

int main(int argc, char** argv)
{
    char input[MAX_BUFFER_SIZE];
    struct sockaddr_in server_address;
    SDPacket* packet;
    
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);
    server_address.sin_addr.s_addr = inet_addr(LOCAL_HOST);

    if(argc <= 1)
    {
        printf("SonDoux> ");
        gets(input);
    }

    else
    {
        strcpy(input, argv[1]);
    }

    client_comm = create_comm(
        LOCAL_HOST, CLIENT_PORT,
        client_packet_handler, 3, 1
    );

    if(!client_comm) return 0;

    packet = create_packet(
        &client_comm->address,
        &server_address,
        input
    );

    if(packet)
    {
        client_comm->stop_signal |= !send_packet(client_comm, packet);
        delete_packet(packet);
    }

    sd_block(&client_comm->stop_signal, 10, 1);
    delete_comm(client_comm);

    WSACleanup();
    return 0;
}