#pragma once

#include "pathtrace/fast_rand.h"
#include "pathtrace/image.h"
#include "pathtrace/scene.h"

namespace pathtracer {
    class Renderer {
    public:
        Image render(const Scene& scene);

    private:
        FastRand gen_;

        Color radiance(const Ray& ray, const Scene& scene, int depth = 0);
    };
}  // namespace pathtracer
