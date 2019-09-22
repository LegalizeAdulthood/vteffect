#include <fstream>
#include <iostream>

#include "vtdump.h"

int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        std::ifstream s(argv[1]);
        dump(s);
    }
    else
    {
        dump(std::cin);
    }
}
