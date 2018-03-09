cmake_minimum_required (VERSION 2.6) 

project (io_stack NONE)

enable_language(C)
enable_language(CXX)

# global needed variables
set(APPLICATION_NAME ${PROJECT_NAME})

set(APPLICATION_VERSION_MAJOR "0")
set(APPLICATION_VERSION_MINOR "1")
set(APPLICATION_VERSION_PATCH "0")

set(APPLICATION_VERSION "${APPLICATION_VERSION_MAJOR}.${APPLICATION_VERSION_MINOR}.${APPLICATION_VERSION_PATCH}")

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(C_CXX_FLAGS "-Wall -Werror -Wformat=2 -Wsign-compare -Wmissing-field-initializers -Wwrite-strings -ggdb -fvisibility=hidden -fno-common")
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(C_CXX_FLAGS "${C_CXX_FLAGS} -Wnewline-eof")
    else()
        # GCC (at least 4.8.4) has a bug where it'll find unreachable free() calls
        # and declare that the code is trying to free a stack pointer.
        set(C_CXX_FLAGS "${C_CXX_FLAGS} -Wno-free-nonheap-object")
    endif()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${C_CXX_FLAGS} -Wmissing-prototypes -Wold-style-definition -Wstrict-prototypes")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 ${C_CXX_FLAGS} -Wmissing-declarations")
endif()  

if((CMAKE_COMPILER_IS_GNUCXX AND CMAKE_C_COMPILER_VERSION VERSION_GREATER "4.7.99") OR
   CMAKE_CXX_COMPILER_ID MATCHES "Clang")
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wshadow")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wshadow")
endif()

if (VALGRIND)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
endif()
if (TESTING)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DTESTING")
endif()
if (DEBUG_MEM)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDEBUG_MEM")
endif()

if (OPENSSL_NO_ASM)
  add_definitions(-DOPENSSL_NO_ASM)
  set(ARCH "generic")
elseif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
  set(ARCH "x86_64")
elseif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "amd64")
  set(ARCH "x86_64")
elseif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "AMD64")
  # cmake reports AMD64 on Windows, but we might be building for 32-bit.
  if (CMAKE_CL_64)
    set(ARCH "x86_64")
  else()
    set(ARCH "x86")
  endif()
elseif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86")
  set(ARCH "x86")
elseif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "i386")
  set(ARCH "x86")
elseif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "i686")
  set(ARCH "x86")
elseif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "aarch64")
  set(ARCH "aarch64")
elseif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "arm64")
  set(ARCH "aarch64")
elseif (${CMAKE_SYSTEM_PROCESSOR} MATCHES "^arm*")
  set(ARCH "arm")
elseif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "mips")
# Just to avoid the “unknown processor” error.
  set(ARCH "generic")
elseif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "ppc64le")
  set(ARCH "ppc64le")
else()
  message(FATAL_ERROR "Unknown processor:" ${CMAKE_SYSTEM_PROCESSOR})
endif()

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