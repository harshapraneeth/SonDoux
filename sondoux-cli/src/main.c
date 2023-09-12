#include <unistd.h>
#include <windows.h>
#include <stdio.h>

#include "includes/sd_constants.h"
#include "includes/sd_utils.h"

int main(int argc, char** argv)
{
    int opt;

    char server_process[MAX_BUFFER_SIZE];
    char client_process[MAX_BUFFER_SIZE];

    strcpy(server_process, "sd_server.exe ");
    strcpy(client_process, "sd_client.exe ");

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    while ((opt = getopt(argc, argv, "sc:")) != -1)
    {
        switch (opt)
        {
            case 's': 

                CreateProcess(
                    NULL,           
                    (const char *) server_process,    
                    NULL, NULL, FALSE,
                    CREATE_NO_WINDOW | DETACHED_PROCESS,
                    NULL, NULL,
                    &si, &pi
                );

                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);

                return 0;

            case 'c':
            
                strcat(client_process, "\"");
                strcat(client_process, optarg);
                strcat(client_process, "\"");
                system(client_process);

                break;

            default: abort();
        }
    }

    return 0;
}