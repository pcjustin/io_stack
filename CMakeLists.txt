cmake_minimum_required (VERSION 2.6) 
find_package (Threads)

add_library(io_stack
    io_stack.c
    io_element.c
    list.c)

add_executable(server
    server.c)
target_link_libraries(server
    io_stack
    ${CMAKE_THREAD_LIBS_INIT})

add_executable(client
    client.c)
target_link_libraries(client
    io_stack
    ${CMAKE_THREAD_LIBS_INIT})