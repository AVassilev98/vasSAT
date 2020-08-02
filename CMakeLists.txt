cmake_minimum_required(VERSION 3.5)

project(vasSAT LANGUAGES CXX VERSION 0.0.1)

# find_package(Catch2 REQUIRED)

if(NOT WIN32)
  string(ASCII 27 Esc)
  set(ColourReset "${Esc}[m")
  set(BoldRed     "${Esc}[1;31m")
endif()

function(message)
  list(GET ARGV 0 MessageType)
  if(MessageType STREQUAL FATAL_ERROR OR MessageType STREQUAL SEND_ERROR)
    list(REMOVE_AT ARGV 0)
    _message(${MessageType} "${BoldRed}${ARGV}${ColourReset}")
  else()
    _message("${ARGV}")
  endif()
endfunction()

# if(NOT Catch2_FOUND)
#     message(FATAL_ERROR "Catch2 Package is Missing!")
# endif()

set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ standard to conform to")
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

set(CMAKE_BUILD_TYPE Debug) 

if(CMAKE_BUILD_TYPE MATCHES "Debug|RelWithDebInfo")
  set(CMAKE_CXX_FLAGS "-O0 -Werror -Wunused-variable -fno-limit-debug-info")
endif()

add_definitions(-Wno-redeclared-class-member)
add_definitions(-Wno-sometimes-uninitialized)
add_definitions(-Wno-deprecated-declarations)
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
  add_definitions( -Wno-unused-local-typedefs)
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  add_definitions(-Wno-unused-local-typedef)
  add_definitions(-Wno-inconsistent-missing-override)
endif ()

set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/run)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_C_COMPILER clang)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_subdirectory(lib/vasSAT)
add_subdirectory(execs)

install(DIRECTORY include/vasSAT DESTINATION include
  FILES_MATCHING
  PATTERN "*.hpp"
  PATTERN "CMakeFiles" EXCLUDE
)
