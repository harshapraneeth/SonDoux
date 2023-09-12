#include <stdio.h>
#include <conio.h>
#include <string.h>

#include "../src/includes/sd_constants.h"
#include "../src/includes/sd_utils.h"
#include "../src/includes/sd_exec.h"

int main(int argc, char** argv)
{
    char input[MAX_BUFFER_SIZE];

    while(true)
    {
        printf("Sondoux> ");
        gets(input);
        printf("%s", exec(input));
    }
}