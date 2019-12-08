cmake_minimum_required(VERSION 3.12)
include("vcpkg")

set(target "pathtrace")
set(src
    aabb
    fast_rand
    image
    matrix
    octree
    ray
    renderer
    scene
    serializers
    shape
    thread_pool

    shapes/mesh
    shapes/plane
    shapes/sphere
    shapes/triangle
)


list(TRANSFORM src APPEND ".cpp")
add_library("${target}" STATIC ${src})


if (USE_VCPKG)
    vcpkg_install(LIBS
        "glm"
        "tinyobjloader"
    )
endif ()

find_package("glm" CONFIG REQUIRED)
find_package("tinyobjloader" CONFIG REQUIRED)

target_link_libraries("${target}"
    "glm"
    "tinyobjloader::tinyobjloader"
)
