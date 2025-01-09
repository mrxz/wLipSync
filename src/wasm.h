#pragma once

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

void * memset( void * s, int c, unsigned long n )
{
    unsigned char * p = ( unsigned char * ) s;

    while ( n-- )
    {
        *p++ = ( unsigned char ) c;
    }

    return s;
}