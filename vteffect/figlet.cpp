#include "effect.h"

#include <cstdio>
#include <cstdlib>

void figlet(const options &opts)
{
    std::string command{"figlet"};
    for (const std::string &arg : opts.m_extra_args)
    {
        command += ' ' + arg;
    }
    std::system(command.c_str());
}
