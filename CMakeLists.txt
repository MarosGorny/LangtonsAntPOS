cmake_minimum_required(VERSION 3.18)
project(LangtonsAntPOS C)

set(CMAKE_C_STANDARD 99)

add_executable(LangtonsAntPOS  main.c ant.c ant.h display.c display.h settings.c settings.h)


target_link_libraries(${PROJECT_NAME} pthread)
