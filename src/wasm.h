#pragma once
#define JS_FUNC __attribute__((import_module("JS")))

void * memcpy( void * s1, const void * s2, unsigned long n )
{
    char * dest = ( char * ) s1;
    const char * src = ( const char * ) s2;

    while ( n-- )
    {
        *dest++ = *src++;
    }

    return s1;
}