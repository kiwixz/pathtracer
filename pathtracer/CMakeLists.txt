cmake_minimum_required(VERSION 3.12)
include("vcpkg")

set(target "pathtracer")
set(src
    app
    main
)


list(TRANSFORM src APPEND ".cpp")
add_executable("${target}" ${src})


if (USE_VCPKG)
    vcpkg_install(LIBS
        "cxxopts"
        "lodepng"
        "nlohmann-json"
        "spdlog"
    )
endif ()

find_package("cxxopts" CONFIG REQUIRED)
find_package("nlohmann_json" CONFIG REQUIRED)
find_package("spdlog" CONFIG REQUIRED)

find_library("lodepng_LIB" "lodepng")

target_link_libraries("${target}"
    "cxxopts::cxxopts"
    "nlohmann_json::nlohmann_json"
    "spdlog::spdlog"

    "${lodepng_LIB}"

    "pathtrace"
)
