#include "effect.h"

#include <cstdio>
#include <string>

namespace
{

const char *const sprites[][4] =
{
    {
        R"(o_.   )",
        R"([\_\  )",
        R"(----\-)",
        R"(~~~~~@)",
    },
    {
        R"(o_/|  )",
        R"([\_|  )",
        R"(---|--)",
        R"(~~~@~~)",
    },
    {
        R"(o__   )",
        R"([\/   )",
        R"(-/----)",
        R"(@~~~~~)",
    },
    {
        R"(  o,  )",
        R"( </   )",
        R"(-/----)",
        R"(@~~~~~)",
    }
};

void cursor_pos(int row, int col)
{
    if (row == 1)
    {
        if (col == 1)
        {
            std::printf("\033[H");
        }
        else
        {
            std::printf("\033[;%dH", col);
        }
    }
    else
    {
        if (col == 1)
        {
            std::printf("\033[%dH", row);
        }
        else
        {
            std::printf("\033[%d;%dH", row, col);
        }
    }
}

} // namespace

void canoe(const options &opts)
{
    std::printf("\033[H");
    cursor_pos(12, 1);
    for (int i = 1; i <= 80; ++i)
    {
        std::putchar('~');
    }

    for (int f = 1; f <= 47; ++f)
    {
        cursor_pos(11, f);
        std::printf(" (`");
        for (int i = 1; i <= 4; ++i)
        {
            cursor_pos(8 + i, 3 + f);
            for (int s = 0; s < 4; ++s)
            {
                std::putchar("~-  "[3 - (i - 1)]);
                std::printf("%s", sprites[3 - ((s + f) % 4)][i - 1]);
            }
        }
        cursor_pos(11, 4 + 3*7 + 6 + f);
        std::printf("-')");
    }

    for (int f = 48; f <= 80; ++f)
    {
        cursor_pos(11, f);
        switch (f)
        {
        case 80:
            std::putchar(' ');
            break;
        case 79:
            std::printf(" (");
            break;
        default:
            if (f < 79)
            {
                std::printf(" (`");
            }
        }
        if (3 + f < 80)
        {
            for (int i = 1; i <= 4; ++i)
            {
                std::string line;
                for (int s = 0; s < 4; ++s)
                {
                    line += "~-  "[3 - (i - 1)];
                    line += sprites[3 - ((s + f) % 4)][i - 1];
                }
                cursor_pos(8 + i, 3 + f);
                if (3 + f + line.length() > 80)
                {
                    line = line.substr(0, 80 - 3 - f);
                }
                std::printf("%s", line.c_str());
            }
            const int col = 4 + 3*7 + 6 + f;
            cursor_pos(11, col);
            switch (col)
            {
            case 80:
                std::putchar('-');
                break;
            case 79:
                std::printf("-'");
                break;
            default:
                if (col <= 78)
                {
                    std::printf("-')");
                }
                break;
            }
        }
    }
}
