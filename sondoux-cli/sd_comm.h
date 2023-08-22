#pragma once
#pragma comment(lib, "ws2_32.lib")

#include <winsock2.h>
#include <stdlib.h>
#include <pthread.h>

#include "sd_packet.h"
#include "sd_queue.h"
#include "sd_thread.h"
#include "sd_constants.h"
#include "sd_utils.h"

typedef struct SDComm
{
    struct sockaddr_in address;

    SDQueue* packet_queue;
    pthread_mutex_t queue_lock;
    void* (*packet_handler)(SDPacket *);

    SDThread* listening_thread;
    SDThread* responding_thread;

    int timeout;
    int stop_signal;
} SDComm;

void delete_comm(SDComm* _comm)
{
    if(!_comm) return;

    _comm->stop_signal = 1;

    join_thread(_comm->listening_thread);
    join_thread(_comm->responding_thread);

    delete_thread(_comm->listening_thread);
    delete_thread(_comm->responding_thread);

    delete_queue(_comm->packet_queue);
    pthread_mutex_destroy(&_comm->queue_lock);

    free(_comm);
}

void* sd_listen(void* args);
void* sd_respond(void* args);
SDComm* create_comm(
    char* ip, int port,
    void* (*packet_handler)(SDPacket *), 
    int max_packets,
    int timeout
){
    SDComm* comm = NULL;
    int result = 0;

    if(!ip) return NULL;
    if(port < 1) return NULL;
    if(!packet_handler) return NULL;
    if(max_packets < 1) return NULL;
    if(timeout < 1) return NULL;

    comm = (SDComm *) malloc(sizeof(SDComm));
    if(!comm) return NULL;

    comm->address.sin_family = AF_INET;
    comm->address.sin_port = htons(port);
    comm->address.sin_addr.s_addr = inet_addr(ip);

    comm->listening_thread = NULL;
    comm->responding_thread = NULL;

    comm->packet_queue = create_queue(max_packets, false);
    if(!comm->packet_queue)
    {
        delete_comm(comm);
        return NULL;
    }

    result = pthread_mutex_init(&comm->queue_lock, NULL);
    if(result)
    {
        delete_comm(comm);
        return NULL;
    }

    comm->listening_thread = create_thread();
    if(!comm->listening_thread)
    {
        delete_comm(comm);
        return NULL;
    }

    comm->responding_thread = create_thread();
    if(!comm->responding_thread)
    {
        delete_comm(comm);
        return NULL;
    }

    comm->packet_handler = packet_handler;
    comm->stop_signal = 0;
    comm->timeout = timeout;
    
    run_thread(
        comm->listening_thread,
        &sd_listen,
        comm
    );

    run_thread(
        comm->responding_thread,
        &sd_respond,
        comm
    );

    return comm;
}

int send_packet(SDComm* comm, SDPacket* packet)
{
    SOCKET sock = 0;
    char* data = NULL;
    int result = 0;

    if(!comm) return result;
    if(!packet) return result;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock == INVALID_SOCKET)
    {
        closesocket(sock);
        return result;
    }

    data = sd_serialize(packet);
    if(!data)
    {
        closesocket(sock);
        return result;
    }

    // removing this somehow messes with sending
    inet_ntoa(comm->address.sin_addr);
    
    result = sendto(
        sock, data, 
        PACKET_SIZE, 0,
        (struct sockaddr *) &packet->dest_addr,
        sizeof(packet->dest_addr)
    );

    free(data);
    closesocket(sock);
    return result == PACKET_SIZE;
}

void* sd_listen(void* args)
{
    SOCKET sock = 0;
    fd_set readfds;
    struct timeval timer;

    char buffer[PACKET_SIZE];
    int result = 0;

    SDComm* comm = NULL;
    SDPacket* packet = NULL;
    SDQElem* elem = NULL;

    comm = (SDComm *) args;
    if(!comm) return NULL;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock == INVALID_SOCKET)
    {
        closesocket(sock);
        return NULL;
    }

    result = bind(
        sock, 
        (struct sockaddr *) &comm->address, 
        sizeof(comm->address)
    );

    if(result == SOCKET_ERROR)
    {
        closesocket(sock);
        return NULL;
    }

    while(!comm->stop_signal)
    {
        pthread_mutex_lock(&comm->queue_lock);

        if(comm->packet_queue->size >= comm->packet_queue->max_size)
        {
            pthread_mutex_unlock(&comm->queue_lock);
            sd_sleep(comm->timeout);
            continue;
        }

        else pthread_mutex_unlock(&comm->queue_lock);

        FD_ZERO(&readfds);
        FD_SET(sock, &readfds);

        timer.tv_sec = comm->timeout;
        timer.tv_usec = 0;

        result = select(sock + 1, &readfds, NULL, NULL, &timer);
        if(result == 0) continue;
        if(result == SOCKET_ERROR) break;

        result = recvfrom(sock, buffer, PACKET_SIZE, 0, NULL, NULL);
        if(result != PACKET_SIZE) continue;

        packet = sd_deserialize(buffer);
        if(!packet) continue;

        elem = create_qelem(packet, PACKET_SIZE);
        if(!elem)
        {
            delete_packet(packet);
            continue;
        }

        pthread_mutex_lock(&comm->queue_lock);
        push(comm->packet_queue, elem);
        pthread_mutex_unlock(&comm->queue_lock);

        delete_packet(packet);
        elem->data = NULL;
        delete_qelem(elem);
    }
    
    closesocket(sock);
    return NULL;
}

void* sd_respond(void* args)
{
    SDQElem* elem = NULL;
    SDPacket* input_packet = NULL;
    SDPacket* output_packet = NULL;
    SDComm* comm = NULL;

    if(!args) return NULL;

    comm = (SDComm *) args;
    if(!comm) return NULL;
    if(!comm->packet_handler) return NULL;

    while(!comm->stop_signal)
    {
        pthread_mutex_lock(&comm->queue_lock);
        elem = pop(comm->packet_queue);
        pthread_mutex_unlock(&comm->queue_lock);

        if(!elem)
        {
            sd_sleep(comm->timeout);
            continue;
        }

        input_packet = (SDPacket *) elem->data;
        if(!input_packet)
        {
            delete_qelem(elem);
            continue;
        }

        output_packet = (SDPacket *) comm->packet_handler(input_packet);
        if(output_packet)
        {
            send_packet(comm, output_packet);
            delete_packet(output_packet);
        }

        elem->data = NULL;
        delete_qelem(elem);
    }

    return NULL;
}
