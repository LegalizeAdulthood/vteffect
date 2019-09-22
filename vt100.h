#pragma once

#include <cstdio>
#include <string>

namespace vt100
{

enum class ascii : char
{
    nul = '\x0',
    soh,
    stx,
    etx,
    eot,
    enq,
    ack,
    bel,
    bs,
    ht,
    lf,
    vt,
    ff,
    cr,
    so,
    si,
    dle,
    dc1,
    dc2,
    dc3,
    dc4,
    nak,
    syn,
    etb,
    can,
    em,
    sub,
    esc,
    fs,
    gs,
    rs,
    us,
    sp,
    del =
    '\177'
};

struct term {};

inline term &operator<<(term &t, char val)
{
    std::putchar(val);
    return t;
}

inline term &operator<<(term &t, ascii val)
{
    std::putchar(static_cast<int>(val));
    return t;
}

inline term &operator<<(term &t, const char *text)
{
    while (*text)
    {
        t << *text++;
    }
    return t;
}

inline term &operator<<(term &t, int val)
{
    return t << std::to_string(val).c_str();
}

inline term &csi()
{
    return term{} << ascii::esc << '[';
}

inline term &csi(int arg1, int omit_arg1, char finalizer)
{
    term &t = csi();
    if (arg1 != omit_arg1)
    {
        t << arg1;
    }
    return t << finalizer;
}

inline term &csi(int arg1, int omit1, int arg2, int omit2, char finalizer)
{
    term &t = csi();
    if (arg1 != omit1)
    {
        t << arg1;
    }
    if (arg2 != omit2)
    {
        t << ';' << arg2;
    }
    return t << finalizer;
}

struct cursor_left { int amount = 1; };
struct cursor_right { int amount = 1; };
struct cursor_up { int amount = 1; };
struct cursor_down { int amount = 1; };
struct cursor_position { int row = 1; int col = 1; };

inline term &operator<<(term &t, cursor_up val)
{
    return csi(val.amount, 1, 'A');
}

inline term &operator<<(term &t, cursor_down val)
{
    return csi(val.amount, 1, 'B');
}

inline term &operator<<(term &t, cursor_right val)
{
    return csi(val.amount, 1, 'C');
}

inline term &operator<<(term &t, cursor_left val)
{
    return csi(val.amount, 1, 'D');
}

inline term &operator<<(term &t, cursor_position val)
{
    return csi(val.row, 1, val.col, 1, 'H');
}

struct erase_in_display { int where; };
struct erase_in_line { int where; };

inline term &operator<<(term &t, erase_in_display val)
{
    return csi(val.where, 1, 
}

}
