cmake_minimum_required(VERSION 3.8)
project(pathtracer)


function ("pathtracer_lib" name)
    file(GLOB_RECURSE src "${name}/*.cpp")
    list(FILTER src EXCLUDE REGEX "stdafx")
    add_library("${name}" "${src}")
endfunction ()

function ("pathtracer_exe" name)
    cmake_parse_arguments(args "" "NAME" "DEPENDS;LIBRARIES" "${ARGN}")
    if (NOT "${args_UNPARSED_ARGUMENTS}" STREQUAL "")
        message(FATAL_ERROR "pathtracer_exe: unrecognized options: ${args_UNPARSED_ARGUMENTS}")
    endif ()

    file(GLOB_RECURSE src "${name}/*.cpp")
    list(FILTER src EXCLUDE REGEX "stdafx")
    add_executable("${name}" "${src}")
    add_dependencies("${name}" "${args_DEPENDS}")
    target_link_libraries("${name}" "${args_LIBRARIES}")
endfunction ()


if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "" FORCE)
endif ()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_STANDARD "17")
set(CMAKE_CXX_STANDARD_REQUIRED "ON")

find_package(Threads REQUIRED)
find_package(fmt REQUIRED)
find_package(glm REQUIRED)
find_package(spdlog REQUIRED)

include_directories("${CMAKE_SOURCE_DIR}")

pathtracer_lib("pathtrace")

pathtracer_exe("app"
    DEPENDS "pathtrace"
    LIBRARIES
        "pathtrace"

        "stdc++fs"
        "Threads::Threads"

        "fmt::fmt"
        "glm"
        "lodepng"
        "spdlog::spdlog"
)
