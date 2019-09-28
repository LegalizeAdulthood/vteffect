#include <fstream>
#include <iostream>

#include <stdout_control.h>
#include "vtasm.h"

int main(int argc, char *argv[])
{
    set_binary_stdout();
    if (argc > 1)
    {
        std::ifstream s(argv[1]);
        assemble(s);
    }
    else
    {
        assemble(std::cin);
    }
}
