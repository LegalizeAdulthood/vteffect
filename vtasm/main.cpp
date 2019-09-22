#include <fstream>
#include <iostream>

#include "vtasm.h"

int main(int argc, char *argv[])
{
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
