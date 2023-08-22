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

void clear_screen()
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
    int t = 0;

    while(t<timeout && !(*((int *) flag)))
    {
        sd_sleep(dt);
        t += dt;
    }
}

int find(char* str, char x)
{
    int i = 0;

    if(!str) return 0;

    while(str[i]!=x && str[i]!='\0') i++;
    return i;
}

int rfind(char* str, char x)
{
    int i = strlen(str)-1;

    if(!str) return i;

    while(i>=0 && str[i]!=x) i--;
    return i;
}

void copy_from(char* dest, char* src, int start, int end)
{
    int i = 0;

    if(!dest) return;
    if(!src) return;
    if(start < 0) return;

    while(start<end && src[start]!='\0') dest[i++] = src[start++];
    dest[i] = '\0';
}

void copy_to(char* dest, char* src, int start, int end)
{
    int i = 0;

    if(!dest) return;
    if(!src) return;
    if(start < 0) return;

    while(start<end && src[i]!='\0') dest[start++] = src[i++];
}

void copy(char* dest, char* src)
{
    if(!dest) return;
    if(!src) return;

    copy_from(dest, src, 0, strlen(src));
}

void fill(char* str, char x, int start, int end)
{
    if(!str) return;
    if(start < 0) return;

    while(start<end) str[start++] = x;
}

void ltrim(char* str)
{
    int i = 0;

    if(!str) return;

    while((str[i]==' ' || str[i]=='\n') && str[i]!='\0') i++;
    copy_from(str, str, i, strlen(str));
}

void rtrim(char* str)
{
    int i = strlen(str)-1;

    if(!str) return;

    while(i>=0 && (str[i]==' ' || str[i]=='\n')) i--;
    copy_from(str, str, 0, i+1);
}

void trim(char* str)
{
    ltrim(str);
    rtrim(str);
}

void parse(char* input, char* cmd)
{
    int i;

    if(!input) return;
    if(!cmd) return;

    trim(input);
    
    i = find(input, ' ');
    copy_from(cmd, input, 0, i);
    copy_from(input, input, i, strlen(input));
    
    ltrim(input);
}

// https://www.strudel.org.uk/itoa/

/*
    * C++ version 0.4 char* style "itoa":
    * Written by Lukás Chmela
    * Released under GPLv3.
*/

char* itoa(int value, char* result, int base) 
{
    char* str = "zyxwvutsrqponmlkjihgfedcba"
                "9876543210123456789"
                "abcdefghijklmnopqrstuvwxyz";

    char* ptr = result;
    char* ptr1 = result;
    char tmp_char;

    int tmp_value;

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

    FAIL_END:
    return NULL;
}

char* ttoa(int time, char* result)
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