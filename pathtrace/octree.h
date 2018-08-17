#pragma once

#include <pathtrace/aabb.h>
#include <pathtrace/shapes/triangle.h>

namespace pathtrace {
    class Octree {
    public:
        Octree() = default;

    private:
        Aabb aabb;
    };
}  // namespace pathtrace
