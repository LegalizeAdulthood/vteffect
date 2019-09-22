#include "screen.h"

#include <fstream>

#include "coord.h"
#include "goto.h"

screen::screen(const std::string &filename)
{
    std::ifstream str(filename.c_str());
    while (str)
    {
        std::string line;
        if( std::getline(str, line))
        {
            m_lines.emplace_back(line);
        }
    }
}

char screen::at(const coord &c) const
{
    if (c.row > m_lines.size())
    {
        return ' ';
    }

    if (c.col > m_lines[c.row - 1].length())
    {
        return ' ';
    }

    return m_lines[c.row - 1][c.col - 1];
}

void screen::show() const
{
    coord last{-1, -1};
    for (int row = 1; row <= 24; ++row)
    {
        for (int col = 1; col <= 80; ++col)
        {
            coord pos{row, col};
            char c = at(pos);
            if (c != ' ')
            {
                goto_coord(pos, last);
                std::putchar(c);
            }
        }
    }
}
