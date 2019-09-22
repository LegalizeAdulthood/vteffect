#include "effect.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <vector>

#include "coord.h"
#include "goto.h"
#include "screen.h"

namespace
{

double distance(double r1, double c1, double r2, double c2)
{
    return std::sqrt((r2-r1)*(r2-r1)*80 + (c2-c1)*(c2-c1)*24);
}

int distance(const coord &c)
{
    return static_cast<int>(distance(c.row, c.col, 12.5, 40.5) + 0.5);
}

int dw_distance(const coord &c)
{
    return static_cast<int>(distance(c.row, 2*c.col, 12.5, 40.5) + 0.5);
}

int distance(const coord &c, const options &opts)
{
    if (opts.m_dw_top != -1 && opts.m_dw_bottom != -1)
    {
        if (c.row >= opts.m_dw_top && c.row <= opts.m_dw_bottom)
        {
            return dw_distance(c);
        }
    }
    return distance(c);
}

using It = std::vector<coord>::const_iterator;

class wiper
{
public:
    wiper(const options &opts);

    virtual void play() = 0;

protected:
    std::vector<coord> limit_coords() const;
    void clear(const coord &c);
    void clear_coords(It begin, It end)
    {
        std::for_each(begin, end, [this](const coord &c) { clear(c); });
    }
    void set_cursor(const coord &c)
    {
        assert(c.row >= 1 && c.row <= 24);
        assert(c.col >= 1 && c.col <= 80);
        goto_coord(c, m_last);
    }
    void set_cursor(int row, int col)
    {
        assert(row >= 1 && row <= 24);
        assert(col >= 1 && col <= 80);
        goto_coord(coord{row, col}, m_last);
    }

    screen m_begin;
    screen m_end;
    const options &m_opts;
    coord m_last = {-1,-1};
};

wiper::wiper(const options &opts)
    : m_begin(opts.m_start_screen),
    m_end(opts.m_end_screen),
    m_opts(opts)
{
}

std::vector<coord> wiper::limit_coords() const
{
    std::vector<coord> coords;
    for (int r = 1; r <= 24; ++r)
    {
        if ((m_opts.m_top == -1 || r < m_opts.m_top)
            || (m_opts.m_bottom == -1 || r > m_opts.m_bottom))
        {
            const int len = ((m_opts.m_dw_top == -1 || r < m_opts.m_dw_top)
                || (m_opts.m_dw_bottom == -1 || r > m_opts.m_dw_bottom))
                ? 80 : 40;
            for (int c = 1; c <= len; ++c)
            {
                coords.push_back({r, c});
            }
        }
    }
    return coords;
}

void wiper::clear(const coord &c)
{
    if (m_begin.at(c) != m_end.at(c))
    {
        set_cursor(c);
        std::putchar(m_end.at(c));
    }
}

class circle_in_wiper : public wiper
{
public:
    using wiper::wiper;

    void play() override;
};

void circle_in_wiper::play()
{
    std::vector<coord> coords = limit_coords();
    std::sort(coords.begin(), coords.end(),
        [this](const coord &lhs, const coord &rhs) {
            int d1 = distance(lhs, m_opts);
            int d2 = distance(rhs, m_opts);
            if (d1 > d2)
            {
                return true;
            }
            if (d1 == d2)
            {
                if (lhs.row > rhs.row)
                {
                    return true;
                }
                if (lhs.row == rhs.row)
                {
                    return lhs.col < rhs.col;
                }
            }
            return false;
        });
    clear_coords(coords.cbegin(), coords.cend());
}

class circle_out_wiper : public wiper
{
public:
    using wiper::wiper;

    void play() override;
};

void circle_out_wiper::play()
{
    std::vector<coord> coords = limit_coords();
    std::sort(coords.begin(), coords.end(),
        [this](const coord &lhs, const coord &rhs) {
            int d1 = distance(lhs, m_opts);
            int d2 = distance(rhs, m_opts);
            if (d1 < d2)
            {
                return true;
            }
            if (d1 == d2)
            {
                if (lhs.row < rhs.row)
                {
                    return true;
                }
                if (lhs.row == rhs.row)
                {
                    return lhs.col < rhs.col;
                }
            }
            return false;
        });
    clear_coords(coords.cbegin(), coords.cend());
}

class random_wiper : public wiper
{
public:
    using wiper::wiper;

    void play() override;
};

void random_wiper::play()
{
    std::vector<coord> locs = limit_coords();
    std::random_device dev;
    std::mt19937 generator(dev());
    std::shuffle(locs.begin(), locs.end(), generator);

    for (const coord &c : locs)
    {
        if (m_begin.at(c) != m_end.at(c))
        {
            set_cursor(c);
            std::putchar(m_end.at(c));
        }
    }
}

class attribute_waver : public wiper
{
public:
    using wiper::wiper;

    void play() override;

protected:
    void set_attr(attributes attr);

private:
    void play_top_left();
    void play_bot_right();
    attributes m_current_attr = attributes::normal;
};

class attribute_weaver : public attribute_waver
{
public:
    using attribute_waver::attribute_waver;

    void play() override;
};

const int wave_width = 4;

bool changes_space(attributes attr)
{
    switch (attr)
    {
    case attributes::underscore:
    case attributes::reverse:
    case attributes::bold_underscore:
    case attributes::bold_reverse:
    case attributes::underscore_blink:
    case attributes::underscore_reverse:
    case attributes::blink_reverse:
    case attributes::bold_underscore_blink:
    case attributes::bold_underscore_reverse:
    case attributes::underscore_blink_reverse:
    case attributes::bold_blink_reverse:
    case attributes::bold_underscore_blink_reverse:
        return true;
    }
    return false;
}

void attribute_waver::play()
{
    switch (m_opts.m_wave_dir)
    {
    case wave_directions::top_left:
        play_top_left();
        break;

    case wave_directions::bot_right:
        play_bot_right();
        break;
    }
}

void attribute_waver::play_top_left()
{
    const int top_row = m_opts.m_top != -1 ? m_opts.m_top : 1;
    const int bot_row = m_opts.m_bottom != -1 ? m_opts.m_bottom : 24;
    const int row_count = bot_row - top_row + 1;
    const int frame_count = 80 + (row_count - wave_width)*wave_width;
    const bool changes = changes_space(m_opts.m_end_attrs.back());
    for (int frame = 1; frame <= frame_count; ++frame)
    {
        const int start_col = frame - wave_width;

        int col = start_col;
        for (int row = top_row; row <= bot_row; ++row)
        {
            if (col > 1 && col <= 81)
            {
                const char c = m_begin.at(coord{row, col - 1});
                if (c != ' ' || changes)
                {
                    set_attr(m_opts.m_end_attrs.back());
                }
                set_cursor(row, col - 1);
                std::putchar(c);
            }
            --col;
        }

        col = start_col;
        set_attr(attributes::normal);
        for (int row = top_row; row <= bot_row; ++row)
        {
            if (col + wave_width >= 1 && col + wave_width <= 80)
            {
                set_cursor(row, col + wave_width);
                std::putchar('/');
            }
            --col;
        }
    }
    set_attr(attributes::normal);
}

void attribute_waver::play_bot_right()
{
    const int top_row = m_opts.m_top != -1 ? m_opts.m_top : 1;
    const int bot_row = m_opts.m_bottom != -1 ? m_opts.m_bottom : 24;
    const int row_count = bot_row - top_row + 1;
    const int frame_count = 80 + (row_count - wave_width)*wave_width + 1;
    const bool changes = changes_space(m_opts.m_end_attrs.back());
    for (int frame = 1; frame <= frame_count; ++frame)
    {
        const int start_col = 80 + row_count - (frame - 1);
        for (int row = top_row; row <= bot_row; ++row)
        {
            const int col = start_col + wave_width - row;
            if (col >= 1 && col <= 80)
            {
                const char c = m_begin.at(coord{row, col});
                if (c != ' ' || changes)
                {
                    set_attr(m_opts.m_end_attrs.back());
                }
                set_cursor(row, col);
                std::putchar(c);
            }
        }

        set_attr(attributes::normal);
        for (int row = top_row; row <= bot_row; ++row)
        {
            const int col = start_col - row;
            if (col >= 1 && col <= 80)
            {
                set_cursor(row, col);
                std::putchar('/');
            }
        }
    }
    set_attr(attributes::normal);
}

void attribute_waver::set_attr(attributes attr)
{
    if (m_current_attr == attr)
    {
        return;
    }
    m_current_attr = attr;
    const char *args = nullptr;
    switch (attr)
    {
    case attributes::normal:                        args = ""; break;
    case attributes::bold:                          args = "1"; break;
    case attributes::underscore:                    args = "4"; break;
    case attributes::blink:                         args = "5"; break;
    case attributes::reverse:                       args = "7"; break;

    case attributes::bold_underscore:               args = "1;4"; break;
    case attributes::bold_blink:                    args = "1;5"; break;
    case attributes::bold_reverse:                  args = "1;7"; break;
    case attributes::underscore_blink:              args = "4;5"; break;
    case attributes::underscore_reverse:            args = "4;7"; break;
    case attributes::blink_reverse:                 args = "5;7"; break;

    case attributes::bold_underscore_blink:         args = "1;4;5"; break;
    case attributes::bold_underscore_reverse:       args = "1;4;7"; break;
    case attributes::bold_blink_reverse:            args = "1;5;7"; break;
    case attributes::underscore_blink_reverse:      args = "4;5;7"; break;

    case attributes::bold_underscore_blink_reverse: args = "1;4;5;7"; break;
    default:
        assert(false && "Unknown attribute enum");
    }

    std::printf("\033[%sm", args);
}

void attribute_weaver::play()
{
}

} // namespace

void wipe_circle_in(const options& opts)
{
    circle_in_wiper(opts).play();
}

void wipe_circle_out(const options& opts)
{
    circle_out_wiper(opts).play();
}

void wipe_random(const options& opts)
{
    random_wiper(opts).play();
}

void attribute_wave(const options& opts)
{
    attribute_waver(opts).play();
}

void attribute_weave(const options &opts)
{
    attribute_weaver(opts).play();
}
