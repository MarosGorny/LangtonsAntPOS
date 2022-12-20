cmake_minimum_required(VERSION 3.23)
project(LangtonsAntPOS C)

set(CMAKE_C_STANDARD 99)

add_executable(LangtonsAntPOS  main.c ant.c ant.h display.c display.h)


target_link_libraries(${PROJECT_NAME} pthread)
