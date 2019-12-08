cmake_minimum_required(VERSION 3.12)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include("doc_target")
include("vcpkg")
include("set_defaults")


option(BUILD_TESTING "Build tests" ON)
option(DOC_GENERATION "Add target 'doc' to generate documentation" OFF)
option(USE_VCPKG "Use vcpkg toolchain to install and find libs" ON)


set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "")
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS}" CACHE STRING "")
set_defaults_tools_cpp()

if (USE_VCPKG)
    vcpkg_setup("2019.11")
endif ()


project("pathtracer"
    VERSION 0.0.0
    LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set_defaults_flags_cpp()

if (BUILD_TESTING)
    enable_testing()
    find_package("Python3" REQUIRED "Interpreter")
    add_test(NAME "check_clang_format"
        COMMAND "${Python3_EXECUTABLE}" "${PROJECT_SOURCE_DIR}/check_clang_format.py"
    )
    add_test(NAME "check_clang_tidy"
        COMMAND "${Python3_EXECUTABLE}" "${PROJECT_SOURCE_DIR}/check_clang_tidy.py" "${PROJECT_BINARY_DIR}"
    )
endif ()


include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
set(subdirs
    "pathtrace"

    "pathtracer"
)


if (DOC_GENERATION)
    doc_target(DIRS ${subdirs})
endif ()

foreach(dir ${subdirs})
    add_subdirectory("${dir}")
endforeach()
