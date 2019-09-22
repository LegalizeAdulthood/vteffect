#include "effect.h"

#include <cmath>
#include <cstdio>

#include "coord.h"
#include "screen.h"

void erase_in_line(const options &)
{
    for (int row = 1; row <= 24; ++row)
    {
        int col = (40-12) + (40-12)*std::sin(row*2.0*M_PI/24);
        std::printf("\033[%d;%dH\033[1K\033[24C\033[K", row, col);
    }
}

void erase_in_display(const options &)
{
    const coord coords[] = {
        {1, 3}, {24, 77},
        {2, 7}, {23, 73},
        {3, 10}, {22, 70},
        {4, 13}, {21, 67},
        {5, 17}, {20, 63},
        {6, 20}, {19, 60},
        {7, 23}, {18, 57},
        {8, 27}, {17, 53},
        {9, 30}, {16, 50},
        {10, 33}, {15, 47},
        {11, 37}, {14, 43},
        {12, 40}, {13, 40}
    };
    for (int i = 0; i < num_of(coords)/2; ++i)
    {
        const coord &c1 = coords[i*2 + 0];
        const coord &c2 = coords[i*2 + 1];
        std::printf("\033[%d;%dH\033[1J\033[%d;%dH\033[J",
            c1.row, c1.col, c2.row, c2.col);
    }
    std::printf("\033[2J");
}

void show(const options &opts)
{
    screen scr{opts.m_start_screen};
    scr.show();
}
