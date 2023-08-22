#include <stdio.h>
#include <unistd.h>
#include <windows.h>

#include "sd_constants.h"

int main(int argc, char** argv)
{
    int opt;

    const char* server_process = "S:/SonDoux/sondoux-cli/sd_server.exe";
    const char* client_process = "S:/SonDoux/sondoux-cli/sd_client.exe";

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    while ((opt = getopt (argc, argv, "sc")) != -1)
    {
        switch (opt)
        {
            case 's': 

                CreateProcess(
                    NULL,           
                    server_process,    
                    NULL, NULL, FALSE,
                    CREATE_NO_WINDOW | DETACHED_PROCESS,
                    NULL, NULL,
                    &si, &pi
                );

                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);

                return 0;

            case 'c':
            
                system(client_process);
                break;

            default: abort ();
        }
    }

    return 0;
}