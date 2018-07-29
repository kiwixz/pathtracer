#pragma once

#include "pathtrace/image.h"
#include "pathtrace/scene.h"

namespace pathtracer {
    class Renderer {
    public:
        /// nr_threads=0 means auto
        Image render(const Scene& scene, unsigned nr_threads = 0);
    };
}  // namespace pathtracer
