#include "effect.h"

#include <algorithm>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace
{

template <typename T>
struct name_value
{
    const char *name;
    T value;
};

const name_value<effects> effect_names[] =
{
    { "lptest", effects::lptest },
    { "erase_in_line", effects::erase_in_line },
    { "erase_in_display", effects::erase_in_display },
    { "wipe_circle_in", effects::wipe_circle_in },
    { "wipe_circle_out", effects::wipe_circle_out },
    { "wipe_random", effects::wipe_random },
    { "box", effects::box },
    { "canoe", effects::canoe },
    { "attribute_wave", effects::attribute_wave },
    { "attribute_weave", effects::attribute_weave },
    { "maze", effects::maze },
    { "figlet", effects::figlet },
    { "show", effects::show },
};

const name_value<attributes> attr_names[] =
{
    { "normal", attributes::normal },
    { "bold", attributes::bold },
    { "underscore", attributes::underscore },
    { "blink", attributes::blink },
    { "reverse", attributes::reverse },

    { "bold_underscore", attributes::bold_underscore },
    { "bold_blink", attributes::bold_blink },
    { "bold_reverse", attributes::bold_reverse },
    { "underscore_blink", attributes::underscore_blink },
    { "underscore_reverse", attributes::underscore_reverse },
    { "blink_reverse", attributes::blink_reverse },

    { "bold_underscore_blink", attributes::bold_underscore_blink },
    { "bold_underscore_reverse", attributes::bold_underscore_reverse },
    { "underscore_blink_reverse", attributes::underscore_blink_reverse },
    { "bold_blink_reverse", attributes::bold_blink_reverse },

    { "bold_underscore_blink_reverse",
        attributes::bold_underscore_blink_reverse },
};

const name_value<wave_directions> wave_dir_names[] =
{
{ "top_left", wave_directions::top_left },
{ "top_right", wave_directions::top_right },
{ "bot_left", wave_directions::bot_left },
{ "bot_right", wave_directions::bot_right },
};

void insufficient_args(const char *arg)
{
    std::string what("Insufficient arguments for ");
    what += arg;
    throw std::runtime_error(what.c_str());
}

void string_arg(std::string &option, int i,
    int argc, char *argv[], const char *arg)
{
    if (i == argc)
    {
        insufficient_args(arg);
    }
    option = argv[i];
}

void int_arg(int &option, int i, int argc, char *argv[], const char *arg)
{
    if (i == argc)
    {
        insufficient_args(arg);
    }
    std::istringstream str{argv[i]};
    str >> option;
}

void int_pair_arg(int &first, int &second, int i,
    int argc, char *argv[], const char *arg)
{
    if (i == argc)
    {
        insufficient_args(arg);
    }
    std::istringstream str{argv[i]};
    char c;
    str >> first >> c >> second;
}

template <typename T>
void enum_arg(T &option, const std::string &enum_name,
    int num_names, const name_value<T> names[], int i,
    int argc, char *argv[], const char *arg)
{
    if (i == argc)
    {
        insufficient_args(arg);
    }
    bool found = false;
    for (int j = 0; j < num_names; ++j)
    {
        if (argv[i] == std::string{names[j].name})
        {
            option = names[j].value;
            found = true;
            break;
        }
    }
    if (!found)
    {
        std::string what{"Unknown " + enum_name + ' ' + argv[i]};
        throw std::runtime_error(what.c_str());
    }
}

template <typename T>
void enum_arg_array(std::vector<T> &option,
    const std::string &enum_name, int num_names, const name_value<T> names[],
    int i, int argc, char *argv[], const char *arg)
{
    if (i == argc)
    {
        insufficient_args(arg);
    }
    const std::string str{argv[i] + std::string{","}};
    auto last_pos = std::begin(str);
    for (auto pos = std::find(std::begin(str), std::end(str), ',');
        last_pos != std::end(str);
        pos = std::find(pos+1, std::end(str), ','))
    {
        bool found = false;
        const std::string attr_name{last_pos, pos};
        for (int j = 0; j < num_names; ++j)
        {
            if (attr_name == names[j].name)
            {
                option.push_back(names[j].value);
                found = true;
                break;
            }
        }
        if (!found)
        {
            std::string what{"Unknown " + enum_name + ' ' + str};
            throw std::runtime_error(what.c_str());
        }
        last_pos = pos + 1;
    }
}

} // namespace

options::options(int argc, char *argv[])
{
    for (int i = 1; i < argc; ++i)
    {
        if (argv[i] == std::string{"-l"})
        {
            ++i;
            int_pair_arg(m_top, m_bottom, i, argc, argv, "-l");
        }
        else if (argv[i] == std::string{"-dw"})
        {
            ++i;
            int_pair_arg(m_dw_top, m_dw_bottom, i, argc, argv, "-dw");
        }
        else if (argv[i] == std::string{"-nl"})
        {
            ++i;
            int_arg(m_num_lines, i, argc, argv, "-nl");
        }
        else if (argv[i] == std::string{"-ss"})
        {
            ++i;
            string_arg(m_start_screen, i, argc, argv, "-ss");
        }
        else if (argv[i] == std::string{"-es"})
        {
            ++i;
            string_arg(m_end_screen, i, argc, argv, "-es");
        }
        else if (argv[i] == std::string{"-sattr"})
        {
            ++i;
            enum_arg(m_start_attr, "attribute", num_of(attr_names), attr_names,
                i, argc, argv, "-sattr");
        }
        else if (argv[i] == std::string{"-eattr"})
        {
            ++i;
            enum_arg_array(m_end_attrs,
                "attribute", num_of(attr_names), attr_names,
                i, argc, argv, "-eattr");
        }
        else if (argv[i] == std::string{"-wavdir"})
        {
            ++i;
            enum_arg(m_wave_dir, "wave_direction",
                num_of(wave_dir_names), wave_dir_names,
                i, argc, argv, "-wavdir");
        }
        else
        {
            enum_arg(m_effect, "effect", num_of(effect_names), effect_names,
                i, argc, argv, "<effect>");

            ++i;
            std::copy(&argv[i], &argv[argc], std::back_inserter(m_extra_args));
        }
    }
}
