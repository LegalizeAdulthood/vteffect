#include "effect.h"

#include <iostream>
#include <stdexcept>

int main(int argc, char *argv[])
{
    try
    {
        options opts(argc, argv);

        switch (opts.m_effect)
        {
        case effects::none:
               break;

        case effects::lptest:
            lptest(opts);
            break;

        case effects::erase_in_line:
            erase_in_line(opts);
            break;

        case effects::erase_in_display:
            erase_in_display(opts);
            break;

        case effects::wipe_circle_in:
            wipe_circle_in(opts);
            break;

        case effects::wipe_circle_out:
            wipe_circle_out(opts);
            break;

        case effects::wipe_random:
            wipe_random(opts);
            break;

        case effects::box:
            box(opts);
            break;

        case effects::canoe:
            canoe(opts);
            break;

        case effects::attribute_wave:
            attribute_wave(opts);
            break;

        case effects::attribute_weave:
            attribute_weave(opts);
            break;

        case effects::maze:
            maze(opts);
            break;

        case effects::figlet:
            figlet(opts);
            break;

        case effects::show:
            show(opts);
            break;

        default:
            throw std::runtime_error("Unimplemented effect");
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }
    return 0;
}
