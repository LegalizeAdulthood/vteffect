add_executable(vtasm
    vtasm.h
    main.cpp
    vtasm.cpp
)
target_link_libraries(vtasm stdout_control)
target_folder(vtasm "Tools")
