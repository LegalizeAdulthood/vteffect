#include "effect.h"

#include <cstdio>

void lptest(const options &opts)
{
    const int num_printable = 126 - 32 + 1;
    const int lines = (opts.m_num_lines > 1) ? opts.m_num_lines : num_printable;

    for (int row = 0; row < lines; ++row)
    {
        for (int col = 0; col < 79; ++col)
        {
            std::putchar(32 + ((row + col) % num_printable));
        }
        std::putchar('\n');
    }
}
