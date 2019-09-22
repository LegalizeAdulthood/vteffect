#include "goto.h"

#include <cstdio>

void goto_coord(const coord &c, coord &last)
{
    if (last.row == c.row)
    {
        if (last.col == c.col-1)
        {
        }
        else if (last.col == c.col+1)
        {
            std::printf("\010\010");
        }
        else if (last.col > c.col)
        {
            const int amount = last.col-c.col+1;
            if (amount > 1)
                std::printf("\033[%dD", amount);
            else
                std::printf("\033[D");
        }
        else
        {
            const int amount = c.col-last.col-1;
            if (amount > 1)
                std::printf("\033[%dC", amount);
            else
                std::printf("\033[C");
        }
    }
    else if (last.col == c.col && last.row == c.row+1)
    {
        std::printf("\033M\010");
    }
    else if (last.col == c.col && last.row == c.row-1)
    {
        std::printf("\n\010");
    }
    else if (c.row == 1)
    {
        if (c.col == 1)
        {
            std::printf("\033[H");
        }
        else
        {
            std::printf("\033[;%dH", c.col);
        }
    }
    else if (c.col == 1)
    {
        std::printf("\033[%dH", c.row);
    }
    else
    {
        std::printf("\033[%d;%dH", c.row, c.col);
    }
    last = c;
}
