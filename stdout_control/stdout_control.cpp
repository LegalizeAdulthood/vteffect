#include "stdout_control.h"

#include <iostream>
#if defined(WIN32)
#include <io.h>
#include <fcntl.h>
#endif

void set_binary_stdout()
{
#if defined(WIN32)
    fflush(stdout);
    _setmode(_fileno(stdout), _O_BINARY);
#endif
}
