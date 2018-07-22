#pragma once

#include "pathtrace/image.h"
#include "pathtrace/scene.h"

namespace pathtracer {
    class Renderer {
    public:
        Image render(const Scene& scene);
    };
}  // namespace pathtracer
