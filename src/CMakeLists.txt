# This file was automatically generated for projects
# without default 'CMakeLists.txt' file.

FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/src/*.*)

idf_component_register(
    SRCS 
        "main.c"
        "pins.c"
        "serial.c"
        "system_util.c"
        "dashboard.c"
        "date_time.c"
        "ver.c"
        "can_comm.c"
    INCLUDE_DIRS 
        "."
)
