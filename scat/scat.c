#include <stdio.h>
#include <stdlib.h>
#include "port.h"

/* scat -- slow cat for playing back ESC animations
 *
 * Usage: scat <baud> <file>
 *
 * Example: scat 9600 twilight.vt
 *
 * Compile: gcc -O2 -o scat scat.c
 */
#define US_PER_SECOND 1000000

int main(int argc, char *argv[])
{
    int baud = 0;
    int delayPerChunk = 0;
    FILE *f = NULL;
    int chunkSize = 1;
    int charCount = 0;

    if (argc < 1)
    {
        return 1;
    }
    baud = atoi(argv[1]);
    /* assume 10 bit frame for a character: 1 start, 8 data, 1 stop */
    delayPerChunk = US_PER_SECOND*chunkSize/(baud/10);
    while (delayPerChunk < 100)
    {
        chunkSize++;
        delayPerChunk = 1000000*chunkSize/baud;
        delayPerChunk = US_PER_SECOND*chunkSize/(baud/10);
    }

    f = (argc == 3) ? fopen(argv[2], "r") : stdin;
    charCount = 0;
    {
        int c;
        for (c = fgetc(f); !feof(f); c = fgetc(f))
        {
            putchar(c);
            if (++charCount % chunkSize == 0)
            {
                fflush(stdout);
                usleep(delayPerChunk);
            }
        }
    }
    fflush(stdout);

    return 0;
}
