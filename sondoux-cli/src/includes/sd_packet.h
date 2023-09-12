#pragma once
#pragma comment(lib, "ws2_32.lib")

#include <winsock2.h>
#include <stdlib.h>

#include "sd_constants.h"

typedef struct SDPacket
{
    struct sockaddr_in src_addr;
    struct sockaddr_in dest_addr;

    char data[MAX_BUFFER_SIZE];
} SDPacket;

const int PACKET_SIZE = sizeof(SDPacket);

void delete_packet(SDPacket* _packet)
{
    if(!_packet) return;
    free(_packet);
}

SDPacket* create_packet(
    struct sockaddr_in* src_addr, 
    struct sockaddr_in* dest_addr, 
    char* data
){
    SDPacket* packet = NULL;

    if(!src_addr) return NULL;
    if(!dest_addr) return NULL;
    if(!data) return NULL;

    packet = (SDPacket *) malloc(sizeof(SDPacket));
    if(!packet) return NULL;

    memcpy(&packet->src_addr, src_addr, sizeof(struct sockaddr_in));
    memcpy(&packet->dest_addr, dest_addr, sizeof(struct sockaddr_in));
    memcpy(packet->data, data, MAX_BUFFER_SIZE);

    return packet;
}

char* sd_serialize(SDPacket* packet)
{
    char* buffer = NULL;

    if(!packet) return NULL;

    buffer = (char *) malloc(PACKET_SIZE);
    if(!buffer) return NULL;

    memcpy(buffer, packet, PACKET_SIZE);
    return buffer;
}

SDPacket* sd_deserialize(char* buffer)
{
    SDPacket* packet = NULL;

    if(!buffer) return NULL;

    packet = (SDPacket *) malloc(PACKET_SIZE);
    if(!packet) return NULL;

    memcpy(packet, buffer, PACKET_SIZE);
    return packet;
}