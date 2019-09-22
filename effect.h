#if !defined(EFFECT_H)
#define EFFECT_H

#include <string>
#include <vector>

// Value indicates start corner and proceeds to opposite corner
enum class wave_directions
{
    top_left,
    top_right,
    bot_left,
    bot_right,
};

enum class effects
{
    none,
    lptest,
    erase_in_line,
    erase_in_display,
    wipe_circle_in,
    wipe_circle_out,
    wipe_random,
    box,
    canoe,
    attribute_wave,
    attribute_weave,
    maze,
    figlet,
    show,
};

enum class attributes
{
    // 1 attribute combo
    normal = 0,
    bold = 1,
    underscore = 2,
    blink = 4,
    reverse = 8,

    // 2 attribute combos
    bold_underscore = 3,
    bold_blink = 5,
    bold_reverse = 9,
    underscore_blink = 6,
    underscore_reverse = 10,
    blink_reverse = 12,

    // 3 attribute combos
    bold_underscore_blink = 7,
    bold_underscore_reverse = 11,
    underscore_blink_reverse = 14,
    bold_blink_reverse = 13,

    // 4 attribute combos
    bold_underscore_blink_reverse = 15,
};

struct options
{
    options(int argc, char *argv[]);

    effects m_effect = effects::none;
    int m_top = -1;
    int m_bottom = -1;
    int m_dw_top = -1;
    int m_dw_bottom = -1;
    int m_num_lines = -1;
    std::string m_start_screen;
    std::string m_end_screen;
    attributes m_start_attr = attributes::normal;
    std::vector<attributes> m_end_attrs{ attributes::normal };
    std::vector<std::string> m_extra_args;
    wave_directions m_wave_dir = wave_directions::bot_right;
};

template <typename T, int N>
int num_of(T (&)[N])
{
    return N;
}

void lptest(const options &opts);
void erase_in_line(const options &opts);
void erase_in_display(const options &opts);
void wipe_circle_in(const options &opts);
void wipe_circle_out(const options &opts);
void wipe_random(const options &opts);
void box(const options &opts);
void canoe(const options &opts);
void attribute_wave(const options &opts);
void attribute_weave(const options &opts);
void maze(const options &opts);
void figlet(const options &opts);
void show(const options &opts);

#endif
