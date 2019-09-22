#if !defined(SCREEN_H)
#define SCREEN_H

#include <string>
#include <vector>

struct coord;

class screen
{
public:
    screen(const std::string &filename);

    char at(const coord &c) const;

    void show() const;

private:
    std::vector<std::string> m_lines;
};

#endif

