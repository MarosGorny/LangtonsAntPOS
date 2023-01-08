cmake_minimum_required(VERSION 3.18)
project(LangtonsAntPOS C)

set(CMAKE_C_STANDARD 99)


add_executable(LangtonsAntPOS server.c ant.c ant.h settings.c settings.h structuresEnums.h client_server_definitions.h client_server_definitions.c server_definition.c server_definition.h)
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")

add_executable(tcp_client client.c client_server_definitions.c client_server_definitions.h structuresEnums.h settings.c settings.h client_definition.c client_definition.h)


target_link_libraries(${PROJECT_NAME} pthread)
