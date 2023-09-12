#pragma once

#include <stdlib.h>
#include <string.h>

#ifdef WIN32
#include <windows.h>

#else
#include <unistd.h>

#endif

int sd_min(int a, int b)
{
    return a<b ? a:b;
}

int sd_max(int a, int b)
{
    return a>b ? a:b;
}

void sd_clear_screen()
{
    #ifdef WIN32
    system("cls");

    #else
    system("clear");

    #endif
}

void sd_sleep(float secs)
{
    #ifdef WIN32
    Sleep(secs * 1000);

    #else
    sleep(secs);

    #endif
}

void sd_block(void* flag, int timeout, int dt)
{
    if(dt < 0) return;

    while(timeout>0 && !(*((int *) flag)))
    {
        sd_sleep(dt);
        timeout -= dt;
    }
}

int sd_find(char* str, char x)
{
    int i = 0;

    if(!str) return 0;

    while(str[i]!='\0' && str[i]!=x) i++;
    return i;
}

int sd_rfind(char* str, char x)
{
    int i = strlen(str)-1;

    if(!str) return i;

    while(i>=0 && str[i]!=x) i--;
    return i;
}

void sd_copy_(char* dest, char* src, int d_start, int d_end, int s_start, int s_end)
{
    if(!dest) return;
    if(!src) return;

    if(d_start < 0) return;
    if(s_start < 0) return;

    while(d_start<d_end && s_start<s_end && src[s_start]!='\0')
    {
        dest[d_start++] = src[s_start++];
    }
}

void sd_copy(char* dest, char* src)
{
    int n;

    if(!dest) return;
    if(!src) return;

    n = strlen(src);
    sd_copy_(dest, src, 0, n, 0, n);
    dest[n] = '\0';
}

void sd_fill(char* str, char x, int start, int end)
{
    if(!str) return;
    if(start < 0) return;

    while(start<end) str[start++] = x;
}

void sd_ltrim_(char* str, char x)
{
    int i, n;

    if(!str) return;
    n = strlen(str);
    i = 0;

    while(str[i]!='\0' && str[i]==x) i++;

    sd_copy_(str, str, 0, n, i, n);
    str[n-i] = '\0';
}

void sd_ltrim(char* str)
{
    int i, n;

    if(!str) return;
    n = strlen(str);
    i = 0;

    while(str[i]!='\0' && (str[i]==' ' || str[i]=='\n')) i++;

    sd_copy_(str, str, 0, n, i, n);
    str[n-i] = '\0';
}

void sd_rtrim_(char* str, char x)
{
    int i, n;

    if(!str) return;
    n = strlen(str);
    i = n-1;

    while(i>=0 && str[i]==x) i--;

    sd_copy_(str, str, 0, n, 0, i+1);
    str[i+1] = '\0';
}

void sd_rtrim(char* str)
{
    int i, n;

    if(!str) return;
    n = strlen(str);
    i = n-1;

    while(i>=0 && (str[i]==' ' || str[i]=='\n')) i--;

    sd_copy_(str, str, 0, n, 0, i+1);
    str[i+1] = '\0';
}

void sd_trim_(char* str, char x)
{
    sd_ltrim_(str, x);
    sd_rtrim_(str, x);
}

void sd_trim(char* str)
{
    sd_ltrim(str);
    sd_rtrim(str);
}

void sd_parse(char* input, char* cmd)
{
    int i, n;

    if(!input) return;
    if(!cmd) return;

    sd_trim(input);
    
    i = sd_find(input, ' ');
    n = strlen(input);

    sd_copy_(cmd, input, 0, i, 0, i);
    cmd[i] = '\0';

    sd_copy_(input, input, 0, n, i, n);
    input[n-i] = '\0';

    sd_trim(input);
    sd_trim_(input, '\"');
    sd_trim(input);
}

// https://www.strudel.org.uk/itoa/

/*
    * C++ version 0.4 char* style "itoa":
    * Written by Lukás Chmela
    * Released under GPLv3.
*/

char* sd_itoa(int value, char* result, int base) 
{
    int tmp_value;

    char tmp_char;
    char* ptr = result;
    char* ptr1 = result;
    char* str = "zyxwvutsrqponmlkjihgfedcba"
                "9876543210123456789"
                "abcdefghijklmnopqrstuvwxyz";

    if(!result) return NULL;
    if(base < 2) return NULL;

    // check that the base if valid
    if (base < 2 || base > 36) 
    { 
        *result = '\0'; 
        return result; 
    }

    do 
    {
        tmp_value = value;
        value /= base;
        *ptr++ = str[35 + (tmp_value - value * base)];
    } 
    while (value);

    // Apply negative sign
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';

    while(ptr1 < ptr) 
    {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    
    return result;
}

char* sd_ttoa(int time, char* result)
{
    int hrs, mins, secs, n; 
    char temp[20];

    if(!result) return result;
    if(time < 0) return result;

    hrs = time/3600; time %= 3600;
    mins = time/60; time %= 60;
    secs = time;

    if(hrs==0 && mins==0 && secs==0)
    {
        strcpy(result, "00:00:00");
        return result;
    }

    if(hrs > 99)
    {
        strcpy(result, "99:99:99");
        return result;
    }
    
    itoa(hrs, temp, 10);

    result[0] = '0';
    if(hrs > 9) strcpy(result, temp);
    else result[1] = temp[0];
    result[2] = ':';
    result[3] = '\0';

    itoa(mins, temp, 10);

    result[3] = '0';
    if(mins > 9) strcpy(result+3, temp);
    else result[4] = temp[0];
    result[5] = ':';
    result[6] = '\0';

    itoa(secs, temp, 10);

    result[6] = '0';
    if(secs > 9) strcpy(result+6, temp);
    else result[7] = temp[0];
    result[8] = '\0';

    return result;
}