add_executable(vteffect
    coord.h
    effect.h
    goto.h
    screen.h
    vt100.h
    canoe.cpp
    chargen.cpp
    effect.cpp
    figlet.cpp
    goto.cpp
    lptest.cpp
    main.cpp
    maze.cpp
    options.cpp
    screen.cpp
    vt52.cpp
    wipe.cpp
)
target_link_libraries(vteffect stdout_control)
set_property(TARGET vteffect PROPERTY CXX_STANDARD 17)
