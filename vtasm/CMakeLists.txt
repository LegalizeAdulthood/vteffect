add_executable(vtasm
    vtasm.h
    main.cpp
    vtasm.cpp
)
target_link_libraries(vtasm stdout_control)
set_property(TARGET vtasm PROPERTY CXX_STANDARD 17)
