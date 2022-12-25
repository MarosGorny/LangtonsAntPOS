cmake_minimum_required(VERSION 3.18)
project(LangtonsAntPOS C)

set(CMAKE_C_STANDARD 99)


add_executable(LangtonsAntPOS  main.c ant.c ant.h display.c display.h settings.c settings.h structuresEnums.h client_server_definitions.h client_server_definitions.c)
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")

add_executable(tcp_client client.c client_server_definitions.c client_server_definitions.h structuresEnums.h settings.c settings.h)
add_executable(tcp_server server.c client_server_definitions.c client_server_definitions.h)


target_link_libraries(${PROJECT_NAME} pthread)
