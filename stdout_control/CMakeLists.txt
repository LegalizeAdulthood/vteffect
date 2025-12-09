add_library(stdout_control
    stdout_control.h
    stdout_control.cpp
)
target_include_directories(stdout_control PUBLIC .)
target_folder(stdout_control "Libraries")
