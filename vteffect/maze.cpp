#include "effect.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <vector>

namespace
{

template <typename T>
class array
{
public:
    array(int horiz, int vert)
        : m_horiz(horiz),
        m_vert(vert),
        m_data(new T *[horiz + 1])
    {
        for (int i = 0; i <= horiz; ++i)
        {
            m_data[i] = new T[vert + 1];
            std::fill(m_data[i], m_data[i]+vert+1, T{});
        }
    }
    ~array()
    {
        for (int i = 0; i <= m_horiz; ++i)
        {
            delete[] m_data[i];
        }
        delete[] m_data;
    }
    T *operator[](int h) { return m_data[h]; }
    const T *operator[](int h) const { return m_data[h]; }

private:
    int m_horiz;
    int m_vert;
    T **m_data;
};

double rnd()
{
    return std::rand()/double(RAND_MAX);
}

int rnd(int x)
{
    return int(rnd()*x + 1);
}

void maze(int horiz, int vert)
{
    array<bool> visited(horiz, vert);
    array<int> neighbours(horiz, vert);
    bool q = false;
    bool z = false;
    int x = rnd(horiz);
    for (int i = 1; i <= horiz; ++i)
    {
        std::printf(i == x ? ".  " : ".--");
    }
    std::printf(".\n");
    int c = 1;
    visited[x][1]= true;
    ++c;
    int r = x;
    int s = 1;
    goto advance;

next_cell:
    if (r != horiz)
    {
        ++r;
    }
    else if (s != vert)
    {
        r = 1;
        ++s;
    }
    else
    {
        r = 1;
        s = 1;
    }

L250:
    if (!visited[r][s]) goto next_cell;

advance:
    if (r-1 == 0 || visited[r-1][s]) goto L530;
    if (s-1 == 0 || visited[r][s-1]) goto L390;
    if (r == horiz || visited[r+1][s]) goto L330;
    x = rnd(3);
    if (x == 1)
        goto L790;
    else if (x == 2)
        goto L820;
    else if (x == 3)
        goto L860;

L330:
    if (s != vert)
    {
        if (visited[r][s+1]) goto L370;
    }
    else
    {
        if (z) goto L370;
        q = true;
    }
    x = rnd(3);
    if (x == 1)
        goto L790;
    else if (x == 2)
        goto L820;
    else if (x == 3)
        goto L910;

L370:
    x = rnd(2);
    if (x == 1)
        goto L790;
    else if (x == 2)
        goto L820;

L390:
    if (r == horiz || visited[r+1][s]) goto L470;
    if (s != vert)
    {
        if (visited[r][s+1]) goto L450;
    }
    else
    {
        if (z) goto L450;
        q = true;
    }
    x = rnd(3);
    if (x == 1)
        goto L790;
    else if (x == 2)
        goto L860;
    else if (x == 3)
        goto L910;

L450:
    x = rnd(2);
    if (x == 1)
        goto L790;
    else if (x == 2)
        goto L860;

L470:
    if (s != vert)
    {
        if (visited[r][s+1]) goto L790;
    }
    else
    {
        if (z) goto L790;
        q = true;
    }
    x = rnd(2);
    if (x == 1)
        goto L790;
    else if (x == 2)
        goto L910;
    goto L790;

L530:
    if (s-1 == 0 || visited[r][s-1]) goto L670;
    if (r == horiz || visited[r+1][s]) goto L610;
    if (s != vert)
    {
        if (visited[r][s+1]) goto L590;
    }
    else
    {
        if (z) goto L590;
        q = true;
    }
    x = rnd(3);
    if (x == 1)
        goto L820;
    else if (x == 2)
        goto L860;
    else if (x == 3)
        goto L910;

L590:
    x = rnd(2);
    if (x == 1)
        goto L820;
    else if (x == 2)
        goto L860;

L610:
    if (s != vert)
    {
        if (visited[r][s+1]) goto L820;
    }
    else
    {
        if (z) goto L820;
        q = true;
    }
    x = rnd(2);
    if (x == 1)
        goto L820;
    else if (x == 2)
        goto L910;
    goto L820;

L670:
    if (r == horiz || visited[r+1][s]) goto L740;
    if (s != vert) goto L700;
    if (z) goto L860;
    q = true;
    goto L830;

L700:
    if (visited[r][s+1]) goto L860;
    x = rnd(2);
    if (x == 1)
        goto L860;
    else if (x == 2)
        goto L910;
    goto L860;

L740:
    if (s != vert) goto L760;
    if (z) goto next_cell;
    q = true;
    goto L910;

L760:
    if (visited[r][s+1]) goto next_cell;
    goto L910;

L790:
    visited[r-1][s] = true;
    ++c;
    neighbours[r-1][s] = 2;
    --r;
    if (c == horiz*vert+1) goto done;
    q = false;
    goto advance;

L820:
    visited[r][s-1] = true;

L830:
    ++c;
    neighbours[r][s-1] = 1;
    --s;
    if (c == horiz*vert+1) goto done;
    q = false;
    goto advance;

L860:
    visited[r+1][s] = true;
    ++c;
    if (neighbours[r][s] == 0) goto L880;
    neighbours[r][s] = 3;
    goto L890;

L880:
    neighbours[r][s] = 2;

L890:
    ++r;
    if (c == horiz*vert+1) goto done;
    goto L530;

L910:
    if (q) goto L960;
    visited[r][s+1] = true;
    ++c;
    if (neighbours[r][s] == 0)
    {
        neighbours[r][s] = 1;
    }
    else
    {
        neighbours[r][s] = 3;
    }
    ++s;
    if (c == horiz*vert+1) goto done;
    goto advance;

L960:
    z = true;
    if (neighbours[r][s] != 0)
    {
        neighbours[r][s] = 3;
        q = false;
        goto next_cell;
    }
    neighbours[r][s] = 1;
    q = false;
    r = 1;
    s = 1;
    goto L250;

done:
    for (int j = 1; j <= vert; ++j)
    {
        std::printf("|");
        for (int i = 1; i <= horiz; ++i)
        {
            if (neighbours[i][j] < 2)
            {
                std::printf("  |");
            }
            else
            {
                std::printf("   ");
            }
        }
        std::printf("\n");
        for (int i = 1; i <= horiz; ++i)
        {
            if (neighbours[i][j] == 0 || neighbours[i][j] == 2)
            {
                std::printf(":--");
            }
            else
            {
                std::printf(":  ");
            }
        }
        std::printf(".\n");
    }
}

} // namespace

void maze(const options &opts)
{
    maze(25, 12);
}

